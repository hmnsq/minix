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

цяло do_exec(struct proc * caller, message *m_ptr);
#ако ! USE_EXEC
#define do_exec NULL
#endif

цяло do_fork(struct proc * caller, message *m_ptr);
#ако ! USE_FORK
#define do_fork NULL
#endif

цяло do_clear(struct proc * caller, message *m_ptr);
#ако ! USE_CLEAR
#define do_clear NULL
#endif

цяло do_trace(struct proc * caller, message *m_ptr);
#ако ! USE_TRACE
#define do_trace NULL
#endif

цяло do_runctl(struct proc * caller, message *m_ptr);
#ако ! USE_RUNCTL
#define do_runctl NULL
#endif

цяло do_update(struct proc * caller, message *m_ptr);
#ако ! USE_UPDATE
#define do_update NULL
#endif

цяло do_exit(struct proc * caller, message *m_ptr);
#ако ! USE_EXIT
#define do_exit NULL
#endif

цяло do_copy(struct proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#ако ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy NULL
#endif

цяло do_umap(struct proc * caller, message *m_ptr);
#ако ! USE_UMAP
#define do_umap NULL
#endif

цяло do_umap_remote(struct proc * caller, message *m_ptr);
#ако ! USE_UMAP_REMOTE
#define do_umap_remote NULL
#endif

цяло do_vumap(struct proc * caller, message *m_ptr);
#ако ! USE_VUMAP
#define do_vumap NULL
#endif

цяло do_memset(struct proc * caller, message *m_ptr);
#ако ! USE_MEMSET
#define do_memset NULL
#endif

цяло do_abort(struct proc * caller, message *m_ptr);
#ако ! USE_ABORT
#define do_abort NULL
#endif

цяло do_getinfo(struct proc * caller, message *m_ptr);
#ако ! USE_GETINFO
#define do_getinfo NULL
#endif

цяло do_privctl(struct proc * caller, message *m_ptr);
#ако ! USE_PRIVCTL
#define do_privctl NULL
#endif

цяло do_irqctl(struct proc * caller, message *m_ptr);
#ако ! USE_IRQCTL
#define do_irqctl NULL
#endif

цяло do_devio(struct proc * caller, message *m_ptr);
#ако ! USE_DEVIO
#define do_devio NULL
#endif

цяло do_vdevio(struct proc * caller, message *m_ptr);
#ако ! USE_VDEVIO
#define do_vdevio NULL
#endif

цяло do_sdevio(struct proc * caller, message *m_ptr);
#ако ! USE_SDEVIO
#define do_sdevio NULL
#endif

цяло do_kill(struct proc * caller, message *m_ptr);
#ако ! USE_KILL
#define do_kill NULL
#endif

цяло do_getksig(struct proc * caller, message *m_ptr);
#ако ! USE_GETKSIG
#define do_getksig NULL
#endif

цяло do_endksig(struct proc * caller, message *m_ptr);
#ако ! USE_ENDKSIG
#define do_endksig NULL
#endif

цяло do_sigsend(struct proc * caller, message *m_ptr);
#ако ! USE_SIGSEND
#define do_sigsend NULL
#endif

цяло do_sigreturn(struct proc * caller, message *m_ptr);
#ако ! USE_SIGRETURN
#define do_sigreturn NULL
#endif

цяло do_times(struct proc * caller, message *m_ptr);
#ако ! USE_TIMES
#define do_times NULL
#endif

цяло do_setalarm(struct proc * caller, message *m_ptr);
#ако ! USE_SETALARM
#define do_setalarm NULL
#endif

цяло do_stime(struct proc * caller, message *m_ptr);
цяло do_settime(struct proc * caller, message *m_ptr);

цяло do_vtimer(struct proc * caller, message *m_ptr);
#ако ! USE_VTIMER
#define do_vtimer NULL
#endif

цяло do_safecopy_to(struct proc * caller, message *m_ptr);
цяло do_safecopy_from(struct proc * caller, message *m_ptr);
цяло do_vsafecopy(struct proc * caller, message *m_ptr);
цяло do_iopenable(struct proc * caller, message *m_ptr);
цяло do_vmctl(struct proc * caller, message *m_ptr);
цяло do_setgrant(struct proc * caller, message *m_ptr);
цяло do_readbios(struct proc * caller, message *m_ptr);

цяло do_safememset(struct proc * caller, message *m_ptr);

цяло do_sprofile(struct proc * caller, message *m_ptr);
#ако ! SPROFILE
#define do_sprofile NULL
#endif

цяло do_getmcontext(struct proc * caller, message *m_ptr);
цяло do_setmcontext(struct proc * caller, message *m_ptr);
#ако ! USE_MCONTEXT
#define do_getmcontext NULL
#define do_setmcontext NULL
#endif

цяло do_schedule(struct proc * caller, message *m_ptr);
цяло do_schedctl(struct proc * caller, message *m_ptr);

цяло do_statectl(struct proc * caller, message *m_ptr);
#ако ! USE_STATECTL
#define do_statectl NULL
#endif

цяло do_padconf(struct proc * caller, message *m_ptr);
#ако ! USE_PADCONF
#define do_padconf NULL
#endif

#endif	/* SYSTEM_H */

