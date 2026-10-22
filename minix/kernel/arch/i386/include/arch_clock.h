#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#شامل "../apic_asm.h"

صحیح init_8253A_timer(unsigned freq);
تهی stop_8253A_timer(تهی);
تهی arch_timer_int_handler(تهی);

#endif /* __CLOCK_X86_H__ */
