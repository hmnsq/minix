/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#include "watchdog.h"
#include "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
struct arch_watchdog *watchdog;
정수 watchdog_enabled;

static void lockup_check(struct nmi_frame * frame)
{
	/* FIXME this should be CPU local */
	static unsigned no_ticks;
	static unsigned last_tick_count = (unsigned) -1;

	/*
	 * when debugging on serial console, printing takes a lot of time some
	 * times while the kernel is certainly not locked up. We don't want to
	 * report a lockup in such situation
	 */
	만약 (serial_debug_active)
		반환;

	만약 (last_tick_count != watchdog_local_timer_ticks) {
		만약 (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_count = watchdog_local_timer_ticks;
		반환;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	만약 (++no_ticks < 10) {
		만약 (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		반환;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

void nmi_watchdog_handler(struct nmi_frame * frame)
{
#만약 SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	만약 (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	만약 (sprofiling)
		nmi_sprofile_handler(frame);
	
	만약 ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#아니면
	만약 (watchdog_enabled) {
		lockup_check(frame);
		만약 (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

정수 nmi_watchdog_start_profiling(const unsigned freq)
{
	정수 err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	만약 (!watchdog_enabled) {
		만약 (arch_watchdog_init())
			반환 ENODEV;
	}

	만약 (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		반환 ENODEV;
	}

	err = watchdog->profile_init(freq);
	만약 (err != OK)
		반환 err;

	watchdog->resetval = watchdog->profile_resetval;

	반환 OK;
}

void nmi_watchdog_stop_profiling(void)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	만약 (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	만약 (!watchdog_enabled)
		arch_watchdog_stop();
}
