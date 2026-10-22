/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#ಅನ್ತರ್ಭಾವ "watchdog.h"
#ಅನ್ತರ್ಭಾವ "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
ಸಂರಚನಾ arch_watchdog *watchdog;
ಪೂರ್ಣಾಙ್ಕ watchdog_enabled;

ಸ್ಥಿರ ಶೂನ್ಯ lockup_check(ಸಂರಚನಾ nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	ಸ್ಥಿರ unsigned no_ticks;
	ಸ್ಥಿರ unsigned last_tick_ಎಣಿಕೆ = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	ಯದಿ (serial_debug_active)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (last_tick_ಎಣಿಕೆ != watchdog_local_timer_ticks) {
		ಯದಿ (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_ಎಣಿಕೆ = watchdog_local_timer_ticks;
		ಪ್ರತ್ಯಾಗಮ;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	ಯದಿ (++no_ticks < 10) {
		ಯದಿ (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		ಪ್ರತ್ಯಾಗಮ;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

ಶೂನ್ಯ nmi_watchdog_handler(ಸಂರಚನಾ nmi_frame * frame)
{
#ಯದಿ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	ಯದಿ (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	ಯದಿ (sprofiling)
		nmi_sprofile_handler(frame);
	
	ಯದಿ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#ಅನ್ಯಥಾ
	ಯದಿ (watchdog_enabled) {
		lockup_check(frame);
		ಯದಿ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

ಪೂರ್ಣಾಙ್ಕ nmi_watchdog_start_profiling(ಧ್ರುವ unsigned freq)
{
	ಪೂರ್ಣಾಙ್ಕ err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	ಯದಿ (!watchdog_enabled) {
		ಯದಿ (arch_watchdog_init())
			ಪ್ರತ್ಯಾಗಮ ENODEV;
	}

	ಯದಿ (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ಪ್ರತ್ಯಾಗಮ ENODEV;
	}

	err = watchdog->profile_init(freq);
	ಯದಿ (err != OK)
		ಪ್ರತ್ಯಾಗಮ err;

	watchdog->resetval = watchdog->profile_resetval;

	ಪ್ರತ್ಯಾಗಮ OK;
}

ಶೂನ್ಯ nmi_watchdog_stop_profiling(ಶೂನ್ಯ)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	ಯದಿ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	ಯದಿ (!watchdog_enabled)
		arch_watchdog_stop();
}
