#include "symbol-internal.h"

const char *sm_resolve(uint32_t address)
{
	int i;
	int j;

	j = 0;

	for(i = 0; ; i++) {
		if(sm_table[i].address < address && sm_table[i].address > sm_table[j].address)
			j = i;

		if(sm_table[i].address == 0xFFFFFFFF)
			break;
	}

	return sm_table[j].name;
}
