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

#include "kernel/kernel.h"

целое do_exec(struct proc * caller, message *m_ptr);
#если ! USE_EXEC
#define do_exec NULL
#endif

целое do_fork(struct proc * caller, message *m_ptr);
#если ! USE_FORK
#define do_fork NULL
#endif

целое do_clear(struct proc * caller, message *m_ptr);
#если ! USE_CLEAR
#define do_clear NULL
#endif

целое do_trace(struct proc * caller, message *m_ptr);
#если ! USE_TRACE
#define do_trace NULL
#endif

целое do_runctl(struct proc * caller, message *m_ptr);
#если ! USE_RUNCTL
#define do_runctl NULL
#endif

целое do_update(struct proc * caller, message *m_ptr);
#если ! USE_UPDATE
#define do_update NULL
#endif

целое do_exit(struct proc * caller, message *m_ptr);
#если ! USE_EXIT
#define do_exit NULL
#endif

целое do_copy(struct proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#если ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy NULL
#endif

целое do_umap(struct proc * caller, message *m_ptr);
#если ! USE_UMAP
#define do_umap NULL
#endif

целое do_umap_remote(struct proc * caller, message *m_ptr);
#если ! USE_UMAP_REMOTE
#define do_umap_remote NULL
#endif

целое do_vumap(struct proc * caller, message *m_ptr);
#если ! USE_VUMAP
#define do_vumap NULL
#endif

целое do_memset(struct proc * caller, message *m_ptr);
#если ! USE_MEMSET
#define do_memset NULL
#endif

целое do_abort(struct proc * caller, message *m_ptr);
#если ! USE_ABORT
#define do_abort NULL
#endif

целое do_getinfo(struct proc * caller, message *m_ptr);
#если ! USE_GETINFO
#define do_getinfo NULL
#endif

целое do_privctl(struct proc * caller, message *m_ptr);
#если ! USE_PRIVCTL
#define do_privctl NULL
#endif

целое do_irqctl(struct proc * caller, message *m_ptr);
#если ! USE_IRQCTL
#define do_irqctl NULL
#endif

целое do_devio(struct proc * caller, message *m_ptr);
#если ! USE_DEVIO
#define do_devio NULL
#endif

целое do_vdevio(struct proc * caller, message *m_ptr);
#если ! USE_VDEVIO
#define do_vdevio NULL
#endif

целое do_sdevio(struct proc * caller, message *m_ptr);
#если ! USE_SDEVIO
#define do_sdevio NULL
#endif

целое do_kill(struct proc * caller, message *m_ptr);
#если ! USE_KILL
#define do_kill NULL
#endif

целое do_getksig(struct proc * caller, message *m_ptr);
#если ! USE_GETKSIG
#define do_getksig NULL
#endif

целое do_endksig(struct proc * caller, message *m_ptr);
#если ! USE_ENDKSIG
#define do_endksig NULL
#endif

целое do_sigsend(struct proc * caller, message *m_ptr);
#если ! USE_SIGSEND
#define do_sigsend NULL
#endif

целое do_sigreturn(struct proc * caller, message *m_ptr);
#если ! USE_SIGRETURN
#define do_sigreturn NULL
#endif

целое do_times(struct proc * caller, message *m_ptr);
#если ! USE_TIMES
#define do_times NULL
#endif

целое do_setalarm(struct proc * caller, message *m_ptr);
#если ! USE_SETALARM
#define do_setalarm NULL
#endif

целое do_stime(struct proc * caller, message *m_ptr);
целое do_settime(struct proc * caller, message *m_ptr);

целое do_vtimer(struct proc * caller, message *m_ptr);
#если ! USE_VTIMER
#define do_vtimer NULL
#endif

целое do_safecopy_to(struct proc * caller, message *m_ptr);
целое do_safecopy_from(struct proc * caller, message *m_ptr);
целое do_vsafecopy(struct proc * caller, message *m_ptr);
целое do_iopenable(struct proc * caller, message *m_ptr);
целое do_vmctl(struct proc * caller, message *m_ptr);
целое do_setgrant(struct proc * caller, message *m_ptr);
целое do_readbios(struct proc * caller, message *m_ptr);

целое do_safememset(struct proc * caller, message *m_ptr);

целое do_sprofile(struct proc * caller, message *m_ptr);
#если ! SPROFILE
#define do_sprofile NULL
#endif

целое do_getmcontext(struct proc * caller, message *m_ptr);
целое do_setmcontext(struct proc * caller, message *m_ptr);
#если ! USE_MCONTEXT
#define do_getmcontext NULL
#define do_setmcontext NULL
#endif

целое do_schedule(struct proc * caller, message *m_ptr);
целое do_schedctl(struct proc * caller, message *m_ptr);

целое do_statectl(struct proc * caller, message *m_ptr);
#если ! USE_STATECTL
#define do_statectl NULL
#endif

целое do_padconf(struct proc * caller, message *m_ptr);
#если ! USE_PADCONF
#define do_padconf NULL
#endif

#endif	/* SYSTEM_H */

