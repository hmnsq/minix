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

#شامل "kernel/system.h"
#شامل <string.h>
#شامل <assert.h>
#شامل <machine/mcontext.h>

#اگر USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
صحیح do_getmcontext(ساخت proc * caller, پیغام * m_ptr)
{
/* Retrieve machine context of a process */

  اندراج ساخت proc *rp;
  صحیح proc_nr, r;
  mcontext_t mc;

  اگر (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	واپس(EINVAL);
  اگر (iskerneln(proc_nr)) واپس(EPERM);
  rp = proc_addr(proc_nr);

#اگر defined(__i386__)
  اگر (!proc_مستعمل_fpu(rp))
	واپس(OK);	/* No state to copy */
#اگر_ختم

  /* Get the mcontext structure into our address space.  */
  اگر ((r = ڈیٹا_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_بائٹیں) &mc, (phys_بائٹیں) حجم(mcontext_t))) != OK)
	واپس(r);

  mc.mc_flags = 0;
#اگر defined(__i386__)
  /* Copy FPU state */
  اگر (proc_مستعمل_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	أكد(حجم(mc.__fpregs.__fp_reg_set) == FPU_XFP_سائز);
	حافظہ_نقل(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_سائز);
  } 
#اگر_ختم


  /* Copy the mcontext structure to the user's address space. */
  اگر ((r = ڈیٹا_copy(KERNEL, (vir_بائٹیں) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_بائٹیں) حجم(mcontext_t))) != OK)
	واپس(r);

  واپس(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
صحیح do_setmcontext(ساخت proc * caller, پیغام * m_ptr)
{
/* Set machine context of a process */

  اندراج ساخت proc *rp;
  صحیح proc_nr, r;
  mcontext_t mc;

  اگر (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) واپس(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  اگر ((r = ڈیٹا_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_بائٹیں) &mc, (phys_بائٹیں) حجم(mcontext_t))) != OK)
	واپس(r);

#اگر defined(__i386__)
  /* Copy FPU state */
  اگر (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	أكد(حجم(mc.__fpregs.__fp_reg_set) == FPU_XFP_سائز);
	حافظہ_نقل(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_سائز);
  } ورنہ
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#اگر_ختم

  واپس(OK);
}

#اگر_ختم
