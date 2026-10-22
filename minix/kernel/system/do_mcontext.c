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

#ಆದರೆ USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_getmcontext(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Retrieve machine context of a process */

  ನೋಂದಣಿ ರಚನೆ proc *rp;
  ಪೂರ್ಣಾಂಕ proc_nr, r;
  mcontext_t mc;

  ಆದರೆ (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	ಹಿಂತಿರುಗಿ(EINVAL);
  ಆದರೆ (iskerneln(proc_nr)) ಹಿಂತಿರುಗಿ(EPERM);
  rp = proc_addr(proc_nr);

#ಆದರೆ defined(__i386__)
  ಆದರೆ (!proc_ಬಳಸಿದ_fpu(rp))
	ಹಿಂತಿರುಗಿ(OK);	/* No state to copy */
#ಆದರೆ_ಅಂತ್ಯ

  /* Get the mcontext structure into our address space.  */
  ಆದರೆ ((r = ದತ್ತಾಂಶ_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_ಬೈಟುಗಳು) &mc, (phys_ಬೈಟುಗಳು) ಗಾತ್ರ(mcontext_t))) != OK)
	ಹಿಂತಿರುಗಿ(r);

  mc.mc_flags = 0;
#ಆದರೆ defined(__i386__)
  /* Copy FPU state */
  ಆದರೆ (proc_ಬಳಸಿದ_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(mc.__fpregs.__fp_reg_set) == FPU_XFP_ಗಾತ್ರ);
	ಸ್ಮರಣೆ_ನಕಲು(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_ಗಾತ್ರ);
  } 
#ಆದರೆ_ಅಂತ್ಯ


  /* Copy the mcontext structure to the user's address space. */
  ಆದರೆ ((r = ದತ್ತಾಂಶ_copy(KERNEL, (vir_ಬೈಟುಗಳು) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_ಬೈಟುಗಳು) ಗಾತ್ರ(mcontext_t))) != OK)
	ಹಿಂತಿರುಗಿ(r);

  ಹಿಂತಿರುಗಿ(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_setmcontext(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Set machine context of a process */

  ನೋಂದಣಿ ರಚನೆ proc *rp;
  ಪೂರ್ಣಾಂಕ proc_nr, r;
  mcontext_t mc;

  ಆದರೆ (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) ಹಿಂತಿರುಗಿ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  ಆದರೆ ((r = ದತ್ತಾಂಶ_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_ಬೈಟುಗಳು) &mc, (phys_ಬೈಟುಗಳು) ಗಾತ್ರ(mcontext_t))) != OK)
	ಹಿಂತಿರುಗಿ(r);

#ಆದರೆ defined(__i386__)
  /* Copy FPU state */
  ಆದರೆ (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(mc.__fpregs.__fp_reg_set) == FPU_XFP_ಗಾತ್ರ);
	ಸ್ಮರಣೆ_ನಕಲು(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_ಗಾತ್ರ);
  } ಇಲ್ಲದಿದ್ದರೆ
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ(OK);
}

#ಆದರೆ_ಅಂತ್ಯ
