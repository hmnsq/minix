/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#ଅନ୍ତର୍ଭାଵ "watchdog.h"
#ଅନ୍ତର୍ଭାଵ "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
ସଂରଚନା arch_watchdog *watchdog;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା watchdog_enabled;

ସ୍ଥିର ଶୂନ୍ୟ lockup_check(ସଂରଚନା nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	ସ୍ଥିର unsigned no_ticks;
	ସ୍ଥିର unsigned last_tick_ଗଣନା = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	ଯଦି (serial_debug_active)
		ଫେରନ୍ତୁ;

	ଯଦି (last_tick_ଗଣନା != watchdog_local_timer_ticks) {
		ଯଦି (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_ଗଣନା = watchdog_local_timer_ticks;
		ଫେରନ୍ତୁ;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	ଯଦି (++no_ticks < 10) {
		ଯଦି (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		ଫେରନ୍ତୁ;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

ଶୂନ୍ୟ nmi_watchdog_handler(ସଂରଚନା nmi_frame * frame)
{
#ଯଦି SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	ଯଦି (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	ଯଦି (sprofiling)
		nmi_sprofile_handler(frame);
	
	ଯଦି ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#ଅନ୍ୟଥା
	ଯଦି (watchdog_enabled) {
		lockup_check(frame);
		ଯଦି (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା nmi_watchdog_start_profiling(ଅଚଳ unsigned freq)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	ଯଦି (!watchdog_enabled) {
		ଯଦି (arch_watchdog_init())
			ଫେରନ୍ତୁ ENODEV;
	}

	ଯଦି (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ଫେରନ୍ତୁ ENODEV;
	}

	err = watchdog->profile_init(freq);
	ଯଦି (err != OK)
		ଫେରନ୍ତୁ err;

	watchdog->resetval = watchdog->profile_resetval;

	ଫେରନ୍ତୁ OK;
}

ଶୂନ୍ୟ nmi_watchdog_stop_profiling(ଶୂନ୍ୟ)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	ଯଦି (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	ଯଦି (!watchdog_enabled)
		arch_watchdog_stop();
}
