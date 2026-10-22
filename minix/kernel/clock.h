#که_نه_تعریف_شوی __CLOCK_H__
#تعریف_کړه __CLOCK_H__

#ضمن "kernel/kernel.h"
#ضمن "arch_clock.h"

صحيح boot_cpu_init_timer(بینښه freq);
صحيح app_cpu_init_timer(بینښه freq);

صحيح timer_int_handler(فراغ);

صحيح init_local_timer(بینښه freq);
/* stop the local timer ticking */
فراغ stop_local_timer(فراغ);
/* let the time tick again with the original settings after it was stopped */
فراغ restart_local_timer(فراغ);
صحيح ثبتوونکی_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_وخت(بینښه ms);
بینښه cpu_وخت_2_ms(u64_t cpu_وخت);

#د_که_پای /* __CLOCK_H__ */
