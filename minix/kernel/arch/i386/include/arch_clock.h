#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#অন্তর্ভাব "../apic_asm.h"

পূর্ণসংখ্যা init_8253A_timer(unsigned freq);
শূন্য stop_8253A_timer(শূন্য);
শূন্য arch_timer_int_handler(শূন্য);

#endif /* __CLOCK_X86_H__ */
