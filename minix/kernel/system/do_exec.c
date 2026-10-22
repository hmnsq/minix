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
#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <minix/endpoint.h>

#যদি USE_EXEC

/*===========================================================================*
 *				do_exec					     *
 *===========================================================================*/
পূর্ণসংখ্যা do_exec(কাঠামো proc * caller, message * m_ptr)
{
/* Handle sys_exec().  A process has done a successful EXEC. Patch it up. */
  নিবন্ধ কাঠামো proc *rp;
  পূর্ণসংখ্যা proc_nr;
  অক্ষর name[PROC_NAME_LEN];

  যদি(!isokendpt(m_ptr->m_lsys_krn_sys_exec.endpt, &proc_nr))
	ফেরত EINVAL;

  rp = proc_addr(proc_nr);

  যদি(rp->p_misc_flags & MF_DELIVERMSG) {
	rp->p_misc_flags &= ~MF_DELIVERMSG;
  }

  /* Save command name for debugging, ps(1) output, etc. */
  যদি(তথ্য_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_exec.name,
	KERNEL, (vir_bytes) name,
	(phys_bytes) আকার(name) - 1) != OK)
  	strncpy(name, "<unset>", PROC_NAME_LEN);

  name[আকার(name)-1] = '\0';

  /* Set process state. */
  arch_proc_init(rp,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ip,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.স্তূপ,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ps_str, name);

  /* No reply to EXEC call */
  RTS_UNSET(rp, RTS_RECEIVING);

  /* Mark fpu_regs contents as not significant, so fpu
   * will be initialized, when it's used next time. */
  rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU if the current process is the owner */
  release_fpu(rp);
  ফেরত(OK);
}
#endif /* USE_EXEC */
