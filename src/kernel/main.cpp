#include "config.h"
#include "memory.h"
#include "console.h"
#include "interrupt.h"
#include "gdt.h"
#include "process.h"
#include "paging.h"
#include "io.h"
#include "ata.h"
#include "debug.h"
#include "kalloc.h"
#include "vfs.h"

/**
 * Virtual memory:
 *  0xC03Exxxx - Reserved for pager

 *  0xC03FB000 - Initial kernel heap page table
 *  0xC03FCxxx - Kernel page directory
 *  0xC03FDxxx - Kernel page table pointers
 *  0xC03FExxx - Page table
 *  0xC03FFxxx - Page table

 *  0xC15xxxxx - High memory stack
 *  0xC1xxxxxx - High memory stack
 *  0xC1900xxx - Low memory stack

 *  0xC2000xxx - Text buffer (console output)

 *  0xD0000000 - Kernel heap
 */

extern unsigned int end_of_kernel;

//const true = 1;
//const false = 0;

void sleep(int seconds)
{
	/* Call scheduler and set wake-up time... */
}

void schedule()
{
	//kc_print_string(".");
	pr_schedule();
}

int main(uint32_t low_mem, uint32_t high_mem)
{
	int i;

	setup_console();

	kc_print_string("Starting KlOS...\n\n");

	kc_print_string("Replacing descriptor table...");
	setup_gdt();
	kc_print_string("\tsuccess\n");

	/* Kernel occupies physical memory pages: */
	uint32_t kernel_first_page = 0x00100000;
	uint32_t kernel_last_page  = ((ptr_t) &end_of_kernel - 0xBFF00000) & 0xFFFFF000;
	kernel_last_page += 0x1000;

	//kc_print_hex(kernel_last_page);

	/* Setup paging */
	kc_print_string("Setting up paging system...");
	setup_paging((uint32_t *)0xC03FC000, (uint32_t **)0xC03FD000);
	kc_print_string("\tsuccess\n");

	/* Enable exception handler */
	kc_print_string("Setting up interrupt handler...");
	setup_interrupts();
	kc_print_string("\tsuccess\n");

	/* Setup memory manager */
	// FIXME: Use high_mem
	kc_print_string("Setting up memory manager...");
	setup_memory_manager(kernel_last_page + 0x4000, 16 * 0x100000);
	setup_heap((uint32_t *) 0xC03FB000);
	kc_print_string("\tsuccess\n");

	/* Move text area to 0xC20000000 and remove identity map */
	kc_print_string("Mapping 0xC2000000\n");
	pg_map(pg_kernel, 0xC2000000, 0xB8000, 1);
	kc_print_string("Resetting text base\n");
	kc_set_text_base(0xC2000000);
	kc_print_string("Removing pagetable 0x0\n");

	pg_remove_pagetable(pg_kernel, 0x0);

	setup_vfs();


	kc_print_string("Initializing processing...");
	setup_process();
	kc_print_string("\tsuccess\n");



	/******************************************************/
	setup_ata_devices();

	int appSize = 8000;

	// Make sure a few pages are available on the heap... to test the allocator
	for(i = 0; i < 30; i++)
		kbrk();

 	kc_print_string("\n\n***********************************\n\n");
	db_mm_info();

	struct pr_process_t *app1 = pr_create_process();

	// Allocate enough pages for application
	for(i = 0; i < 2; i++) {
		uint32_t myPage = mm_page_allocate();

		pg_map(app1->page_table, i << 12, myPage, 0);
		pg_map(pg_kernel, i << 12, myPage, 1);
	}

	// Read application from disk
	unsigned char *buffer = 0x0;
	ata_read(0, buffer, 0, appSize);

	// Fork... now there are two apps :D
	struct pr_process_t *app2 = pr_fork_process(app1);

	is_install_hook(&schedule);

	asm("sti");

	app1->state = pr_state_running;
	app2->state = pr_state_running;

	while(1) {
	}
}

