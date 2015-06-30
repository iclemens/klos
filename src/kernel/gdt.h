#ifndef __GDT_H_
#define __GDT_H_

void setup_gdt();

uint32_t gd_add_entry();
void gd_clear(uint32_t index);
void gd_reload();

void gd_set_base(uint32_t index, uint32_t base);
void gd_set_limit(uint32_t index, uint32_t limit);
void gd_set_tss(uint32_t index, int gran, int avl, int present, int dpl, int busy);
void gd_set_segment(uint32_t index, int gran, int avl, int present, int dpl, int dtype, int type);

#endif
