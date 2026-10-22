/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#अन्तर्भाव "watchdog.h"
#अन्तर्भाव "arch/i386/glo.h"

अचिन्हित watchdog_local_timer_ticks = 0U;
संरचना arch_watchdog *watchdog;
पूर्णाङ्क watchdog_enabled;

स्थिर शून्य lockup_check(संरचना nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	स्थिर अचिन्हित no_ticks;
	स्थिर अचिन्हित last_tick_गन्ती = (अचिन्हित) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	यदि (serial_debug_active)
		फर्काऊ;

	यदि (last_tick_गन्ती != watchdog_local_timer_ticks) {
		यदि (no_ticks == 1) {
			म_लिखो("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_गन्ती = watchdog_local_timer_ticks;
		फर्काऊ;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	यदि (++no_ticks < 10) {
		यदि (no_ticks == 1)
			म_लिखो("WARNING watchdog : possible kernel lockup\n");
		फर्काऊ;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

शून्य nmi_watchdog_handler(संरचना nmi_frame * frame)
{
#यदि SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	यदि (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	यदि (sprofiling)
		nmi_sprofile_handler(frame);
	
	यदि ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#अन्यथा
	यदि (watchdog_enabled) {
		lockup_check(frame);
		यदि (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

पूर्णाङ्क nmi_watchdog_start_profiling(अचर अचिन्हित freq)
{
	पूर्णाङ्क err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	यदि (!watchdog_enabled) {
		यदि (arch_watchdog_init())
			फर्काऊ ENODEV;
	}

	यदि (!watchdog->profile_init) {
		म_लिखो("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		फर्काऊ ENODEV;
	}

	err = watchdog->profile_init(freq);
	यदि (err != OK)
		फर्काऊ err;

	watchdog->resetval = watchdog->profile_resetval;

	फर्काऊ OK;
}

शून्य nmi_watchdog_stop_profiling(शून्य)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	यदि (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	यदि (!watchdog_enabled)
		arch_watchdog_stop();
}
