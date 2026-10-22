#ifndef __CLOCK_H__
#define __CLOCK_H__

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_clock.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା boot_cpu_init_timer(unsigned freq);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା app_cpu_init_timer(unsigned freq);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା timer_int_handler(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_local_timer(unsigned freq);
/* stop the local timer ticking */
ଶୂନ୍ୟ stop_local_timer(ଶୂନ୍ୟ);
/* let the time tick again with the original settings after it was stopped */
ଶୂନ୍ୟ restart_local_timer(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
