/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#include "watchdog.h"
#include "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
sa_nracanaa arch_watchdog *watchdog;
ཧྲིལ watchdog_enabled;

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
	གལ་སྲིད (serial_debug_active)
		ལོག;

	གལ་སྲིད (last_tick_count != watchdog_local_timer_ticks) {
		གལ་སྲིད (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_count = watchdog_local_timer_ticks;
		ལོག;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	གལ་སྲིད (++no_ticks < 10) {
		གལ་སྲིད (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		ལོག;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

void nmi_watchdog_handler(sa_nracanaa nmi_frame * frame)
{
#གལ་སྲིད SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	གལ་སྲིད (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	གལ་སྲིད (sprofiling)
		nmi_sprofile_handler(frame);
	
	གལ་སྲིད ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#དེ་མིན
	གལ་སྲིད (watchdog_enabled) {
		lockup_check(frame);
		གལ་སྲིད (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

ཧྲིལ nmi_watchdog_start_profiling(const unsigned freq)
{
	ཧྲིལ err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	གལ་སྲིད (!watchdog_enabled) {
		གལ་སྲིད (arch_watchdog_init())
			ལོག ENODEV;
	}

	གལ་སྲིད (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ལོག ENODEV;
	}

	err = watchdog->profile_init(freq);
	གལ་སྲིད (err != OK)
		ལོག err;

	watchdog->resetval = watchdog->profile_resetval;

	ལོག OK;
}

void nmi_watchdog_stop_profiling(void)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	གལ་སྲིད (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	གལ་སྲིད (!watchdog_enabled)
		arch_watchdog_stop();
}
