/* The kernel call implemented in this file:
 *   m_type:	SYS_EXEC
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_exec.endpt		(process that did exec call)
 *   m_lsys_krn_sys_exec.stack		(new stack pointer)
 *   m_lsys_krn_sys_exec.name		(pointer to program name)
 *   m_lsys_krn_sys_exec.ip		(new instruction pointer)
 *   m_lsys_krn_sys_exec.ps_str		(struct ps_strings *)
 */
#समावेश "kernel/system.h"
#समावेश <string.h>
#समावेश <minix/endpoint.h>

#अगर USE_EXEC

/*===========================================================================*
 *				do_exec					     *
 *===========================================================================*/
पूर्णांक do_exec(काष्ठा proc * caller, message * m_ptr)
{
/* Handle sys_exec().  A process has done a successful EXEC. Patch it up. */
  रेजिस्टर काष्ठा proc *rp;
  पूर्णांक proc_nr;
  अक्षर name[PROC_NAME_LEN];

  अगर(!isokendpt(m_ptr->m_lsys_krn_sys_exec.endpt, &proc_nr))
	वापस EINVAL;

  rp = proc_addr(proc_nr);

  अगर(rp->p_misc_flags & MF_DELIVERMSG) {
	rp->p_misc_flags &= ~MF_DELIVERMSG;
  }

  /* Save command name for debugging, ps(1) output, etc. */
  अगर(तथ्य_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_exec.name,
	KERNEL, (vir_bytes) name,
	(phys_bytes) माप(name) - 1) != OK)
  	म_नकलन(name, "<unset>", PROC_NAME_LEN);

  name[माप(name)-1] = '\0';

  /* Set process state. */
  arch_proc_init(rp,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ip,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ढेर_2,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ps_str, name);

  /* No reply to EXEC call */
  RTS_UNSET(rp, RTS_RECEIVING);

  /* Mark fpu_regs contents as not significant, so fpu
   * will be initialized, when it's used next time. */
  rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU if the current process is the owner */
  release_fpu(rp);
  वापस(OK);
}
#पूर्ण_अगर /* USE_EXEC */
