#include "config.h"

#ifndef __SYMBOL_INTERNAL_H_
#define __SYMBOL_INTERNAL_H_

struct sm_table_entry {
	uint32_t address;
	const char *name;
};

struct sm_table_entry sm_table[] = {
	{0x00000000, "NULL"}, 
	#include "symbol-table.h"
	{0xFFFFFFFF, "NULL"}
};

#endif
