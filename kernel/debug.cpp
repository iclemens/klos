#include "config.h"
#include "console.h"
#include "memory.h"
#include "symbol.h"
#include "kalloc.h"

void db_dump_dregs()
{
	uint32_t dr0, dr1, dr2, dr3, dr4, dr5, dr6, dr7;

	asm volatile("mov %%dr0, %0" : "=r" (dr0));
	asm volatile("mov %%dr1, %0" : "=r" (dr1));
	asm volatile("mov %%dr2, %0" : "=r" (dr2));
	asm volatile("mov %%dr3, %0" : "=r" (dr3));
	
	asm volatile("mov %%dr6, %0" : "=r" (dr6));
	asm volatile("mov %%dr7, %0" : "=r" (dr7));

	kc_printf("DR0: %x\t DR1: %x \t DR2: %x\t DR3: %x\n", dr0, dr1, dr2, dr3);
	kc_printf("DR6: %x\t DR7: %x\n",dr6, dr7);
}

void db_stack_trace(uint32_t ebp)
{
	uint32_t *stack_frame = (uint32_t *)ebp;

	if(stack_frame == 0) {
		kc_print_string("Stack trace not possible ebp == 0\n");
		return;
	}

	kc_print_string("Stack trace:\n");

	while(1) {
		kc_print_hex(stack_frame[1]);
		kc_print_string(" (");
		kc_print_string(sm_resolve(stack_frame[1]));
		kc_print_string(")\n");

		if(((uint32_t)stack_frame & 0xFFFFF000) != (stack_frame[0] & 0xFFFFF000))
			break;

		stack_frame = (uint32_t *) stack_frame[0];
	}
}

void db_mm_info()
{
  uint32_t lo, hi;
  
  mm_get_pages_free(lo, hi);  
  kc_printf("MEM: %d pages available; ", lo + hi, lo, hi);
  
  size_t allocated, total;
  kh_get_memory_info(allocated, total);
  
  kc_printf("HEAP: %d of %d bytes used\n", allocated, total);
}
