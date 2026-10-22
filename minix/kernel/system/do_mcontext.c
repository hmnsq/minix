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

#𐡄𐡍 USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
صحيح do_getmcontext(بنية proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  سجل بنية proc *rp;
  صحيح proc_nr, r;
  mcontext_t mc;

  𐡄𐡍 (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	أرجع(EINVAL);
  𐡄𐡍 (iskerneln(proc_nr)) أرجع(EPERM);
  rp = proc_addr(proc_nr);

#𐡄𐡍 defined(__i386__)
  𐡄𐡍 (!proc_used_fpu(rp))
	أرجع(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  𐡄𐡍 ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) حجم(mcontext_t))) != OK)
	أرجع(r);

  mc.mc_flags = 0;
#𐡄𐡍 defined(__i386__)
  /* Copy FPU state */
  𐡄𐡍 (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	أكد(حجم(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  𐡄𐡍 ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) حجم(mcontext_t))) != OK)
	أرجع(r);

  أرجع(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
صحيح do_setmcontext(بنية proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  سجل بنية proc *rp;
  صحيح proc_nr, r;
  mcontext_t mc;

  𐡄𐡍 (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) أرجع(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  𐡄𐡍 ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) حجم(mcontext_t))) != OK)
	أرجع(r);

#𐡄𐡍 defined(__i386__)
  /* Copy FPU state */
  𐡄𐡍 (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	أكد(حجم(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } وإلا
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  أرجع(OK);
}

#endif
