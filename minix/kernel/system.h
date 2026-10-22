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

#অন্তর্ভাব "kernel/kernel.h"

পূর্ণসংখ্যা do_exec(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_EXEC
#define do_exec ফাঁকা
#endif

পূর্ণসংখ্যা do_fork(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_FORK
#define do_fork ফাঁকা
#endif

পূর্ণসংখ্যা do_clear(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_CLEAR
#define do_clear ফাঁকা
#endif

পূর্ণসংখ্যা do_trace(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_TRACE
#define do_trace ফাঁকা
#endif

পূর্ণসংখ্যা do_runctl(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_RUNCTL
#define do_runctl ফাঁকা
#endif

পূর্ণসংখ্যা do_update(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_UPDATE
#define do_update ফাঁকা
#endif

পূর্ণসংখ্যা do_exit(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_EXIT
#define do_exit ফাঁকা
#endif

পূর্ণসংখ্যা do_copy(কাঠামো proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#যদি ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy ফাঁকা
#endif

পূর্ণসংখ্যা do_umap(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_UMAP
#define do_umap ফাঁকা
#endif

পূর্ণসংখ্যা do_umap_remote(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_UMAP_REMOTE
#define do_umap_remote ফাঁকা
#endif

পূর্ণসংখ্যা do_vumap(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_VUMAP
#define do_vumap ফাঁকা
#endif

পূর্ণসংখ্যা do_memset(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_MEMSET
#define do_memset ফাঁকা
#endif

পূর্ণসংখ্যা do_abort(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_ABORT
#define do_abort ফাঁকা
#endif

পূর্ণসংখ্যা do_getinfo(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_GETINFO
#define do_getinfo ফাঁকা
#endif

পূর্ণসংখ্যা do_privctl(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_PRIVCTL
#define do_privctl ফাঁকা
#endif

পূর্ণসংখ্যা do_irqctl(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_IRQCTL
#define do_irqctl ফাঁকা
#endif

পূর্ণসংখ্যা do_devio(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_DEVIO
#define do_devio ফাঁকা
#endif

পূর্ণসংখ্যা do_vdevio(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_VDEVIO
#define do_vdevio ফাঁকা
#endif

পূর্ণসংখ্যা do_sdevio(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_SDEVIO
#define do_sdevio ফাঁকা
#endif

পূর্ণসংখ্যা do_kill(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_KILL
#define do_kill ফাঁকা
#endif

পূর্ণসংখ্যা do_getksig(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_GETKSIG
#define do_getksig ফাঁকা
#endif

পূর্ণসংখ্যা do_endksig(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_ENDKSIG
#define do_endksig ফাঁকা
#endif

পূর্ণসংখ্যা do_sigsend(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_SIGSEND
#define do_sigsend ফাঁকা
#endif

পূর্ণসংখ্যা do_sigreturn(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_SIGRETURN
#define do_sigreturn ফাঁকা
#endif

পূর্ণসংখ্যা do_times(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_TIMES
#define do_times ফাঁকা
#endif

পূর্ণসংখ্যা do_setalarm(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_SETALARM
#define do_setalarm ফাঁকা
#endif

পূর্ণসংখ্যা do_stime(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_settime(কাঠামো proc * caller, message *m_ptr);

পূর্ণসংখ্যা do_vtimer(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_VTIMER
#define do_vtimer ফাঁকা
#endif

পূর্ণসংখ্যা do_safecopy_to(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_safecopy_from(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_vsafecopy(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_iopenable(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_vmctl(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_setgrant(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_readbios(কাঠামো proc * caller, message *m_ptr);

পূর্ণসংখ্যা do_safememset(কাঠামো proc * caller, message *m_ptr);

পূর্ণসংখ্যা do_sprofile(কাঠামো proc * caller, message *m_ptr);
#যদি ! SPROFILE
#define do_sprofile ফাঁকা
#endif

পূর্ণসংখ্যা do_getmcontext(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_setmcontext(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_MCONTEXT
#define do_getmcontext ফাঁকা
#define do_setmcontext ফাঁকা
#endif

পূর্ণসংখ্যা do_schedule(কাঠামো proc * caller, message *m_ptr);
পূর্ণসংখ্যা do_schedctl(কাঠামো proc * caller, message *m_ptr);

পূর্ণসংখ্যা do_statectl(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_STATECTL
#define do_statectl ফাঁকা
#endif

পূর্ণসংখ্যা do_padconf(কাঠামো proc * caller, message *m_ptr);
#যদি ! USE_PADCONF
#define do_padconf ফাঁকা
#endif

#endif	/* SYSTEM_H */

