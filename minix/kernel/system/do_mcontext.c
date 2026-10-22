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

#यदि USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
पूर्णाङ्क do_getmcontext(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Retrieve machine context of a process */

  पञ्जिका संरचना proc *rp;
  पूर्णाङ्क proc_nr, r;
  mcontext_t mc;

  यदि (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	प्रत्यागम(EINVAL);
  यदि (iskerneln(proc_nr)) प्रत्यागम(EPERM);
  rp = proc_addr(proc_nr);

#यदि defined(__i386__)
  यदि (!proc_प्रयुक्तम्_fpu(rp))
	प्रत्यागम(OK);	/* No state to copy */
#यदि_अन्तः

  /* Get the mcontext structure into our address space.  */
  यदि ((r = दत्त_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_अष्टकानि) &mc, (phys_अष्टकानि) परिमाण(mcontext_t))) != OK)
	प्रत्यागम(r);

  mc.mc_flags = 0;
#यदि defined(__i386__)
  /* Copy FPU state */
  यदि (proc_प्रयुक्तम्_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	प्रतिज्ञा(परिमाण(mc.__fpregs.__fp_reg_set) == FPU_XFP_आकार);
	स्मृतिं_प्रतिलिख(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_आकार);
  } 
#यदि_अन्तः


  /* Copy the mcontext structure to the user's address space. */
  यदि ((r = दत्त_copy(KERNEL, (vir_अष्टकानि) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_अष्टकानि) परिमाण(mcontext_t))) != OK)
	प्रत्यागम(r);

  प्रत्यागम(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
पूर्णाङ्क do_setmcontext(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Set machine context of a process */

  पञ्जिका संरचना proc *rp;
  पूर्णाङ्क proc_nr, r;
  mcontext_t mc;

  यदि (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) प्रत्यागम(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  यदि ((r = दत्त_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_अष्टकानि) &mc, (phys_अष्टकानि) परिमाण(mcontext_t))) != OK)
	प्रत्यागम(r);

#यदि defined(__i386__)
  /* Copy FPU state */
  यदि (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	प्रतिज्ञा(परिमाण(mc.__fpregs.__fp_reg_set) == FPU_XFP_आकार);
	स्मृतिं_प्रतिलिख(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_आकार);
  } अन्यथा
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#यदि_अन्तः

  प्रत्यागम(OK);
}

#यदि_अन्तः
