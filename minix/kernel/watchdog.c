/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#شامل "watchdog.h"
#شامل "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
ساختار arch_watchdog *watchdog;
صحیح watchdog_enabled;

ایستا تهی lockup_check(ساختار nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	ایستا unsigned no_ticks;
	ایستا unsigned last_tick_شمار = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	اگر (serial_debug_active)
		بازگشت;

	اگر (last_tick_شمار != watchdog_local_timer_ticks) {
		اگر (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_شمار = watchdog_local_timer_ticks;
		بازگشت;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	اگر (++no_ticks < 10) {
		اگر (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		بازگشت;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

تهی nmi_watchdog_handler(ساختار nmi_frame * frame)
{
#اگر SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	اگر (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	اگر (sprofiling)
		nmi_sprofile_handler(frame);
	
	اگر ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#وگرنه
	اگر (watchdog_enabled) {
		lockup_check(frame);
		اگر (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

صحیح nmi_watchdog_start_profiling(ثابت unsigned freq)
{
	صحیح err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	اگر (!watchdog_enabled) {
		اگر (arch_watchdog_init())
			بازگشت ENODEV;
	}

	اگر (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		بازگشت ENODEV;
	}

	err = watchdog->profile_init(freq);
	اگر (err != OK)
		بازگشت err;

	watchdog->resetval = watchdog->profile_resetval;

	بازگشت OK;
}

تهی nmi_watchdog_stop_profiling(تهی)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	اگر (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	اگر (!watchdog_enabled)
		arch_watchdog_stop();
}
