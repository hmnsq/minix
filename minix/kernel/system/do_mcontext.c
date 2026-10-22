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

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <string.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <machine/mcontext.h>

#जइ USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
पूर्णाङ्क do_getmcontext(संरचना proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  पञ्जिका संरचना proc *rp;
  पूर्णाङ्क proc_nr, r;
  mcontext_t mc;

  जइ (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	प्रत्यागम(EINVAL);
  जइ (iskerneln(proc_nr)) प्रत्यागम(EPERM);
  rp = proc_addr(proc_nr);

#जइ defined(__i386__)
  जइ (!proc_used_fpu(rp))
	प्रत्यागम(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  जइ ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) परिमाण(mcontext_t))) != OK)
	प्रत्यागम(r);

  mc.mc_flags = 0;
#जइ defined(__i386__)
  /* Copy FPU state */
  जइ (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	प्रतिज्ञा(परिमाण(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  जइ ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) परिमाण(mcontext_t))) != OK)
	प्रत्यागम(r);

  प्रत्यागम(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
पूर्णाङ्क do_setmcontext(संरचना proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  पञ्जिका संरचना proc *rp;
  पूर्णाङ्क proc_nr, r;
  mcontext_t mc;

  जइ (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) प्रत्यागम(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  जइ ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) परिमाण(mcontext_t))) != OK)
	प्रत्यागम(r);

#जइ defined(__i386__)
  /* Copy FPU state */
  जइ (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	प्रतिज्ञा(परिमाण(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } अण्णहा
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  प्रत्यागम(OK);
}

#endif
