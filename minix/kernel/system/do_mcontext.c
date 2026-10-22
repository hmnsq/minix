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

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <machine/mcontext.h>

#যদি USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_getmcontext(কাঠামো proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  নিবন্ধ কাঠামো proc *rp;
  পূর্ণসংখ্যা proc_nr, r;
  mcontext_t mc;

  যদি (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	ফেরত(EINVAL);
  যদি (iskerneln(proc_nr)) ফেরত(EPERM);
  rp = proc_addr(proc_nr);

#যদি defined(__i386__)
  যদি (!proc_used_fpu(rp))
	ফেরত(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  যদি ((r = তথ্য_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) আকার(mcontext_t))) != OK)
	ফেরত(r);

  mc.mc_flags = 0;
#যদি defined(__i386__)
  /* Copy FPU state */
  যদি (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	প্রতিজ্ঞা(আকার(mc.__fpregs.__fp_reg_set) == FPU_XFP_আকার);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_আকার);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  যদি ((r = তথ্য_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) আকার(mcontext_t))) != OK)
	ফেরত(r);

  ফেরত(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_setmcontext(কাঠামো proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  নিবন্ধ কাঠামো proc *rp;
  পূর্ণসংখ্যা proc_nr, r;
  mcontext_t mc;

  যদি (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) ফেরত(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  যদি ((r = তথ্য_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) আকার(mcontext_t))) != OK)
	ফেরত(r);

#যদি defined(__i386__)
  /* Copy FPU state */
  যদি (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	প্রতিজ্ঞা(আকার(mc.__fpregs.__fp_reg_set) == FPU_XFP_আকার);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_আকার);
  } অন্যথা
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  ফেরত(OK);
}

#endif
