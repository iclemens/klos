#ifndef __IO_H_
#define __IO_H_

#include "config.h"

 void outb(unsigned int value, unsigned int port);
 void outl(uint32_t, unsigned int port);

 unsigned char inb(unsigned int port);
 uint16_t inw(unsigned int port);
 uint32_t inl(unsigned int port);

//#include "io.cpp"

#endif
