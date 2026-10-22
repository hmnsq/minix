/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#شامل "watchdog.h"
#شامل "arch/i386/glo.h"

غیرعلامتی watchdog_local_timer_ticks = 0U;
ساخت arch_watchdog *watchdog;
صحیح watchdog_enabled;

ساکن خالی lockup_check(ساخت nmi_چوکھٹا * چوکھٹا)
{
	/* FIXME this should be CPU local */
	ساکن غیرعلامتی no_ticks;
	ساکن غیرعلامتی آخری_tick_گنتی = (غیرعلامتی) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	اگر (serial_debug_فعال)
		واپس;

	اگر (آخری_tick_گنتی != watchdog_local_timer_ticks) {
		اگر (no_ticks == 1) {
			لکھو("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		آخری_tick_گنتی = watchdog_local_timer_ticks;
		واپس;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	اگر (++no_ticks < 10) {
		اگر (no_ticks == 1)
			لکھو("WARNING watchdog : possible kernel lockup\n");
		واپس;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(چوکھٹا);
}

خالی nmi_watchdog_handler(ساخت nmi_چوکھٹا * چوکھٹا)
{
#اگر SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	اگر (watchdog_enabled && !sprofiling)
		lockup_check(چوکھٹا);
	اگر (sprofiling)
		nmi_sprofile_handler(چوکھٹا);
	
	اگر ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#ورنہ
	اگر (watchdog_enabled) {
		lockup_check(چوکھٹا);
		اگر (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#اگر_ختم
}

صحیح nmi_watchdog_آغاز_profiling(ثابت غیرعلامتی freq)
{
	صحیح err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	اگر (!watchdog_enabled) {
		اگر (arch_watchdog_init())
			واپس ENODEV;
	}

	اگر (!watchdog->profile_init) {
		لکھو("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		واپس ENODEV;
	}

	err = watchdog->profile_init(freq);
	اگر (err != OK)
		واپس err;

	watchdog->resetval = watchdog->profile_resetval;

	واپس OK;
}

خالی nmi_watchdog_stop_profiling(خالی)
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
