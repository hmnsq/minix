#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#ضمن "../apic_asm.h"

صحيح init_8253A_timer(unsigned freq);
فراغ stop_8253A_timer(فراغ);
فراغ arch_timer_int_handler(فراغ);

#endif /* __CLOCK_X86_H__ */
