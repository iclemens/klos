#include "config.h"
#include "kalloc.h"
#include "memory.h"
#include "paging.h"
#include "console.h"

/**
 * K+R Style Kernel Memory Allocator
 */

struct heap_block_hdr_t {
	struct heap_block_hdr_t *next_block;
	uint32_t block_size;		/* Note: size includes this header! */
};

// The heap starts at this address
uint32_t kernel_heap_start = 0xD0000000;

// The amount of pages allocated to the kernel heap
uint32_t kernel_heap_alloc = 0x0;

// Start of the free blocks list
struct heap_block_hdr_t kernel_heap_free_base;
struct heap_block_hdr_t *kernel_heap_free = NULL;

/**
 * Allocates another page of memory for use on the heap
 */
int kbrk()
{
	uint32_t allocated_page = mm_page_allocate();

	if(allocated_page == 0)
		return 0;

	pg_map(pg_kernel, kernel_heap_start + kernel_heap_alloc * 0x1000, allocated_page, 1);

	struct heap_block_hdr_t *block = (struct heap_block_hdr_t *) (kernel_heap_start + kernel_heap_alloc * 0x1000);
	block->block_size = 0x1000 / sizeof(struct heap_block_hdr_t);
	block->next_block = 0;

	kernel_heap_alloc++;

	kfree(block + 1);

	return 1;
}

/**
 * Allocates a region on the kernel's heap
 */
void *kalloc(uint32_t size)
{			
	//kc_printf("Alloc\n");
	struct heap_block_hdr_t *p, *prevp;
	unsigned nunits;

	nunits = (size + sizeof(struct heap_block_hdr_t) - 1) / sizeof(struct heap_block_hdr_t) + 1;

	if((prevp = kernel_heap_free) == NULL) {
		kernel_heap_free_base.block_size = 0;
		kernel_heap_free = prevp = &kernel_heap_free_base;
		kernel_heap_free_base.next_block = kernel_heap_free;
	}

	for(p = prevp->next_block; ; prevp = p, p = p->next_block)
	{
		if(p->block_size >= nunits) {

			if(p->block_size == nunits) {
				prevp->next_block = p->next_block;
			} else {
				// Not all units required, relocate header
				p->block_size -= nunits;
				p += p->block_size;
				p->block_size = nunits;
			}

			kernel_heap_free = prevp;
			//kc_printf("OK (%x)\n", (uint32_t) (p+1));
			return (void *) (p + 1);
		}

		if(p == kernel_heap_free)
			if(kbrk() == NULL)
				return NULL;
	}
}

/**
 * Marks a region in kernel memory as free
 */
void kfree(void *address)
{	
	struct heap_block_hdr_t *bp, *p;

	bp = (struct heap_block_hdr_t *) address - 1;
	for(p = kernel_heap_free; !(bp > p && bp < p->next_block); p = p->next_block)
		if(p >= p->next_block && (bp > p || bp < p->next_block))
			break;

	if(bp + bp->block_size == p->next_block) {
		bp->block_size += p->next_block->block_size;
		bp->next_block = p->next_block->next_block;
	} else
		bp->next_block = p->next_block;

	if(p + p->block_size == bp) {
		p->block_size += bp->block_size;
		p->next_block = bp->next_block;
	} else
		p->next_block = bp;

	kernel_heap_free = p;
}

/**
 * Allocates a page on the heap but does not insert it into the "free" list
 */
void *kalloc_page()
{
	ptr_t address, aligned_address;
	size_t alignment, overhead, nbytes;

	struct heap_block_hdr_t *p, *prevp, *r, *q;	
	
	alignment = 0x1000;	// Align to page boundary	
	
	if((prevp = kernel_heap_free) == NULL) {
		kernel_heap_free_base.block_size = 0;
		kernel_heap_free = prevp = &kernel_heap_free_base;
		kernel_heap_free_base.next_block = kernel_heap_free;
	}
	
	for(p = prevp->next_block; ; prevp = p, p = p->next_block)
	{
		address = (ptr_t) p;
		aligned_address = (address / alignment) * alignment;
		
		// Align to _next_ page boundary
		if(address > aligned_address)
			aligned_address += 0x1000;
		
		// Compute space wasted by alignment
		overhead = aligned_address - address;
		
		// Compute amount of space after point of alignment (zero if negative)
		if((p->block_size * sizeof(struct heap_block_hdr_t)) > overhead) {		
			nbytes = p->block_size * sizeof(struct heap_block_hdr_t) - overhead;
		} else {
			nbytes = 0;
		}
		
		if(nbytes >= 0x1000) {
			q = p->next_block;
			
			// If units before block: change amount of free units
			if(overhead > 0) {
				p->block_size = overhead / sizeof(struct heap_block_hdr_t);
			} else {
				p = prevp;
			}
		
			// If units after block: add another segment
			if(nbytes > 0x1000) {
				r = (struct heap_block_hdr_t*) (aligned_address + 0x1000);				
				r->block_size = (nbytes - 0x1000) / sizeof(struct heap_block_hdr_t);
				r->next_block = q;
				p->next_block = r;			
			} else {
				p->next_block = p->next_block->next_block;
			}
			
			return (void *) aligned_address;			
		}

		// Couldn't find page, allocate new block
		if(p == kernel_heap_free) {
			break;
		}
	}
	
	uint32_t allocated_page = mm_page_allocate();

	if(allocated_page == 0)
		return 0;

	pg_map(pg_kernel, kernel_heap_start + kernel_heap_alloc * 0x1000, allocated_page, 1);
	void *block = (void *) (kernel_heap_start + kernel_heap_alloc * 0x1000);
	kernel_heap_alloc++;

	return block;
}

/**
 * Add page to free-list
 */
void kfree_page(void *page)
{
	struct heap_block_hdr_t *block = (struct heap_block_hdr_t *) page;
	block->block_size = 0x1000 / sizeof(struct heap_block_hdr_t);
	block->next_block = 0;

	#ifdef ALLOC_DEBUG
	kc_printf("kfree_page(%x); size = %x\n", block, block->block_size * sizeof(struct heap_block_hdr_t));	
	#endif	
	
	kfree(block + 1);
}

/**
 * Dumps the list of free blocks on the heap
 */
void kh_print_block_list()
{
	struct heap_block_hdr_t *p, *prevp;

	if((prevp = kernel_heap_free) == NULL) {
		kernel_heap_free_base.block_size = 0;
		kernel_heap_free = prevp = &kernel_heap_free_base;
		kernel_heap_free_base.next_block = kernel_heap_free;
	}	
	
	for(p = prevp->next_block; ; prevp = p, p = p->next_block)
	{
		kc_printf("%x (%x)>", (uint32_t) p, p->block_size * sizeof(struct heap_block_hdr_t));
		
		if(p == kernel_heap_free)
			break;
	}
	
	kc_printf("\n");
}

/**
 * Computes the amount of free space on the heap
 */
void kh_get_memory_info(size_t &allocated, size_t &total)
{
	// Amount of heap space allocated
	struct heap_block_hdr_t *p, *prevp;

	if((prevp = kernel_heap_free) == NULL) {
		kernel_heap_free_base.block_size = 0;
		kernel_heap_free = prevp = &kernel_heap_free_base;
		kernel_heap_free_base.next_block = kernel_heap_free;
	}	

	allocated = 0;

	for(p = prevp->next_block; ; prevp = p, p = p->next_block)
	{
		allocated += p->block_size * sizeof(struct heap_block_hdr_t);
		
		if(p == kernel_heap_free)
			break;
	}
		
	// Amount of free bytes on the heap
	total = kernel_heap_alloc * 0x1000;
	
	allocated = total - allocated;	
}

