#ifndef __CLOCK_H__
#define __CLOCK_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_clock.h"

ಪೂರ್ಣಾಙ್ಕ boot_cpu_init_timer(unsigned freq);
ಪೂರ್ಣಾಙ್ಕ app_cpu_init_timer(unsigned freq);

ಪೂರ್ಣಾಙ್ಕ timer_int_handler(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ init_local_timer(unsigned freq);
/* stop the local timer ticking */
ಶೂನ್ಯ stop_local_timer(ಶೂನ್ಯ);
/* let the time tick again with the original settings after it was stopped */
ಶೂನ್ಯ restart_local_timer(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
