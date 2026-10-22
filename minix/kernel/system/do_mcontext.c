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

#ຖ້າ USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
ຈຳນວນເຕັມ do_getmcontext(sa_nracanaa proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  pa_njikaa sa_nracanaa proc *rp;
  ຈຳນວນເຕັມ proc_nr, r;
  mcontext_t mc;

  ຖ້າ (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	ສົ່ງຄືນ(EINVAL);
  ຖ້າ (iskerneln(proc_nr)) ສົ່ງຄືນ(EPERM);
  rp = proc_addr(proc_nr);

#ຖ້າ defined(__i386__)
  ຖ້າ (!proc_used_fpu(rp))
	ສົ່ງຄືນ(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  ຖ້າ ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) parimaa_na(mcontext_t))) != OK)
	ສົ່ງຄືນ(r);

  mc.mc_flags = 0;
#ຖ້າ defined(__i386__)
  /* Copy FPU state */
  ຖ້າ (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	assert(parimaa_na(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE);
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  ຖ້າ ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) parimaa_na(mcontext_t))) != OK)
	ສົ່ງຄືນ(r);

  ສົ່ງຄືນ(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
ຈຳນວນເຕັມ do_setmcontext(sa_nracanaa proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  pa_njikaa sa_nracanaa proc *rp;
  ຈຳນວນເຕັມ proc_nr, r;
  mcontext_t mc;

  ຖ້າ (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) ສົ່ງຄືນ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  ຖ້າ ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) parimaa_na(mcontext_t))) != OK)
	ສົ່ງຄືນ(r);

#ຖ້າ defined(__i386__)
  /* Copy FPU state */
  ຖ້າ (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	assert(parimaa_na(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	memcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE);
  } ບໍ່ດັ່ງນັ້ນ
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  ສົ່ງຄືນ(OK);
}

#endif
