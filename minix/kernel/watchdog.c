/*
 * This is arch independent NMI watchdog implementation part. It is used to
 * detect kernel lockups and help debugging. each architecture must add its own
 * low level code that triggers periodic checks
 */

#include "watchdog.h"
#include "arch/i386/glo.h"

unsigned watchdog_local_timer_ticks = 0U;
sa_nracanaa arch_watchdog *watchdog;
puur_naa_nka watchdog_enabled;

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
	ᱡᱩᱫᱤ (serial_debug_active)
		ᱨᱩᱣᱟᱹᱲ;

	ᱡᱩᱫᱤ (last_tick_count != watchdog_local_timer_ticks) {
		ᱡᱩᱫᱤ (no_ticks == 1) {
			printf("watchdog : kernel unlocked\n");
			no_ticks = 0;
		}
		/* we are still ticking, everything seems good */
		last_tick_count = watchdog_local_timer_ticks;
		ᱨᱩᱣᱟᱹᱲ;
	}

	/*
	 * if watchdog_local_timer_ticks didn't changed since last time, give it
	 * some more time and only if it still dead, trigger the watchdog alarm
	 */
	ᱡᱩᱫᱤ (++no_ticks < 10) {
		ᱡᱩᱫᱤ (no_ticks == 1)
			printf("WARNING watchdog : possible kernel lockup\n");
		ᱨᱩᱣᱟᱹᱲ;
	}

	/* if we get this far, the kernel is locked up */
	arch_watchdog_lockup(frame);
}

void nmi_watchdog_handler(sa_nracanaa nmi_frame * frame)
{
#ᱡᱩᱫᱤ SPROFILE
	/*
	 * Do not check for lockups while profiling, it is extremely likely that
	 * a false positive is detected if the frequency is high
	 */
	ᱡᱩᱫᱤ (watchdog_enabled && !sprofiling)
		lockup_check(frame);
	ᱡᱩᱫᱤ (sprofiling)
		nmi_sprofile_handler(frame);
	
	ᱡᱩᱫᱤ ((watchdog_enabled || sprofiling) && watchdog->reinit)
		watchdog->reinit(cpuid);
#ᱵᱟᱝ
	ᱡᱩᱫᱤ (watchdog_enabled) {
		lockup_check(frame);
		ᱡᱩᱫᱤ (watchdog->reinit)
			watchdog->reinit(cpuid);
	}
#endif
}

puur_naa_nka nmi_watchdog_start_profiling(const unsigned freq)
{
	puur_naa_nka err;
	
	/* if watchdog hasn't been enabled, we must enable it now */
	ᱡᱩᱫᱤ (!watchdog_enabled) {
		ᱡᱩᱫᱤ (arch_watchdog_init())
			ᱨᱩᱣᱟᱹᱲ ENODEV;
	}

	ᱡᱩᱫᱤ (!watchdog->profile_init) {
		printf("WARNING NMI watchdog profiling not supported\n");
		nmi_watchdog_stop_profiling();
		ᱨᱩᱣᱟᱹᱲ ENODEV;
	}

	err = watchdog->profile_init(freq);
	ᱡᱩᱫᱤ (err != OK)
		ᱨᱩᱣᱟᱹᱲ err;

	watchdog->resetval = watchdog->profile_resetval;

	ᱨᱩᱣᱟᱹᱲ OK;
}

void nmi_watchdog_stop_profiling(void)
{
	/*
	 * if we do not rearm the NMI source, we are done, if we want to keep
	 * the watchdog running, we reset is to its normal value
	 */

	ᱡᱩᱫᱤ (watchdog)
		watchdog->resetval = watchdog->watchdog_resetval;

	ᱡᱩᱫᱤ (!watchdog_enabled)
		arch_watchdog_stop();
}
