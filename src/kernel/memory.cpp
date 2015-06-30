#include "config.h"
#include "paging.h"
#include "console.h"

uint32_t *mm_hi_mem_stack_base = (uint32_t *) 0xC1500000;
uint32_t *mm_lo_mem_stack_base = (uint32_t *) 0xC1900000;

uint32_t *mm_hi_mem_stack = (uint32_t *) 0xC1500000;
uint32_t *mm_lo_mem_stack = (uint32_t *) 0xC1900000;

/**
 * Returns a free physical page (from low memory)
 */
uint32_t mm_page_allocate_lo()
{
	if(mm_lo_mem_stack == mm_lo_mem_stack_base)
		return 0;

	mm_lo_mem_stack--;
	return *mm_lo_mem_stack;
}

/**
 * Returns a free physical page (prefers high memory)
 */
uint32_t mm_page_allocate()
{
	if(mm_hi_mem_stack == mm_hi_mem_stack_base) 
		return mm_page_allocate_lo();
	
	mm_hi_mem_stack--;
	return *mm_hi_mem_stack;
}

/**
 * Mark page as free
 */
void mm_page_free(uint32_t page)
{
	if(page < 0x100000) {
		*mm_lo_mem_stack = page;
		mm_lo_mem_stack++;
	} else {
		*mm_hi_mem_stack = page;
		mm_hi_mem_stack++;
	}
}

/**
 * ...
 */
void setup_memory_manager(uint32_t first_free_page, uint32_t memory_size)
{
	int i;

	//kc_print_string("\n");
	//kc_print_string(" Setting up pagetable\n");

	pg_setup_pagetable(pg_kernel, 0xC1500000, first_free_page - 0x00100000 + 0xC0000000, first_free_page, 1);
	first_free_page += 0x1000;

	//kc_print_string(" Mapping memory pages\n");

	for(i = 0; i < memory_size / 0x1000 * 0x04 / 0x1000 + 0x01; i++) {
		pg_map(pg_kernel, 0xC1500000 + i * 0x1000, first_free_page, 1);
		first_free_page += 0x1000;
	}

	//kc_print_string(" Mark free memory\n");

	/* Mark some low memory as free */
	for(i = first_free_page; i < memory_size; i += 0x1000)
		mm_page_free(i);
}

void mm_get_pages_free(uint32_t &lo, uint32_t &hi)
{
  hi = (mm_hi_mem_stack - mm_hi_mem_stack_base) / sizeof(uint32_t);
  lo = (mm_lo_mem_stack - mm_lo_mem_stack_base) / sizeof(uint32_t);  
}

