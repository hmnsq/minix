/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#समावेश "watchdog.h"
#समावेश "arch/i386/glo.h"

अचिन्हित watchdog_local_timer_ticks = 0U;
काष्ठा arch_watchdog *watchdog;
पूर्णांक watchdog_enabled;

अटल व्योम lockup_check(काष्ठा nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	अटल अचिन्हित no_ticks;
	अटल अचिन्हित last_tick_गिनती = (अचिन्हित) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	अगर (serial_debug_active)
		वापस;

	अगर (last_tick_गिनती != watchdog_local_timer_ticks) {
		अगर (no_ticks == 1) {
			म_लिखो("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_गिनती = watchdog_local_timer_ticks;
		वापस;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	अगर (++no_ticks < 10) {
		अगर (no_ticks == 1)
			म_लिखो("WARNING watchdog : possible kernel lockup\n");
		वापस;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

व्योम nmi_watchdog_handler(काष्ठा nmi_frame * frame)
{
#अगर SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	अगर (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	अगर (sprofiling)
		nmi_sprofile_handler(frame);
	
	अगर ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#अन्यथा
	अगर (watchdog_enabled) {
		lockup_check(frame);
		अगर (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#पूर्ण_अगर
}

पूर्णांक nmi_watchdog_start_profiling(स्थिर अचिन्हित freq)
{
	पूर्णांक err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	अगर (!watchdog_enabled) {
		अगर (arch_watchdog_init())
			वापस ENODEV;
	}

	अगर (!watchdog->profile_init) {
		म_लिखो("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		वापस ENODEV;
	}

	err = watchdog->profile_init(freq);
	अगर (err != OK)
		वापस err;

	watchdog->resetval = watchdog->profile_resetval;

	वापस OK;
}

व्योम nmi_watchdog_stop_profiling(व्योम)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	अगर (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	अगर (!watchdog_enabled)
		arch_watchdog_stop();
}
