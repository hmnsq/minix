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
#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <minix/endpoint.h>

#எனில் USE_EXEC

/*===========================================================================*
 *				do_exec					     *
 *===========================================================================*/
முழு do_exec(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Handle sys_exec().  A process has done a successful EXEC. Patch it up. */
  பதிவு கட்டமைப்பு proc *rp;
  முழு proc_nr;
  எழுத்து name[PROC_NAME_LEN];

  எனில்(!isokendpt(m_ptr->m_lsys_krn_sys_exec.endpt, &proc_nr))
	திரும்பு EINVAL;

  rp = proc_addr(proc_nr);

  எனில்(rp->p_misc_flags & MF_DELIVERMSG) {
	rp->p_misc_flags &= ~MF_DELIVERMSG;
  }

  /* Save command name for debugging, ps(1) output, etc. */
  எனில்(data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_exec.name,
	KERNEL, (vir_bytes) name,
	(phys_bytes) அளவு(name) - 1) != OK)
  	strncpy(name, "<unset>", PROC_NAME_LEN);

  name[அளவு(name)-1] = '\0';

  /* Set process state. */
  arch_proc_init(rp,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ip,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.அடுக்கு,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ps_str, name);

  /* No reply to EXEC call */
  RTS_UNSET(rp, RTS_RECEIVING);

  /* Mark fpu_regs contents as not significant, so fpu
   * will be initialized, when it's used next time. */
  rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU if the current process is the owner */
  release_fpu(rp);
  திரும்பு(OK);
}
#endif /* USE_EXEC */
