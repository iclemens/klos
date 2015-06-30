#include "config.h"

#ifndef __ELF_INTERNAL_H_
#define __ELF_INTERNAL_H_

struct elf_header {
	unsigned char ident[16];
	
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t program_offset;
	uint32_t section_offset;
	uint32_t flags;

	uint16_t program_size;
	uint16_t program_count;

	uint16_t section_size;
	uint16_t section_count;

	uint16_t section_table_idx;
};

#endif