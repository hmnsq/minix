#अगर_अघोषित __CLOCK_H__
#घोषणा __CLOCK_H__

#समावेश "kernel/kernel.h"
#समावेश "arch_clock.h"

पूर्णांक boot_cpu_init_timer(अचिन्हित freq);
पूर्णांक app_cpu_init_timer(अचिन्हित freq);

पूर्णांक timer_int_handler(व्योम);

पूर्णांक init_local_timer(अचिन्हित freq);
/* stop the local timer ticking */
व्योम stop_local_timer(व्योम);
/* let the time tick again with the original settings after it was stopped */
व्योम restart_local_timer(व्योम);
पूर्णांक register_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_time(अचिन्हित ms);
अचिन्हित cpu_time_2_ms(u64_t cpu_time);

#पूर्ण_अगर /* __CLOCK_H__ */
