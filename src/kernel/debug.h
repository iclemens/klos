#ifndef __DEBUG_H_
#define __DEBUG_H_

void db_stack_trace(uint32_t ebp);
void db_dump_dregs();
void db_mm_info();

#endif