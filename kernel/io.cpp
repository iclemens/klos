#include "config.h"
#include "io.h"

 void outb(unsigned int value, unsigned int port)
{
	asm volatile("outb %b0, %w1" : : "a"(value), "d"(port));
}

 unsigned char inb(unsigned int port)
{
	unsigned char tmp;
	asm volatile("inb %w1, %b0" : "=a"(tmp) : "d"(port));
	return tmp;
}

 uint16_t inw(unsigned int port)
{
	uint16_t tmp;
	asm volatile("inw %w1, %w0" : "=a"(tmp) : "d"(port));
	return tmp;
}

 void outl(uint32_t value, unsigned int port)
{
	asm volatile("outl %0, %w1" : : "a"(value), "d"(port));
}

 uint32_t inl(unsigned int port)
{
	uint32_t tmp;
	asm volatile("inl %w1, %0" : "=a"(tmp) : "d"(port));
	return tmp;
}
