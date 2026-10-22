#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#include "../apic_asm.h"

puur_naa_nka init_8253A_timer(unsigned freq);
void stop_8253A_timer(void);
void arch_timer_int_handler(void);

#endif /* __CLOCK_X86_H__ */
