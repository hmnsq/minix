/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "watchdog.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch/i386/glo.h"

ਬਿਨਾਨਿਸ਼ਾਨੀ watchdog_local_timer_ticks = 0U;
ਬਣਤਰ arch_watchdog *watchdog;
ਪੂਰਨ watchdog_enabled;

ਸਥਿਰ ਖਾਲੀ lockup_check(ਬਣਤਰ nmi_ਚੌਖਟਾ * ਚੌਖਟਾ)
{
	/* FIXME this should be CPU local */
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ no_ticks;
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਆਖਰੀ_tick_ਗਿਣਤੀ = (ਬਿਨਾਨਿਸ਼ਾਨੀ) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	ਜੇ (serial_debug_ਸਰਗਰਮ)
		ਵਾਪਸ;

	ਜੇ (ਆਖਰੀ_tick_ਗਿਣਤੀ != watchdog_local_timer_ticks) {
		ਜੇ (no_ticks == 1) {
			ਲਿਖੋ("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		ਆਖਰੀ_tick_ਗਿਣਤੀ = watchdog_local_timer_ticks;
		ਵਾਪਸ;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	ਜੇ (++no_ticks < 10) {
		ਜੇ (no_ticks == 1)
			ਲਿਖੋ("WARNING watchdog : possible kernel lockup\n");
		ਵਾਪਸ;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(ਚੌਖਟਾ);
}

ਖਾਲੀ nmi_watchdog_handler(ਬਣਤਰ nmi_ਚੌਖਟਾ * ਚੌਖਟਾ)
{
#ਜੇ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	ਜੇ (watchdog_enabled && !sprofiling)
		lockup_check(ਚੌਖਟਾ);
	ਜੇ (sprofiling)
		nmi_sprofile_handler(ਚੌਖਟਾ);
	
	ਜੇ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#ਨਹੀਂਤਾਂ
	ਜੇ (watchdog_enabled) {
		lockup_check(ਚੌਖਟਾ);
		ਜੇ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ nmi_watchdog_ਸ਼ੁਰੂ_profiling(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	ਪੂਰਨ err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	ਜੇ (!watchdog_enabled) {
		ਜੇ (arch_watchdog_init())
			ਵਾਪਸ ENODEV;
	}

	ਜੇ (!watchdog->profile_init) {
		ਲਿਖੋ("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ਵਾਪਸ ENODEV;
	}

	err = watchdog->profile_init(freq);
	ਜੇ (err != OK)
		ਵਾਪਸ err;

	watchdog->resetval = watchdog->profile_resetval;

	ਵਾਪਸ OK;
}

ਖਾਲੀ nmi_watchdog_stop_profiling(ਖਾਲੀ)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	ਜੇ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	ਜੇ (!watchdog_enabled)
		arch_watchdog_stop();
}
