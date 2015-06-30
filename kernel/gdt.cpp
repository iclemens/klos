#include "config.h"
#include "gdt.h"
#include "gdt-internal.h"

extern "C" {
	extern void gdt_reload_asm();
}

/**
 * Resets a GDT entry
 */
void gd_clear(uint32_t index)
{
	gdt[index].base_low = 0;
	gdt[index].base_mid = 0;
	gdt[index].base_high = 0;
	gdt[index].limit_low = 0;
	gdt[index].flags1 = 0;
	gdt[index].flags2 = 0;
}

/**
 * Sets the base address of a descriptor entry
 */
void gd_set_base(uint32_t index, uint32_t base)
{
	gdt[index].base_low = (base & 0xFFFF);
	gdt[index].base_mid = ((base >> 16) & 0xFF);
	gdt[index].base_high = ((base >> 24) & 0xFF);
}

/**
 * Sets the limit of a descriptor entry
 */
void gd_set_limit(uint32_t index, uint32_t limit)
{
	gdt[index].limit_low = (limit & 0xFFFF);
	gdt[index].flags2 = (gdt[index].flags2 & 0xF0) | ((limit >> 16) & 0x0F);
}

/**
 * Loads a task state segment into the GDT
 */
void gd_set_tss(uint32_t index, int gran, int avl, int present, int dpl, int busy)
{
	gdt[index].flags2 = (gdt[index].flags2 & 0x0F);
	gdt[index].flags2 |= ((gran & 0x01) << 7);
	gdt[index].flags2 |= ((avl & 0x01) << 4);

	gdt[index].flags1 = ((present & 0x01) << 7) | ((dpl & 0x03) << 5)
							| 0x08 | ((busy & 0x01) << 1) | 0x01;
}

/**
 * Loads a segment descriptor into the GDT
 */
void gd_set_segment(uint32_t index, int gran, int avl, int present, int dpl, int dtype, int type)
{
	gdt[index].flags2 = (gdt[index].flags2 & 0x0F);

	gdt[index].flags2 |= ((gran & 0x01) << 7);
	gdt[index].flags2 |= 0x01 << 6;
	gdt[index].flags2 |= ((avl & 0x01) << 4);

	gdt[index].flags1 = ((present & 0x01) << 7) | ((dpl & 0x03) << 5) | ((dtype & 0x01) << 4) | (type & 0x0F);
}

/**
 * Reloads the kernel GDT, must be called after changing the
 * "gdt" datastructure (a call to to gd_set*).
 */
void gd_reload()
{
	gdtp.offset = (unsigned int) gdt;
	gdtp.limit = (sizeof(struct gdt_entry) * gd_nentries) - 1;

	gdt_reload_asm();
}

/**
 * Adds a GDT entry (if possible) and returns a reference 
 */
uint32_t gd_add_entry()
{
	uint32_t entry = gd_nentries;
	gd_nentries++;

	gd_clear(entry);

	return entry;
}

/**
 * Replaces the current (bootloader-supplied) GDT with one
 * more to our liking. In addition we now control where
 * the actual GDT is located in memory.
 *
 * 0x00: Zeros
 * 0x08: Kernel Code
 * 0x10: Kernel Data
 * 0x18: User Code
 * 0x20: User Data
 */
void setup_gdt()
{
	gd_nentries = 0;

	/* Create entry 0 */
	gd_add_entry();

	/* Create entry 1 */
	gd_add_entry();
	gd_set_base(1, 0x00000000);
	gd_set_limit(1, 0xFFFFFFFF);

	gd_set_segment(1, 
		1, // Granularity
		0, // AVL
		1, // Present
		0, // Ring
		1, // DType
		10); // Type

	/* Create entry 2 */
	gd_add_entry();
	gd_set_base(2, 0x00000000);
	gd_set_limit(2, 0xFFFFFFFF);

	gd_set_segment(2,
		1, // Granularity
		0, // AVL
		1, // Present
		0, // Ring
		1, // DType
		2); // Type

	/* Create entry 3 */
	gd_add_entry();
	gd_set_base(3, 0x00000000);
	gd_set_limit(3, 0xFFFFFFFF);

	gd_set_segment(3, 
		1, // Granularity
		0, // AVL
		1, // Present
		3, // Ring		// SHOULD BE 3
		1, // DType
		10); // Type		// 10?

	/* Create entry 4 */
	gd_add_entry();
	gd_set_base(4, 0x00000000);
	gd_set_limit(4, 0xFFFFFFFF);

	gd_set_segment(4,
		1, // Granularity
		0, // AVL
		1, // Present
		3, // Ring		// SHOULD BE 3
		1, // DType
		2); // Type

	gd_reload();
}
