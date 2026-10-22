#اگر_تعریف_نشده __WATCHDOG_H__
#تعریف_کن __WATCHDOG_H__

#ضمن "kernel/kernel.h"
#ضمن "arch_watchdog.h"

خارجي صحيح watchdog_enabled; /* if set to non-zero the watch dog is enabled */
خارجي بیعلامت watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

تعريفنوع فراغ (* arch_watchdog_روش_t)(ثابت بیعلامت);
تعريفنوع صحيح (* arch_watchdog_profile_init_t)(ثابت بیعلامت);

بنية arch_watchdog {
	arch_watchdog_روش_t		init;	/* initial setup */
	arch_watchdog_روش_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

خارجي بنية arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
صحيح arch_watchdog_init(فراغ);
فراغ arch_watchdog_stop(فراغ);

#اگر_تعریف_شده __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
فراغ arch_watchdog_lockup(ثابت بنية nmi_قاب * قاب);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
فراغ nmi_watchdog_handler(بنية nmi_قاب * قاب);
#پایان_اگر

/*
 * start and stop profiling using the NMI watchdog
 */
صحيح nmi_watchdog_شروع_profiling(ثابت بیعلامت freq);
فراغ nmi_watchdog_stop_profiling(فراغ);

#پایان_اگر /* __WATCHDOG_H__ */
