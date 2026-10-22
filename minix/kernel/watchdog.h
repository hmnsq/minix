#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __WATCHDOG_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __WATCHDOG_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_watchdog.h"

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ watchdog_enabled; /* if set to non-zero the watch dog is enabled */
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (* arch_watchdog_ವಿಧಾನ_t)(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (* arch_watchdog_profile_init_t)(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ);

ರಚನೆ arch_watchdog {
	arch_watchdog_ವಿಧಾನ_t		init;	/* initial setup */
	arch_watchdog_ವಿಧಾನ_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

ಬಾಹ್ಯ ರಚನೆ arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
ಪೂರ್ಣಾಂಕ arch_watchdog_init(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_watchdog_stop(ಶೂನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
ಶೂನ್ಯ arch_watchdog_lockup(ಸ್ಥಿರಾಂಕ ರಚನೆ nmi_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
ಶೂನ್ಯ nmi_watchdog_handler(ರಚನೆ nmi_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು);
#ಆದರೆ_ಅಂತ್ಯ

/*
 * start and stop profiling using the NMI watchdog
 */
ಪೂರ್ಣಾಂಕ nmi_watchdog_ಆರಂಭ_profiling(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ freq);
ಶೂನ್ಯ nmi_watchdog_stop_profiling(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ /* __WATCHDOG_H__ */
