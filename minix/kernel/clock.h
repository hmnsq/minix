#ifndef __CLOCK_H__
#define __CLOCK_H__

#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "arch_clock.h"

முழு boot_cpu_init_timer(unsigned freq);
முழு app_cpu_init_timer(unsigned freq);

முழு timer_int_handler(வெறுமை);

முழு init_local_timer(unsigned freq);
/* stop the local timer ticking */
வெறுமை stop_local_timer(வெறுமை);
/* let the time tick again with the original settings after it was stopped */
வெறுமை restart_local_timer(வெறுமை);
முழு register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
