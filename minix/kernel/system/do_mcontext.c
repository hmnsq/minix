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

#ضمن "kernel/system.h"
#ضمن <string.h>
#ضمن <assert.h>
#ضمن <machine/mcontext.h>

#إذا USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
صحيح do_getmcontext(بنية proc * caller, پیام * m_ptr)
{
/* Retrieve machine context of a process */

  سجل بنية proc *rp;
  صحيح proc_nr, r;
  mcontext_t mc;

  إذا (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	أرجع(EINVAL);
  إذا (iskerneln(proc_nr)) أرجع(EPERM);
  rp = proc_addr(proc_nr);

#إذا defined(__i386__)
  إذا (!proc_استفاده_شده_fpu(rp))
	أرجع(OK);	/* No state to copy */
#پایان_اگر

  /* Get the mcontext structure into our address space.  */
  إذا ((r = داده_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_بایتها) &mc, (phys_بایتها) حجم(mcontext_t))) != OK)
	أرجع(r);

  mc.mc_flags = 0;
#إذا defined(__i386__)
  /* Copy FPU state */
  إذا (proc_استفاده_شده_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	أكد(حجم(mc.__fpregs.__fp_reg_set) == FPU_XFP_اندازه);
	کپی_حافظه(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_اندازه);
  } 
#پایان_اگر


  /* Copy the mcontext structure to the user's address space. */
  إذا ((r = داده_copy(KERNEL, (vir_بایتها) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_بایتها) حجم(mcontext_t))) != OK)
	أرجع(r);

  أرجع(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
صحيح do_setmcontext(بنية proc * caller, پیام * m_ptr)
{
/* Set machine context of a process */

  سجل بنية proc *rp;
  صحيح proc_nr, r;
  mcontext_t mc;

  إذا (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) أرجع(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  إذا ((r = داده_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_بایتها) &mc, (phys_بایتها) حجم(mcontext_t))) != OK)
	أرجع(r);

#إذا defined(__i386__)
  /* Copy FPU state */
  إذا (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	أكد(حجم(mc.__fpregs.__fp_reg_set) == FPU_XFP_اندازه);
	کپی_حافظه(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_اندازه);
  } وإلا
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#پایان_اگر

  أرجع(OK);
}

#پایان_اگر
