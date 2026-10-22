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

#جے_بنا_تعریف SYSTEM_H
#تعریف_کرو SYSTEM_H

#ضمن "kernel/kernel.h"

صحيح do_exec(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_EXEC
#تعریف_کرو do_exec عدم
#جے_انت

صحيح do_fork(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_FORK
#تعریف_کرو do_fork عدم
#جے_انت

صحيح do_clear(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_CLEAR
#تعریف_کرو do_clear عدم
#جے_انت

صحيح do_trace(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_TRACE
#تعریف_کرو do_trace عدم
#جے_انت

صحيح do_runctl(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_RUNCTL
#تعریف_کرو do_runctl عدم
#جے_انت

صحيح do_update(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_UPDATE
#تعریف_کرو do_update عدم
#جے_انت

صحيح do_exit(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_EXIT
#تعریف_کرو do_exit عدم
#جے_انت

صحيح do_copy(بنية proc * caller, سنیہا *m_ptr);
#تعریف_کرو do_vircopy 	do_copy
#إذا ! (USE_VIRCOPY || USE_PHYSCOPY)
#تعریف_کرو do_copy عدم
#جے_انت

صحيح do_umap(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_UMAP
#تعریف_کرو do_umap عدم
#جے_انت

صحيح do_umap_remote(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_UMAP_REMOTE
#تعریف_کرو do_umap_remote عدم
#جے_انت

صحيح do_vumap(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_VUMAP
#تعریف_کرو do_vumap عدم
#جے_انت

صحيح do_memset(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_MEMSET
#تعریف_کرو do_memset عدم
#جے_انت

صحيح do_abort(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_ABORT
#تعریف_کرو do_abort عدم
#جے_انت

صحيح do_getinfo(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_GETINFO
#تعریف_کرو do_getinfo عدم
#جے_انت

صحيح do_privctl(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_PRIVCTL
#تعریف_کرو do_privctl عدم
#جے_انت

صحيح do_irqctl(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_IRQCTL
#تعریف_کرو do_irqctl عدم
#جے_انت

صحيح do_devio(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_DEVIO
#تعریف_کرو do_devio عدم
#جے_انت

صحيح do_vdevio(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_VDEVIO
#تعریف_کرو do_vdevio عدم
#جے_انت

صحيح do_sdevio(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_SDEVIO
#تعریف_کرو do_sdevio عدم
#جے_انت

صحيح do_kill(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_KILL
#تعریف_کرو do_kill عدم
#جے_انت

صحيح do_getksig(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_GETKSIG
#تعریف_کرو do_getksig عدم
#جے_انت

صحيح do_endksig(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_ENDKSIG
#تعریف_کرو do_endksig عدم
#جے_انت

صحيح do_sigsend(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_SIGSEND
#تعریف_کرو do_sigsend عدم
#جے_انت

صحيح do_sigreturn(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_SIGRETURN
#تعریف_کرو do_sigreturn عدم
#جے_انت

صحيح do_times(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_TIMES
#تعریف_کرو do_times عدم
#جے_انت

صحيح do_setalarm(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_SETALARM
#تعریف_کرو do_setalarm عدم
#جے_انت

صحيح do_stime(بنية proc * caller, سنیہا *m_ptr);
صحيح do_settime(بنية proc * caller, سنیہا *m_ptr);

صحيح do_vtimer(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_VTIMER
#تعریف_کرو do_vtimer عدم
#جے_انت

صحيح do_safecopy_منزل(بنية proc * caller, سنیہا *m_ptr);
صحيح do_safecopy_from(بنية proc * caller, سنیہا *m_ptr);
صحيح do_vsafecopy(بنية proc * caller, سنیہا *m_ptr);
صحيح do_iopenable(بنية proc * caller, سنیہا *m_ptr);
صحيح do_vmctl(بنية proc * caller, سنیہا *m_ptr);
صحيح do_setgrant(بنية proc * caller, سنیہا *m_ptr);
صحيح do_readbios(بنية proc * caller, سنیہا *m_ptr);

صحيح do_safememset(بنية proc * caller, سنیہا *m_ptr);

صحيح do_sprofile(بنية proc * caller, سنیہا *m_ptr);
#إذا ! SPROFILE
#تعریف_کرو do_sprofile عدم
#جے_انت

صحيح do_getmcontext(بنية proc * caller, سنیہا *m_ptr);
صحيح do_setmcontext(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_MCONTEXT
#تعریف_کرو do_getmcontext عدم
#تعریف_کرو do_setmcontext عدم
#جے_انت

صحيح do_ویلا_سارنی(بنية proc * caller, سنیہا *m_ptr);
صحيح do_schedctl(بنية proc * caller, سنیہا *m_ptr);

صحيح do_statectl(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_STATECTL
#تعریف_کرو do_statectl عدم
#جے_انت

صحيح do_padconf(بنية proc * caller, سنیہا *m_ptr);
#إذا ! USE_PADCONF
#تعریف_کرو do_padconf عدم
#جے_انت

#جے_انت	/* SYSTEM_H */

