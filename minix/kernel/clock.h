#ifndef __CLOCK_H__
#define __CLOCK_H__

#شامل "kernel/kernel.h"
#شامل "arch_clock.h"

صحیح boot_cpu_init_timer(unsigned freq);
صحیح app_cpu_init_timer(unsigned freq);

صحیح timer_int_handler(تهی);

صحیح init_local_timer(unsigned freq);
/* stop the local timer ticking */
تهی stop_local_timer(تهی);
/* let the time tick again with the original settings after it was stopped */
تهی restart_local_timer(تهی);
صحیح register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
