#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_watchdog.h"

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ watchdog_enabled; /* if set to non-zero the watch dog is enabled */
ಬಾಹ್ಯ unsigned watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (* arch_watchdog_method_t)(ಧ್ರುವ unsigned);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (* arch_watchdog_profile_init_t)(ಧ್ರುವ unsigned);

ಸಂರಚನಾ arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

ಬಾಹ್ಯ ಸಂರಚನಾ arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
ಪೂರ್ಣಾಙ್ಕ arch_watchdog_init(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_watchdog_stop(ಶೂನ್ಯ);

#ifdef __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
ಶೂನ್ಯ arch_watchdog_lockup(ಧ್ರುವ ಸಂರಚನಾ nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
ಶೂನ್ಯ nmi_watchdog_handler(ಸಂರಚನಾ nmi_frame * frame);
#endif

/*
 * start and stop profiling using the NMI watchdog
 */
ಪೂರ್ಣಾಙ್ಕ nmi_watchdog_start_profiling(ಧ್ರುವ unsigned freq);
ಶೂನ್ಯ nmi_watchdog_stop_profiling(ಶೂನ್ಯ);

#endif /* __WATCHDOG_H__ */
