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
#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <string.h>
#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_EXEC

/*===========================================================================*
 *				do_exec					     *
 *===========================================================================*/
పూర్ణాంకం do_exec(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_exec().  A process has done a successful EXEC. Patch it up. */
  నమోదు నిర్మాణం proc *rp;
  పూర్ణాంకం proc_nr;
  అక్షరం name[PROC_NAME_LEN];

  ఒకవేళ(!isokendpt(m_ptr->m_lsys_krn_sys_exec.endpt, &proc_nr))
	తిరిగి EINVAL;

  rp = proc_addr(proc_nr);

  ఒకవేళ(rp->p_misc_flags & MF_DELIVERMSG) {
	rp->p_misc_flags &= ~MF_DELIVERMSG;
  }

  /* Save command name for debugging, ps(1) output, etc. */
  ఒకవేళ(దత్తాంశం_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_exec.name,
	KERNEL, (vir_బైట్లు) name,
	(phys_బైట్లు) పరిమాణం(name) - 1) != OK)
  	strncpy(name, "<unset>", PROC_NAME_LEN);

  name[పరిమాణం(name)-1] = '\0';

  /* Set process state. */
  arch_proc_init(rp,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ip,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.దొంతర,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ps_str, name);

  /* No reply to EXEC call */
  RTS_UNSET(rp, RTS_RECEIVING);

  /* Mark fpu_regs contents as not significant, so fpu
   * will be initialized, when it's used next time. */
  rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU if the current process is the owner */
  release_fpu(rp);
  తిరిగి(OK);
}
#అయితే_ముగింపు /* USE_EXEC */
