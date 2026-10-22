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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <machine/mcontext.h>

#ಯದಿ USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_getmcontext(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp;
  ಪೂರ್ಣಾಙ್ಕ proc_nr, r;
  mcontext_t mc;

  ಯದಿ (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	ಪ್ರತ್ಯಾಗಮ(EINVAL);
  ಯದಿ (iskerneln(proc_nr)) ಪ್ರತ್ಯಾಗಮ(EPERM);
  rp = proc_addr(proc_nr);

#ಯದಿ defined(__i386__)
  ಯದಿ (!proc_used_fpu(rp))
	ಪ್ರತ್ಯಾಗಮ(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  ಯದಿ ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) ಪರಿಮಾಣ(mcontext_t))) != OK)
	ಪ್ರತ್ಯಾಗಮ(r);

  mc.mc_flags = 0;
#ಯದಿ defined(__i386__)
  /* Copy FPU state */
  ಯದಿ (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  ಯದಿ ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) ಪರಿಮಾಣ(mcontext_t))) != OK)
	ಪ್ರತ್ಯಾಗಮ(r);

  ಪ್ರತ್ಯಾಗಮ(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_setmcontext(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp;
  ಪೂರ್ಣಾಙ್ಕ proc_nr, r;
  mcontext_t mc;

  ಯದಿ (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) ಪ್ರತ್ಯಾಗಮ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  ಯದಿ ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) ಪರಿಮಾಣ(mcontext_t))) != OK)
	ಪ್ರತ್ಯಾಗಮ(r);

#ಯದಿ defined(__i386__)
  /* Copy FPU state */
  ಯದಿ (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } ಅನ್ಯಥಾ
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  ಪ್ರತ್ಯಾಗಮ(OK);
}

#endif
