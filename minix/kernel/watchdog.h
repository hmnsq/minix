#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#অন্তর্ভাৱ "kernel/kernel.h"
#অন্তর্ভাৱ "arch_watchdog.h"

বাহ্য পূর্ণাঙ্ক watchdog_enabled; /* if set to non-zero the watch dog is enabled */
বাহ্য unsigned watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

প্রকারনির্দেশ শূন্য (* arch_watchdog_method_t)(ধ্রুৱ unsigned);
প্রকারনির্দেশ পূর্ণাঙ্ক (* arch_watchdog_profile_init_t)(ধ্রুৱ unsigned);

সংরচনা arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

বাহ্য সংরচনা arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
পূর্ণাঙ্ক arch_watchdog_init(শূন্য);
শূন্য arch_watchdog_stop(শূন্য);

#ifdef __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
শূন্য arch_watchdog_lockup(ধ্রুৱ সংরচনা nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
শূন্য nmi_watchdog_handler(সংরচনা nmi_frame * frame);
#endif

/*
 * start and stop profiling using the NMI watchdog
 */
পূর্ণাঙ্ক nmi_watchdog_start_profiling(ধ্রুৱ unsigned freq);
শূন্য nmi_watchdog_stop_profiling(শূন্য);

#endif /* __WATCHDOG_H__ */
