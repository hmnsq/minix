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

ကိန်းပြည့် do_exec(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_EXEC
#define do_exec NULL
#endif

ကိန်းပြည့် do_fork(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_FORK
#define do_fork NULL
#endif

ကိန်းပြည့် do_clear(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_CLEAR
#define do_clear NULL
#endif

ကိန်းပြည့် do_trace(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_TRACE
#define do_trace NULL
#endif

ကိန်းပြည့် do_runctl(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_RUNCTL
#define do_runctl NULL
#endif

ကိန်းပြည့် do_update(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_UPDATE
#define do_update NULL
#endif

ကိန်းပြည့် do_exit(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_EXIT
#define do_exit NULL
#endif

ကိန်းပြည့် do_copy(sa_nracanaa proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#အကယ်၍ ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy NULL
#endif

ကိန်းပြည့် do_umap(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_UMAP
#define do_umap NULL
#endif

ကိန်းပြည့် do_umap_remote(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_UMAP_REMOTE
#define do_umap_remote NULL
#endif

ကိန်းပြည့် do_vumap(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_VUMAP
#define do_vumap NULL
#endif

ကိန်းပြည့် do_memset(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_MEMSET
#define do_memset NULL
#endif

ကိန်းပြည့် do_abort(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_ABORT
#define do_abort NULL
#endif

ကိန်းပြည့် do_getinfo(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_GETINFO
#define do_getinfo NULL
#endif

ကိန်းပြည့် do_privctl(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_PRIVCTL
#define do_privctl NULL
#endif

ကိန်းပြည့် do_irqctl(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_IRQCTL
#define do_irqctl NULL
#endif

ကိန်းပြည့် do_devio(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_DEVIO
#define do_devio NULL
#endif

ကိန်းပြည့် do_vdevio(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_VDEVIO
#define do_vdevio NULL
#endif

ကိန်းပြည့် do_sdevio(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_SDEVIO
#define do_sdevio NULL
#endif

ကိန်းပြည့် do_kill(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_KILL
#define do_kill NULL
#endif

ကိန်းပြည့် do_getksig(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_GETKSIG
#define do_getksig NULL
#endif

ကိန်းပြည့် do_endksig(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_ENDKSIG
#define do_endksig NULL
#endif

ကိန်းပြည့် do_sigsend(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_SIGSEND
#define do_sigsend NULL
#endif

ကိန်းပြည့် do_sigreturn(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_SIGRETURN
#define do_sigreturn NULL
#endif

ကိန်းပြည့် do_times(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_TIMES
#define do_times NULL
#endif

ကိန်းပြည့် do_setalarm(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_SETALARM
#define do_setalarm NULL
#endif

ကိန်းပြည့် do_stime(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_settime(sa_nracanaa proc * caller, message *m_ptr);

ကိန်းပြည့် do_vtimer(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_VTIMER
#define do_vtimer NULL
#endif

ကိန်းပြည့် do_safecopy_to(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_safecopy_from(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_vsafecopy(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_iopenable(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_vmctl(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_setgrant(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_readbios(sa_nracanaa proc * caller, message *m_ptr);

ကိန်းပြည့် do_safememset(sa_nracanaa proc * caller, message *m_ptr);

ကိန်းပြည့် do_sprofile(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! SPROFILE
#define do_sprofile NULL
#endif

ကိန်းပြည့် do_getmcontext(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_setmcontext(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_MCONTEXT
#define do_getmcontext NULL
#define do_setmcontext NULL
#endif

ကိန်းပြည့် do_schedule(sa_nracanaa proc * caller, message *m_ptr);
ကိန်းပြည့် do_schedctl(sa_nracanaa proc * caller, message *m_ptr);

ကိန်းပြည့် do_statectl(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_STATECTL
#define do_statectl NULL
#endif

ကိန်းပြည့် do_padconf(sa_nracanaa proc * caller, message *m_ptr);
#အကယ်၍ ! USE_PADCONF
#define do_padconf NULL
#endif

#endif	/* SYSTEM_H */

