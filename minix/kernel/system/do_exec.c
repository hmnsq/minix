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
#include "kernel/system.h"
#include <string.h>
#include <minix/endpoint.h>

#ᯀᯮᯝ USE_EXEC

/*===========================================================================*
 *				do_exec					     *
 *===========================================================================*/
puur_naa_nka do_exec(sa_nracanaa proc * caller, message * m_ptr)
{
/* Handle sys_exec().  A process has done a successful EXEC. Patch it up. */
  pa_njikaa sa_nracanaa proc *rp;
  puur_naa_nka proc_nr;
  var_na name[PROC_NAME_LEN];

  ᯀᯮᯝ(!isokendpt(m_ptr->m_lsys_krn_sys_exec.endpt, &proc_nr))
	ᯀᯮᯞ EINVAL;

  rp = proc_addr(proc_nr);

  ᯀᯮᯝ(rp->p_misc_flags & MF_DELIVERMSG) {
	rp->p_misc_flags &= ~MF_DELIVERMSG;
  }

  /* Save command name for debugging, ps(1) output, etc. */
  ᯀᯮᯝ(data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_exec.name,
	KERNEL, (vir_bytes) name,
	(phys_bytes) parimaa_na(name) - 1) != OK)
  	strncpy(name, "<unset>", PROC_NAME_LEN);

  name[parimaa_na(name)-1] = '\0';

  /* Set process state. */
  arch_proc_init(rp,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ip,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.stack,
	(u32_t) m_ptr->m_lsys_krn_sys_exec.ps_str, name);

  /* No reply to EXEC call */
  RTS_UNSET(rp, RTS_RECEIVING);

  /* Mark fpu_regs contents as not significant, so fpu
   * will be initialized, when it's used next time. */
  rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU if the current process is the owner */
  release_fpu(rp);
  ᯀᯮᯞ(OK);
}
#endif /* USE_EXEC */
