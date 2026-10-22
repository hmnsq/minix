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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYSTEM_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSTEM_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

ਪੂਰਨ do_exec(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_EXEC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_exec ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_fork(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_FORK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_fork ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_clear(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_CLEAR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_clear ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_trace(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_TRACE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_trace ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_runctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_RUNCTL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_runctl ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_update(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_UPDATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_update ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_exit(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_EXIT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_exit ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_copy(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_vircopy 	do_copy
#ਜੇ ! (USE_VIRCOPY || USE_PHYSCOPY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_copy ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_umap(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_UMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_umap ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_umap_remote(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_UMAP_REMOTE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_umap_remote ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_vumap(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_VUMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_vumap ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_memset(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_MEMSET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_memset ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_abort(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_ABORT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_abort ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_getinfo(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_GETINFO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_getinfo ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_privctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_PRIVCTL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_privctl ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_irqctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_IRQCTL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_irqctl ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_devio(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_DEVIO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_devio ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_vdevio(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_VDEVIO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_vdevio ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_sdevio(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_SDEVIO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_sdevio ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_kill(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_KILL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_kill ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_getksig(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_GETKSIG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_getksig ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_endksig(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_ENDKSIG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_endksig ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_sigsend(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_SIGSEND
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_sigsend ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_sigreturn(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_SIGRETURN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_sigreturn ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_times(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_TIMES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_times ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_setalarm(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_SETALARM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_setalarm ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_stime(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_settime(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);

ਪੂਰਨ do_vtimer(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_VTIMER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_vtimer ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_safecopy_ਮੰਜ਼ਿਲ(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_safecopy_from(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_vsafecopy(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_iopenable(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_vmctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_setgrant(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_readbios(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);

ਪੂਰਨ do_safememset(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);

ਪੂਰਨ do_sprofile(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! SPROFILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_sprofile ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_getmcontext(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_setmcontext(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_MCONTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_getmcontext ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_setmcontext ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_ਸਮਾਂ_ਸਾਰਣੀ(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ do_schedctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);

ਪੂਰਨ do_statectl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_STATECTL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_statectl ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਪੂਰਨ do_padconf(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);
#ਜੇ ! USE_PADCONF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ do_padconf ਨਿਰਾ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ	/* SYSTEM_H */

