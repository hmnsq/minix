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

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

ಪೂರ್ಣಾಙ್ಕ do_exec(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_EXEC
#define do_exec ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_fork(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_FORK
#define do_fork ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_clear(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_CLEAR
#define do_clear ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_trace(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_TRACE
#define do_trace ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_runctl(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_RUNCTL
#define do_runctl ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_update(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_UPDATE
#define do_update ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_exit(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_EXIT
#define do_exit ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_copy(ಸಂರಚನಾ proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#ಯದಿ ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_umap(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_UMAP
#define do_umap ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_umap_remote(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_UMAP_REMOTE
#define do_umap_remote ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_vumap(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_VUMAP
#define do_vumap ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_memset(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_MEMSET
#define do_memset ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_abort(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_ABORT
#define do_abort ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_getinfo(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_GETINFO
#define do_getinfo ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_privctl(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_PRIVCTL
#define do_privctl ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_irqctl(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_IRQCTL
#define do_irqctl ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_devio(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_DEVIO
#define do_devio ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_vdevio(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_VDEVIO
#define do_vdevio ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_sdevio(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_SDEVIO
#define do_sdevio ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_kill(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_KILL
#define do_kill ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_getksig(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_GETKSIG
#define do_getksig ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_endksig(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_ENDKSIG
#define do_endksig ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_sigsend(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_SIGSEND
#define do_sigsend ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_sigreturn(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_SIGRETURN
#define do_sigreturn ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_times(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_TIMES
#define do_times ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_setalarm(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_SETALARM
#define do_setalarm ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_stime(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_settime(ಸಂರಚನಾ proc * caller, message *m_ptr);

ಪೂರ್ಣಾಙ್ಕ do_vtimer(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_VTIMER
#define do_vtimer ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_safecopy_to(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_safecopy_from(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_vsafecopy(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_iopenable(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_vmctl(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_setgrant(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_readbios(ಸಂರಚನಾ proc * caller, message *m_ptr);

ಪೂರ್ಣಾಙ್ಕ do_safememset(ಸಂರಚನಾ proc * caller, message *m_ptr);

ಪೂರ್ಣಾಙ್ಕ do_sprofile(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! SPROFILE
#define do_sprofile ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_getmcontext(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_setmcontext(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_MCONTEXT
#define do_getmcontext ರಿಕ್ತ
#define do_setmcontext ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_schedule(ಸಂರಚನಾ proc * caller, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ do_schedctl(ಸಂರಚನಾ proc * caller, message *m_ptr);

ಪೂರ್ಣಾಙ್ಕ do_statectl(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_STATECTL
#define do_statectl ರಿಕ್ತ
#endif

ಪೂರ್ಣಾಙ್ಕ do_padconf(ಸಂರಚನಾ proc * caller, message *m_ptr);
#ಯದಿ ! USE_PADCONF
#define do_padconf ರಿಕ್ತ
#endif

#endif	/* SYSTEM_H */

