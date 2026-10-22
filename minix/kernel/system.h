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

#यदि_अपरिभाषितम् SYSTEM_H
#परिभाषय SYSTEM_H

#अन्तर्भाव "kernel/kernel.h"

पूर्णाङ्क do_exec(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_EXEC
#परिभाषय do_exec रिक्त
#यदि_अन्तः

पूर्णाङ्क do_fork(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_FORK
#परिभाषय do_fork रिक्त
#यदि_अन्तः

पूर्णाङ्क do_clear(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_CLEAR
#परिभाषय do_clear रिक्त
#यदि_अन्तः

पूर्णाङ्क do_trace(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_TRACE
#परिभाषय do_trace रिक्त
#यदि_अन्तः

पूर्णाङ्क do_runctl(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_RUNCTL
#परिभाषय do_runctl रिक्त
#यदि_अन्तः

पूर्णाङ्क do_update(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_UPDATE
#परिभाषय do_update रिक्त
#यदि_अन्तः

पूर्णाङ्क do_exit(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_EXIT
#परिभाषय do_exit रिक्त
#यदि_अन्तः

पूर्णाङ्क do_copy(संरचना proc * caller, सन्देशः *m_ptr);
#परिभाषय do_vircopy 	do_copy
#यदि ! (USE_VIRCOPY || USE_PHYSCOPY)
#परिभाषय do_copy रिक्त
#यदि_अन्तः

पूर्णाङ्क do_umap(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_UMAP
#परिभाषय do_umap रिक्त
#यदि_अन्तः

पूर्णाङ्क do_umap_remote(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_UMAP_REMOTE
#परिभाषय do_umap_remote रिक्त
#यदि_अन्तः

पूर्णाङ्क do_vumap(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_VUMAP
#परिभाषय do_vumap रिक्त
#यदि_अन्तः

पूर्णाङ्क do_memset(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_MEMSET
#परिभाषय do_memset रिक्त
#यदि_अन्तः

पूर्णाङ्क do_abort(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_ABORT
#परिभाषय do_abort रिक्त
#यदि_अन्तः

पूर्णाङ्क do_getinfo(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_GETINFO
#परिभाषय do_getinfo रिक्त
#यदि_अन्तः

पूर्णाङ्क do_privctl(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_PRIVCTL
#परिभाषय do_privctl रिक्त
#यदि_अन्तः

पूर्णाङ्क do_irqctl(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_IRQCTL
#परिभाषय do_irqctl रिक्त
#यदि_अन्तः

पूर्णाङ्क do_devio(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_DEVIO
#परिभाषय do_devio रिक्त
#यदि_अन्तः

पूर्णाङ्क do_vdevio(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_VDEVIO
#परिभाषय do_vdevio रिक्त
#यदि_अन्तः

पूर्णाङ्क do_sdevio(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_SDEVIO
#परिभाषय do_sdevio रिक्त
#यदि_अन्तः

पूर्णाङ्क do_kill(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_KILL
#परिभाषय do_kill रिक्त
#यदि_अन्तः

पूर्णाङ्क do_getksig(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_GETKSIG
#परिभाषय do_getksig रिक्त
#यदि_अन्तः

पूर्णाङ्क do_endksig(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_ENDKSIG
#परिभाषय do_endksig रिक्त
#यदि_अन्तः

पूर्णाङ्क do_sigsend(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_SIGSEND
#परिभाषय do_sigsend रिक्त
#यदि_अन्तः

पूर्णाङ्क do_sigreturn(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_SIGRETURN
#परिभाषय do_sigreturn रिक्त
#यदि_अन्तः

पूर्णाङ्क do_times(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_TIMES
#परिभाषय do_times रिक्त
#यदि_अन्तः

पूर्णाङ्क do_setalarm(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_SETALARM
#परिभाषय do_setalarm रिक्त
#यदि_अन्तः

पूर्णाङ्क do_stime(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_settime(संरचना proc * caller, सन्देशः *m_ptr);

पूर्णाङ्क do_vtimer(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_VTIMER
#परिभाषय do_vtimer रिक्त
#यदि_अन्तः

पूर्णाङ्क do_safecopy_गन्तव्यम्(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_safecopy_from(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_vsafecopy(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_iopenable(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_vmctl(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_setgrant(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_readbios(संरचना proc * caller, सन्देशः *m_ptr);

पूर्णाङ्क do_safememset(संरचना proc * caller, सन्देशः *m_ptr);

पूर्णाङ्क do_sprofile(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! SPROFILE
#परिभाषय do_sprofile रिक्त
#यदि_अन्तः

पूर्णाङ्क do_getmcontext(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_setmcontext(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_MCONTEXT
#परिभाषय do_getmcontext रिक्त
#परिभाषय do_setmcontext रिक्त
#यदि_अन्तः

पूर्णाङ्क do_कालसारणी(संरचना proc * caller, सन्देशः *m_ptr);
पूर्णाङ्क do_schedctl(संरचना proc * caller, सन्देशः *m_ptr);

पूर्णाङ्क do_statectl(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_STATECTL
#परिभाषय do_statectl रिक्त
#यदि_अन्तः

पूर्णाङ्क do_padconf(संरचना proc * caller, सन्देशः *m_ptr);
#यदि ! USE_PADCONF
#परिभाषय do_padconf रिक्त
#यदि_अन्तः

#यदि_अन्तः	/* SYSTEM_H */

