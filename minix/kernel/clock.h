#اگر_غیر_تعریف_شدہ __CLOCK_H__
#تعریف_کرو __CLOCK_H__

#شامل "kernel/kernel.h"
#شامل "arch_clock.h"

صحیح boot_cpu_init_timer(غیرعلامتی freq);
صحیح app_cpu_init_timer(غیرعلامتی freq);

صحیح timer_int_handler(خالی);

صحیح init_local_timer(غیرعلامتی freq);
/* stop the local timer ticking */
خالی stop_local_timer(خالی);
/* let the time tick again with the original settings after it was stopped */
خالی restart_local_timer(خالی);
صحیح رجسٹر_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_وقت(غیرعلامتی ms);
غیرعلامتی cpu_وقت_2_ms(u64_t cpu_وقت);

#اگر_ختم /* __CLOCK_H__ */
