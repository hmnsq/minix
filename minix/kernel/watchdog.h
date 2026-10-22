#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_watchdog.h"

બાહ્ય પૂર્ણાંક watchdog_enabled; /* if set to non-zero the watch dog is enabled */
બાહ્ય unsigned watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

પ્રકારવ્યાખ્યા શૂન્ય (* arch_watchdog_method_t)(અચલ unsigned);
પ્રકારવ્યાખ્યા પૂર્ણાંક (* arch_watchdog_profile_init_t)(અચલ unsigned);

સંરચના arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

બાહ્ય સંરચના arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
પૂર્ણાંક arch_watchdog_init(શૂન્ય);
શૂન્ય arch_watchdog_stop(શૂન્ય);

#ifdef __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
શૂન્ય arch_watchdog_lockup(અચલ સંરચના nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
શૂન્ય nmi_watchdog_handler(સંરચના nmi_frame * frame);
#endif

/*
 * start and stop profiling using the NMI watchdog
 */
પૂર્ણાંક nmi_watchdog_start_profiling(અચલ unsigned freq);
શૂન્ય nmi_watchdog_stop_profiling(શૂન્ય);

#endif /* __WATCHDOG_H__ */
