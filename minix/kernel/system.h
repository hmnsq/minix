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

#ضمن "kernel/kernel.h"

صحيح do_exec(بنية proc * caller, message *m_ptr);
#אם ! USE_EXEC
#define do_exec عدم
#endif

صحيح do_fork(بنية proc * caller, message *m_ptr);
#אם ! USE_FORK
#define do_fork عدم
#endif

صحيح do_clear(بنية proc * caller, message *m_ptr);
#אם ! USE_CLEAR
#define do_clear عدم
#endif

صحيح do_trace(بنية proc * caller, message *m_ptr);
#אם ! USE_TRACE
#define do_trace عدم
#endif

صحيح do_runctl(بنية proc * caller, message *m_ptr);
#אם ! USE_RUNCTL
#define do_runctl عدم
#endif

صحيح do_update(بنية proc * caller, message *m_ptr);
#אם ! USE_UPDATE
#define do_update عدم
#endif

صحيح do_exit(بنية proc * caller, message *m_ptr);
#אם ! USE_EXIT
#define do_exit عدم
#endif

صحيح do_copy(بنية proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#אם ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy عدم
#endif

صحيح do_umap(بنية proc * caller, message *m_ptr);
#אם ! USE_UMAP
#define do_umap عدم
#endif

صحيح do_umap_remote(بنية proc * caller, message *m_ptr);
#אם ! USE_UMAP_REMOTE
#define do_umap_remote عدم
#endif

صحيح do_vumap(بنية proc * caller, message *m_ptr);
#אם ! USE_VUMAP
#define do_vumap عدم
#endif

صحيح do_memset(بنية proc * caller, message *m_ptr);
#אם ! USE_MEMSET
#define do_memset عدم
#endif

صحيح do_abort(بنية proc * caller, message *m_ptr);
#אם ! USE_ABORT
#define do_abort عدم
#endif

صحيح do_getinfo(بنية proc * caller, message *m_ptr);
#אם ! USE_GETINFO
#define do_getinfo عدم
#endif

صحيح do_privctl(بنية proc * caller, message *m_ptr);
#אם ! USE_PRIVCTL
#define do_privctl عدم
#endif

صحيح do_irqctl(بنية proc * caller, message *m_ptr);
#אם ! USE_IRQCTL
#define do_irqctl عدم
#endif

صحيح do_devio(بنية proc * caller, message *m_ptr);
#אם ! USE_DEVIO
#define do_devio عدم
#endif

صحيح do_vdevio(بنية proc * caller, message *m_ptr);
#אם ! USE_VDEVIO
#define do_vdevio عدم
#endif

صحيح do_sdevio(بنية proc * caller, message *m_ptr);
#אם ! USE_SDEVIO
#define do_sdevio عدم
#endif

صحيح do_kill(بنية proc * caller, message *m_ptr);
#אם ! USE_KILL
#define do_kill عدم
#endif

صحيح do_getksig(بنية proc * caller, message *m_ptr);
#אם ! USE_GETKSIG
#define do_getksig عدم
#endif

صحيح do_endksig(بنية proc * caller, message *m_ptr);
#אם ! USE_ENDKSIG
#define do_endksig عدم
#endif

صحيح do_sigsend(بنية proc * caller, message *m_ptr);
#אם ! USE_SIGSEND
#define do_sigsend عدم
#endif

صحيح do_sigreturn(بنية proc * caller, message *m_ptr);
#אם ! USE_SIGRETURN
#define do_sigreturn عدم
#endif

صحيح do_times(بنية proc * caller, message *m_ptr);
#אם ! USE_TIMES
#define do_times عدم
#endif

صحيح do_setalarm(بنية proc * caller, message *m_ptr);
#אם ! USE_SETALARM
#define do_setalarm عدم
#endif

صحيح do_stime(بنية proc * caller, message *m_ptr);
صحيح do_settime(بنية proc * caller, message *m_ptr);

صحيح do_vtimer(بنية proc * caller, message *m_ptr);
#אם ! USE_VTIMER
#define do_vtimer عدم
#endif

صحيح do_safecopy_to(بنية proc * caller, message *m_ptr);
صحيح do_safecopy_from(بنية proc * caller, message *m_ptr);
صحيح do_vsafecopy(بنية proc * caller, message *m_ptr);
صحيح do_iopenable(بنية proc * caller, message *m_ptr);
صحيح do_vmctl(بنية proc * caller, message *m_ptr);
صحيح do_setgrant(بنية proc * caller, message *m_ptr);
صحيح do_readbios(بنية proc * caller, message *m_ptr);

صحيح do_safememset(بنية proc * caller, message *m_ptr);

صحيح do_sprofile(بنية proc * caller, message *m_ptr);
#אם ! SPROFILE
#define do_sprofile عدم
#endif

صحيح do_getmcontext(بنية proc * caller, message *m_ptr);
صحيح do_setmcontext(بنية proc * caller, message *m_ptr);
#אם ! USE_MCONTEXT
#define do_getmcontext عدم
#define do_setmcontext عدم
#endif

صحيح do_schedule(بنية proc * caller, message *m_ptr);
صحيح do_schedctl(بنية proc * caller, message *m_ptr);

صحيح do_statectl(بنية proc * caller, message *m_ptr);
#אם ! USE_STATECTL
#define do_statectl عدم
#endif

صحيح do_padconf(بنية proc * caller, message *m_ptr);
#אם ! USE_PADCONF
#define do_padconf عدم
#endif

#endif	/* SYSTEM_H */

