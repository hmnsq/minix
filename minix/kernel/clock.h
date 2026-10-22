#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __CLOCK_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __CLOCK_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_clock.h"

ਪੂਰਨ boot_cpu_init_timer(ਬਿਨਾਨਿਸ਼ਾਨੀ freq);
ਪੂਰਨ app_cpu_init_timer(ਬਿਨਾਨਿਸ਼ਾਨੀ freq);

ਪੂਰਨ timer_int_handler(ਖਾਲੀ);

ਪੂਰਨ init_local_timer(ਬਿਨਾਨਿਸ਼ਾਨੀ freq);
/* stop the local timer ticking */
ਖਾਲੀ stop_local_timer(ਖਾਲੀ);
/* let the time tick again with the original settings after it was stopped */
ਖਾਲੀ restart_local_timer(ਖਾਲੀ);
ਪੂਰਨ ਰਜਿਸਟਰ_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_ਸਮਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ ms);
ਬਿਨਾਨਿਸ਼ਾਨੀ cpu_ਸਮਾਂ_2_ms(u64_t cpu_ਸਮਾਂ);

#ਜੇ_ਅੰਤ /* __CLOCK_H__ */
