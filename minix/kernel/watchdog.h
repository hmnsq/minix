#اگر_غیر_تعریف_شدہ __WATCHDOG_H__
#تعریف_کرو __WATCHDOG_H__

#شامل "kernel/kernel.h"
#شامل "arch_watchdog.h"

خارجی صحیح watchdog_enabled; /* if set to non-zero the watch dog is enabled */
خارجی غیرعلامتی watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

قسمتعریف خالی (* arch_watchdog_طریقہ_t)(ثابت غیرعلامتی);
قسمتعریف صحیح (* arch_watchdog_profile_init_t)(ثابت غیرعلامتی);

ساخت arch_watchdog {
	arch_watchdog_طریقہ_t		init;	/* initial setup */
	arch_watchdog_طریقہ_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

خارجی ساخت arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
صحیح arch_watchdog_init(خالی);
خالی arch_watchdog_stop(خالی);

#اگر_تعریف_شدہ __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
خالی arch_watchdog_lockup(ثابت ساخت nmi_چوکھٹا * چوکھٹا);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
خالی nmi_watchdog_handler(ساخت nmi_چوکھٹا * چوکھٹا);
#اگر_ختم

/*
 * start and stop profiling using the NMI watchdog
 */
صحیح nmi_watchdog_آغاز_profiling(ثابت غیرعلامتی freq);
خالی nmi_watchdog_stop_profiling(خالی);

#اگر_ختم /* __WATCHDOG_H__ */
