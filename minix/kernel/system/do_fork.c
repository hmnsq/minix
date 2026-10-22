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

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ "kernel/vm.h"
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>

#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>
#ଅନ୍ତର୍ଭାଵ <minix/u64.h>

#ଯଦି USE_FORK

/*===========================================================================*
 *				do_fork					     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_fork(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Handle sys_fork().
 * m_lsys_krn_sys_fork.endpt has forked.
 * The child is m_lsys_krn_sys_fork.slot.
 */
#ଯଦି defined(__i386__)
  ଅକ୍ଷର *old_fpu_save_area_p;
#endif
  ପଞ୍ଜିକରଣ ସଂରଚନା proc *rpc;		/* child process pointer */
  ସଂରଚନା proc *rpp;			/* parent process pointer */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା gen;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା p_proc;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen;

  ଯଦି(!isokendpt(m_ptr->m_lsys_krn_sys_fork.endpt, &p_proc))
	ଫେରନ୍ତୁ EINVAL;

  rpp = proc_addr(p_proc);
  rpc = proc_addr(m_ptr->m_lsys_krn_sys_fork.slot);
  ଯଦି (isemptyp(rpp) || ! isemptyp(rpc)) ଫେରନ୍ତୁ(EINVAL);

  ପ୍ରତିଜ୍ଞା(!(rpp->p_misc_flags & MF_DELIVERMSG));

  /* needs to be receiving so we know where the message buffer is */
  ଯଦି(!RTS_ISSET(rpp, RTS_RECEIVING)) {
	printf("kernel: fork not done synchronously?\n");
	ଫେରନ୍ତୁ EINVAL;
  }

  /* make sure that the FPU context is saved in parent before copy */
  save_fpu(rpp);
  /* Copy parent 'proc' struct to child. And reinitialize some fields. */
  gen = _ENDPOINT_G(rpc->p_endpoint);
#ଯଦି defined(__i386__)
  old_fpu_save_area_p = rpc->p_seg.fpu_state;
#endif
  *rpc = *rpp;				/* copy 'proc' struct */
#ଯଦି defined(__i386__)
  rpc->p_seg.fpu_state = old_fpu_save_area_p;
  ଯଦି(proc_used_fpu(rpp))
	memcpy(rpc->p_seg.fpu_state, rpp->p_seg.fpu_state, FPU_XFP_SIZE);
#endif
  ଯଦି(++gen >= _ENDPOINT_MAX_GENERATION)	/* increase generation */
	gen = 1;			/* generation number wraparound */
  rpc->p_nr = m_ptr->m_lsys_krn_sys_fork.slot;	/* this was obliterated by copy */
  rpc->p_endpoint = _ENDPOINT(gen, rpc->p_nr);	/* new endpoint of slot */

  rpc->p_reg.retreg = 0;	/* child sees pid = 0 to know it is child */
  rpc->p_user_time = 0;		/* set all the accounting times to 0 */
  rpc->p_sys_time = 0;

  rpc->p_misc_flags &=
	~(MF_VIRT_TIMER | MF_PROF_TIMER | MF_SC_TRACE | MF_SPROF_SEEN | MF_STEP);
  rpc->p_virt_ବାମ = 0;		/* disable, clear the process-virtual timers */
  rpc->p_prof_ବାମ = 0;

  /* Mark process name as being a forked copy */
  namelen = strlen(rpc->p_name);
#define FORKSTR "*F"
  ଯଦି(namelen+strlen(FORKSTR) < ଆକାର(rpc->p_name))
	strcat(rpc->p_name, FORKSTR);

  /* the child process is not runnable until it's scheduled. */
  RTS_SET(rpc, RTS_NO_QUANTUM);
  reset_proc_accounting(rpc);

  rpc->p_cpu_time_ବାମ = 0;
  rpc->p_cycles = 0;
  rpc->p_kcall_cycles = 0;
  rpc->p_kipc_cycles = 0;

  rpc->p_tick_cycles = 0;
  cpuavg_init(&rpc->p_cpuavg);

  /* If the parent is a privileged process, take away the privileges from the 
   * child process and inhibit it from running by setting the NO_PRIV flag.
   * The caller should explicitly set the new privileges before executing.
   */
  ଯଦି (priv(rpp)->s_flags & SYS_PROC) {
      rpc->p_priv = priv_addr(USER_PRIV_ID);
      rpc->p_rts_flags |= RTS_NO_PRIV;
  }

  /* Calculate endpoint identifier, so caller knows what it is. */
  m_ptr->m_krn_lsys_sys_fork.endpt = rpc->p_endpoint;
  m_ptr->m_krn_lsys_sys_fork.msgaddr = rpp->p_delivermsg_vir;

  /* Don't schedule process in VM mode until it has a new pagetable. */
  ଯଦି(m_ptr->m_lsys_krn_sys_fork.flags & PFF_VMINHIBIT) {
  	RTS_SET(rpc, RTS_VMINHIBIT);
  }

  /* 
   * Only one in group should have RTS_SIGNALED, child doesn't inherit tracing.
   */
  RTS_UNSET(rpc, (RTS_SIGNALED | RTS_SIG_PENDING | RTS_P_STOP));
  (ଶୂନ୍ୟ) sigemptyset(&rpc->p_pending);

#ଯଦି defined(__i386__)
  rpc->p_seg.p_cr3 = 0;
  rpc->p_seg.p_cr3_v = ଖାଲି;
#ନଚେତଯଦି defined(__arm__)
  rpc->p_seg.p_ttbr = 0;
  rpc->p_seg.p_ttbr_v = ଖାଲି;
#endif

  ଫେରନ୍ତୁ OK;
}

#endif /* USE_FORK */
