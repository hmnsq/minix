#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __WATCHDOG_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __WATCHDOG_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_watchdog.h"

ਬਾਹਰੀ ਪੂਰਨ watchdog_enabled; /* if set to non-zero the watch dog is enabled */
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (* arch_watchdog_ਵਿਧੀ_t)(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (* arch_watchdog_profile_init_t)(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ);

ਬਣਤਰ arch_watchdog {
	arch_watchdog_ਵਿਧੀ_t		init;	/* initial setup */
	arch_watchdog_ਵਿਧੀ_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

ਬਾਹਰੀ ਬਣਤਰ arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
ਪੂਰਨ arch_watchdog_init(ਖਾਲੀ);
ਖਾਲੀ arch_watchdog_stop(ਖਾਲੀ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
ਖਾਲੀ arch_watchdog_lockup(ਅਚਲ ਬਣਤਰ nmi_ਚੌਖਟਾ * ਚੌਖਟਾ);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
ਖਾਲੀ nmi_watchdog_handler(ਬਣਤਰ nmi_ਚੌਖਟਾ * ਚੌਖਟਾ);
#ਜੇ_ਅੰਤ

/*
 * start and stop profiling using the NMI watchdog
 */
ਪੂਰਨ nmi_watchdog_ਸ਼ੁਰੂ_profiling(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ freq);
ਖਾਲੀ nmi_watchdog_stop_profiling(ਖਾਲੀ);

#ਜੇ_ਅੰਤ /* __WATCHDOG_H__ */
