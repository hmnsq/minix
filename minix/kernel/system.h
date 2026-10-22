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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYSTEM_H
#ವ್ಯಾಖ್ಯಾನಿಸು SYSTEM_H

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

ಪೂರ್ಣಾಂಕ do_exec(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_EXEC
#ವ್ಯಾಖ್ಯಾನಿಸು do_exec ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_fork(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_FORK
#ವ್ಯಾಖ್ಯಾನಿಸು do_fork ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_clear(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_CLEAR
#ವ್ಯಾಖ್ಯಾನಿಸು do_clear ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_trace(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_TRACE
#ವ್ಯಾಖ್ಯಾನಿಸು do_trace ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_runctl(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_RUNCTL
#ವ್ಯಾಖ್ಯಾನಿಸು do_runctl ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_update(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_UPDATE
#ವ್ಯಾಖ್ಯಾನಿಸು do_update ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_exit(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_EXIT
#ವ್ಯಾಖ್ಯಾನಿಸು do_exit ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_copy(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ವ್ಯಾಖ್ಯಾನಿಸು do_vircopy 	do_copy
#ಆದರೆ ! (USE_VIRCOPY || USE_PHYSCOPY)
#ವ್ಯಾಖ್ಯಾನಿಸು do_copy ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_umap(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_UMAP
#ವ್ಯಾಖ್ಯಾನಿಸು do_umap ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_umap_remote(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_UMAP_REMOTE
#ವ್ಯಾಖ್ಯಾನಿಸು do_umap_remote ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_vumap(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_VUMAP
#ವ್ಯಾಖ್ಯಾನಿಸು do_vumap ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_memset(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_MEMSET
#ವ್ಯಾಖ್ಯಾನಿಸು do_memset ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_abort(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_ABORT
#ವ್ಯಾಖ್ಯಾನಿಸು do_abort ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_getinfo(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_GETINFO
#ವ್ಯಾಖ್ಯಾನಿಸು do_getinfo ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_privctl(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_PRIVCTL
#ವ್ಯಾಖ್ಯಾನಿಸು do_privctl ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_irqctl(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_IRQCTL
#ವ್ಯಾಖ್ಯಾನಿಸು do_irqctl ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_devio(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_DEVIO
#ವ್ಯಾಖ್ಯಾನಿಸು do_devio ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_vdevio(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_VDEVIO
#ವ್ಯಾಖ್ಯಾನಿಸು do_vdevio ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_sdevio(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_SDEVIO
#ವ್ಯಾಖ್ಯಾನಿಸು do_sdevio ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_kill(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_KILL
#ವ್ಯಾಖ್ಯಾನಿಸು do_kill ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_getksig(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_GETKSIG
#ವ್ಯಾಖ್ಯಾನಿಸು do_getksig ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_endksig(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_ENDKSIG
#ವ್ಯಾಖ್ಯಾನಿಸು do_endksig ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_sigsend(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_SIGSEND
#ವ್ಯಾಖ್ಯಾನಿಸು do_sigsend ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_sigreturn(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_SIGRETURN
#ವ್ಯಾಖ್ಯಾನಿಸು do_sigreturn ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_times(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_TIMES
#ವ್ಯಾಖ್ಯಾನಿಸು do_times ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_setalarm(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_SETALARM
#ವ್ಯಾಖ್ಯಾನಿಸು do_setalarm ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_stime(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_settime(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);

ಪೂರ್ಣಾಂಕ do_vtimer(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_VTIMER
#ವ್ಯಾಖ್ಯಾನಿಸು do_vtimer ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_safecopy_ಗಮ್ಯ(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_safecopy_from(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_vsafecopy(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_iopenable(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_vmctl(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_setgrant(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_readbios(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);

ಪೂರ್ಣಾಂಕ do_safememset(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);

ಪೂರ್ಣಾಂಕ do_sprofile(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! SPROFILE
#ವ್ಯಾಖ್ಯಾನಿಸು do_sprofile ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_getmcontext(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_setmcontext(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_MCONTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು do_getmcontext ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು do_setmcontext ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_ವೇಳಾಪಟ್ಟಿ(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ do_schedctl(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);

ಪೂರ್ಣಾಂಕ do_statectl(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_STATECTL
#ವ್ಯಾಖ್ಯಾನಿಸು do_statectl ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ do_padconf(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);
#ಆದರೆ ! USE_PADCONF
#ವ್ಯಾಖ್ಯಾನಿಸು do_padconf ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ	/* SYSTEM_H */

