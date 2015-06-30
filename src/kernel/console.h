#ifndef __CONSOLE_H_
#define __CONSOLE_H_

#include "config.h"

void setup_console();
void kc_set_text_base(uint32_t base);

void kc_clear();

void kc_write_character(char c);
void kc_print_string(const char *string);

void kc_print_hex(unsigned int nr);
void kc_printf(char *fmt, ...);

#endif
