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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>

#ਜੇ USE_FORK

/*===========================================================================*
 *				do_fork					     *
 *===========================================================================*/
ਪੂਰਨ do_fork(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Handle sys_fork().
 * m_lsys_krn_sys_fork.endpt has forked.
 * The child is m_lsys_krn_sys_fork.slot.
 */
#ਜੇ defined(__i386__)
  ਅੱਖਰ *old_fpu_save_area_p;
#ਜੇ_ਅੰਤ
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rpc;		/* child process pointer */
  ਬਣਤਰ proc *rpp;			/* parent process pointer */
  ਪੂਰਨ gen;
  ਪੂਰਨ p_proc;
  ਪੂਰਨ namelen;

  ਜੇ(!isokendpt(m_ptr->m_lsys_krn_sys_fork.endpt, &p_proc))
	ਵਾਪਸ EINVAL;

  rpp = proc_addr(p_proc);
  rpc = proc_addr(m_ptr->m_lsys_krn_sys_fork.ਥਾਂ);
  ਜੇ (isemptyp(rpp) || ! isemptyp(rpc)) ਵਾਪਸ(EINVAL);

  ਪ੍ਰਤਿਜ੍ਞਾ(!(rpp->p_misc_flags & MF_DELIVERMSG));

  /* needs to be receiving so we know where the message buffer is */
  ਜੇ(!RTS_ISSET(rpp, RTS_RECEIVING)) {
	ਲਿਖੋ("kernel: fork not done synchronously?\n");
	ਵਾਪਸ EINVAL;
  }

  /* make sure that the FPU context is saved in parent before copy */
  save_fpu(rpp);
  /* Copy parent 'proc' struct to child. And reinitialize some fields. */
  gen = _ENDPOINT_G(rpc->p_endpoint);
#ਜੇ defined(__i386__)
  old_fpu_save_area_p = rpc->p_seg.fpu_state;
#ਜੇ_ਅੰਤ
  *rpc = *rpp;				/* copy 'proc' struct */
#ਜੇ defined(__i386__)
  rpc->p_seg.fpu_state = old_fpu_save_area_p;
  ਜੇ(proc_ਵਰਤਿਆ_fpu(rpp))
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(rpc->p_seg.fpu_state, rpp->p_seg.fpu_state, FPU_XFP_ਮਾਪ);
#ਜੇ_ਅੰਤ
  ਜੇ(++gen >= _ENDPOINT_MAX_GENERATION)	/* increase generation */
	gen = 1;			/* generation number wraparound */
  rpc->p_nr = m_ptr->m_lsys_krn_sys_fork.ਥਾਂ;	/* this was obliterated by copy */
  rpc->p_endpoint = _ENDPOINT(gen, rpc->p_nr);	/* new endpoint of slot */

  rpc->p_reg.retreg = 0;	/* child sees pid = 0 to know it is child */
  rpc->p_user_ਸਮਾਂ = 0;		/* set all the accounting times to 0 */
  rpc->p_sys_ਸਮਾਂ = 0;

  rpc->p_misc_flags &=
	~(MF_VIRT_TIMER | MF_PROF_TIMER | MF_SC_TRACE | MF_SPROF_ਵੇਖਿਆ | MF_ਕਦਮ);
  rpc->p_virt_ਖੱਬਾ = 0;		/* disable, clear the process-virtual timers */
  rpc->p_prof_ਖੱਬਾ = 0;

  /* Mark process name as being a forked copy */
  namelen = ਲੜੀ_ਲੰਬਾਈ(rpc->p_name);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORKSTR "*F"
  ਜੇ(namelen+ਲੜੀ_ਲੰਬਾਈ(FORKSTR) < ਆਕਾਰ(rpc->p_name))
	strcat(rpc->p_name, FORKSTR);

  /* the child process is not runnable until it's scheduled. */
  RTS_ਸੈੱਟ(rpc, RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ);
  reset_proc_accounting(rpc);

  rpc->p_cpu_ਸਮਾਂ_ਖੱਬਾ = 0;
  rpc->p_ਚੱਕਰਾਂ = 0;
  rpc->p_kcall_ਚੱਕਰਾਂ = 0;
  rpc->p_kipc_ਚੱਕਰਾਂ = 0;

  rpc->p_tick_ਚੱਕਰਾਂ = 0;
  cpuavg_init(&rpc->p_cpuavg);

  /* If the parent is a privileged process, take away the privileges from the 
   * child process and inhibit it from running by setting the NO_PRIV flag.
   * The caller should explicitly set the new privileges before executing.
   */
  ਜੇ (priv(rpp)->s_flags & SYS_PROC) {
      rpc->p_priv = priv_addr(USER_PRIV_ID);
      rpc->p_rts_flags |= RTS_NO_PRIV;
  }

  /* Calculate endpoint identifier, so caller knows what it is. */
  m_ptr->m_krn_lsys_sys_fork.endpt = rpc->p_endpoint;
  m_ptr->m_krn_lsys_sys_fork.msgaddr = rpp->p_delivermsg_vir;

  /* Don't schedule process in VM mode until it has a new pagetable. */
  ਜੇ(m_ptr->m_lsys_krn_sys_fork.flags & PFF_VMINHIBIT) {
  	RTS_ਸੈੱਟ(rpc, RTS_VMINHIBIT);
  }

  /* 
   * Only one in group should have RTS_SIGNALED, child doesn't inherit tracing.
   */
  RTS_UNSET(rpc, (RTS_SIGNALED | RTS_SIG_PENDING | RTS_P_STOP));
  (ਖਾਲੀ) sigemptyset(&rpc->p_pending);

#ਜੇ defined(__i386__)
  rpc->p_seg.p_cr3 = 0;
  rpc->p_seg.p_cr3_v = ਨਿਰਾ;
#ਜਾਂਜੇ defined(__arm__)
  rpc->p_seg.p_ttbr = 0;
  rpc->p_seg.p_ttbr_v = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

  ਵਾਪਸ OK;
}

#ਜੇ_ਅੰਤ /* USE_FORK */
