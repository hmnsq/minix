#جيڪڏهن_وصف_نه_ٿيل __CLOCK_H__
#وصف_ڪريو __CLOCK_H__

#ضمن "kernel/kernel.h"
#ضمن "arch_clock.h"

صحيح boot_cpu_init_timer(بنانشان freq);
صحيح app_cpu_init_timer(بنانشان freq);

صحيح timer_int_handler(فراغ);

صحيح init_local_timer(بنانشان freq);
/* stop the local timer ticking */
فراغ stop_local_timer(فراغ);
/* let the time tick again with the original settings after it was stopped */
فراغ restart_local_timer(فراغ);
صحيح رجسٽر_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_وقت(بنانشان ms);
بنانشان cpu_وقت_2_ms(u64_t cpu_وقت);

#جيڪڏهن_پورو /* __CLOCK_H__ */
