/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#அந்தர்பாவ "watchdog.h"
#அந்தர்பாவ "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
கட்டமைப்பு arch_watchdog *watchdog;
முழு watchdog_enabled;

நிலையான வெறுமை lockup_check(கட்டமைப்பு nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	நிலையான unsigned no_ticks;
	நிலையான unsigned last_tick_மொத்தம் = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	எனில் (serial_debug_active)
		திரும்பு;

	எனில் (last_tick_மொத்தம் != watchdog_local_timer_ticks) {
		எனில் (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_மொத்தம் = watchdog_local_timer_ticks;
		திரும்பு;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	எனில் (++no_ticks < 10) {
		எனில் (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		திரும்பு;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

வெறுமை nmi_watchdog_handler(கட்டமைப்பு nmi_frame * frame)
{
#எனில் SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	எனில் (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	எனில் (sprofiling)
		nmi_sprofile_handler(frame);
	
	எனில் ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#இல்லையெனில்
	எனில் (watchdog_enabled) {
		lockup_check(frame);
		எனில் (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

முழு nmi_watchdog_start_profiling(மாறிலி unsigned freq)
{
	முழு err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	எனில் (!watchdog_enabled) {
		எனில் (arch_watchdog_init())
			திரும்பு ENODEV;
	}

	எனில் (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		திரும்பு ENODEV;
	}

	err = watchdog->profile_init(freq);
	எனில் (err != OK)
		திரும்பு err;

	watchdog->resetval = watchdog->profile_resetval;

	திரும்பு OK;
}

வெறுமை nmi_watchdog_stop_profiling(வெறுமை)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	எனில் (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	எனில் (!watchdog_enabled)
		arch_watchdog_stop();
}
