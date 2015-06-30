/**
 * paging.c :: IOS Paging Subsystem
 *
 * Handles creation and maintainance of the page directory and associated tables.
 *
 * Because these routines are used _before_ memory and interrupts are enabled,
 * using STI or kalloc() is not a good idea. This is why flags are pushed
 * onto the stack (PUSHF/POPF) and some data is statically allocated.
 */

#include "config.h"
#include "paging.h"
#include "process.h"
#include "kalloc.h"
#include "memory.h"
#include "console.h"

extern "C" {
	extern void swap_pagetable(uint32_t page_directory);

	extern uint32_t kernel_heap_start;
	extern uint32_t kernel_heap_alloc;
}

extern struct pr_process_t *pr_root;
extern struct pr_process_t *pr_current;

/**
 * The kernel's page directory is a template for all other
 * page directories. It maps the upper 3GB to the kernel
 * and other system regions. In addition the lower 3GB are
 * left alone.
 *
 * @var References to kernel pagetable
 */
struct pg_table_t pg_kernel_real;
struct pg_table_t *pg_kernel;

/**
 * @var Pointer to the active pagetable
 */
struct pg_table_t *pg_active;

inline void pg_invalidate_tlb_entry(uint32_t address)
{
	asm volatile("invlpg (%%eax)" : : "a" (address) );
}

inline void pg_invalidate_tlb()
{
  swap_pagetable(pg_active->physical);
}

inline uint32_t get_flags()
{
	uint32_t flags;

	asm volatile("pushf");
	asm volatile("pop %0" : "=m"(flags));

	return flags;
}

inline void set_flags(uint32_t flags)
{
	asm volatile("push %0" : : "m"(flags));
	asm volatile("popf");
}

void pg_activate_table(struct pg_table_t *table)
{
	// Already active
	if(pg_active == table)
		return;

	if(!table)
		table = pg_kernel;

	swap_pagetable(table->physical);
	pg_active = table;
	
}

inline uint32_t pg_pde_index(uint32_t virt)
{
	return (virt >> 22) & 0x3FF;
}

inline uint32_t pg_pte_index(uint32_t virt)
{
	return (virt >> 12) & 0x3FF;
}

/**
 * Returns the physical address of a page in the page-table
 */
uint32_t pg_get_physical(struct pg_table_t *table, uint32_t virt)
{
	return table->tables[ pg_pde_index(virt) ][ pg_pte_index(virt) ] & pte_addressmask;
}

struct pg_table_t *pg_copy_user_page_directory(pg_table_t *src)
{
	int i;

	uint32_t flags = get_flags();
	asm volatile("cli");

	struct pg_table_t *user = 
		(struct pg_table_t *) kalloc(sizeof(struct pg_table_t));

	// Allocate memory for page directory and pointers to page tables
	user->directory = (uint32_t *) kalloc_page();
	user->tables    = (uint32_t **) kalloc_page();

	user->physical = pg_get_physical(pg_active, (ptr_t) user->directory);

	/** Copy entire page directory **/
	for(i = 0x000; i < 0x400; i++) {
		// Treat user-space differently

		if( ( i < 0x300 ) && ((src->directory[i] & pde_usertable) == pde_usertable) && ((src->directory[i] & pde_present) == pde_present)) {
			src->directory[i] = src->directory[i] & 0xFFFFFFFD;
			src->directory[i] |= pde_copyonwrite;
		}

		user->directory[i] 	= src->directory[i];
		user->tables[i] 	= src->tables[i];
	}

    set_flags(flags);

	return user;
}

void pg_free_user_page_directory(struct pg_table_t *table)
{
  // FIXME: Free the directory / tables / all used memory

	/*for(uint32_t i; i < 0x400; i++) {
		for(uint32_t j; j < 0x400; j++) {
		}
	}*/

	kfree_page(table->tables);
	kfree_page(table->directory);
  
	kfree(table);
}

struct pg_table_t *pg_create_user_page_directory()
{
	int i;

	uint32_t flags = get_flags();
	asm volatile("cli");

	struct pg_table_t *user = 
		(struct pg_table_t *) kalloc(sizeof(struct pg_table_t));

	// Allocate memory for page directory and pointers to page tables
	user->directory = (uint32_t *) kalloc_page();
	user->tables    = (uint32_t **) kalloc_page();

	user->physical = pg_get_physical(pg_active, (ptr_t) user->directory);

	/** Initialize lower gigabyte **/
	for(i = 0x000; i < 0x300; i++) {
		user->directory[i] 	= 0x00;
		user->tables[i] 	= NULL;
	}

	/** Copy upper gigabyte from current table **/
	for(i = 0x300; i < 0x400; i++) {
		user->directory[i] = pg_kernel->directory[i];
		user->tables[i]    = pg_kernel->tables[i];
	}

    set_flags(flags);

	return user;
}

/**
 * Setup new pagetable
 */
void pg_setup_pagetable(struct pg_table_t *table, uint32_t virt, int supervisor)
{
	int i;

	if(table->tables[ pg_pde_index(virt) ] == NULL) {
		table->tables[ pg_pde_index(virt) ] = (uint32_t *) kalloc_page();
		table->directory[ pg_pde_index(virt) ] = pde_present | pde_writable | (supervisor?0:pde_usertable) |
			(pg_get_physical(pg_active, (ptr_t) table->tables[ pg_pde_index(virt) ]) & pde_addressmask);
	}

	pg_invalidate_tlb_entry((ptr_t) table->tables[ pg_pde_index(virt) ]);

	// Clear page table for use
	for(i = 0; i < 1024; i++) {
		table->tables[ pg_pde_index(virt) ][i] = 0;
	}

	pg_invalidate_tlb_entry(virt);
}

/**
 * Setup new pagetable
 */
void pg_setup_pagetable(struct pg_table_t *table, uint32_t virt, uint32_t virt_tbl, uint32_t phys_tbl, int supervisor)
{
	int i;

	if(table->tables[ pg_pde_index(virt) ] == NULL) {
		table->tables[ pg_pde_index(virt) ] = (uint32_t *) virt_tbl;
		table->directory[ pg_pde_index(virt) ] = pde_present | pde_writable | (supervisor?0:pde_usertable) |
			(phys_tbl & pde_addressmask);
	}

	pg_invalidate_tlb_entry(virt_tbl);

	// Clear page table for use
	for(i = 0; i < 1024; i++) {
		table->tables[ pg_pde_index(virt) ][i] = 0;
	}

	pg_invalidate_tlb_entry(virt);
}

/**
 * Remove pagetable
 */
uint32_t pg_remove_pagetable(struct pg_table_t *table, uint32_t virt)
{
	uint32_t addr;

	uint32_t flags = get_flags();
	asm volatile("cli");

	addr = (table->directory[ pg_pde_index(virt) ] & pde_addressmask);

	table->directory[ pg_pde_index(virt) ] = 0;
	table->tables[ pg_pde_index(virt) ] = NULL;

	pg_invalidate_tlb_entry(virt);

	set_flags(flags);

	return addr;
}

/**
 * Maps physical page 'physical' to virtual page 'virtual'.
 * This function creates a new page table if nessecary.
 */
void pg_map(struct pg_table_t *table, uint32_t virt, uint32_t physical, int supervisor)
{
	uint32_t flags = get_flags();
	asm volatile("cli");

	// Check whether the page table exists in memory
	if((table->directory[ pg_pde_index(virt) ] & pde_addressmask) == 0) {
		//kc_print_string("Setting up page table\n");
 		pg_setup_pagetable(table, virt, supervisor);
	}

	pg_invalidate_tlb_entry( (ptr_t) table->tables );
	pg_invalidate_tlb_entry( (ptr_t) table->tables[ pg_pde_index(virt) ] );

	// Modify the actual page table...
	table->tables[ pg_pde_index(virt) ][ pg_pte_index(virt) ] = 
		(physical & pte_addressmask) | (supervisor?0:pte_userpage) | pte_writable | pte_present;

	pg_invalidate_tlb_entry(virt);

	set_flags(flags);
}

/**
 * Removes the virtual page from the page table. 
 * FIXME: Memory is NOT freed! Should probably return it the
 * kernel allocator (and use kalloc to get memory for paging)
 */
void pg_unmap(struct pg_table_t *table, ptr_t virt)
{
	uint32_t flags = get_flags();
	asm volatile("cli");

	if((table->directory[ pg_pde_index(virt) ] & pde_addressmask) == 0)
		return;

	table->tables[ pg_pde_index(virt) ][ pg_pte_index(virt) ] = 0;
	pg_invalidate_tlb_entry(virt);

 	set_flags(flags);
}

/**
 * Setup heap without depending too much on pg_* functionality
 */
void setup_heap(uint32_t* heap_table)
{
	uint32_t allocated_page = mm_page_allocate();

	pg_kernel->directory[ pg_pde_index(0xD0000000) ] = 
		(allocated_page & pde_addressmask) | pde_writable | pde_present | pde_usertable;

	pg_kernel->tables[ pg_pde_index((ptr_t) heap_table) ][ pg_pte_index((ptr_t) heap_table) ] = 
		(allocated_page & pte_addressmask) | pte_writable | pte_present;

	pg_kernel->tables[ pg_pde_index(0xD0000000) ] = heap_table;

	pg_invalidate_tlb_entry((ptr_t) pg_kernel->tables[ pg_pde_index(0xD0000000) ]);

	allocated_page = mm_page_allocate();

	pg_kernel->tables[ pg_pde_index(0xD0000000) ][ pg_pte_index(0xD0000000) ] = 
		(allocated_page & pte_addressmask) | pte_writable | pte_present;

	//pg_map(pg_kernel, kernel_heap_start + kernel_heap_alloc * 0x1000, allocated_page, 1);
}

void setup_paging(uint32_t *page_directory, uint32_t **page_tables) 
{
	pg_kernel_real.directory = page_directory;
	pg_kernel_real.tables    = page_tables;

	pg_kernel = &pg_kernel_real;

	pg_active = pg_kernel;

	// Copy physical address of kernel pagetable from cr3 now, otherwise we'll have to
	// determine which address the bootstrap script chose using more complex
	// calculations later on.
	asm volatile("mov %%cr3, %0" : "=q" (pg_kernel->physical));
}

int pg_resolve_fault(uint32_t virt)
{
	// First resolve page directory issue...
	uint32_t d = pg_active->directory[ pg_pde_index(virt) ];

	if((d & pde_present) == pde_present) {
		if((d & pde_writable) == 0x0 && ((d & pde_copyonwrite) == pde_copyonwrite) ) {
			// Search for other copies of this 
			int numCopies = 0;

			struct pr_process_t* proc;
			struct pr_process_t* last;

			for(proc = pr_root; proc != NULL; proc = proc->next_process) {
				uint32_t c = proc->page_table->directory[ pg_pde_index(virt) ];

				if((c & pde_present) == pde_present && proc != pr_current &&
					(c & pde_addressmask) == (d & pde_addressmask)) {
					numCopies++;
					last = proc;
				}
			}

			// Remove copy on write from the other page directory entry
			if(numCopies == 1) {
				uint32_t c = last->page_table->directory[ pg_pde_index(virt) ];
				if((c & pde_copyonwrite) == pde_copyonwrite)
					last->page_table->directory[ pg_pde_index(virt) ] =	
						(c & (0xFFFFFFFF - pde_copyonwrite)) | pde_writable;
			}

			// Copy page table and set copy-on-write on each PTE
			pg_active->tables[ pg_pde_index(virt) ] = NULL;
			pg_active->directory[ pg_pde_index(virt) ] = 0x0;

			pg_setup_pagetable(pg_active, virt, (d & pde_usertable) == 0x0);

			for(int i = 0; i < 1024; i++) {
				uint32_t c = last->page_table->tables[ pg_pde_index(virt) ][i];

				if((c & pte_writable) == pte_writable) {
					c = (c & (0xFFFFFFFF - pte_writable)) | pte_copyonwrite;
				}

				last->page_table->tables[ pg_pde_index(virt) ][i] = c;
				pg_active->tables[ pg_pde_index(virt) ][i] = c;
			}

			//kc_print_string("PDE OK!\n");
			//return true;
		}
	}

	// Then resolve page table issue
	d = pg_active->tables[ pg_pde_index(virt) ][ pg_pte_index(virt) ];

	if((d & pte_present) == pte_present) {
		if((d & pte_writable) == 0x0 && ((d & pte_copyonwrite) == pte_copyonwrite) ) {

			// Search for other copies of this 
			int numCopies = 0;

			struct pr_process_t* proc;
			struct pr_process_t* last;

			for(proc = pr_root; proc != NULL; proc = proc->next_process) {
				uint32_t c = proc->page_table->tables[ pg_pde_index(virt) ][ pg_pte_index(virt) ];

				if((c & pte_present) == pte_present && proc != pr_current &&
					(c & pte_addressmask) == (d & pte_addressmask)) {
					numCopies++;
					last = proc;
				}
			}

			// Remove copy on write from the other table
			if(numCopies == 1) {
				uint32_t c = last->page_table->tables[ pg_pde_index(virt) ][ pg_pte_index(virt) ];
				if((c & pte_copyonwrite) == pte_copyonwrite)
					last->page_table->tables[ pg_pde_index(virt) ][ pg_pte_index(virt) ] =
						(c & (0xFFFFFFFF - pte_copyonwrite)) | pte_writable;
			}

			// Copy page
			uint32_t physical = mm_page_allocate();

			// Temp mapping for copy
			pg_map(pg_active, 0xC03E0000, physical, 1);

			uint32_t *src = (uint32_t *) (virt & 0xFFFFF000);
			uint32_t *dst = (uint32_t *) 0xC03E0000;

			for(int i = 0; i < 1024; i++)
				dst[i] = src[i];

			pg_unmap(pg_active, 0xC03E0000);
			pg_unmap(pg_active, virt);

			// Remove copy on write flag
			pg_map(pg_active, virt, physical, (d & pte_userpage) == 0x0);

			return true;
		}
	}

	return false;
}

void pg_debug(uint32_t virt)
{
	kc_print_string("Debugging information for: ");
	kc_print_hex(virt);
	kc_print_string("\n");

	kc_print_string("\tDirectory(virt) = ");
	kc_print_hex( pg_active->directory[ pg_pde_index(virt) ] );
	kc_print_string("\n");
}

