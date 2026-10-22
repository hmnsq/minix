#ifndef __CLOCK_H__
#define __CLOCK_H__

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_clock.h"

પૂર્ણાંક boot_cpu_init_timer(unsigned freq);
પૂર્ણાંક app_cpu_init_timer(unsigned freq);

પૂર્ણાંક timer_int_handler(શૂન્ય);

પૂર્ણાંક init_local_timer(unsigned freq);
/* stop the local timer ticking */
શૂન્ય stop_local_timer(શૂન્ય);
/* let the time tick again with the original settings after it was stopped */
શૂન્ય restart_local_timer(શૂન્ય);
પૂર્ણાંક register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
