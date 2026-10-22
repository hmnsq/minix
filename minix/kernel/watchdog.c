/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#ضمن "watchdog.h"
#ضمن "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
بنية arch_watchdog *watchdog;
صحيح watchdog_enabled;

ساكن فراغ lockup_check(بنية nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	ساكن unsigned no_ticks;
	ساكن unsigned last_tick_کيراءن = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	إذا (serial_debug_active)
		أرجع;

	إذا (last_tick_کيراءن != watchdog_local_timer_ticks) {
		إذا (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_کيراءن = watchdog_local_timer_ticks;
		أرجع;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	إذا (++no_ticks < 10) {
		إذا (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		أرجع;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

فراغ nmi_watchdog_handler(بنية nmi_frame * frame)
{
#إذا SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	إذا (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	إذا (sprofiling)
		nmi_sprofile_handler(frame);
	
	إذا ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#وإلا
	إذا (watchdog_enabled) {
		lockup_check(frame);
		إذا (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

صحيح nmi_watchdog_start_profiling(ثابت unsigned freq)
{
	صحيح err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	إذا (!watchdog_enabled) {
		إذا (arch_watchdog_init())
			أرجع ENODEV;
	}

	إذا (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		أرجع ENODEV;
	}

	err = watchdog->profile_init(freq);
	إذا (err != OK)
		أرجع err;

	watchdog->resetval = watchdog->profile_resetval;

	أرجع OK;
}

فراغ nmi_watchdog_stop_profiling(فراغ)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	إذا (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	إذا (!watchdog_enabled)
		arch_watchdog_stop();
}
