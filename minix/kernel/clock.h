#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __CLOCK_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __CLOCK_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_clock.h"

ಪೂರ್ಣಾಂಕ boot_cpu_init_timer(ಚಿಹ್ನೆರಹಿತ freq);
ಪೂರ್ಣಾಂಕ app_cpu_init_timer(ಚಿಹ್ನೆರಹಿತ freq);

ಪೂರ್ಣಾಂಕ timer_int_handler(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ init_local_timer(ಚಿಹ್ನೆರಹಿತ freq);
/* stop the local timer ticking */
ಶೂನ್ಯ stop_local_timer(ಶೂನ್ಯ);
/* let the time tick again with the original settings after it was stopped */
ಶೂನ್ಯ restart_local_timer(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ ರಿಜಿಸ್ಟರ್_local_timer_handler(irq_handler_t handler);

u64_t ms_2_cpu_ಸಮಯ(ಚಿಹ್ನೆರಹಿತ ms);
ಚಿಹ್ನೆರಹಿತ cpu_ಸಮಯ_2_ms(u64_t cpu_ಸಮಯ);

#ಆದರೆ_ಅಂತ್ಯ /* __CLOCK_H__ */
