#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#ಅನ್ತರ್ಭಾವ "../apic_asm.h"

ಪೂರ್ಣಾಙ್ಕ init_8253A_timer(unsigned freq);
ಶೂನ್ಯ stop_8253A_timer(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_timer_int_handler(ಶೂನ್ಯ);

#endif /* __CLOCK_X86_H__ */
