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

#അന്തര്ഭാവ "kernel/kernel.h"

പൂർണ്ണസംഖ്യ do_exec(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_EXEC
#define do_exec ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_fork(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_FORK
#define do_fork ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_clear(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_CLEAR
#define do_clear ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_trace(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_TRACE
#define do_trace ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_runctl(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_RUNCTL
#define do_runctl ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_update(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_UPDATE
#define do_update ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_exit(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_EXIT
#define do_exit ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_copy(ഘടന proc * caller, message *m_ptr);
#define do_vircopy 	do_copy
#എങ്കിൽ ! (USE_VIRCOPY || USE_PHYSCOPY)
#define do_copy ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_umap(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_UMAP
#define do_umap ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_umap_remote(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_UMAP_REMOTE
#define do_umap_remote ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_vumap(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_VUMAP
#define do_vumap ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_memset(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_MEMSET
#define do_memset ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_abort(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_ABORT
#define do_abort ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_getinfo(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_GETINFO
#define do_getinfo ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_privctl(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_PRIVCTL
#define do_privctl ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_irqctl(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_IRQCTL
#define do_irqctl ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_devio(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_DEVIO
#define do_devio ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_vdevio(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_VDEVIO
#define do_vdevio ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_sdevio(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_SDEVIO
#define do_sdevio ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_kill(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_KILL
#define do_kill ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_getksig(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_GETKSIG
#define do_getksig ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_endksig(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_ENDKSIG
#define do_endksig ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_sigsend(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_SIGSEND
#define do_sigsend ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_sigreturn(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_SIGRETURN
#define do_sigreturn ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_times(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_TIMES
#define do_times ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_setalarm(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_SETALARM
#define do_setalarm ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_stime(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_settime(ഘടന proc * caller, message *m_ptr);

പൂർണ്ണസംഖ്യ do_vtimer(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_VTIMER
#define do_vtimer ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_safecopy_to(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_safecopy_from(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_vsafecopy(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_iopenable(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_vmctl(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_setgrant(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_readbios(ഘടന proc * caller, message *m_ptr);

പൂർണ്ണസംഖ്യ do_safememset(ഘടന proc * caller, message *m_ptr);

പൂർണ്ണസംഖ്യ do_sprofile(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! SPROFILE
#define do_sprofile ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_getmcontext(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_setmcontext(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_MCONTEXT
#define do_getmcontext ശൂന്യ
#define do_setmcontext ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_schedule(ഘടന proc * caller, message *m_ptr);
പൂർണ്ണസംഖ്യ do_schedctl(ഘടന proc * caller, message *m_ptr);

പൂർണ്ണസംഖ്യ do_statectl(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_STATECTL
#define do_statectl ശൂന്യ
#endif

പൂർണ്ണസംഖ്യ do_padconf(ഘടന proc * caller, message *m_ptr);
#എങ്കിൽ ! USE_PADCONF
#define do_padconf ശൂന്യ
#endif

#endif	/* SYSTEM_H */

