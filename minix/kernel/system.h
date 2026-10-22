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

#اگر_تعریف_نشده SYSTEM_H
#تعریف_کن SYSTEM_H

#ضمن "kernel/kernel.h"

صحيح do_exec(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_EXEC
#تعریف_کن do_exec عدم
#پایان_اگر

صحيح do_fork(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_FORK
#تعریف_کن do_fork عدم
#پایان_اگر

صحيح do_clear(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_CLEAR
#تعریف_کن do_clear عدم
#پایان_اگر

صحيح do_trace(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_TRACE
#تعریف_کن do_trace عدم
#پایان_اگر

صحيح do_runctl(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_RUNCTL
#تعریف_کن do_runctl عدم
#پایان_اگر

صحيح do_update(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_UPDATE
#تعریف_کن do_update عدم
#پایان_اگر

صحيح do_exit(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_EXIT
#تعریف_کن do_exit عدم
#پایان_اگر

صحيح do_copy(بنية proc * caller, پیام *m_ptr);
#تعریف_کن do_vircopy 	do_copy
#إذا ! (USE_VIRCOPY || USE_PHYSCOPY)
#تعریف_کن do_copy عدم
#پایان_اگر

صحيح do_umap(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_UMAP
#تعریف_کن do_umap عدم
#پایان_اگر

صحيح do_umap_remote(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_UMAP_REMOTE
#تعریف_کن do_umap_remote عدم
#پایان_اگر

صحيح do_vumap(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_VUMAP
#تعریف_کن do_vumap عدم
#پایان_اگر

صحيح do_memset(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_MEMSET
#تعریف_کن do_memset عدم
#پایان_اگر

صحيح do_abort(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_ABORT
#تعریف_کن do_abort عدم
#پایان_اگر

صحيح do_getinfo(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_GETINFO
#تعریف_کن do_getinfo عدم
#پایان_اگر

صحيح do_privctl(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_PRIVCTL
#تعریف_کن do_privctl عدم
#پایان_اگر

صحيح do_irqctl(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_IRQCTL
#تعریف_کن do_irqctl عدم
#پایان_اگر

صحيح do_devio(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_DEVIO
#تعریف_کن do_devio عدم
#پایان_اگر

صحيح do_vdevio(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_VDEVIO
#تعریف_کن do_vdevio عدم
#پایان_اگر

صحيح do_sdevio(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_SDEVIO
#تعریف_کن do_sdevio عدم
#پایان_اگر

صحيح do_kill(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_KILL
#تعریف_کن do_kill عدم
#پایان_اگر

صحيح do_getksig(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_GETKSIG
#تعریف_کن do_getksig عدم
#پایان_اگر

صحيح do_endksig(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_ENDKSIG
#تعریف_کن do_endksig عدم
#پایان_اگر

صحيح do_sigsend(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_SIGSEND
#تعریف_کن do_sigsend عدم
#پایان_اگر

صحيح do_sigreturn(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_SIGRETURN
#تعریف_کن do_sigreturn عدم
#پایان_اگر

صحيح do_times(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_TIMES
#تعریف_کن do_times عدم
#پایان_اگر

صحيح do_setalarm(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_SETALARM
#تعریف_کن do_setalarm عدم
#پایان_اگر

صحيح do_stime(بنية proc * caller, پیام *m_ptr);
صحيح do_settime(بنية proc * caller, پیام *m_ptr);

صحيح do_vtimer(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_VTIMER
#تعریف_کن do_vtimer عدم
#پایان_اگر

صحيح do_safecopy_مقصد(بنية proc * caller, پیام *m_ptr);
صحيح do_safecopy_from(بنية proc * caller, پیام *m_ptr);
صحيح do_vsafecopy(بنية proc * caller, پیام *m_ptr);
صحيح do_iopenable(بنية proc * caller, پیام *m_ptr);
صحيح do_vmctl(بنية proc * caller, پیام *m_ptr);
صحيح do_setgrant(بنية proc * caller, پیام *m_ptr);
صحيح do_readbios(بنية proc * caller, پیام *m_ptr);

صحيح do_safememset(بنية proc * caller, پیام *m_ptr);

صحيح do_sprofile(بنية proc * caller, پیام *m_ptr);
#إذا ! SPROFILE
#تعریف_کن do_sprofile عدم
#پایان_اگر

صحيح do_getmcontext(بنية proc * caller, پیام *m_ptr);
صحيح do_setmcontext(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_MCONTEXT
#تعریف_کن do_getmcontext عدم
#تعریف_کن do_setmcontext عدم
#پایان_اگر

صحيح do_زمانبندی(بنية proc * caller, پیام *m_ptr);
صحيح do_schedctl(بنية proc * caller, پیام *m_ptr);

صحيح do_statectl(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_STATECTL
#تعریف_کن do_statectl عدم
#پایان_اگر

صحيح do_padconf(بنية proc * caller, پیام *m_ptr);
#إذا ! USE_PADCONF
#تعریف_کن do_padconf عدم
#پایان_اگر

#پایان_اگر	/* SYSTEM_H */

