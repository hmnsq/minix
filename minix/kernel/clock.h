#నిర్వచితం_కాకపోతే __CLOCK_H__
#నిర్వచించు __CLOCK_H__

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_clock.h"

పూర్ణాంకం boot_cpu_init_timer(గుర్తులేని freq);
పూర్ణాంకం app_cpu_init_timer(గుర్తులేని freq);

పూర్ణాంకం timer_int_handler(శూన్య);

పూర్ణాంకం init_local_timer(గుర్తులేని freq);
/* stop the local timer ticking */
శూన్య stop_local_timer(శూన్య);
/* let the time tick again with the original settings after it was stopped */
శూన్య restart_local_timer(శూన్య);
పూర్ణాంకం రిజిస్టరు_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_సమయం(గుర్తులేని ms);
గుర్తులేని cpu_సమయం_2_ms(u64_t cpu_సమయం);

#అయితే_ముగింపు /* __CLOCK_H__ */
