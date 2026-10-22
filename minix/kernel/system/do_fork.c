/* The kernel call implemented in this file:
 *   m_type:	SYS_FORK
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_fork.endpt		(parent, process that forked)
 *   m_lsys_krn_sys_fork.slot		(child's process table slot)
 *   m_lsys_krn_sys_fork.flags		(fork flags)
 *   m_krn_lsys_sys_fork.endpt		(endpoint of the child)
 *   m_krn_lsys_sys_fork.msgaddr	(new memory map for the child)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>
#ಅನ್ತರ್ಭಾವ <minix/u64.h>

#ಆದರೆ USE_FORK

/*===========================================================================*
 *				do_fork					     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_fork(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Handle sys_fork().
 * m_lsys_krn_sys_fork.endpt has forked.
 * The child is m_lsys_krn_sys_fork.slot.
 */
#ಆದರೆ defined(__i386__)
  ಅಕ್ಷರ *old_fpu_save_area_p;
#ಆದರೆ_ಅಂತ್ಯ
  ನೋಂದಣಿ ರಚನೆ proc *rpc;		/* child process pointer */
  ರಚನೆ proc *rpp;			/* parent process pointer */
  ಪೂರ್ಣಾಂಕ gen;
  ಪೂರ್ಣಾಂಕ p_proc;
  ಪೂರ್ಣಾಂಕ namelen;

  ಆದರೆ(!isokendpt(m_ptr->m_lsys_krn_sys_fork.endpt, &p_proc))
	ಹಿಂತಿರುಗಿ EINVAL;

  rpp = proc_addr(p_proc);
  rpc = proc_addr(m_ptr->m_lsys_krn_sys_fork.ಸ್ಥಾನ);
  ಆದರೆ (isemptyp(rpp) || ! isemptyp(rpc)) ಹಿಂತಿರುಗಿ(EINVAL);

  ಪ್ರತಿಜ್ಞಾ(!(rpp->p_misc_flags & MF_DELIVERMSG));

  /* needs to be receiving so we know where the message buffer is */
  ಆದರೆ(!RTS_ISSET(rpp, RTS_RECEIVING)) {
	ಬರೆ("kernel: fork not done synchronously?\n");
	ಹಿಂತಿರುಗಿ EINVAL;
  }

  /* make sure that the FPU context is saved in parent before copy */
  save_fpu(rpp);
  /* Copy parent 'proc' struct to child. And reinitialize some fields. */
  gen = _ENDPOINT_G(rpc->p_endpoint);
#ಆದರೆ defined(__i386__)
  old_fpu_save_area_p = rpc->p_seg.fpu_state;
#ಆದರೆ_ಅಂತ್ಯ
  *rpc = *rpp;				/* copy 'proc' struct */
#ಆದರೆ defined(__i386__)
  rpc->p_seg.fpu_state = old_fpu_save_area_p;
  ಆದರೆ(proc_ಬಳಸಿದ_fpu(rpp))
	ಸ್ಮರಣೆ_ನಕಲು(rpc->p_seg.fpu_state, rpp->p_seg.fpu_state, FPU_XFP_ಗಾತ್ರ);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(++gen >= _ENDPOINT_MAX_GENERATION)	/* increase generation */
	gen = 1;			/* generation number wraparound */
  rpc->p_nr = m_ptr->m_lsys_krn_sys_fork.ಸ್ಥಾನ;	/* this was obliterated by copy */
  rpc->p_endpoint = _ENDPOINT(gen, rpc->p_nr);	/* new endpoint of slot */

  rpc->p_reg.retreg = 0;	/* child sees pid = 0 to know it is child */
  rpc->p_user_ಸಮಯ = 0;		/* set all the accounting times to 0 */
  rpc->p_sys_ಸಮಯ = 0;

  rpc->p_misc_flags &=
	~(MF_VIRT_TIMER | MF_PROF_TIMER | MF_SC_TRACE | MF_SPROF_ನೋಡಿದ | MF_ಹೆಜ್ಜೆ);
  rpc->p_virt_ಎಡ = 0;		/* disable, clear the process-virtual timers */
  rpc->p_prof_ಎಡ = 0;

  /* Mark process name as being a forked copy */
  namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(rpc->p_name);
#ವ್ಯಾಖ್ಯಾನಿಸು FORKSTR "*F"
  ಆದರೆ(namelen+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(FORKSTR) < ಗಾತ್ರ(rpc->p_name))
	strcat(rpc->p_name, FORKSTR);

  /* the child process is not runnable until it's scheduled. */
  RTS_ಗಣ(rpc, RTS_NO_ಕಾಲಾಂಶ);
  reset_proc_accounting(rpc);

  rpc->p_cpu_ಸಮಯ_ಎಡ = 0;
  rpc->p_ಚಕ್ರಗಳು = 0;
  rpc->p_kcall_ಚಕ್ರಗಳು = 0;
  rpc->p_kipc_ಚಕ್ರಗಳು = 0;

  rpc->p_tick_ಚಕ್ರಗಳು = 0;
  cpuavg_init(&rpc->p_cpuavg);

  /* If the parent is a privileged process, take away the privileges from the 
   * child process and inhibit it from running by setting the NO_PRIV flag.
   * The caller should explicitly set the new privileges before executing.
   */
  ಆದರೆ (priv(rpp)->s_flags & SYS_PROC) {
      rpc->p_priv = priv_addr(USER_PRIV_ID);
      rpc->p_rts_flags |= RTS_NO_PRIV;
  }

  /* Calculate endpoint identifier, so caller knows what it is. */
  m_ptr->m_krn_lsys_sys_fork.endpt = rpc->p_endpoint;
  m_ptr->m_krn_lsys_sys_fork.msgaddr = rpp->p_delivermsg_vir;

  /* Don't schedule process in VM mode until it has a new pagetable. */
  ಆದರೆ(m_ptr->m_lsys_krn_sys_fork.flags & PFF_VMINHIBIT) {
  	RTS_ಗಣ(rpc, RTS_VMINHIBIT);
  }

  /* 
   * Only one in group should have RTS_SIGNALED, child doesn't inherit tracing.
   */
  RTS_UNSET(rpc, (RTS_SIGNALED | RTS_SIG_PENDING | RTS_P_STOP));
  (ಶೂನ್ಯ) sigemptyset(&rpc->p_pending);

#ಆದರೆ defined(__i386__)
  rpc->p_seg.p_cr3 = 0;
  rpc->p_seg.p_cr3_v = ಖಾಲಿ;
#ಅಥವಾಆದರೆ defined(__arm__)
  rpc->p_seg.p_ttbr = 0;
  rpc->p_seg.p_ttbr_v = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_FORK */
