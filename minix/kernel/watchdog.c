/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#include "watchdog.h"
#include "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
sa_nracanaa arch_watchdog *watchdog;
ຈຳນວນເຕັມ watchdog_enabled;

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
	ຖ້າ (serial_debug_active)
		ສົ່ງຄືນ;

	ຖ້າ (last_tick_count != watchdog_local_timer_ticks) {
		ຖ້າ (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_count = watchdog_local_timer_ticks;
		ສົ່ງຄືນ;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	ຖ້າ (++no_ticks < 10) {
		ຖ້າ (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		ສົ່ງຄືນ;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

void nmi_watchdog_handler(sa_nracanaa nmi_frame * frame)
{
#ຖ້າ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	ຖ້າ (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	ຖ້າ (sprofiling)
		nmi_sprofile_handler(frame);
	
	ຖ້າ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#ບໍ່ດັ່ງນັ້ນ
	ຖ້າ (watchdog_enabled) {
		lockup_check(frame);
		ຖ້າ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

ຈຳນວນເຕັມ nmi_watchdog_start_profiling(const unsigned freq)
{
	ຈຳນວນເຕັມ err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	ຖ້າ (!watchdog_enabled) {
		ຖ້າ (arch_watchdog_init())
			ສົ່ງຄືນ ENODEV;
	}

	ຖ້າ (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ສົ່ງຄືນ ENODEV;
	}

	err = watchdog->profile_init(freq);
	ຖ້າ (err != OK)
		ສົ່ງຄືນ err;

	watchdog->resetval = watchdog->profile_resetval;

	ສົ່ງຄືນ OK;
}

void nmi_watchdog_stop_profiling(void)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	ຖ້າ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	ຖ້າ (!watchdog_enabled)
		arch_watchdog_stop();
}
