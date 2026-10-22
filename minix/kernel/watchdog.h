#నిర్వచితం_కాకపోతే __WATCHDOG_H__
#నిర్వచించు __WATCHDOG_H__

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_watchdog.h"

బాహ్య పూర్ణాంకం watchdog_enabled; /* if set to non-zero the watch dog is enabled */
బాహ్య గుర్తులేని watchdog_local_timer_ticks; /* is timer still ticking? */

/*
 * as the implementation is not only architecture dependent but like in x86 case
 * very much model specific, we need to keep a collection of methods that
 * implement it in runtime after the correct arch/model was detected
 */

రకనిర్వచనం శూన్య (* arch_watchdog_పద్ధతి_t)(స్థిరరాశి గుర్తులేని);
రకనిర్వచనం పూర్ణాంకం (* arch_watchdog_profile_init_t)(స్థిరరాశి గుర్తులేని);

నిర్మాణం arch_watchdog {
	arch_watchdog_పద్ధతి_t		init;	/* initial setup */
	arch_watchdog_పద్ధతి_t		reinit;	/* reinit after a tick */
	arch_watchdog_profile_init_t	profile_init;
	u64_t				resetval;
	u64_t				watchdog_resetval;
	u64_t				profile_resetval;
};

బాహ్య నిర్మాణం arch_watchdog *watchdog;

/* let the arch code do whatever it needs to setup or quit the watchdog */
పూర్ణాంకం arch_watchdog_init(శూన్య);
శూన్య arch_watchdog_stop(శూన్య);

#నిర్వచితమైతే __i386__
/* if the watchdog detects lockup, let the arch code to handle it */
శూన్య arch_watchdog_lockup(స్థిరరాశి నిర్మాణం nmi_చట్రం * చట్రం);

/* generic NMI handler. Takes one argument which points to where the arch
 * specific low level handler dumped CPU information and can be inspected by the
 * arch specific code of the watchdog implementation */
శూన్య nmi_watchdog_handler(నిర్మాణం nmi_చట్రం * చట్రం);
#అయితే_ముగింపు

/*
 * start and stop profiling using the NMI watchdog
 */
పూర్ణాంకం nmi_watchdog_ప్రారంభం_profiling(స్థిరరాశి గుర్తులేని freq);
శూన్య nmi_watchdog_stop_profiling(శూన్య);

#అయితే_ముగింపు /* __WATCHDOG_H__ */
