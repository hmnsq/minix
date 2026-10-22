#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#அந்தர்பாவ "../apic_asm.h"

முழு init_8253A_timer(unsigned freq);
வெறுமை stop_8253A_timer(வெறுமை);
வெறுமை arch_timer_int_handler(வெறுமை);

#endif /* __CLOCK_X86_H__ */
