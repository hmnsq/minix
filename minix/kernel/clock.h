#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "kernel/kernel.h"
#include "arch_clock.h"

පූර්ණ_සංඛ්‍යා boot_cpu_init_timer(unsigned freq);
පූර්ණ_සංඛ්‍යා app_cpu_init_timer(unsigned freq);

පූර්ණ_සංඛ්‍යා timer_int_handler(void);

පූර්ණ_සංඛ්‍යා init_local_timer(unsigned freq);
/* stop the local timer ticking */
void stop_local_timer(void);
/* let the time tick again with the original settings after it was stopped */
void restart_local_timer(void);
පූර්ණ_සංඛ්‍යා register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
