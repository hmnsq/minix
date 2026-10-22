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

#include "kernel/system.h"
#include <string.h>
#include <assert.h>
#include <machine/mcontext.h>

#ถ้า USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
จำนวนเต็ม do_getmcontext(sa_nracanaa proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  pa_njikaa sa_nracanaa proc *rp;
  จำนวนเต็ม proc_nr, r;
  mcontext_t mc;

  ถ้า (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	คืน(EINVAL);
  ถ้า (iskerneln(proc_nr)) คืน(EPERM);
  rp = proc_addr(proc_nr);

#ถ้า defined(__i386__)
  ถ้า (!proc_used_fpu(rp))
	คืน(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  ถ้า ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) parimaa_na(mcontext_t))) != OK)
	คืน(r);

  mc.mc_flags = 0;
#ถ้า defined(__i386__)
  /* Copy FPU state */
  ถ้า (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	assert(parimaa_na(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  ถ้า ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) parimaa_na(mcontext_t))) != OK)
	คืน(r);

  คืน(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
จำนวนเต็ม do_setmcontext(sa_nracanaa proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  pa_njikaa sa_nracanaa proc *rp;
  จำนวนเต็ม proc_nr, r;
  mcontext_t mc;

  ถ้า (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) คืน(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  ถ้า ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) parimaa_na(mcontext_t))) != OK)
	คืน(r);

#ถ้า defined(__i386__)
  /* Copy FPU state */
  ถ้า (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	assert(parimaa_na(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } มิฉะนั้น
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  คืน(OK);
}

#endif
