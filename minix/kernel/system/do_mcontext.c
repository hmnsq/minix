/* The kernel calls that are implemented in this file:
 *   m_type:	SYS_SETMCONTEXT
 *   m_type:	SYS_GETMCONTEXT
 *
 * The parameters for SYS_SETMCONTEXT kernel call are:
 *   m_lsys_krn_sys_setmcontext.endpt	# proc endpoint doing call
 *   m_lsys_krn_sys_setmcontext.ctx_ptr	# pointer to mcontext structure
 *
 * The parameters for SYS_GETMCONTEXT kernel call are:
 *   m_lsys_krn_sys_getmcontext.endpt	# proc endpoint doing call
 *   m_lsys_krn_sys_getmcontext.ctx_ptr	# pointer to mcontext structure
 */

#include "kernel/system.h"
#include <string.h>
#include <assert.h>
#include <machine/mcontext.h>

#만약 USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
정수 do_getmcontext(struct proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  register struct proc *rp;
  정수 proc_nr, r;
  mcontext_t mc;

  만약 (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	반환(EINVAL);
  만약 (iskerneln(proc_nr)) 반환(EPERM);
  rp = proc_addr(proc_nr);

#만약 defined(__i386__)
  만약 (!proc_used_fpu(rp))
	반환(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  만약 ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) sizeof(mcontext_t))) != OK)
	반환(r);

  mc.mc_flags = 0;
#만약 defined(__i386__)
  /* Copy FPU state */
  만약 (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	assert(sizeof(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  만약 ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) sizeof(mcontext_t))) != OK)
	반환(r);

  반환(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
정수 do_setmcontext(struct proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  register struct proc *rp;
  정수 proc_nr, r;
  mcontext_t mc;

  만약 (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) 반환(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  만약 ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) sizeof(mcontext_t))) != OK)
	반환(r);

#만약 defined(__i386__)
  /* Copy FPU state */
  만약 (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	assert(sizeof(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } 아니면
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  반환(OK);
}

#endif
