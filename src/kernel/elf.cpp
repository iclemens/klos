#include "elf-internal.h"
#include "console.h"

void elf_dump(uint32_t *binary)
{
	kc_print_string("Dumping ELF:\n");
	
	struct elf_header *header;

	header = (struct elf_header *) binary;

	header->ident[15] = 0;

	kc_print_string((const char *)header->ident);
}
