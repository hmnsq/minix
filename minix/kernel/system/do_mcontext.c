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

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <string.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <machine/mcontext.h>

#ఒకవేళ USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
పూర్ణాంకం do_getmcontext(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Retrieve machine context of a process */

  నమోదు నిర్మాణం proc *rp;
  పూర్ణాంకం proc_nr, r;
  mcontext_t mc;

  ఒకవేళ (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	తిరిగి(EINVAL);
  ఒకవేళ (iskerneln(proc_nr)) తిరిగి(EPERM);
  rp = proc_addr(proc_nr);

#ఒకవేళ defined(__i386__)
  ఒకవేళ (!proc_వాడిన_fpu(rp))
	తిరిగి(OK);	/* No state to copy */
#అయితే_ముగింపు

  /* Get the mcontext structure into our address space.  */
  ఒకవేళ ((r = దత్తాంశం_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_బైట్లు) &mc, (phys_బైట్లు) పరిమాణం(mcontext_t))) != OK)
	తిరిగి(r);

  mc.mc_flags = 0;
#ఒకవేళ defined(__i386__)
  /* Copy FPU state */
  ఒకవేళ (proc_వాడిన_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	ప్రతిజ్ఞా(పరిమాణం(mc.__fpregs.__fp_reg_set) == FPU_XFP_కొలత);
	జ్ఞాపకం_నకలు(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_కొలత);
  } 
#అయితే_ముగింపు


  /* Copy the mcontext structure to the user's address space. */
  ఒకవేళ ((r = దత్తాంశం_copy(KERNEL, (vir_బైట్లు) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_బైట్లు) పరిమాణం(mcontext_t))) != OK)
	తిరిగి(r);

  తిరిగి(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
పూర్ణాంకం do_setmcontext(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Set machine context of a process */

  నమోదు నిర్మాణం proc *rp;
  పూర్ణాంకం proc_nr, r;
  mcontext_t mc;

  ఒకవేళ (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) తిరిగి(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  ఒకవేళ ((r = దత్తాంశం_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_బైట్లు) &mc, (phys_బైట్లు) పరిమాణం(mcontext_t))) != OK)
	తిరిగి(r);

#ఒకవేళ defined(__i386__)
  /* Copy FPU state */
  ఒకవేళ (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	ప్రతిజ్ఞా(పరిమాణం(mc.__fpregs.__fp_reg_set) == FPU_XFP_కొలత);
	జ్ఞాపకం_నకలు(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_కొలత);
  } లేకపోతే
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#అయితే_ముగింపు

  తిరిగి(OK);
}

#అయితే_ముగింపు
