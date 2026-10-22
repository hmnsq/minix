#ifndef __CLOCK_H__
#define __CLOCK_H__

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_clock.h"

पूर्णाङ्क boot_cpu_init_timer(unsigned freq);
पूर्णाङ्क app_cpu_init_timer(unsigned freq);

पूर्णाङ्क timer_int_handler(शून्य);

पूर्णाङ्क init_local_timer(unsigned freq);
/* stop the local timer ticking */
शून्य stop_local_timer(शून्य);
/* let the time tick again with the original settings after it was stopped */
शून्य restart_local_timer(शून्य);
पूर्णाङ्क register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
