#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

void setup_interrupts();
void is_install_handler(int interrupt, void *service_routine, int dpl);
void is_install_hook( void(*hook)() );

extern "C" void is_c8259(void);
extern "C" void is_cstub(void);
extern "C" void is_cpanic(void);
extern "C" void is_cpic(int pic);
extern "C" void is_ctimer(void);
extern "C" void is_ckeyboard(void);
extern "C" void is_capi(unsigned int a, unsigned int b, unsigned int c, unsigned int d);

#endif
