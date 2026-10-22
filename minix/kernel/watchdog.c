/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#ಅನ್ತರ್ಭಾವ "watchdog.h"
#ಅನ್ತರ್ಭಾವ "arch/i386/glo.h"

ಚಿಹ್ನೆರಹಿತ watchdog_local_timer_ticks = 0U;
ರಚನೆ arch_watchdog *watchdog;
ಪೂರ್ಣಾಂಕ watchdog_enabled;

ಸ್ಥಿರ ಶೂನ್ಯ lockup_check(ರಚನೆ nmi_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು)
{
	/* FIXME this should be CPU local */
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ no_ticks;
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಕೊನೆಯ_tick_ಎಣಿಕೆ = (ಚಿಹ್ನೆರಹಿತ) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	ಆದರೆ (serial_debug_ಸಕ್ರಿಯ)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (ಕೊನೆಯ_tick_ಎಣಿಕೆ != watchdog_local_timer_ticks) {
		ಆದರೆ (no_ticks == 1) {
			ಬರೆ("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		ಕೊನೆಯ_tick_ಎಣಿಕೆ = watchdog_local_timer_ticks;
		ಹಿಂತಿರುಗಿ;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	ಆದರೆ (++no_ticks < 10) {
		ಆದರೆ (no_ticks == 1)
			ಬರೆ("WARNING watchdog : possible kernel lockup\n");
		ಹಿಂತಿರುಗಿ;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(ಚೌಕಟ್ಟು);
}

ಶೂನ್ಯ nmi_watchdog_handler(ರಚನೆ nmi_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು)
{
#ಆದರೆ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	ಆದರೆ (watchdog_enabled && !sprofiling)
		lockup_check(ಚೌಕಟ್ಟು);
	ಆದರೆ (sprofiling)
		nmi_sprofile_handler(ಚೌಕಟ್ಟು);
	
	ಆದರೆ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (watchdog_enabled) {
		lockup_check(ಚೌಕಟ್ಟು);
		ಆದರೆ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ nmi_watchdog_ಆರಂಭ_profiling(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ freq)
{
	ಪೂರ್ಣಾಂಕ err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	ಆದರೆ (!watchdog_enabled) {
		ಆದರೆ (arch_watchdog_init())
			ಹಿಂತಿರುಗಿ ENODEV;
	}

	ಆದರೆ (!watchdog->profile_init) {
		ಬರೆ("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ಹಿಂತಿರುಗಿ ENODEV;
	}

	err = watchdog->profile_init(freq);
	ಆದರೆ (err != OK)
		ಹಿಂತಿರುಗಿ err;

	watchdog->resetval = watchdog->profile_resetval;

	ಹಿಂತಿರುಗಿ OK;
}

ಶೂನ್ಯ nmi_watchdog_stop_profiling(ಶೂನ್ಯ)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	ಆದರೆ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	ಆದರೆ (!watchdog_enabled)
		arch_watchdog_stop();
}
