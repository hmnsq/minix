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

#અન્તર્ભાવ "kernel/kernel.h"

પૂર્ણાંક do_exec(સંરચના proc * caller, message *m_ptr);
#જો ! USE_EXEC
#define do_exec ખાલી
#endif

પૂર્ણાંક do_fork(સંરચના proc * caller, message *m_ptr);
#જો ! USE_FORK
#define do_fork ખાલી
#endif

પૂર્ણાંક do_clear(સંરચના proc * caller, message *m_ptr);
#જો ! USE_CLEAR
#define do_clear ખાલી
#endif

પૂર્ણાંક do_trace(સંરચના proc * caller, message *m_ptr);
#જો ! USE_TRACE
#define do_trace ખાલી
#endif

પૂર્ણાંક do_runctl(સંરચના proc * caller, message *m_ptr);
#જો ! USE_RUNCTL
#define do_runctl ખાલી
#endif

પૂર્ણાંક do_update(સંરચના proc * caller, message *m_ptr);
#જો ! USE_UPDATE
#define do_update ખાલી
#endif

પૂર્ણાંક do_exit(સંરચના proc * caller, message *m_ptr);
#જો ! USE_EXIT
#define do_exit ખાલી
#endif

પૂર્ણાંક do_copy(સંરચના proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#જો ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy ખાલી
#endif

પૂર્ણાંક do_umap(સંરચના proc * caller, message *m_ptr);
#જો ! USE_UMAP
#define do_umap ખાલી
#endif

પૂર્ણાંક do_umap_remote(સંરચના proc * caller, message *m_ptr);
#જો ! USE_UMAP_REMOTE
#define do_umap_remote ખાલી
#endif

પૂર્ણાંક do_vumap(સંરચના proc * caller, message *m_ptr);
#જો ! USE_VUMAP
#define do_vumap ખાલી
#endif

પૂર્ણાંક do_memset(સંરચના proc * caller, message *m_ptr);
#જો ! USE_MEMSET
#define do_memset ખાલી
#endif

પૂર્ણાંક do_abort(સંરચના proc * caller, message *m_ptr);
#જો ! USE_ABORT
#define do_abort ખાલી
#endif

પૂર્ણાંક do_getinfo(સંરચના proc * caller, message *m_ptr);
#જો ! USE_GETINFO
#define do_getinfo ખાલી
#endif

પૂર્ણાંક do_privctl(સંરચના proc * caller, message *m_ptr);
#જો ! USE_PRIVCTL
#define do_privctl ખાલી
#endif

પૂર્ણાંક do_irqctl(સંરચના proc * caller, message *m_ptr);
#જો ! USE_IRQCTL
#define do_irqctl ખાલી
#endif

પૂર્ણાંક do_devio(સંરચના proc * caller, message *m_ptr);
#જો ! USE_DEVIO
#define do_devio ખાલી
#endif

પૂર્ણાંક do_vdevio(સંરચના proc * caller, message *m_ptr);
#જો ! USE_VDEVIO
#define do_vdevio ખાલી
#endif

પૂર્ણાંક do_sdevio(સંરચના proc * caller, message *m_ptr);
#જો ! USE_SDEVIO
#define do_sdevio ખાલી
#endif

પૂર્ણાંક do_kill(સંરચના proc * caller, message *m_ptr);
#જો ! USE_KILL
#define do_kill ખાલી
#endif

પૂર્ણાંક do_getksig(સંરચના proc * caller, message *m_ptr);
#જો ! USE_GETKSIG
#define do_getksig ખાલી
#endif

પૂર્ણાંક do_endksig(સંરચના proc * caller, message *m_ptr);
#જો ! USE_ENDKSIG
#define do_endksig ખાલી
#endif

પૂર્ણાંક do_sigsend(સંરચના proc * caller, message *m_ptr);
#જો ! USE_SIGSEND
#define do_sigsend ખાલી
#endif

પૂર્ણાંક do_sigreturn(સંરચના proc * caller, message *m_ptr);
#જો ! USE_SIGRETURN
#define do_sigreturn ખાલી
#endif

પૂર્ણાંક do_times(સંરચના proc * caller, message *m_ptr);
#જો ! USE_TIMES
#define do_times ખાલી
#endif

પૂર્ણાંક do_setalarm(સંરચના proc * caller, message *m_ptr);
#જો ! USE_SETALARM
#define do_setalarm ખાલી
#endif

પૂર્ણાંક do_stime(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_settime(સંરચના proc * caller, message *m_ptr);

પૂર્ણાંક do_vtimer(સંરચના proc * caller, message *m_ptr);
#જો ! USE_VTIMER
#define do_vtimer ખાલી
#endif

પૂર્ણાંક do_safecopy_to(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_safecopy_from(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_vsafecopy(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_iopenable(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_vmctl(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_setgrant(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_readbios(સંરચના proc * caller, message *m_ptr);

પૂર્ણાંક do_safememset(સંરચના proc * caller, message *m_ptr);

પૂર્ણાંક do_sprofile(સંરચના proc * caller, message *m_ptr);
#જો ! SPROFILE
#define do_sprofile ખાલી
#endif

પૂર્ણાંક do_getmcontext(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_setmcontext(સંરચના proc * caller, message *m_ptr);
#જો ! USE_MCONTEXT
#define do_getmcontext ખાલી
#define do_setmcontext ખાલી
#endif

પૂર્ણાંક do_schedule(સંરચના proc * caller, message *m_ptr);
પૂર્ણાંક do_schedctl(સંરચના proc * caller, message *m_ptr);

પૂર્ણાંક do_statectl(સંરચના proc * caller, message *m_ptr);
#જો ! USE_STATECTL
#define do_statectl ખાલી
#endif

પૂર્ણાંક do_padconf(સંરચના proc * caller, message *m_ptr);
#જો ! USE_PADCONF
#define do_padconf ખાલી
#endif

#endif	/* SYSTEM_H */

