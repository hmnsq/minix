#ifndef __CLOCK_H__
#define __CLOCK_H__

#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "arch_clock.h"

পূর্ণসংখ্যা boot_cpu_init_timer(unsigned freq);
পূর্ণসংখ্যা app_cpu_init_timer(unsigned freq);

পূর্ণসংখ্যা timer_int_handler(শূন্য);

পূর্ণসংখ্যা init_local_timer(unsigned freq);
/* stop the local timer ticking */
শূন্য stop_local_timer(শূন্য);
/* let the time tick again with the original settings after it was stopped */
শূন্য restart_local_timer(শূন্য);
পূর্ণসংখ্যা register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
