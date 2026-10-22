/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#అన్తర్భావ "watchdog.h"
#అన్తర్భావ "arch/i386/glo.h"

గుర్తులేని watchdog_local_timer_ticks = 0U;
నిర్మాణం arch_watchdog *watchdog;
పూర్ణాంకం watchdog_enabled;

స్థిర శూన్య lockup_check(నిర్మాణం nmi_చట్రం * చట్రం)
{
	/* FIXME this should be CPU local */
	స్థిర గుర్తులేని no_ticks;
	స్థిర గుర్తులేని చివరి_tick_లెక్క = (గుర్తులేని) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	ఒకవేళ (serial_debug_చురుకైన)
		తిరిగి;

	ఒకవేళ (చివరి_tick_లెక్క != watchdog_local_timer_ticks) {
		ఒకవేళ (no_ticks == 1) {
			రాయి("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		చివరి_tick_లెక్క = watchdog_local_timer_ticks;
		తిరిగి;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	ఒకవేళ (++no_ticks < 10) {
		ఒకవేళ (no_ticks == 1)
			రాయి("WARNING watchdog : possible kernel lockup\n");
		తిరిగి;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(చట్రం);
}

శూన్య nmi_watchdog_handler(నిర్మాణం nmi_చట్రం * చట్రం)
{
#ఒకవేళ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	ఒకవేళ (watchdog_enabled && !sprofiling)
		lockup_check(చట్రం);
	ఒకవేళ (sprofiling)
		nmi_sprofile_handler(చట్రం);
	
	ఒకవేళ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#లేకపోతే
	ఒకవేళ (watchdog_enabled) {
		lockup_check(చట్రం);
		ఒకవేళ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#అయితే_ముగింపు
}

పూర్ణాంకం nmi_watchdog_ప్రారంభం_profiling(స్థిరరాశి గుర్తులేని freq)
{
	పూర్ణాంకం err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	ఒకవేళ (!watchdog_enabled) {
		ఒకవేళ (arch_watchdog_init())
			తిరిగి ENODEV;
	}

	ఒకవేళ (!watchdog->profile_init) {
		రాయి("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		తిరిగి ENODEV;
	}

	err = watchdog->profile_init(freq);
	ఒకవేళ (err != OK)
		తిరిగి err;

	watchdog->resetval = watchdog->profile_resetval;

	తిరిగి OK;
}

శూన్య nmi_watchdog_stop_profiling(శూన్య)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	ఒకవేళ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	ఒకవేళ (!watchdog_enabled)
		arch_watchdog_stop();
}
