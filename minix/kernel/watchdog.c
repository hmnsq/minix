/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#अन्तर्भाव "watchdog.h"
#अन्तर्भाव "arch/i386/glo.h"

अचिन्हित watchdog_local_timer_ticks = 0U;
संरचना arch_watchdog *watchdog;
पूर्णांक watchdog_enabled;

स्थिर शून्य lockup_check(संरचना nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	स्थिर अचिन्हित no_ticks;
	स्थिर अचिन्हित last_tick_मोजणी = (अचिन्हित) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	जर (serial_debug_active)
		परत;

	जर (last_tick_मोजणी != watchdog_local_timer_ticks) {
		जर (no_ticks == 1) {
			म_लिखो("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_मोजणी = watchdog_local_timer_ticks;
		परत;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	जर (++no_ticks < 10) {
		जर (no_ticks == 1)
			म_लिखो("WARNING watchdog : possible kernel lockup\n");
		परत;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

शून्य nmi_watchdog_handler(संरचना nmi_frame * frame)
{
#जर SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	जर (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	जर (sprofiling)
		nmi_sprofile_handler(frame);
	
	जर ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#अन्यथा
	जर (watchdog_enabled) {
		lockup_check(frame);
		जर (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

पूर्णांक nmi_watchdog_start_profiling(स्थिरांक अचिन्हित freq)
{
	पूर्णांक err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	जर (!watchdog_enabled) {
		जर (arch_watchdog_init())
			परत ENODEV;
	}

	जर (!watchdog->profile_init) {
		म_लिखो("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		परत ENODEV;
	}

	err = watchdog->profile_init(freq);
	जर (err != OK)
		परत err;

	watchdog->resetval = watchdog->profile_resetval;

	परत OK;
}

शून्य nmi_watchdog_stop_profiling(शून्य)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	जर (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	जर (!watchdog_enabled)
		arch_watchdog_stop();
}
