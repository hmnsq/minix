#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_watchdog.h"

बाह्य पूर्णाङ्क watchdog_enabled; /* if set to non-zero the watch dog is enabled */
बाह्य अचिन्हित watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

प्रकारनिर्देश शून्य (* arch_watchdog_method_t)(अचर अचिन्हित);
प्रकारनिर्देश पूर्णाङ्क (* arch_watchdog_profile_init_t)(अचर अचिन्हित);

संरचना arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

बाह्य संरचना arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
पूर्णाङ्क arch_watchdog_init(शून्य);
शून्य arch_watchdog_stop(शून्य);

#ifdef __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
शून्य arch_watchdog_lockup(अचर संरचना nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
शून्य nmi_watchdog_handler(संरचना nmi_frame * frame);
#endif

/*
 * start and stop profiling using the NMI watchdog
 */
पूर्णाङ्क nmi_watchdog_start_profiling(अचर अचिन्हित freq);
शून्य nmi_watchdog_stop_profiling(शून्य);

#endif /* __WATCHDOG_H__ */
