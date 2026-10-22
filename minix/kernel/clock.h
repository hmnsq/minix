#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "kernel/kernel.h"
#include "arch_clock.h"

ကိန်းပြည့် boot_cpu_init_timer(unsigned freq);
ကိန်းပြည့် app_cpu_init_timer(unsigned freq);

ကိန်းပြည့် timer_int_handler(void);

ကိန်းပြည့် init_local_timer(unsigned freq);
/* stop the local timer ticking */
void stop_local_timer(void);
/* let the time tick again with the original settings after it was stopped */
void restart_local_timer(void);
ကိန်းပြည့် register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
