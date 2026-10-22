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

#अगर_अघोषित SYSTEM_H
#घोषणा SYSTEM_H

#समावेश "kernel/kernel.h"

पूर्णांक do_exec(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_EXEC
#घोषणा do_exec शून्य
#पूर्ण_अगर

पूर्णांक do_fork(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_FORK
#घोषणा do_fork शून्य
#पूर्ण_अगर

पूर्णांक do_clear(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_CLEAR
#घोषणा do_clear शून्य
#पूर्ण_अगर

पूर्णांक do_trace(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_TRACE
#घोषणा do_trace शून्य
#पूर्ण_अगर

पूर्णांक do_runctl(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_RUNCTL
#घोषणा do_runctl शून्य
#पूर्ण_अगर

पूर्णांक do_update(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_UPDATE
#घोषणा do_update शून्य
#पूर्ण_अगर

पूर्णांक do_exit(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_EXIT
#घोषणा do_exit शून्य
#पूर्ण_अगर

पूर्णांक do_copy(काष्ठा proc * caller, message *m_ptr);
#घोषणा do_vircopy 	do_copy
#अगर ! (USE_VIRCOPY || USE_PHYSCOPY)
#घोषणा do_copy शून्य
#पूर्ण_अगर

पूर्णांक do_umap(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_UMAP
#घोषणा do_umap शून्य
#पूर्ण_अगर

पूर्णांक do_umap_remote(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_UMAP_REMOTE
#घोषणा do_umap_remote शून्य
#पूर्ण_अगर

पूर्णांक do_vumap(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_VUMAP
#घोषणा do_vumap शून्य
#पूर्ण_अगर

पूर्णांक do_memset(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_MEMSET
#घोषणा do_memset शून्य
#पूर्ण_अगर

पूर्णांक do_abort(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_ABORT
#घोषणा do_abort शून्य
#पूर्ण_अगर

पूर्णांक do_getinfo(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_GETINFO
#घोषणा do_getinfo शून्य
#पूर्ण_अगर

पूर्णांक do_privctl(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_PRIVCTL
#घोषणा do_privctl शून्य
#पूर्ण_अगर

पूर्णांक do_irqctl(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_IRQCTL
#घोषणा do_irqctl शून्य
#पूर्ण_अगर

पूर्णांक do_devio(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_DEVIO
#घोषणा do_devio शून्य
#पूर्ण_अगर

पूर्णांक do_vdevio(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_VDEVIO
#घोषणा do_vdevio शून्य
#पूर्ण_अगर

पूर्णांक do_sdevio(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_SDEVIO
#घोषणा do_sdevio शून्य
#पूर्ण_अगर

पूर्णांक do_kill(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_KILL
#घोषणा do_kill शून्य
#पूर्ण_अगर

पूर्णांक do_getksig(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_GETKSIG
#घोषणा do_getksig शून्य
#पूर्ण_अगर

पूर्णांक do_endksig(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_ENDKSIG
#घोषणा do_endksig शून्य
#पूर्ण_अगर

पूर्णांक do_sigsend(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_SIGSEND
#घोषणा do_sigsend शून्य
#पूर्ण_अगर

पूर्णांक do_sigreturn(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_SIGRETURN
#घोषणा do_sigreturn शून्य
#पूर्ण_अगर

पूर्णांक do_times(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_TIMES
#घोषणा do_times शून्य
#पूर्ण_अगर

पूर्णांक do_setalarm(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_SETALARM
#घोषणा do_setalarm शून्य
#पूर्ण_अगर

पूर्णांक do_stime(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_settime(काष्ठा proc * caller, message *m_ptr);

पूर्णांक do_vtimer(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_VTIMER
#घोषणा do_vtimer शून्य
#पूर्ण_अगर

पूर्णांक do_safecopy_to(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_safecopy_from(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_vsafecopy(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_iopenable(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_vmctl(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_setgrant(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_readbios(काष्ठा proc * caller, message *m_ptr);

पूर्णांक do_safememset(काष्ठा proc * caller, message *m_ptr);

पूर्णांक do_sprofile(काष्ठा proc * caller, message *m_ptr);
#अगर ! SPROFILE
#घोषणा do_sprofile शून्य
#पूर्ण_अगर

पूर्णांक do_getmcontext(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_setmcontext(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_MCONTEXT
#घोषणा do_getmcontext शून्य
#घोषणा do_setmcontext शून्य
#पूर्ण_अगर

पूर्णांक do_schedule(काष्ठा proc * caller, message *m_ptr);
पूर्णांक do_schedctl(काष्ठा proc * caller, message *m_ptr);

पूर्णांक do_statectl(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_STATECTL
#घोषणा do_statectl शून्य
#पूर्ण_अगर

पूर्णांक do_padconf(काष्ठा proc * caller, message *m_ptr);
#अगर ! USE_PADCONF
#घोषणा do_padconf शून्य
#पूर्ण_अगर

#पूर्ण_अगर	/* SYSTEM_H */

