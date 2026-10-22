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

#که_نه_تعریف_شوی SYSTEM_H
#تعریف_کړه SYSTEM_H

#ضمن "kernel/kernel.h"

صحيح do_exec(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_EXEC
#تعریف_کړه do_exec عدم
#د_که_پای

صحيح do_fork(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_FORK
#تعریف_کړه do_fork عدم
#د_که_پای

صحيح do_clear(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_CLEAR
#تعریف_کړه do_clear عدم
#د_که_پای

صحيح do_trace(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_TRACE
#تعریف_کړه do_trace عدم
#د_که_پای

صحيح do_runctl(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_RUNCTL
#تعریف_کړه do_runctl عدم
#د_که_پای

صحيح do_update(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_UPDATE
#تعریف_کړه do_update عدم
#د_که_پای

صحيح do_exit(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_EXIT
#تعریف_کړه do_exit عدم
#د_که_پای

صحيح do_copy(بنية proc * caller, پیغام *m_ptr);
#تعریف_کړه do_vircopy 	do_copy
#إذا ! (USE_VIRCOPY || USE_PHYSCOPY)
#تعریف_کړه do_copy عدم
#د_که_پای

صحيح do_umap(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_UMAP
#تعریف_کړه do_umap عدم
#د_که_پای

صحيح do_umap_remote(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_UMAP_REMOTE
#تعریف_کړه do_umap_remote عدم
#د_که_پای

صحيح do_vumap(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_VUMAP
#تعریف_کړه do_vumap عدم
#د_که_پای

صحيح do_memset(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_MEMSET
#تعریف_کړه do_memset عدم
#د_که_پای

صحيح do_abort(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_ABORT
#تعریف_کړه do_abort عدم
#د_که_پای

صحيح do_getinfo(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_GETINFO
#تعریف_کړه do_getinfo عدم
#د_که_پای

صحيح do_privctl(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_PRIVCTL
#تعریف_کړه do_privctl عدم
#د_که_پای

صحيح do_irqctl(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_IRQCTL
#تعریف_کړه do_irqctl عدم
#د_که_پای

صحيح do_devio(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_DEVIO
#تعریف_کړه do_devio عدم
#د_که_پای

صحيح do_vdevio(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_VDEVIO
#تعریف_کړه do_vdevio عدم
#د_که_پای

صحيح do_sdevio(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_SDEVIO
#تعریف_کړه do_sdevio عدم
#د_که_پای

صحيح do_kill(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_KILL
#تعریف_کړه do_kill عدم
#د_که_پای

صحيح do_getksig(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_GETKSIG
#تعریف_کړه do_getksig عدم
#د_که_پای

صحيح do_endksig(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_ENDKSIG
#تعریف_کړه do_endksig عدم
#د_که_پای

صحيح do_sigsend(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_SIGSEND
#تعریف_کړه do_sigsend عدم
#د_که_پای

صحيح do_sigreturn(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_SIGRETURN
#تعریف_کړه do_sigreturn عدم
#د_که_پای

صحيح do_times(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_TIMES
#تعریف_کړه do_times عدم
#د_که_پای

صحيح do_setalarm(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_SETALARM
#تعریف_کړه do_setalarm عدم
#د_که_پای

صحيح do_stime(بنية proc * caller, پیغام *m_ptr);
صحيح do_settime(بنية proc * caller, پیغام *m_ptr);

صحيح do_vtimer(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_VTIMER
#تعریف_کړه do_vtimer عدم
#د_که_پای

صحيح do_safecopy_منزل(بنية proc * caller, پیغام *m_ptr);
صحيح do_safecopy_from(بنية proc * caller, پیغام *m_ptr);
صحيح do_vsafecopy(بنية proc * caller, پیغام *m_ptr);
صحيح do_iopenable(بنية proc * caller, پیغام *m_ptr);
صحيح do_vmctl(بنية proc * caller, پیغام *m_ptr);
صحيح do_setgrant(بنية proc * caller, پیغام *m_ptr);
صحيح do_readbios(بنية proc * caller, پیغام *m_ptr);

صحيح do_safememset(بنية proc * caller, پیغام *m_ptr);

صحيح do_sprofile(بنية proc * caller, پیغام *m_ptr);
#إذا ! SPROFILE
#تعریف_کړه do_sprofile عدم
#د_که_پای

صحيح do_getmcontext(بنية proc * caller, پیغام *m_ptr);
صحيح do_setmcontext(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_MCONTEXT
#تعریف_کړه do_getmcontext عدم
#تعریف_کړه do_setmcontext عدم
#د_که_پای

صحيح do_مهالوېش(بنية proc * caller, پیغام *m_ptr);
صحيح do_schedctl(بنية proc * caller, پیغام *m_ptr);

صحيح do_statectl(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_STATECTL
#تعریف_کړه do_statectl عدم
#د_که_پای

صحيح do_padconf(بنية proc * caller, پیغام *m_ptr);
#إذا ! USE_PADCONF
#تعریف_کړه do_padconf عدم
#د_که_پای

#د_که_پای	/* SYSTEM_H */

