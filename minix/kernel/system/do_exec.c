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
#ضمن "kernel/system.h"
#ضمن <string.h>
#ضمن <minix/endpoint.h>

#إذا USE_EXEC

/*===========================================================================*
 *				do_exec					     *
 *===========================================================================*/
صحيح do_exec(بنية proc * caller, پيغام * m_ptr)
{
/* Handle sys_exec().  A process has done a successful EXEC. Patch it up. */
  سجل بنية proc *rp;
  صحيح proc_nr;
  حرف name[PROC_NAME_LEN];

  إذا(!isokendpt(m_ptr->m_lsys_krn_sys_exec.endpt, &proc_nr))
	أرجع EINVAL;

  rp = proc_addr(proc_nr);

  إذا(rp->p_misc_flags & MF_DELIVERMSG) {
	rp->p_misc_flags &= ~MF_DELIVERMSG;
  }

  /* Save command name for debugging, ps(1) output, etc. */
  إذا(معلومات_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_exec.name,
	KERNEL, (vir_بائيٽون) name,
	(phys_بائيٽون) حجم(name) - 1) != OK)
  	strncpy(name, "<unset>", PROC_NAME_LEN);

  name[حجم(name)-1] = '\0';

  /* Set process state. */
  arch_proc_init(rp,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ip,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ڍير,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ps_str, name);

  /* No reply to EXEC call */
  RTS_UNSET(rp, RTS_RECEIVING);

  /* Mark fpu_regs contents as not significant, so fpu
   * will be initialized, when it's used next time. */
  rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU if the current process is the owner */
  release_fpu(rp);
  أرجع(OK);
}
#جيڪڏهن_پورو /* USE_EXEC */
