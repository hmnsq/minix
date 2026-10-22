#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#અન્તર્ભાવ "../apic_asm.h"

પૂર્ણાંક init_8253A_timer(unsigned freq);
શૂન્ય stop_8253A_timer(શૂન્ય);
શૂન્ય arch_timer_int_handler(શૂન્ય);

#endif /* __CLOCK_X86_H__ */
