#include "config.h"

#ifndef __EXCEPTIONS_H_
#define __EXCEPTIONS_H_

void is_install_exception_handlers();
extern "C" void is_cexception(uint32_t exception_id, uint32_t error_code);

#endif