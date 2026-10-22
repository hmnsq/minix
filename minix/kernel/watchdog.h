#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_watchdog.h"

ബാഹ്യ പൂർണ്ണസംഖ്യ watchdog_enabled; /* if set to non-zero the watch dog is enabled */
ബാഹ്യ unsigned watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

തരനിർവചനം ശൂന്യം (* arch_watchdog_method_t)(സ്ഥിരം unsigned);
തരനിർവചനം പൂർണ്ണസംഖ്യ (* arch_watchdog_profile_init_t)(സ്ഥിരം unsigned);

ഘടന arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

ബാഹ്യ ഘടന arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
പൂർണ്ണസംഖ്യ arch_watchdog_init(ശൂന്യം);
ശൂന്യം arch_watchdog_stop(ശൂന്യം);

#ifdef __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
ശൂന്യം arch_watchdog_lockup(സ്ഥിരം ഘടന nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
ശൂന്യം nmi_watchdog_handler(ഘടന nmi_frame * frame);
#endif

/*
 * start and stop profiling using the NMI watchdog
 */
പൂർണ്ണസംഖ്യ nmi_watchdog_start_profiling(സ്ഥിരം unsigned freq);
ശൂന്യം nmi_watchdog_stop_profiling(ശൂന്യം);

#endif /* __WATCHDOG_H__ */
