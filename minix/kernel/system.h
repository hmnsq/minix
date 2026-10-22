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

#அந்தர்பாவ "kernel/kernel.h"

முழு do_exec(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_EXEC
#define do_exec வெற்று
#endif

முழு do_fork(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_FORK
#define do_fork வெற்று
#endif

முழு do_clear(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_CLEAR
#define do_clear வெற்று
#endif

முழு do_trace(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_TRACE
#define do_trace வெற்று
#endif

முழு do_runctl(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_RUNCTL
#define do_runctl வெற்று
#endif

முழு do_update(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_UPDATE
#define do_update வெற்று
#endif

முழு do_exit(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_EXIT
#define do_exit வெற்று
#endif

முழு do_copy(கட்டமைப்பு proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#எனில் ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy வெற்று
#endif

முழு do_umap(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_UMAP
#define do_umap வெற்று
#endif

முழு do_umap_remote(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_UMAP_REMOTE
#define do_umap_remote வெற்று
#endif

முழு do_vumap(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_VUMAP
#define do_vumap வெற்று
#endif

முழு do_memset(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_MEMSET
#define do_memset வெற்று
#endif

முழு do_abort(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_ABORT
#define do_abort வெற்று
#endif

முழு do_getinfo(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_GETINFO
#define do_getinfo வெற்று
#endif

முழு do_privctl(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_PRIVCTL
#define do_privctl வெற்று
#endif

முழு do_irqctl(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_IRQCTL
#define do_irqctl வெற்று
#endif

முழு do_devio(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_DEVIO
#define do_devio வெற்று
#endif

முழு do_vdevio(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_VDEVIO
#define do_vdevio வெற்று
#endif

முழு do_sdevio(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_SDEVIO
#define do_sdevio வெற்று
#endif

முழு do_kill(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_KILL
#define do_kill வெற்று
#endif

முழு do_getksig(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_GETKSIG
#define do_getksig வெற்று
#endif

முழு do_endksig(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_ENDKSIG
#define do_endksig வெற்று
#endif

முழு do_sigsend(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_SIGSEND
#define do_sigsend வெற்று
#endif

முழு do_sigreturn(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_SIGRETURN
#define do_sigreturn வெற்று
#endif

முழு do_times(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_TIMES
#define do_times வெற்று
#endif

முழு do_setalarm(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_SETALARM
#define do_setalarm வெற்று
#endif

முழு do_stime(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_settime(கட்டமைப்பு proc * caller, message *m_ptr);

முழு do_vtimer(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_VTIMER
#define do_vtimer வெற்று
#endif

முழு do_safecopy_to(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_safecopy_from(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_vsafecopy(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_iopenable(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_vmctl(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_setgrant(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_readbios(கட்டமைப்பு proc * caller, message *m_ptr);

முழு do_safememset(கட்டமைப்பு proc * caller, message *m_ptr);

முழு do_sprofile(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! SPROFILE
#define do_sprofile வெற்று
#endif

முழு do_getmcontext(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_setmcontext(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_MCONTEXT
#define do_getmcontext வெற்று
#define do_setmcontext வெற்று
#endif

முழு do_schedule(கட்டமைப்பு proc * caller, message *m_ptr);
முழு do_schedctl(கட்டமைப்பு proc * caller, message *m_ptr);

முழு do_statectl(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_STATECTL
#define do_statectl வெற்று
#endif

முழு do_padconf(கட்டமைப்பு proc * caller, message *m_ptr);
#எனில் ! USE_PADCONF
#define do_padconf வெற்று
#endif

#endif	/* SYSTEM_H */

