#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_watchdog.h"

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା watchdog_enabled; /* if set to non-zero the watch dog is enabled */
ବାହ୍ୟ unsigned watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (* arch_watchdog_method_t)(ଅଚଳ unsigned);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (* arch_watchdog_profile_init_t)(ଅଚଳ unsigned);

ସଂରଚନା arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

ବାହ୍ୟ ସଂରଚନା arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_watchdog_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ arch_watchdog_stop(ଶୂନ୍ୟ);

#ifdef __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
ଶୂନ୍ୟ arch_watchdog_lockup(ଅଚଳ ସଂରଚନା nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
ଶୂନ୍ୟ nmi_watchdog_handler(ସଂରଚନା nmi_frame * frame);
#endif

/*
 * start and stop profiling using the NMI watchdog
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା nmi_watchdog_start_profiling(ଅଚଳ unsigned freq);
ଶୂନ୍ୟ nmi_watchdog_stop_profiling(ଶୂନ୍ୟ);

#endif /* __WATCHDOG_H__ */
