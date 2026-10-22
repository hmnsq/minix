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

#اگر_غیر_تعریف_شدہ SYSTEM_H
#تعریف_کرو SYSTEM_H

#شامل "kernel/kernel.h"

صحیح do_exec(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_EXEC
#تعریف_کرو do_exec تہی
#اگر_ختم

صحیح do_fork(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_FORK
#تعریف_کرو do_fork تہی
#اگر_ختم

صحیح do_clear(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_CLEAR
#تعریف_کرو do_clear تہی
#اگر_ختم

صحیح do_trace(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_TRACE
#تعریف_کرو do_trace تہی
#اگر_ختم

صحیح do_runctl(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_RUNCTL
#تعریف_کرو do_runctl تہی
#اگر_ختم

صحیح do_update(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_UPDATE
#تعریف_کرو do_update تہی
#اگر_ختم

صحیح do_exit(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_EXIT
#تعریف_کرو do_exit تہی
#اگر_ختم

صحیح do_copy(ساخت proc * caller, پیغام *m_ptr);
#تعریف_کرو do_vircopy 	do_copy
#اگر ! (USE_VIRCOPY || USE_PHYSCOPY)
#تعریف_کرو do_copy تہی
#اگر_ختم

صحیح do_umap(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_UMAP
#تعریف_کرو do_umap تہی
#اگر_ختم

صحیح do_umap_remote(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_UMAP_REMOTE
#تعریف_کرو do_umap_remote تہی
#اگر_ختم

صحیح do_vumap(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_VUMAP
#تعریف_کرو do_vumap تہی
#اگر_ختم

صحیح do_memset(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_MEMSET
#تعریف_کرو do_memset تہی
#اگر_ختم

صحیح do_abort(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_ABORT
#تعریف_کرو do_abort تہی
#اگر_ختم

صحیح do_getinfo(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_GETINFO
#تعریف_کرو do_getinfo تہی
#اگر_ختم

صحیح do_privctl(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_PRIVCTL
#تعریف_کرو do_privctl تہی
#اگر_ختم

صحیح do_irqctl(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_IRQCTL
#تعریف_کرو do_irqctl تہی
#اگر_ختم

صحیح do_devio(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_DEVIO
#تعریف_کرو do_devio تہی
#اگر_ختم

صحیح do_vdevio(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_VDEVIO
#تعریف_کرو do_vdevio تہی
#اگر_ختم

صحیح do_sdevio(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_SDEVIO
#تعریف_کرو do_sdevio تہی
#اگر_ختم

صحیح do_kill(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_KILL
#تعریف_کرو do_kill تہی
#اگر_ختم

صحیح do_getksig(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_GETKSIG
#تعریف_کرو do_getksig تہی
#اگر_ختم

صحیح do_endksig(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_ENDKSIG
#تعریف_کرو do_endksig تہی
#اگر_ختم

صحیح do_sigsend(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_SIGSEND
#تعریف_کرو do_sigsend تہی
#اگر_ختم

صحیح do_sigreturn(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_SIGRETURN
#تعریف_کرو do_sigreturn تہی
#اگر_ختم

صحیح do_times(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_TIMES
#تعریف_کرو do_times تہی
#اگر_ختم

صحیح do_setalarm(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_SETALARM
#تعریف_کرو do_setalarm تہی
#اگر_ختم

صحیح do_stime(ساخت proc * caller, پیغام *m_ptr);
صحیح do_settime(ساخت proc * caller, پیغام *m_ptr);

صحیح do_vtimer(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_VTIMER
#تعریف_کرو do_vtimer تہی
#اگر_ختم

صحیح do_safecopy_مقصد(ساخت proc * caller, پیغام *m_ptr);
صحیح do_safecopy_from(ساخت proc * caller, پیغام *m_ptr);
صحیح do_vsafecopy(ساخت proc * caller, پیغام *m_ptr);
صحیح do_iopenable(ساخت proc * caller, پیغام *m_ptr);
صحیح do_vmctl(ساخت proc * caller, پیغام *m_ptr);
صحیح do_setgrant(ساخت proc * caller, پیغام *m_ptr);
صحیح do_readbios(ساخت proc * caller, پیغام *m_ptr);

صحیح do_safememset(ساخت proc * caller, پیغام *m_ptr);

صحیح do_sprofile(ساخت proc * caller, پیغام *m_ptr);
#اگر ! SPROFILE
#تعریف_کرو do_sprofile تہی
#اگر_ختم

صحیح do_getmcontext(ساخت proc * caller, پیغام *m_ptr);
صحیح do_setmcontext(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_MCONTEXT
#تعریف_کرو do_getmcontext تہی
#تعریف_کرو do_setmcontext تہی
#اگر_ختم

صحیح do_جدول_وقت(ساخت proc * caller, پیغام *m_ptr);
صحیح do_schedctl(ساخت proc * caller, پیغام *m_ptr);

صحیح do_statectl(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_STATECTL
#تعریف_کرو do_statectl تہی
#اگر_ختم

صحیح do_padconf(ساخت proc * caller, پیغام *m_ptr);
#اگر ! USE_PADCONF
#تعریف_کرو do_padconf تہی
#اگر_ختم

#اگر_ختم	/* SYSTEM_H */

