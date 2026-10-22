#اگر_تعریف_نشده __CLOCK_H__
#تعریف_کن __CLOCK_H__

#ضمن "kernel/kernel.h"
#ضمن "arch_clock.h"

صحيح boot_cpu_init_timer(بیعلامت freq);
صحيح app_cpu_init_timer(بیعلامت freq);

صحيح timer_int_handler(فراغ);

صحيح init_local_timer(بیعلامت freq);
/* stop the local timer ticking */
فراغ stop_local_timer(فراغ);
/* let the time tick again with the original settings after it was stopped */
فراغ restart_local_timer(فراغ);
صحيح ثبات_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_زمان(بیعلامت ms);
بیعلامت cpu_زمان_2_ms(u64_t cpu_زمان);

#پایان_اگر /* __CLOCK_H__ */
