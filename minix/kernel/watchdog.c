/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#અન્તર્ભાવ "watchdog.h"
#અન્તર્ભાવ "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
સંરચના arch_watchdog *watchdog;
પૂર્ણાંક watchdog_enabled;

સ્થિર શૂન્ય lockup_check(સંરચના nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	સ્થિર unsigned no_ticks;
	સ્થિર unsigned last_tick_ગણતરી = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	જો (serial_debug_active)
		પાછા;

	જો (last_tick_ગણતરી != watchdog_local_timer_ticks) {
		જો (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_ગણતરી = watchdog_local_timer_ticks;
		પાછા;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	જો (++no_ticks < 10) {
		જો (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		પાછા;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

શૂન્ય nmi_watchdog_handler(સંરચના nmi_frame * frame)
{
#જો SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	જો (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	જો (sprofiling)
		nmi_sprofile_handler(frame);
	
	જો ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#નહીંતર
	જો (watchdog_enabled) {
		lockup_check(frame);
		જો (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

પૂર્ણાંક nmi_watchdog_start_profiling(અચલ unsigned freq)
{
	પૂર્ણાંક err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	જો (!watchdog_enabled) {
		જો (arch_watchdog_init())
			પાછા ENODEV;
	}

	જો (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		પાછા ENODEV;
	}

	err = watchdog->profile_init(freq);
	જો (err != OK)
		પાછા err;

	watchdog->resetval = watchdog->profile_resetval;

	પાછા OK;
}

શૂન્ય nmi_watchdog_stop_profiling(શૂન્ય)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	જો (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	જો (!watchdog_enabled)
		arch_watchdog_stop();
}
