/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#include "watchdog.h"
#include "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
sa_nracanaa arch_watchdog *watchdog;
ចំនួនគត់ watchdog_enabled;

static void lockup_check(sa_nracanaa nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	static unsigned no_ticks;
	static unsigned last_tick_count = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	ប្រសិនបើ (serial_debug_active)
		ត្រឡប់;

	ប្រសិនបើ (last_tick_count != watchdog_local_timer_ticks) {
		ប្រសិនបើ (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_count = watchdog_local_timer_ticks;
		ត្រឡប់;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	ប្រសិនបើ (++no_ticks < 10) {
		ប្រសិនបើ (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		ត្រឡប់;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

void nmi_watchdog_handler(sa_nracanaa nmi_frame * frame)
{
#ប្រសិនបើ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	ប្រសិនបើ (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	ប្រសិនបើ (sprofiling)
		nmi_sprofile_handler(frame);
	
	ប្រសិនបើ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#បើមិនដូច្នេះ
	ប្រសិនបើ (watchdog_enabled) {
		lockup_check(frame);
		ប្រសិនបើ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

ចំនួនគត់ nmi_watchdog_start_profiling(const unsigned freq)
{
	ចំនួនគត់ err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	ប្រសិនបើ (!watchdog_enabled) {
		ប្រសិនបើ (arch_watchdog_init())
			ត្រឡប់ ENODEV;
	}

	ប្រសិនបើ (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ត្រឡប់ ENODEV;
	}

	err = watchdog->profile_init(freq);
	ប្រសិនបើ (err != OK)
		ត្រឡប់ err;

	watchdog->resetval = watchdog->profile_resetval;

	ត្រឡប់ OK;
}

void nmi_watchdog_stop_profiling(void)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	ប្រសិនបើ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	ប្រសិនបើ (!watchdog_enabled)
		arch_watchdog_stop();
}
