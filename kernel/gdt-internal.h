#ifndef __GDT_INTERNAL_H_
#define __GDT_INTERNAL_H_

#define GD_MAX_ENTRIES 6

struct gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t flags1; // Type (4), S, DPL (2), P
	uint8_t	flags2; // Limit (4), Avl, L, D/B, G
	uint8_t base_high;

} __attribute__((packed,aligned(4)));

struct gdt_pointer {
	uint16_t limit;
	uint32_t offset;
} __attribute__((packed,aligned(4)));

struct gdt_entry gdt[GD_MAX_ENTRIES];
struct gdt_pointer gdtp;

int gd_nentries;

#endif
