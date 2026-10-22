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

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <machine/mcontext.h>

#ଯଦି USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_getmcontext(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  ପଞ୍ଜିକରଣ ସଂରଚନା proc *rp;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr, r;
  mcontext_t mc;

  ଯଦି (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	ଫେରନ୍ତୁ(EINVAL);
  ଯଦି (iskerneln(proc_nr)) ଫେରନ୍ତୁ(EPERM);
  rp = proc_addr(proc_nr);

#ଯଦି defined(__i386__)
  ଯଦି (!proc_used_fpu(rp))
	ଫେରନ୍ତୁ(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  ଯଦି ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) ଆକାର(mcontext_t))) != OK)
	ଫେରନ୍ତୁ(r);

  mc.mc_flags = 0;
#ଯଦି defined(__i386__)
  /* Copy FPU state */
  ଯଦି (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	ପ୍ରତିଜ୍ଞା(ଆକାର(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  ଯଦି ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) ଆକାର(mcontext_t))) != OK)
	ଫେରନ୍ତୁ(r);

  ଫେରନ୍ତୁ(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_setmcontext(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  ପଞ୍ଜିକରଣ ସଂରଚନା proc *rp;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr, r;
  mcontext_t mc;

  ଯଦି (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) ଫେରନ୍ତୁ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  ଯଦି ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) ଆକାର(mcontext_t))) != OK)
	ଫେରନ୍ତୁ(r);

#ଯଦି defined(__i386__)
  /* Copy FPU state */
  ଯଦି (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	ପ୍ରତିଜ୍ଞା(ଆକାର(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } ଅନ୍ୟଥା
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  ଫେରନ୍ତୁ(OK);
}

#endif
