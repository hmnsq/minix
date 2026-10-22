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

#شامل "kernel/kernel.h"

صحیح do_exec(ساختار proc * caller, message *m_ptr);
#اگر ! USE_EXEC
#define do_exec پوچ
#endif

صحیح do_fork(ساختار proc * caller, message *m_ptr);
#اگر ! USE_FORK
#define do_fork پوچ
#endif

صحیح do_clear(ساختار proc * caller, message *m_ptr);
#اگر ! USE_CLEAR
#define do_clear پوچ
#endif

صحیح do_trace(ساختار proc * caller, message *m_ptr);
#اگر ! USE_TRACE
#define do_trace پوچ
#endif

صحیح do_runctl(ساختار proc * caller, message *m_ptr);
#اگر ! USE_RUNCTL
#define do_runctl پوچ
#endif

صحیح do_update(ساختار proc * caller, message *m_ptr);
#اگر ! USE_UPDATE
#define do_update پوچ
#endif

صحیح do_exit(ساختار proc * caller, message *m_ptr);
#اگر ! USE_EXIT
#define do_exit پوچ
#endif

صحیح do_copy(ساختار proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#اگر ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy پوچ
#endif

صحیح do_umap(ساختار proc * caller, message *m_ptr);
#اگر ! USE_UMAP
#define do_umap پوچ
#endif

صحیح do_umap_remote(ساختار proc * caller, message *m_ptr);
#اگر ! USE_UMAP_REMOTE
#define do_umap_remote پوچ
#endif

صحیح do_vumap(ساختار proc * caller, message *m_ptr);
#اگر ! USE_VUMAP
#define do_vumap پوچ
#endif

صحیح do_memset(ساختار proc * caller, message *m_ptr);
#اگر ! USE_MEMSET
#define do_memset پوچ
#endif

صحیح do_abort(ساختار proc * caller, message *m_ptr);
#اگر ! USE_ABORT
#define do_abort پوچ
#endif

صحیح do_getinfo(ساختار proc * caller, message *m_ptr);
#اگر ! USE_GETINFO
#define do_getinfo پوچ
#endif

صحیح do_privctl(ساختار proc * caller, message *m_ptr);
#اگر ! USE_PRIVCTL
#define do_privctl پوچ
#endif

صحیح do_irqctl(ساختار proc * caller, message *m_ptr);
#اگر ! USE_IRQCTL
#define do_irqctl پوچ
#endif

صحیح do_devio(ساختار proc * caller, message *m_ptr);
#اگر ! USE_DEVIO
#define do_devio پوچ
#endif

صحیح do_vdevio(ساختار proc * caller, message *m_ptr);
#اگر ! USE_VDEVIO
#define do_vdevio پوچ
#endif

صحیح do_sdevio(ساختار proc * caller, message *m_ptr);
#اگر ! USE_SDEVIO
#define do_sdevio پوچ
#endif

صحیح do_kill(ساختار proc * caller, message *m_ptr);
#اگر ! USE_KILL
#define do_kill پوچ
#endif

صحیح do_getksig(ساختار proc * caller, message *m_ptr);
#اگر ! USE_GETKSIG
#define do_getksig پوچ
#endif

صحیح do_endksig(ساختار proc * caller, message *m_ptr);
#اگر ! USE_ENDKSIG
#define do_endksig پوچ
#endif

صحیح do_sigsend(ساختار proc * caller, message *m_ptr);
#اگر ! USE_SIGSEND
#define do_sigsend پوچ
#endif

صحیح do_sigreturn(ساختار proc * caller, message *m_ptr);
#اگر ! USE_SIGRETURN
#define do_sigreturn پوچ
#endif

صحیح do_times(ساختار proc * caller, message *m_ptr);
#اگر ! USE_TIMES
#define do_times پوچ
#endif

صحیح do_setalarm(ساختار proc * caller, message *m_ptr);
#اگر ! USE_SETALARM
#define do_setalarm پوچ
#endif

صحیح do_stime(ساختار proc * caller, message *m_ptr);
صحیح do_settime(ساختار proc * caller, message *m_ptr);

صحیح do_vtimer(ساختار proc * caller, message *m_ptr);
#اگر ! USE_VTIMER
#define do_vtimer پوچ
#endif

صحیح do_safecopy_to(ساختار proc * caller, message *m_ptr);
صحیح do_safecopy_from(ساختار proc * caller, message *m_ptr);
صحیح do_vsafecopy(ساختار proc * caller, message *m_ptr);
صحیح do_iopenable(ساختار proc * caller, message *m_ptr);
صحیح do_vmctl(ساختار proc * caller, message *m_ptr);
صحیح do_setgrant(ساختار proc * caller, message *m_ptr);
صحیح do_readbios(ساختار proc * caller, message *m_ptr);

صحیح do_safememset(ساختار proc * caller, message *m_ptr);

صحیح do_sprofile(ساختار proc * caller, message *m_ptr);
#اگر ! SPROFILE
#define do_sprofile پوچ
#endif

صحیح do_getmcontext(ساختار proc * caller, message *m_ptr);
صحیح do_setmcontext(ساختار proc * caller, message *m_ptr);
#اگر ! USE_MCONTEXT
#define do_getmcontext پوچ
#define do_setmcontext پوچ
#endif

صحیح do_schedule(ساختار proc * caller, message *m_ptr);
صحیح do_schedctl(ساختار proc * caller, message *m_ptr);

صحیح do_statectl(ساختار proc * caller, message *m_ptr);
#اگر ! USE_STATECTL
#define do_statectl پوچ
#endif

صحیح do_padconf(ساختار proc * caller, message *m_ptr);
#اگر ! USE_PADCONF
#define do_padconf پوچ
#endif

#endif	/* SYSTEM_H */

