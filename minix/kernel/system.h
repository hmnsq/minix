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

#جيڪڏهن_وصف_نه_ٿيل SYSTEM_H
#وصف_ڪريو SYSTEM_H

#ضمن "kernel/kernel.h"

صحيح do_exec(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_EXEC
#وصف_ڪريو do_exec عدم
#جيڪڏهن_پورو

صحيح do_fork(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_FORK
#وصف_ڪريو do_fork عدم
#جيڪڏهن_پورو

صحيح do_clear(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_CLEAR
#وصف_ڪريو do_clear عدم
#جيڪڏهن_پورو

صحيح do_trace(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_TRACE
#وصف_ڪريو do_trace عدم
#جيڪڏهن_پورو

صحيح do_runctl(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_RUNCTL
#وصف_ڪريو do_runctl عدم
#جيڪڏهن_پورو

صحيح do_update(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_UPDATE
#وصف_ڪريو do_update عدم
#جيڪڏهن_پورو

صحيح do_exit(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_EXIT
#وصف_ڪريو do_exit عدم
#جيڪڏهن_پورو

صحيح do_copy(بنية proc * caller, پيغام *m_ptr);
#وصف_ڪريو do_vircopy 	do_copy
#إذا ! (USE_VIRCOPY || USE_PHYSCOPY)
#وصف_ڪريو do_copy عدم
#جيڪڏهن_پورو

صحيح do_umap(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_UMAP
#وصف_ڪريو do_umap عدم
#جيڪڏهن_پورو

صحيح do_umap_remote(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_UMAP_REMOTE
#وصف_ڪريو do_umap_remote عدم
#جيڪڏهن_پورو

صحيح do_vumap(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_VUMAP
#وصف_ڪريو do_vumap عدم
#جيڪڏهن_پورو

صحيح do_memset(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_MEMSET
#وصف_ڪريو do_memset عدم
#جيڪڏهن_پورو

صحيح do_abort(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_ABORT
#وصف_ڪريو do_abort عدم
#جيڪڏهن_پورو

صحيح do_getinfo(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_GETINFO
#وصف_ڪريو do_getinfo عدم
#جيڪڏهن_پورو

صحيح do_privctl(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_PRIVCTL
#وصف_ڪريو do_privctl عدم
#جيڪڏهن_پورو

صحيح do_irqctl(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_IRQCTL
#وصف_ڪريو do_irqctl عدم
#جيڪڏهن_پورو

صحيح do_devio(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_DEVIO
#وصف_ڪريو do_devio عدم
#جيڪڏهن_پورو

صحيح do_vdevio(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_VDEVIO
#وصف_ڪريو do_vdevio عدم
#جيڪڏهن_پورو

صحيح do_sdevio(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_SDEVIO
#وصف_ڪريو do_sdevio عدم
#جيڪڏهن_پورو

صحيح do_kill(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_KILL
#وصف_ڪريو do_kill عدم
#جيڪڏهن_پورو

صحيح do_getksig(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_GETKSIG
#وصف_ڪريو do_getksig عدم
#جيڪڏهن_پورو

صحيح do_endksig(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_ENDKSIG
#وصف_ڪريو do_endksig عدم
#جيڪڏهن_پورو

صحيح do_sigsend(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_SIGSEND
#وصف_ڪريو do_sigsend عدم
#جيڪڏهن_پورو

صحيح do_sigreturn(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_SIGRETURN
#وصف_ڪريو do_sigreturn عدم
#جيڪڏهن_پورو

صحيح do_times(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_TIMES
#وصف_ڪريو do_times عدم
#جيڪڏهن_پورو

صحيح do_setalarm(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_SETALARM
#وصف_ڪريو do_setalarm عدم
#جيڪڏهن_پورو

صحيح do_stime(بنية proc * caller, پيغام *m_ptr);
صحيح do_settime(بنية proc * caller, پيغام *m_ptr);

صحيح do_vtimer(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_VTIMER
#وصف_ڪريو do_vtimer عدم
#جيڪڏهن_پورو

صحيح do_safecopy_منزل(بنية proc * caller, پيغام *m_ptr);
صحيح do_safecopy_from(بنية proc * caller, پيغام *m_ptr);
صحيح do_vsafecopy(بنية proc * caller, پيغام *m_ptr);
صحيح do_iopenable(بنية proc * caller, پيغام *m_ptr);
صحيح do_vmctl(بنية proc * caller, پيغام *m_ptr);
صحيح do_setgrant(بنية proc * caller, پيغام *m_ptr);
صحيح do_readbios(بنية proc * caller, پيغام *m_ptr);

صحيح do_safememset(بنية proc * caller, پيغام *m_ptr);

صحيح do_sprofile(بنية proc * caller, پيغام *m_ptr);
#إذا ! SPROFILE
#وصف_ڪريو do_sprofile عدم
#جيڪڏهن_پورو

صحيح do_getmcontext(بنية proc * caller, پيغام *m_ptr);
صحيح do_setmcontext(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_MCONTEXT
#وصف_ڪريو do_getmcontext عدم
#وصف_ڪريو do_setmcontext عدم
#جيڪڏهن_پورو

صحيح do_وقت_جدول(بنية proc * caller, پيغام *m_ptr);
صحيح do_schedctl(بنية proc * caller, پيغام *m_ptr);

صحيح do_statectl(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_STATECTL
#وصف_ڪريو do_statectl عدم
#جيڪڏهن_پورو

صحيح do_padconf(بنية proc * caller, پيغام *m_ptr);
#إذا ! USE_PADCONF
#وصف_ڪريو do_padconf عدم
#جيڪڏهن_پورو

#جيڪڏهن_پورو	/* SYSTEM_H */

