/* Function prototypes for the system library.  The prototypes in this file 
 * are undefined to NULL if the kernel call is not enabled in config.h.
 * The implementation is contained in src/kernel/system/.  
 *
 * The system library allows to access system services by doing a kernel call.
 * System calls are transformed into request messages to the SYS task that is 
 * responsible for handling the call. By convention, sys_call() is transformed 
 * into a message with type SYS_CALL that is handled in a function do_call(). 
 * 
 * Changes:
 *   Mar 01, 2010   SYS_CLEAR and SYS_EXIT split (Cristiano Giuffrida)
 *   Jul 30, 2005   created SYS_INT86 to support BIOS driver  (Philip Homburg) 
 *   Jul 13, 2005   created SYS_PRIVCTL to manage services  (Jorrit N. Herder) 
 *   Jul 09, 2005   updated SYS_KILL to signal services  (Jorrit N. Herder) 
 *   Jun 21, 2005   created SYS_NICE for nice(2) kernel call  (Ben J. Gras)
 *   Jun 21, 2005   created SYS_MEMSET to speed up exec(2)  (Ben J. Gras)
 *   Jan 20, 2005   updated SYS_COPY for virtual_copy()  (Jorrit N. Herder)
 *   Oct 24, 2004   created SYS_GETKSIG to support PM  (Jorrit N. Herder) 
 *   Oct 10, 2004   created handler for unused calls  (Jorrit N. Herder) 
 *   Sep 09, 2004   updated SYS_EXIT to let services exit  (Jorrit N. Herder) 
 *   Aug 25, 2004   rewrote SYS_SETALARM to clean up code  (Jorrit N. Herder)
 *   Jul 13, 2004   created SYS_SEGCTL to support drivers  (Jorrit N. Herder) 
 *   May 24, 2004   created SYS_SDEVIO to support drivers  (Jorrit N. Herder) 
 *   May 24, 2004   created SYS_GETINFO to retrieve info  (Jorrit N. Herder) 
 *   Apr 18, 2004   created SYS_VDEVIO to support drivers  (Jorrit N. Herder) 
 *   Feb 24, 2004   created SYS_IRQCTL to support drivers  (Jorrit N. Herder) 
 *   Feb 02, 2004   created SYS_DEVIO to support drivers  (Jorrit N. Herder) 
 */ 

#ifndef SYSTEM_H
#define SYSTEM_H

#अन्तर्भाव "kernel/kernel.h"

पूर्णाङ्क do_exec(संरचना proc * caller, message *m_ptr);
#सचे ! USE_EXEC
#define do_exec रिक्त
#endif

पूर्णाङ्क do_fork(संरचना proc * caller, message *m_ptr);
#सचे ! USE_FORK
#define do_fork रिक्त
#endif

पूर्णाङ्क do_clear(संरचना proc * caller, message *m_ptr);
#सचे ! USE_CLEAR
#define do_clear रिक्त
#endif

पूर्णाङ्क do_trace(संरचना proc * caller, message *m_ptr);
#सचे ! USE_TRACE
#define do_trace रिक्त
#endif

पूर्णाङ्क do_runctl(संरचना proc * caller, message *m_ptr);
#सचे ! USE_RUNCTL
#define do_runctl रिक्त
#endif

पूर्णाङ्क do_update(संरचना proc * caller, message *m_ptr);
#सचे ! USE_UPDATE
#define do_update रिक्त
#endif

पूर्णाङ्क do_exit(संरचना proc * caller, message *m_ptr);
#सचे ! USE_EXIT
#define do_exit रिक्त
#endif

पूर्णाङ्क do_copy(संरचना proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#सचे ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy रिक्त
#endif

पूर्णाङ्क do_umap(संरचना proc * caller, message *m_ptr);
#सचे ! USE_UMAP
#define do_umap रिक्त
#endif

पूर्णाङ्क do_umap_remote(संरचना proc * caller, message *m_ptr);
#सचे ! USE_UMAP_REMOTE
#define do_umap_remote रिक्त
#endif

पूर्णाङ्क do_vumap(संरचना proc * caller, message *m_ptr);
#सचे ! USE_VUMAP
#define do_vumap रिक्त
#endif

पूर्णाङ्क do_memset(संरचना proc * caller, message *m_ptr);
#सचे ! USE_MEMSET
#define do_memset रिक्त
#endif

पूर्णाङ्क do_abort(संरचना proc * caller, message *m_ptr);
#सचे ! USE_ABORT
#define do_abort रिक्त
#endif

पूर्णाङ्क do_getinfo(संरचना proc * caller, message *m_ptr);
#सचे ! USE_GETINFO
#define do_getinfo रिक्त
#endif

पूर्णाङ्क do_privctl(संरचना proc * caller, message *m_ptr);
#सचे ! USE_PRIVCTL
#define do_privctl रिक्त
#endif

पूर्णाङ्क do_irqctl(संरचना proc * caller, message *m_ptr);
#सचे ! USE_IRQCTL
#define do_irqctl रिक्त
#endif

पूर्णाङ्क do_devio(संरचना proc * caller, message *m_ptr);
#सचे ! USE_DEVIO
#define do_devio रिक्त
#endif

पूर्णाङ्क do_vdevio(संरचना proc * caller, message *m_ptr);
#सचे ! USE_VDEVIO
#define do_vdevio रिक्त
#endif

पूर्णाङ्क do_sdevio(संरचना proc * caller, message *m_ptr);
#सचे ! USE_SDEVIO
#define do_sdevio रिक्त
#endif

पूर्णाङ्क do_kill(संरचना proc * caller, message *m_ptr);
#सचे ! USE_KILL
#define do_kill रिक्त
#endif

पूर्णाङ्क do_getksig(संरचना proc * caller, message *m_ptr);
#सचे ! USE_GETKSIG
#define do_getksig रिक्त
#endif

पूर्णाङ्क do_endksig(संरचना proc * caller, message *m_ptr);
#सचे ! USE_ENDKSIG
#define do_endksig रिक्त
#endif

पूर्णाङ्क do_sigsend(संरचना proc * caller, message *m_ptr);
#सचे ! USE_SIGSEND
#define do_sigsend रिक्त
#endif

पूर्णाङ्क do_sigreturn(संरचना proc * caller, message *m_ptr);
#सचे ! USE_SIGRETURN
#define do_sigreturn रिक्त
#endif

पूर्णाङ्क do_times(संरचना proc * caller, message *m_ptr);
#सचे ! USE_TIMES
#define do_times रिक्त
#endif

पूर्णाङ्क do_setalarm(संरचना proc * caller, message *m_ptr);
#सचे ! USE_SETALARM
#define do_setalarm रिक्त
#endif

पूर्णाङ्क do_stime(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_settime(संरचना proc * caller, message *m_ptr);

पूर्णाङ्क do_vtimer(संरचना proc * caller, message *m_ptr);
#सचे ! USE_VTIMER
#define do_vtimer रिक्त
#endif

पूर्णाङ्क do_safecopy_to(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_safecopy_from(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_vsafecopy(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_iopenable(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_vmctl(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_setgrant(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_readbios(संरचना proc * caller, message *m_ptr);

पूर्णाङ्क do_safememset(संरचना proc * caller, message *m_ptr);

पूर्णाङ्क do_sprofile(संरचना proc * caller, message *m_ptr);
#सचे ! SPROFILE
#define do_sprofile रिक्त
#endif

पूर्णाङ्क do_getmcontext(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_setmcontext(संरचना proc * caller, message *m_ptr);
#सचे ! USE_MCONTEXT
#define do_getmcontext रिक्त
#define do_setmcontext रिक्त
#endif

पूर्णाङ्क do_schedule(संरचना proc * caller, message *m_ptr);
पूर्णाङ्क do_schedctl(संरचना proc * caller, message *m_ptr);

पूर्णाङ्क do_statectl(संरचना proc * caller, message *m_ptr);
#सचे ! USE_STATECTL
#define do_statectl रिक्त
#endif

पूर्णाङ्क do_padconf(संरचना proc * caller, message *m_ptr);
#सचे ! USE_PADCONF
#define do_padconf रिक्त
#endif

#endif	/* SYSTEM_H */

