#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "arch_watchdog.h"

வெளி முழு watchdog_enabled; /* if set to non-zero the watch dog is enabled */
வெளி unsigned watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

வகைவரை வெறுமை (* arch_watchdog_method_t)(மாறிலி unsigned);
வகைவரை முழு (* arch_watchdog_profile_init_t)(மாறிலி unsigned);

கட்டமைப்பு arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

வெளி கட்டமைப்பு arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
முழு arch_watchdog_init(வெறுமை);
வெறுமை arch_watchdog_stop(வெறுமை);

#ifdef __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
வெறுமை arch_watchdog_lockup(மாறிலி கட்டமைப்பு nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
வெறுமை nmi_watchdog_handler(கட்டமைப்பு nmi_frame * frame);
#endif

/*
 * start and stop profiling using the NMI watchdog
 */
முழு nmi_watchdog_start_profiling(மாறிலி unsigned freq);
வெறுமை nmi_watchdog_stop_profiling(வெறுமை);

#endif /* __WATCHDOG_H__ */
