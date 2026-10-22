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

#समावेश "kernel/system.h"
#समावेश <string.h>
#समावेश <assert.h>
#समावेश <machine/mcontext.h>

#अगर USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
पूर्णांक do_getmcontext(काष्ठा proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  रेजिस्टर काष्ठा proc *rp;
  पूर्णांक proc_nr, r;
  mcontext_t mc;

  अगर (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	वापस(EINVAL);
  अगर (iskerneln(proc_nr)) वापस(EPERM);
  rp = proc_addr(proc_nr);

#अगर defined(__i386__)
  अगर (!proc_used_fpu(rp))
	वापस(OK);	/* No state to copy */
#पूर्ण_अगर

  /* Get the mcontext structure into our address space.  */
  अगर ((r = तथ्य_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) माप(mcontext_t))) != OK)
	वापस(r);

  mc.mc_flags = 0;
#अगर defined(__i386__)
  /* Copy FPU state */
  अगर (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	निश्चित(माप(mc.__fpregs.__fp_reg_set) == FPU_XFP_आकार);
	स_नकल(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_आकार);
  } 
#पूर्ण_अगर


  /* Copy the mcontext structure to the user's address space. */
  अगर ((r = तथ्य_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) माप(mcontext_t))) != OK)
	वापस(r);

  वापस(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
पूर्णांक do_setmcontext(काष्ठा proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  रेजिस्टर काष्ठा proc *rp;
  पूर्णांक proc_nr, r;
  mcontext_t mc;

  अगर (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) वापस(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  अगर ((r = तथ्य_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) माप(mcontext_t))) != OK)
	वापस(r);

#अगर defined(__i386__)
  /* Copy FPU state */
  अगर (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	निश्चित(माप(mc.__fpregs.__fp_reg_set) == FPU_XFP_आकार);
	स_नकल(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_आकार);
  } अन्यथा
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#पूर्ण_अगर

  वापस(OK);
}

#पूर्ण_अगर
