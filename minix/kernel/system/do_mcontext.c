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

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <machine/mcontext.h>

#എങ്കിൽ USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_getmcontext(ഘടന proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  രജിസ്റ്റർ ഘടന proc *rp;
  പൂർണ്ണസംഖ്യ proc_nr, r;
  mcontext_t mc;

  എങ്കിൽ (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	തിരികെ(EINVAL);
  എങ്കിൽ (iskerneln(proc_nr)) തിരികെ(EPERM);
  rp = proc_addr(proc_nr);

#എങ്കിൽ defined(__i386__)
  എങ്കിൽ (!proc_used_fpu(rp))
	തിരികെ(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  എങ്കിൽ ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) വലുപ്പം(mcontext_t))) != OK)
	തിരികെ(r);

  mc.mc_flags = 0;
#എങ്കിൽ defined(__i386__)
  /* Copy FPU state */
  എങ്കിൽ (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	പ്രതിജ്ഞാ(വലുപ്പം(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  എങ്കിൽ ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) വലുപ്പം(mcontext_t))) != OK)
	തിരികെ(r);

  തിരികെ(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_setmcontext(ഘടന proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  രജിസ്റ്റർ ഘടന proc *rp;
  പൂർണ്ണസംഖ്യ proc_nr, r;
  mcontext_t mc;

  എങ്കിൽ (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) തിരികെ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  എങ്കിൽ ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) വലുപ്പം(mcontext_t))) != OK)
	തിരികെ(r);

#എങ്കിൽ defined(__i386__)
  /* Copy FPU state */
  എങ്കിൽ (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	പ്രതിജ്ഞാ(വലുപ്പം(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } അല്ലെങ്കിൽ
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  തിരികെ(OK);
}

#endif
