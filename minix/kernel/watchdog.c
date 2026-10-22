/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#include "watchdog.h"
#include "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
struct arch_watchdog *watchdog;
ціле watchdog_enabled;

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
	якщо (serial_debug_active)
		повернути;

	якщо (last_tick_count != watchdog_local_timer_ticks) {
		якщо (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_count = watchdog_local_timer_ticks;
		повернути;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	якщо (++no_ticks < 10) {
		якщо (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		повернути;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

void nmi_watchdog_handler(struct nmi_frame * frame)
{
#якщо SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	якщо (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	якщо (sprofiling)
		nmi_sprofile_handler(frame);
	
	якщо ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#інакше
	якщо (watchdog_enabled) {
		lockup_check(frame);
		якщо (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

ціле nmi_watchdog_start_profiling(const unsigned freq)
{
	ціле err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	якщо (!watchdog_enabled) {
		якщо (arch_watchdog_init())
			повернути ENODEV;
	}

	якщо (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		повернути ENODEV;
	}

	err = watchdog->profile_init(freq);
	якщо (err != OK)
		повернути err;

	watchdog->resetval = watchdog->profile_resetval;

	повернути OK;
}

void nmi_watchdog_stop_profiling(void)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	якщо (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	якщо (!watchdog_enabled)
		arch_watchdog_stop();
}
