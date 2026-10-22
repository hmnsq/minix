#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#شامل "kernel/kernel.h"
#شامل "arch_watchdog.h"

بیرونی صحیح watchdog_enabled; /* if set to non-zero the watch dog is enabled */
بیرونی unsigned watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

تعریفنوع تهی (* arch_watchdog_method_t)(ثابت unsigned);
تعریفنوع صحیح (* arch_watchdog_profile_init_t)(ثابت unsigned);

ساختار arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

بیرونی ساختار arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
صحیح arch_watchdog_init(تهی);
تهی arch_watchdog_stop(تهی);

#ifdef __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
تهی arch_watchdog_lockup(ثابت ساختار nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
تهی nmi_watchdog_handler(ساختار nmi_frame * frame);
#endif

/*
 * start and stop profiling using the NMI watchdog
 */
صحیح nmi_watchdog_start_profiling(ثابت unsigned freq);
تهی nmi_watchdog_stop_profiling(تهی);

#endif /* __WATCHDOG_H__ */
