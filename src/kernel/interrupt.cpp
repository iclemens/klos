#include "io.h"
#include "console.h"
#include "vfs.h"
#include "process.h"
#include "debug.h"
#include "exceptions.h"

/**
 * 8259 Programmable Interrupt Controller
 *
 * Hmmm... should move the IDT elsewhere as it is not part of the 8259 :D
 */

#include "exceptions.h"

/**
 * _isr, implemented in assembly, receives an
 * interrupt and calls the interrupt_handler() function.
 *
 * This should never be called directly, execution of
 * iret will cause a crash.
 */
extern "C" {
	extern void is_panic();

	extern void is_stub();

	/* 8259 Interrupts */
	extern void is_8259();
	extern void is_timer();
	extern void is_keyboard();

	/* Generic */
	extern void is_master_pic();
	extern void is_slave_pic();

	extern void is_api();
}

#include "config.h"
#include "interrupt.h"

struct idt_entry {
	uint16_t offset_low;
	uint16_t segment;
	uint8_t zero;
	uint8_t flags;
	uint16_t offset_high;
} __attribute__((packed,aligned(4)));

struct idt_pointer {
	uint16_t limit;
	uint32_t offset;
} __attribute__((packed,aligned(4)));

/* Should we put this _in_ the kernel? */
struct idt_entry idt[256];
struct idt_pointer idtp;

void (*sched_hook)();

/**
 * Initializes the PICs and enables interrupts
 */
void setup_interrupts()
{
	sched_hook = 0;

	int i;
	for(i = 0; i < 256; i++) {
	  // FIXME: This only works for 32-bit addresses
	  idt[i].offset_low = (ptr_t) is_panic & 0xFFFF;
		idt[i].segment = 0x08;	/* Code segment */
		idt[i].zero = 0;
		idt[i].flags = 0x8E;	/* DPL = 0, Present, Interrupt gate */
		idt[i].offset_high = ((ptr_t) is_panic >> 16) & 0xFFFF;
	}

	idtp.offset = (ptr_t) idt;
	idtp.limit = 0x07FF;
	//asm volatile("lidt (%0)" : : "m" (idtp));

	/* Mask everything */
	outb(0xFF, 0x21);	/* Used to be FF, changed to F5 because of bochs */
	outb(0xFF, 0xA1); /* Which does not support single + level sensitive mode */

	/* Setup the master PIC */
	outb(0x11, 0x20);		/* ICW 1 */
	outb(0x20, 0x21);		/* ICW 2, master offset at 0x20 */
	outb(0x04, 0x21);		/* ICW 3, pic2 connected to line 2 */
	outb(0x05, 0x21);		/* ICW 4, master pic, 8086 mode */

	/* Setup the slave PIC */
	outb(0x11, 0xA0);		/* ICW 1 */
	outb(0x28, 0xA1);		/* ICW 2, slave offset at 0x28 */
	outb(0x02, 0xA1);		/* ICW 3, pic2 connected to line 2 */
	outb(0x01, 0xA1);		/* ICW 4, slave pic, 8086 mode */

	// Install timer interrupt...
	uint16_t frequency = 65535;	/* 1193180/65535 = 18.2Hz */

	outb(0x36, 0x43);
	outb(frequency & 0xFF, 0x40);
	outb((frequency >> 8) & 0xFF, 0x40);

	is_install_exception_handlers();

	is_install_handler(0x20, (void *) is_timer, 0);
	is_install_handler(0x21, (void *) is_keyboard, 0);

	for(i = 0x22; i < (0x20 + 0x0F); i++)
		is_install_handler(i, (void *) is_8259, 0);

	/* Ring 3 gate, callable from userspace */
	is_install_handler(0x80, (void *) is_api, 3);

	/* Restore mask (only keyboard for now) */
	outb(0xFC, 0x21);
	outb(0xFF, 0xA1);

	asm volatile("sti");
}

/**
 * Set scheduling hook...
 */
void is_install_hook( void(*hook)() )
{
	sched_hook = hook;
}

void is_install_handler(int interrupt, void *service_routine, int dpl)
{
	idt[interrupt].offset_low = (ptr_t) service_routine & 0xFFFF;
	idt[interrupt].segment = 0x08;	/* Code segment */
	idt[interrupt].zero = 0;
	idt[interrupt].flags = 0x80 | ((dpl & 0x03) << 5) | 0x0E;
	idt[interrupt].offset_high = ((ptr_t) service_routine >> 16) & 0xFFFF;
}

void is_cstub(void)
{
	kc_print_string("\nStub interrupt handler!\n");
}

void is_c8259(void)
{
	kc_print_string("\n8259 Interrupt!\n");

	outb(0x20, 0x20);			/* End of Interrupt PIC 1 */
	outb(0x20, 0xA0);			/* End of Interrupt PIC 2 */
}

/**
 * This function handles interrrupts, note that it is
 * not being invoked by the processor directly. A small
 * assembly routine (is_panic) is used instead.
 */
void is_cpanic(void)
{
	unsigned int stack;
	asm volatile("movl %%esp, %0" : "=g" (stack) );

	kc_print_string("\nKernel panic!\n");
	kc_print_string("Don't know how to handle interrupt\n");

	unsigned int *content = (unsigned int *) stack;

	db_stack_trace(content[10]);

	asm volatile("cli");
	asm volatile("hlt");

	//outb(0x20, 0x20);			/* End of Interrupt PIC 1 */
	//outb(0x20, 0xA0);			/* End of Interrupt PIC 2 */
}

void is_cpic(int pic)
{
	kc_print_string("\nReceived interrupt from 8259...\n");

	is_cpanic();
}

void is_ctimer(void)
{
	// 18.2 Hz
	static uint32_t time = 0;

	time += 10;

	uint32_t time_sec;
	time_sec = time / 182;

	outb(0x20, 0x20);			/* End of Interrupt PIC 1 */

	if(sched_hook != 0)
		sched_hook();
}

void is_ckeyboard(void)
{
	int status = inb(0x64);
	int scancode = inb(0x60);

	outb(0x20, 0x20);			/* End of Interrupt PIC 1 */
}

void is_capi(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
	unsigned int i;
	uint8_t tmp;
	unsigned char *string = (unsigned char *) c;

	switch(a) {
		case 0x01: // Exit current program
			pr_terminate(pr_get_current());

			kc_print_string("Process terminated!\n");
			db_mm_info();

			pr_schedule();

			break;

		case 0x02: // Fork
			{
				struct pr_process_t *forked = pr_fork_process(pr_get_current());
				kc_print_string("Process forked!\n");

				uint32_t *stack;

				// Set return code for parent process
				stack = (uint32_t *) (pr_get_current()->kernel_stack + 4096);
				stack[-6] = forked->pid;

				// Set return code for forked process
				stack = (uint32_t *) (forked->kernel_stack + 4096);
				stack[-6] = 0;

				forked->state = pr_state_running;

				for(i = 0; i < 4; i++)
				  stack[i - 17] = 0x20;

				pr_resume(forked);
			}
			break;

		case 0x03: // Read
			break;

		case 0x04: // Write
			for(i = 0 ; i < d; i++) {
				kc_write_character(*(string + i));
			}
			break;


		case 0x05: // Open (char *, int int)
			//tmp = vfs_open(string, c);
			asm volatile("mov %%eax, %0" : : "m" (tmp));
			break;

		case 0x14: // Get PID
			tmp = pr_getpid();
			asm volatile("mov %%eax, %0" : : "m" (tmp));
			break;

		default:
			kc_print_string("Invalid syscall!\n");
			is_cpanic();
	}
}
