#ifndef __CLOCK_H__
#define __CLOCK_H__

#ضمن "kernel/kernel.h"
#ضمن "arch_clock.h"

صحيح boot_cpu_init_timer(unsigned freq);
صحيح app_cpu_init_timer(unsigned freq);

صحيح timer_int_handler(فراغ);

صحيح init_local_timer(unsigned freq);
/* stop the local timer ticking */
فراغ stop_local_timer(فراغ);
/* let the time tick again with the original settings after it was stopped */
فراغ restart_local_timer(فراغ);
صحيح register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
