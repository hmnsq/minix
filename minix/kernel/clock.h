#ifndef __CLOCK_H__
#define __CLOCK_H__

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_clock.h"

पूर्णांक boot_cpu_init_timer(अचिन्हित freq);
पूर्णांक app_cpu_init_timer(अचिन्हित freq);

पूर्णांक timer_int_handler(शून्य);

पूर्णांक init_local_timer(अचिन्हित freq);
/* stop the local timer ticking */
शून्य stop_local_timer(शून्य);
/* let the time tick again with the original settings after it was stopped */
शून्य restart_local_timer(शून्य);
पूर्णांक register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(अचिन्हित ms);
अचिन्हित cpu_time_2_ms(u64_t cpu_time);

#endif /* __CLOCK_H__ */
