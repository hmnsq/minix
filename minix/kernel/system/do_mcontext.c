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

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <machine/mcontext.h>

#જો USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
પૂર્ણાંક do_getmcontext(સંરચના proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  નોંધણી સંરચના proc *rp;
  પૂર્ણાંક proc_nr, r;
  mcontext_t mc;

  જો (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	પાછા(EINVAL);
  જો (iskerneln(proc_nr)) પાછા(EPERM);
  rp = proc_addr(proc_nr);

#જો defined(__i386__)
  જો (!proc_used_fpu(rp))
	પાછા(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  જો ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) કદ(mcontext_t))) != OK)
	પાછા(r);

  mc.mc_flags = 0;
#જો defined(__i386__)
  /* Copy FPU state */
  જો (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	પ્રતિજ્ઞા(કદ(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  જો ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) કદ(mcontext_t))) != OK)
	પાછા(r);

  પાછા(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
પૂર્ણાંક do_setmcontext(સંરચના proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  નોંધણી સંરચના proc *rp;
  પૂર્ણાંક proc_nr, r;
  mcontext_t mc;

  જો (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) પાછા(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  જો ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) કદ(mcontext_t))) != OK)
	પાછા(r);

#જો defined(__i386__)
  /* Copy FPU state */
  જો (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	પ્રતિજ્ઞા(કદ(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } નહીંતર
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  પાછા(OK);
}

#endif
