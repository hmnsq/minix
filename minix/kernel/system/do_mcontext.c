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

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <machine/mcontext.h>

#எனில் USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
முழு do_getmcontext(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  பதிவு கட்டமைப்பு proc *rp;
  முழு proc_nr, r;
  mcontext_t mc;

  எனில் (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	திரும்பு(EINVAL);
  எனில் (iskerneln(proc_nr)) திரும்பு(EPERM);
  rp = proc_addr(proc_nr);

#எனில் defined(__i386__)
  எனில் (!proc_used_fpu(rp))
	திரும்பு(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  எனில் ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) அளவு(mcontext_t))) != OK)
	திரும்பு(r);

  mc.mc_flags = 0;
#எனில் defined(__i386__)
  /* Copy FPU state */
  எனில் (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	ப்ரதிஜ்ஞா(அளவு(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  எனில் ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) அளவு(mcontext_t))) != OK)
	திரும்பு(r);

  திரும்பு(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
முழு do_setmcontext(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  பதிவு கட்டமைப்பு proc *rp;
  முழு proc_nr, r;
  mcontext_t mc;

  எனில் (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) திரும்பு(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  எனில் ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) அளவு(mcontext_t))) != OK)
	திரும்பு(r);

#எனில் defined(__i386__)
  /* Copy FPU state */
  எனில் (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	ப்ரதிஜ்ஞா(அளவு(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } இல்லையெனில்
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  திரும்பு(OK);
}

#endif
