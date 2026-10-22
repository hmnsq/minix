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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/mcontext.h>

#ਜੇ USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
ਪੂਰਨ do_getmcontext(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Retrieve machine context of a process */

  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp;
  ਪੂਰਨ proc_nr, r;
  mcontext_t mc;

  ਜੇ (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	ਵਾਪਸ(EINVAL);
  ਜੇ (iskerneln(proc_nr)) ਵਾਪਸ(EPERM);
  rp = proc_addr(proc_nr);

#ਜੇ defined(__i386__)
  ਜੇ (!proc_ਵਰਤਿਆ_fpu(rp))
	ਵਾਪਸ(OK);	/* No state to copy */
#ਜੇ_ਅੰਤ

  /* Get the mcontext structure into our address space.  */
  ਜੇ ((r = ਅੰਕੜੇ_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_ਬਾਈਟਾਂ) &mc, (phys_ਬਾਈਟਾਂ) ਆਕਾਰ(mcontext_t))) != OK)
	ਵਾਪਸ(r);

  mc.mc_flags = 0;
#ਜੇ defined(__i386__)
  /* Copy FPU state */
  ਜੇ (proc_ਵਰਤਿਆ_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(mc.__fpregs.__fp_reg_set) == FPU_XFP_ਮਾਪ);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_ਮਾਪ);
  } 
#ਜੇ_ਅੰਤ


  /* Copy the mcontext structure to the user's address space. */
  ਜੇ ((r = ਅੰਕੜੇ_copy(KERNEL, (vir_ਬਾਈਟਾਂ) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_ਬਾਈਟਾਂ) ਆਕਾਰ(mcontext_t))) != OK)
	ਵਾਪਸ(r);

  ਵਾਪਸ(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
ਪੂਰਨ do_setmcontext(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Set machine context of a process */

  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp;
  ਪੂਰਨ proc_nr, r;
  mcontext_t mc;

  ਜੇ (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) ਵਾਪਸ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  ਜੇ ((r = ਅੰਕੜੇ_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_ਬਾਈਟਾਂ) &mc, (phys_ਬਾਈਟਾਂ) ਆਕਾਰ(mcontext_t))) != OK)
	ਵਾਪਸ(r);

#ਜੇ defined(__i386__)
  /* Copy FPU state */
  ਜੇ (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(mc.__fpregs.__fp_reg_set) == FPU_XFP_ਮਾਪ);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_ਮਾਪ);
  } ਨਹੀਂਤਾਂ
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#ਜੇ_ਅੰਤ

  ਵਾਪਸ(OK);
}

#ਜੇ_ਅੰਤ
