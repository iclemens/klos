#ifndef __PAGING_H_
#define __PAGING_H_

uint32_t pg_get_physical(struct pg_table_t *table, uint32_t virt);

/*struct pte_t {
  unsigned int present  :1;
  unsigned int writable :1;
  unsigned int userpage :1;
  
  :2; // 2 bits reserved

  unsigned int accessed :1;
  unsigned int dirty    :1;

  :1;
};*/

enum pte_flags {
	pte_present     = 0x01,
	pte_writable    = 0x02,
	pte_userpage    = 0x04,	// User mode (0 = supervisor)

	pte_accessed    = 0x20,	// Has been accessed
	pte_dirty       = 0x40,	// Has been written to

	pte_copyonwrite = 0x100, // Copy-on-write
	pte_shared      = 0x200, // Page-table shared with other process

	pte_addressmask = 0xFFFFF000
};

enum pde_flags {
	pde_present 	= 0x01,
	pde_writable 	= 0x02,
	pde_usertable 	= 0x04,
	pde_writetrough = 0x08,	// ???
	pde_cached 		= 0x10, // Will be cached
	pde_accessed 	= 0x20,
	pde_size 		= 0x80, // 4KB or 4MB

	pde_copyonwrite = 0x200, // Copy-on-write
	pde_shared		= 0x100, // Page shared with outher process

	pde_addressmask = 0xFFFFF000
};

struct pg_table_t {
	uint32_t *directory;	// the actual directory
	uint32_t **tables;	// page containing pointers to the actual tables

	// Physical address, used when loading CR3
	uint32_t physical;
};

extern struct pg_table_t *pg_kernel;

void setup_paging(uint32_t *page_directory, uint32_t **page_tables);
void setup_heap(uint32_t *heap_table);

struct pg_table_t *pg_create_user_page_directory();
struct pg_table_t *pg_copy_user_page_directory(pg_table_t *src);
void pg_free_user_page_directory(struct pg_table_t *table);

void pg_activate_table(struct pg_table_t *table);

int pg_resolve_fault(uint32_t virt);

void pg_setup_pagetable(struct pg_table_t *table, uint32_t virt, int supervisor);
void pg_setup_pagetable(struct pg_table_t *table, uint32_t virt, uint32_t virt_tbl, uint32_t phys_tbl, int supervirsor);

void pg_map(struct pg_table_t *table, uint32_t virt, uint32_t physical, int supervisor);
uint32_t pg_remove_pagetable(struct pg_table_t *table, uint32_t virt);
void pg_unmap(struct pg_table_t *table, ptr_t virt);

#endif
