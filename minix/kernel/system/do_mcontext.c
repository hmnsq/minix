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

#شامل "kernel/system.h"
#شامل <string.h>
#شامل <assert.h>
#شامل <machine/mcontext.h>

#اگر USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
صحیح do_getmcontext(ساختار proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  ثبات ساختار proc *rp;
  صحیح proc_nr, r;
  mcontext_t mc;

  اگر (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	بازگشت(EINVAL);
  اگر (iskerneln(proc_nr)) بازگشت(EPERM);
  rp = proc_addr(proc_nr);

#اگر defined(__i386__)
  اگر (!proc_used_fpu(rp))
	بازگشت(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  اگر ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) اندازه(mcontext_t))) != OK)
	بازگشت(r);

  mc.mc_flags = 0;
#اگر defined(__i386__)
  /* Copy FPU state */
  اگر (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	أكد(اندازه(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  اگر ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) اندازه(mcontext_t))) != OK)
	بازگشت(r);

  بازگشت(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
صحیح do_setmcontext(ساختار proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  ثبات ساختار proc *rp;
  صحیح proc_nr, r;
  mcontext_t mc;

  اگر (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) بازگشت(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  اگر ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) اندازه(mcontext_t))) != OK)
	بازگشت(r);

#اگر defined(__i386__)
  /* Copy FPU state */
  اگر (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	أكد(اندازه(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } وگرنه
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  بازگشت(OK);
}

#endif
