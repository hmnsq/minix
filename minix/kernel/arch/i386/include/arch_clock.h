#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#അന്തര്ഭാവ "../apic_asm.h"

പൂർണ്ണസംഖ്യ init_8253A_timer(unsigned freq);
ശൂന്യം stop_8253A_timer(ശൂന്യം);
ശൂന്യം arch_timer_int_handler(ശൂന്യം);

#endif /* __CLOCK_X86_H__ */
