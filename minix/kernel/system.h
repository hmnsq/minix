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

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_exec(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_EXEC
#define do_exec ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_fork(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_FORK
#define do_fork ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_clear(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_CLEAR
#define do_clear ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_trace(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_TRACE
#define do_trace ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_runctl(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_RUNCTL
#define do_runctl ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_update(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_UPDATE
#define do_update ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_exit(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_EXIT
#define do_exit ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_copy(ସଂରଚନା proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#ଯଦି ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_umap(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_UMAP
#define do_umap ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_umap_remote(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_UMAP_REMOTE
#define do_umap_remote ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vumap(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_VUMAP
#define do_vumap ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_memset(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_MEMSET
#define do_memset ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_abort(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_ABORT
#define do_abort ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_getinfo(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_GETINFO
#define do_getinfo ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_privctl(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_PRIVCTL
#define do_privctl ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_irqctl(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_IRQCTL
#define do_irqctl ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_devio(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_DEVIO
#define do_devio ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vdevio(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_VDEVIO
#define do_vdevio ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_sdevio(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_SDEVIO
#define do_sdevio ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_kill(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_KILL
#define do_kill ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_getksig(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_GETKSIG
#define do_getksig ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_endksig(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_ENDKSIG
#define do_endksig ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_sigsend(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_SIGSEND
#define do_sigsend ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_sigreturn(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_SIGRETURN
#define do_sigreturn ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_times(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_TIMES
#define do_times ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_setalarm(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_SETALARM
#define do_setalarm ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_stime(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_settime(ସଂରଚନା proc * caller, message *m_ptr);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vtimer(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_VTIMER
#define do_vtimer ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_safecopy_to(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_safecopy_from(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vsafecopy(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_iopenable(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vmctl(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_setgrant(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_readbios(ସଂରଚନା proc * caller, message *m_ptr);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_safememset(ସଂରଚନା proc * caller, message *m_ptr);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_sprofile(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! SPROFILE
#define do_sprofile ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_getmcontext(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_setmcontext(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_MCONTEXT
#define do_getmcontext ଖାଲି
#define do_setmcontext ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_schedule(ସଂରଚନା proc * caller, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_schedctl(ସଂରଚନା proc * caller, message *m_ptr);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_statectl(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_STATECTL
#define do_statectl ଖାଲି
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_padconf(ସଂରଚନା proc * caller, message *m_ptr);
#ଯଦି ! USE_PADCONF
#define do_padconf ଖାଲି
#endif

#endif	/* SYSTEM_H */

