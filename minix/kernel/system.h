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

#నిర్వచితం_కాకపోతే SYSTEM_H
#నిర్వచించు SYSTEM_H

#అన్తర్భావ "kernel/kernel.h"

పూర్ణాంకం do_exec(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_EXEC
#నిర్వచించు do_exec ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_fork(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_FORK
#నిర్వచించు do_fork ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_clear(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_CLEAR
#నిర్వచించు do_clear ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_trace(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_TRACE
#నిర్వచించు do_trace ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_runctl(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_RUNCTL
#నిర్వచించు do_runctl ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_update(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_UPDATE
#నిర్వచించు do_update ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_exit(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_EXIT
#నిర్వచించు do_exit ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_copy(నిర్మాణం proc * caller, సందేశం *m_ptr);
#నిర్వచించు do_vircopy 	do_copy
#ఒకవేళ ! (USE_VIRCOPY || USE_PHYSCOPY)
#నిర్వచించు do_copy ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_umap(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_UMAP
#నిర్వచించు do_umap ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_umap_remote(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_UMAP_REMOTE
#నిర్వచించు do_umap_remote ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_vumap(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_VUMAP
#నిర్వచించు do_vumap ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_memset(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_MEMSET
#నిర్వచించు do_memset ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_abort(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_ABORT
#నిర్వచించు do_abort ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_getinfo(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_GETINFO
#నిర్వచించు do_getinfo ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_privctl(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_PRIVCTL
#నిర్వచించు do_privctl ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_irqctl(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_IRQCTL
#నిర్వచించు do_irqctl ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_devio(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_DEVIO
#నిర్వచించు do_devio ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_vdevio(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_VDEVIO
#నిర్వచించు do_vdevio ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_sdevio(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_SDEVIO
#నిర్వచించు do_sdevio ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_kill(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_KILL
#నిర్వచించు do_kill ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_getksig(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_GETKSIG
#నిర్వచించు do_getksig ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_endksig(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_ENDKSIG
#నిర్వచించు do_endksig ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_sigsend(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_SIGSEND
#నిర్వచించు do_sigsend ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_sigreturn(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_SIGRETURN
#నిర్వచించు do_sigreturn ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_times(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_TIMES
#నిర్వచించు do_times ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_setalarm(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_SETALARM
#నిర్వచించు do_setalarm ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_stime(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_settime(నిర్మాణం proc * caller, సందేశం *m_ptr);

పూర్ణాంకం do_vtimer(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_VTIMER
#నిర్వచించు do_vtimer ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_safecopy_గమ్యం(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_safecopy_from(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_vsafecopy(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_iopenable(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_vmctl(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_setgrant(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_readbios(నిర్మాణం proc * caller, సందేశం *m_ptr);

పూర్ణాంకం do_safememset(నిర్మాణం proc * caller, సందేశం *m_ptr);

పూర్ణాంకం do_sprofile(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! SPROFILE
#నిర్వచించు do_sprofile ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_getmcontext(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_setmcontext(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_MCONTEXT
#నిర్వచించు do_getmcontext ఖాళీ
#నిర్వచించు do_setmcontext ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_కాలపట్టిక(నిర్మాణం proc * caller, సందేశం *m_ptr);
పూర్ణాంకం do_schedctl(నిర్మాణం proc * caller, సందేశం *m_ptr);

పూర్ణాంకం do_statectl(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_STATECTL
#నిర్వచించు do_statectl ఖాళీ
#అయితే_ముగింపు

పూర్ణాంకం do_padconf(నిర్మాణం proc * caller, సందేశం *m_ptr);
#ఒకవేళ ! USE_PADCONF
#నిర్వచించు do_padconf ఖాళీ
#అయితే_ముగింపు

#అయితే_ముగింపు	/* SYSTEM_H */

