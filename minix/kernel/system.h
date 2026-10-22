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

#অন্তর্ভাৱ "kernel/kernel.h"

পূর্ণাঙ্ক do_exec(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_EXEC
#define do_exec রিক্ত
#endif

পূর্ণাঙ্ক do_fork(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_FORK
#define do_fork রিক্ত
#endif

পূর্ণাঙ্ক do_clear(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_CLEAR
#define do_clear রিক্ত
#endif

পূর্ণাঙ্ক do_trace(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_TRACE
#define do_trace রিক্ত
#endif

পূর্ণাঙ্ক do_runctl(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_RUNCTL
#define do_runctl রিক্ত
#endif

পূর্ণাঙ্ক do_update(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_UPDATE
#define do_update রিক্ত
#endif

পূর্ণাঙ্ক do_exit(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_EXIT
#define do_exit রিক্ত
#endif

পূর্ণাঙ্ক do_copy(সংরচনা proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#যদি ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy রিক্ত
#endif

পূর্ণাঙ্ক do_umap(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_UMAP
#define do_umap রিক্ত
#endif

পূর্ণাঙ্ক do_umap_remote(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_UMAP_REMOTE
#define do_umap_remote রিক্ত
#endif

পূর্ণাঙ্ক do_vumap(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_VUMAP
#define do_vumap রিক্ত
#endif

পূর্ণাঙ্ক do_memset(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_MEMSET
#define do_memset রিক্ত
#endif

পূর্ণাঙ্ক do_abort(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_ABORT
#define do_abort রিক্ত
#endif

পূর্ণাঙ্ক do_getinfo(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_GETINFO
#define do_getinfo রিক্ত
#endif

পূর্ণাঙ্ক do_privctl(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_PRIVCTL
#define do_privctl রিক্ত
#endif

পূর্ণাঙ্ক do_irqctl(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_IRQCTL
#define do_irqctl রিক্ত
#endif

পূর্ণাঙ্ক do_devio(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_DEVIO
#define do_devio রিক্ত
#endif

পূর্ণাঙ্ক do_vdevio(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_VDEVIO
#define do_vdevio রিক্ত
#endif

পূর্ণাঙ্ক do_sdevio(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_SDEVIO
#define do_sdevio রিক্ত
#endif

পূর্ণাঙ্ক do_kill(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_KILL
#define do_kill রিক্ত
#endif

পূর্ণাঙ্ক do_getksig(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_GETKSIG
#define do_getksig রিক্ত
#endif

পূর্ণাঙ্ক do_endksig(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_ENDKSIG
#define do_endksig রিক্ত
#endif

পূর্ণাঙ্ক do_sigsend(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_SIGSEND
#define do_sigsend রিক্ত
#endif

পূর্ণাঙ্ক do_sigreturn(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_SIGRETURN
#define do_sigreturn রিক্ত
#endif

পূর্ণাঙ্ক do_times(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_TIMES
#define do_times রিক্ত
#endif

পূর্ণাঙ্ক do_setalarm(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_SETALARM
#define do_setalarm রিক্ত
#endif

পূর্ণাঙ্ক do_stime(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_settime(সংরচনা proc * caller, message *m_ptr);

পূর্ণাঙ্ক do_vtimer(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_VTIMER
#define do_vtimer রিক্ত
#endif

পূর্ণাঙ্ক do_safecopy_to(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_safecopy_from(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_vsafecopy(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_iopenable(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_vmctl(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_setgrant(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_readbios(সংরচনা proc * caller, message *m_ptr);

পূর্ণাঙ্ক do_safememset(সংরচনা proc * caller, message *m_ptr);

পূর্ণাঙ্ক do_sprofile(সংরচনা proc * caller, message *m_ptr);
#যদি ! SPROFILE
#define do_sprofile রিক্ত
#endif

পূর্ণাঙ্ক do_getmcontext(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_setmcontext(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_MCONTEXT
#define do_getmcontext রিক্ত
#define do_setmcontext রিক্ত
#endif

পূর্ণাঙ্ক do_schedule(সংরচনা proc * caller, message *m_ptr);
পূর্ণাঙ্ক do_schedctl(সংরচনা proc * caller, message *m_ptr);

পূর্ণাঙ্ক do_statectl(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_STATECTL
#define do_statectl রিক্ত
#endif

পূর্ণাঙ্ক do_padconf(সংরচনা proc * caller, message *m_ptr);
#যদি ! USE_PADCONF
#define do_padconf রিক্ত
#endif

#endif	/* SYSTEM_H */

