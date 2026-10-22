#ifndef __CLOCK_H__
#define __CLOCK_H__

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_clock.h"

പൂർണ്ണസംഖ്യ boot_cpu_init_timer(unsigned freq);
പൂർണ്ണസംഖ്യ app_cpu_init_timer(unsigned freq);

പൂർണ്ണസംഖ്യ timer_int_handler(ശൂന്യം);

പൂർണ്ണസംഖ്യ init_local_timer(unsigned freq);
/* stop the local timer ticking */
ശൂന്യം stop_local_timer(ശൂന്യം);
/* let the time tick again with the original settings after it was stopped */
ശൂന്യം restart_local_timer(ശൂന്യം);
പൂർണ്ണസംഖ്യ register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
