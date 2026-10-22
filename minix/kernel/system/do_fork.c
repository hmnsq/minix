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

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव "kernel/vm.h"
#अन्तर्भाव <signal.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <assert.h>

#अन्तर्भाव <minix/endpoint.h>
#अन्तर्भाव <minix/u64.h>

#यदि USE_FORK

/*===========================================================================*
 *				do_fork					     *
 *===========================================================================*/
पूर्णाङ्क do_fork(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Handle sys_fork().
 * m_lsys_krn_sys_fork.endpt has forked.
 * The child is m_lsys_krn_sys_fork.slot.
 */
#यदि defined(__i386__)
  वर्ण *old_fpu_save_area_p;
#यदि_अन्तः
  पञ्जिका संरचना proc *rpc;		/* child process pointer */
  संरचना proc *rpp;			/* parent process pointer */
  पूर्णाङ्क gen;
  पूर्णाङ्क p_proc;
  पूर्णाङ्क namelen;

  यदि(!isokendpt(m_ptr->m_lsys_krn_sys_fork.endpt, &p_proc))
	प्रत्यागम EINVAL;

  rpp = proc_addr(p_proc);
  rpc = proc_addr(m_ptr->m_lsys_krn_sys_fork.slot);
  यदि (isemptyp(rpp) || ! isemptyp(rpc)) प्रत्यागम(EINVAL);

  प्रतिज्ञा(!(rpp->p_misc_flags & MF_DELIVERMSG));

  /* needs to be receiving so we know where the message buffer is */
  यदि(!RTS_ISSET(rpp, RTS_RECEIVING)) {
	प्रारूप_लिख("kernel: fork not done synchronously?\n");
	प्रत्यागम EINVAL;
  }

  /* make sure that the FPU context is saved in parent before copy */
  save_fpu(rpp);
  /* Copy parent 'proc' struct to child. And reinitialize some fields. */
  gen = _ENDPOINT_G(rpc->p_endpoint);
#यदि defined(__i386__)
  old_fpu_save_area_p = rpc->p_seg.fpu_state;
#यदि_अन्तः
  *rpc = *rpp;				/* copy 'proc' struct */
#यदि defined(__i386__)
  rpc->p_seg.fpu_state = old_fpu_save_area_p;
  यदि(proc_प्रयुक्तम्_fpu(rpp))
	स्मृतिं_प्रतिलिख(rpc->p_seg.fpu_state, rpp->p_seg.fpu_state, FPU_XFP_आकार);
#यदि_अन्तः
  यदि(++gen >= _ENDPOINT_MAX_GENERATION)	/* increase generation */
	gen = 1;			/* generation number wraparound */
  rpc->p_nr = m_ptr->m_lsys_krn_sys_fork.slot;	/* this was obliterated by copy */
  rpc->p_endpoint = _ENDPOINT(gen, rpc->p_nr);	/* new endpoint of slot */

  rpc->p_reg.retreg = 0;	/* child sees pid = 0 to know it is child */
  rpc->p_user_time = 0;		/* set all the accounting times to 0 */
  rpc->p_sys_time = 0;

  rpc->p_misc_flags &=
	~(MF_VIRT_TIMER | MF_PROF_TIMER | MF_SC_TRACE | MF_SPROF_आलोकितम् | MF_चरणः);
  rpc->p_virt_वामः = 0;		/* disable, clear the process-virtual timers */
  rpc->p_prof_वामः = 0;

  /* Mark process name as being a forked copy */
  namelen = वाक्यदैर्घ्यम्(rpc->p_name);
#परिभाषय FORKSTR "*F"
  यदि(namelen+वाक्यदैर्घ्यम्(FORKSTR) < परिमाण(rpc->p_name))
	वाक्यं_योजय(rpc->p_name, FORKSTR);

  /* the child process is not runnable until it's scheduled. */
  RTS_समुच्चयः(rpc, RTS_NO_कालांशः);
  reset_proc_accounting(rpc);

  rpc->p_cpu_time_वामः = 0;
  rpc->p_आवृत्तयः = 0;
  rpc->p_kcall_आवृत्तयः = 0;
  rpc->p_kipc_आवृत्तयः = 0;

  rpc->p_tick_आवृत्तयः = 0;
  cpuavg_init(&rpc->p_cpuavg);

  /* If the parent is a privileged process, take away the privileges from the 
   * child process and inhibit it from running by setting the NO_PRIV flag.
   * The caller should explicitly set the new privileges before executing.
   */
  यदि (priv(rpp)->s_flags & SYS_PROC) {
      rpc->p_priv = priv_addr(USER_PRIV_ID);
      rpc->p_rts_flags |= RTS_NO_PRIV;
  }

  /* Calculate endpoint identifier, so caller knows what it is. */
  m_ptr->m_krn_lsys_sys_fork.endpt = rpc->p_endpoint;
  m_ptr->m_krn_lsys_sys_fork.msgaddr = rpp->p_delivermsg_vir;

  /* Don't schedule process in VM mode until it has a new pagetable. */
  यदि(m_ptr->m_lsys_krn_sys_fork.flags & PFF_VMINHIBIT) {
  	RTS_समुच्चयः(rpc, RTS_VMINHIBIT);
  }

  /* 
   * Only one in group should have RTS_SIGNALED, child doesn't inherit tracing.
   */
  RTS_UNSET(rpc, (RTS_SIGNALED | RTS_SIG_PENDING | RTS_P_STOP));
  (शून्य) sigemptyset(&rpc->p_pending);

#यदि defined(__i386__)
  rpc->p_seg.p_cr3 = 0;
  rpc->p_seg.p_cr3_v = रिक्त;
#नोचेद्यदि defined(__arm__)
  rpc->p_seg.p_ttbr = 0;
  rpc->p_seg.p_ttbr_v = रिक्त;
#यदि_अन्तः

  प्रत्यागम OK;
}

#यदि_अन्तः /* USE_FORK */
