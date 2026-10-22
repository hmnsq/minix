/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#अन्तर्भाव "watchdog.h"
#अन्तर्भाव "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
संरचना arch_watchdog *watchdog;
पूर्णाङ्क watchdog_enabled;

स्थिर शून्य lockup_check(संरचना nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	स्थिर unsigned no_ticks;
	स्थिर unsigned last_tick_गणनम् = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	जइ (serial_debug_active)
		प्रत्यागम;

	जइ (last_tick_गणनम् != watchdog_local_timer_ticks) {
		जइ (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_गणनम् = watchdog_local_timer_ticks;
		प्रत्यागम;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	जइ (++no_ticks < 10) {
		जइ (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		प्रत्यागम;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

शून्य nmi_watchdog_handler(संरचना nmi_frame * frame)
{
#जइ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	जइ (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	जइ (sprofiling)
		nmi_sprofile_handler(frame);
	
	जइ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#अण्णहा
	जइ (watchdog_enabled) {
		lockup_check(frame);
		जइ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

पूर्णाङ्क nmi_watchdog_start_profiling(ध्रुव unsigned freq)
{
	पूर्णाङ्क err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	जइ (!watchdog_enabled) {
		जइ (arch_watchdog_init())
			प्रत्यागम ENODEV;
	}

	जइ (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		प्रत्यागम ENODEV;
	}

	err = watchdog->profile_init(freq);
	जइ (err != OK)
		प्रत्यागम err;

	watchdog->resetval = watchdog->profile_resetval;

	प्रत्यागम OK;
}

शून्य nmi_watchdog_stop_profiling(शून्य)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	जइ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	जइ (!watchdog_enabled)
		arch_watchdog_stop();
}
