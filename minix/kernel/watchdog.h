#अगर_अघोषित __WATCHDOG_H__
#घोषणा __WATCHDOG_H__

#समावेश "kernel/kernel.h"
#समावेश "arch_watchdog.h"

बाह्य पूर्णांक watchdog_enabled; /* if set to non-zero the watch dog is enabled */
बाह्य अचिन्हित watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

प्रकार व्योम (* arch_watchdog_method_t)(स्थिर अचिन्हित);
प्रकार पूर्णांक (* arch_watchdog_profile_init_t)(स्थिर अचिन्हित);

काष्ठा arch_watchdog {
	arch_watchdog_method_t		init;	/* initial setup */
	arch_watchdog_method_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

बाह्य काष्ठा arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
पूर्णांक arch_watchdog_init(व्योम);
व्योम arch_watchdog_stop(व्योम);

#अगर_घोषित __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
व्योम arch_watchdog_lockup(स्थिर काष्ठा nmi_frame * frame);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
व्योम nmi_watchdog_handler(काष्ठा nmi_frame * frame);
#पूर्ण_अगर

/*
 * start and stop profiling using the NMI watchdog
 */
पूर्णांक nmi_watchdog_start_profiling(स्थिर अचिन्हित freq);
व्योम nmi_watchdog_stop_profiling(व्योम);

#पूर्ण_अगर /* __WATCHDOG_H__ */
