/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#അന്തര്ഭാവ "watchdog.h"
#അന്തര്ഭാവ "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
ഘടന arch_watchdog *watchdog;
പൂർണ്ണസംഖ്യ watchdog_enabled;

സ്ഥിതം ശൂന്യം lockup_check(ഘടന nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	സ്ഥിതം unsigned no_ticks;
	സ്ഥിതം unsigned last_tick_എണ്ണം = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	എങ്കിൽ (serial_debug_active)
		തിരികെ;

	എങ്കിൽ (last_tick_എണ്ണം != watchdog_local_timer_ticks) {
		എങ്കിൽ (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_എണ്ണം = watchdog_local_timer_ticks;
		തിരികെ;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	എങ്കിൽ (++no_ticks < 10) {
		എങ്കിൽ (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		തിരികെ;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

ശൂന്യം nmi_watchdog_handler(ഘടന nmi_frame * frame)
{
#എങ്കിൽ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	എങ്കിൽ (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	എങ്കിൽ (sprofiling)
		nmi_sprofile_handler(frame);
	
	എങ്കിൽ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#അല്ലെങ്കിൽ
	എങ്കിൽ (watchdog_enabled) {
		lockup_check(frame);
		എങ്കിൽ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

പൂർണ്ണസംഖ്യ nmi_watchdog_start_profiling(സ്ഥിരം unsigned freq)
{
	പൂർണ്ണസംഖ്യ err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	എങ്കിൽ (!watchdog_enabled) {
		എങ്കിൽ (arch_watchdog_init())
			തിരികെ ENODEV;
	}

	എങ്കിൽ (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		തിരികെ ENODEV;
	}

	err = watchdog->profile_init(freq);
	എങ്കിൽ (err != OK)
		തിരികെ err;

	watchdog->resetval = watchdog->profile_resetval;

	തിരികെ OK;
}

ശൂന്യം nmi_watchdog_stop_profiling(ശൂന്യം)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	എങ്കിൽ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	എങ്കിൽ (!watchdog_enabled)
		arch_watchdog_stop();
}
