#include "exceptions.h"
#include "interrupt.h"
#include "console.h"
#include "debug.h"
#include "process.h"

#include "symbol.h"

extern "C" {
	/* CPU Interrupts */
	extern void is_int0();
	extern void is_int1();
	extern void is_int2();
	extern void is_int3();
	extern void is_int4();
	extern void is_int5();
	extern void is_int6();
	extern void is_int7();
	extern void is_int8();
	extern void is_int9();
	extern void is_int10();
	extern void is_int11();
	extern void is_int12();	
	extern void is_int13();
	extern void is_int14();
	extern void is_int15();
	extern void is_int16();
	extern void is_int17();
	extern void is_int18();
	extern void is_int19();
	extern void is_int20();
	extern void is_int21();
	extern void is_int22();
	extern void is_int23();
	extern void is_int24();
	extern void is_int25();
	extern void is_int26();
	extern void is_int27();
	extern void is_int28();
	extern void is_int29();
	extern void is_int30();
	extern void is_int31();
}

void is_install_exception_handlers()
{
	is_install_handler(0x00, (void *) is_int0, 0);
	is_install_handler(0x01, (void *) is_int1, 0);
	is_install_handler(0x02, (void *) is_int2, 0);
	is_install_handler(0x03, (void *) is_int3, 0);
	is_install_handler(0x04, (void *) is_int4, 0);
	is_install_handler(0x05, (void *) is_int5, 0);
	is_install_handler(0x06, (void *) is_int6, 0);
	is_install_handler(0x07, (void *) is_int7, 0);
	is_install_handler(0x08, (void *) is_int8, 0);
	is_install_handler(0x09, (void *) is_int9, 0);
	is_install_handler(0x0A, (void *) is_int10, 0);
	is_install_handler(0x0B, (void *) is_int11, 0);
	is_install_handler(0x0C, (void *) is_int12, 0);	
	is_install_handler(0x0D, (void *) is_int13, 0);
	is_install_handler(0x0E, (void *) is_int14, 0);
	is_install_handler(0x0F, (void *) is_int15, 0);
	is_install_handler(0x10, (void *) is_int16, 0);
	is_install_handler(0x11, (void *) is_int17, 0);
	is_install_handler(0x12, (void *) is_int18, 0);
	is_install_handler(0x13, (void *) is_int19, 0);
	is_install_handler(0x14, (void *) is_int20, 0);
	is_install_handler(0x15, (void *) is_int21, 0);
	is_install_handler(0x16, (void *) is_int22, 0);
	is_install_handler(0x17, (void *) is_int23, 0);
	is_install_handler(0x18, (void *) is_int24, 0);
	is_install_handler(0x19, (void *) is_int25, 0);
	is_install_handler(0x1A, (void *) is_int26, 0);
	is_install_handler(0x1B, (void *) is_int27, 0);
	is_install_handler(0x1C, (void *) is_int28, 0);
	is_install_handler(0x1D, (void *) is_int29, 0);
	is_install_handler(0x1E, (void *) is_int30, 0);
	is_install_handler(0x1F, (void *) is_int31, 0);
}

void is_cexception(uint32_t exception_id, uint32_t error_code)
{
	uint32_t tmp;
	unsigned int stack;
	uint32_t ebp;
	
	asm volatile("movl %%ebp, %0" : "=g" (ebp) );

	asm volatile("movl %%esp, %0" : "=g" (stack) );
	unsigned int *content = (unsigned int *) stack;
	
	switch(exception_id) {
		case 0:
			kc_print_string("Division by zero\n");
			break;
		case 1:
			kc_print_string("Debug exception\n");
			db_dump_dregs();
			break;
		case 2:
			kc_print_string("NMI Interrupt\n");
			break;
		case 3:
			kc_print_string("Breakpoint exception\n");
			break;
		case 4:
			kc_print_string("Overflow exception\n");
			break;
		case 5:
			kc_print_string("BOUND Range exceeded exception\n");
			break;
		case 6:
			kc_print_string("Invalid opcode\n");
			break;
		case 7:
			kc_print_string("Device not available\n");
			break;
		case 8:
			//kc_print_string("Double fault\n");
			asm volatile("cli");
			asm volatile("hlt");
			break;
		case 9:
			kc_print_string("Coprocessor segment overrun\n");
			break;
		case 10: // 0xA
			kc_print_string("Invalid TSS exception\n");
			break;
		case 11: // 0xB
			kc_print_string("Segment not present\n");
			break;
		case 12: // 0xC
			kc_print_string("Stack fault\n");
			break;
		case 13: // 0xD
			kc_print_string("General protection fault (");	
			kc_print_hex(error_code);
			kc_print_string(")\n");

			if(pr_get_current() != NULL) {
				kc_print_string("Suspending process: ");
				kc_print_hex(pr_getpid());
				kc_print_string("\n");

				pr_suspend(pr_get_current());
				pr_schedule();
			}

			break;
		case 14: // 0xE
		
			uint32_t cr2;
			asm("mov %%cr2, %0" : "=r" (cr2));
						
			#ifdef PF_DEBUG
			kc_print_string("Page fault (");
			kc_print_hex(cr2);
			kc_print_string(", ");
			kc_print_hex(error_code);
			kc_print_string(")\n");

			content = (uint32_t *) (((uint32_t) content & 0xFFFFF000) + 0xFF0);
			kc_print_string("Return values: ");
			kc_print_hex(content[0]);
			kc_print_string(", ");
			kc_print_hex(content[1]);
			kc_print_string(", ");
			kc_print_hex(content[2]);
			kc_print_string(", ");
			kc_print_hex(content[3]);
			kc_print_string("\n");
			#endif

			// Ask pager to resolve the issue
			if(pg_resolve_fault(cr2)) {
				#ifdef PF_DEBUG
				kc_print_string("Resolved...\n");
				#endif

				return;
			}

			/*kc_print_string("Flags:");

			if(content[17] & 0x01)
				kc_print_string(" Present");

			if(content[17] & 0x02)
				kc_print_string(" Read/write");

			if(content[17] & 0x04)
				kc_print_string(" Supervisor");

			kc_print_string("\n");*/

			if(pr_get_current() != NULL) {
				kc_print_string("Suspending process: ");
				kc_print_hex(pr_getpid());
				kc_print_string("\n");
				pr_suspend(pr_get_current());
				pr_schedule();
			}

			break;
		case 16:
			kc_print_string("x87 FPU floating-point error\n");
			break;
		case 17:
			kc_print_string("Alignment check exception\n");
			break;
		case 18:
			kc_print_string("Machine check exception\n");
			break;
		case 19:
			kc_print_string("SIMD floating point exception\n");
			break;
		default:
			kc_print_string("\n\nException: ");
			kc_print_hex(exception_id);
			kc_print_string("\n");
	}

	kc_print_string("\n");
	kc_print_string("EDI: "); kc_print_hex(content[10]); kc_print_string("\t");
	kc_print_string("ESI: "); kc_print_hex(content[11]); kc_print_string("\t");
	kc_print_string("EBP: "); kc_print_hex(content[12]); kc_print_string("\t");
	kc_print_string("ESP: "); kc_print_hex(content[13]); kc_print_string("\n");
	kc_print_string("EBX: "); kc_print_hex(content[14]); kc_print_string("\t");
	kc_print_string("EDX: "); kc_print_hex(content[15]); kc_print_string("\t");
	kc_print_string("ECX: "); kc_print_hex(content[16]); kc_print_string("\t");
	kc_print_string("EAX: "); kc_print_hex(content[17]); kc_print_string("\n");
	kc_print_string("\n");

	db_stack_trace(content[12]);

	kc_print_string("Halting CPU!");

	asm volatile("cli");
	asm volatile("hlt");
}
