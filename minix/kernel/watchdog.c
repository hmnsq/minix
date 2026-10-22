/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#অন্তর্ভাব "watchdog.h"
#অন্তর্ভাব "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
কাঠামো arch_watchdog *watchdog;
পূর্ণসংখ্যা watchdog_enabled;

স্থির শূন্য lockup_check(কাঠামো nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	স্থির unsigned no_ticks;
	স্থির unsigned last_tick_গুনতি = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	যদি (serial_debug_active)
		ফেরত;

	যদি (last_tick_গুনতি != watchdog_local_timer_ticks) {
		যদি (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_গুনতি = watchdog_local_timer_ticks;
		ফেরত;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	যদি (++no_ticks < 10) {
		যদি (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		ফেরত;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

শূন্য nmi_watchdog_handler(কাঠামো nmi_frame * frame)
{
#যদি SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	যদি (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	যদি (sprofiling)
		nmi_sprofile_handler(frame);
	
	যদি ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#অন্যথা
	যদি (watchdog_enabled) {
		lockup_check(frame);
		যদি (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

পূর্ণসংখ্যা nmi_watchdog_start_profiling(ধ্রুবক unsigned freq)
{
	পূর্ণসংখ্যা err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	যদি (!watchdog_enabled) {
		যদি (arch_watchdog_init())
			ফেরত ENODEV;
	}

	যদি (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ফেরত ENODEV;
	}

	err = watchdog->profile_init(freq);
	যদি (err != OK)
		ফেরত err;

	watchdog->resetval = watchdog->profile_resetval;

	ফেরত OK;
}

শূন্য nmi_watchdog_stop_profiling(শূন্য)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	যদি (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	যদি (!watchdog_enabled)
		arch_watchdog_stop();
}
