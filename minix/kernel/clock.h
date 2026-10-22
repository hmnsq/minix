#ifndef __CLOCK_H__
#define __CLOCK_H__

#অন্তর্ভাৱ "kernel/kernel.h"
#অন্তর্ভাৱ "arch_clock.h"

পূর্ণাঙ্ক boot_cpu_init_timer(unsigned freq);
পূর্ণাঙ্ক app_cpu_init_timer(unsigned freq);

পূর্ণাঙ্ক timer_int_handler(শূন্য);

পূর্ণাঙ্ক init_local_timer(unsigned freq);
/* stop the local timer ticking */
শূন্য stop_local_timer(শূন্য);
/* let the time tick again with the original settings after it was stopped */
শূন্য restart_local_timer(শূন্য);
পূর্ণাঙ্ক register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
