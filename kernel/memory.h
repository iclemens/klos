#ifndef __MEMORY_H_
#define __MEMORY_H_

void setup_memory_manager(uint32_t first_free_page, uint32_t memory_size);
uint32_t mm_page_allocate();
void mm_get_pages_free(uint32_t &lo, uint32_t &hi);

#endif
