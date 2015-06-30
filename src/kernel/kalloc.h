#ifndef __KALLOC_H_
#define __KALLOC_H_

void *kalloc(uint32_t size);
void kfree(void *address);

void *kalloc_page();
void kfree_page(void *page);

int kbrk(); // INTERNAL METHOD!
void kh_get_memory_info(size_t &allocated, size_t &total);

#endif
