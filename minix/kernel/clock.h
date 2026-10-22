#جے_بنا_تعریف __CLOCK_H__
#تعریف_کرو __CLOCK_H__

#ضمن "kernel/kernel.h"
#ضمن "arch_clock.h"

صحيح boot_cpu_init_timer(بنانشانی freq);
صحيح app_cpu_init_timer(بنانشانی freq);

صحيح timer_int_handler(فراغ);

صحيح init_local_timer(بنانشانی freq);
/* stop the local timer ticking */
فراغ stop_local_timer(فراغ);
/* let the time tick again with the original settings after it was stopped */
فراغ restart_local_timer(فراغ);
صحيح رجسٹر_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_ویلا(بنانشانی ms);
بنانشانی cpu_ویلا_2_ms(u64_t cpu_ویلا);

#جے_انت /* __CLOCK_H__ */
