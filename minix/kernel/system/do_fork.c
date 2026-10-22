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

#ضمن "kernel/system.h"
#ضمن "kernel/vm.h"
#ضمن <signal.h>
#ضمن <string.h>
#ضمن <assert.h>

#ضمن <minix/endpoint.h>
#ضمن <minix/u64.h>

#إذا USE_FORK

/*===========================================================================*
 *				do_fork					     *
 *===========================================================================*/
صحيح do_fork(بنية proc * caller, سنیہا * m_ptr)
{
/* Handle sys_fork().
 * m_lsys_krn_sys_fork.endpt has forked.
 * The child is m_lsys_krn_sys_fork.slot.
 */
#إذا defined(__i386__)
  حرف *old_fpu_save_area_p;
#جے_انت
  سجل بنية proc *rpc;		/* child process pointer */
  بنية proc *rpp;			/* parent process pointer */
  صحيح gen;
  صحيح p_proc;
  صحيح namelen;

  إذا(!isokendpt(m_ptr->m_lsys_krn_sys_fork.endpt, &p_proc))
	أرجع EINVAL;

  rpp = proc_addr(p_proc);
  rpc = proc_addr(m_ptr->m_lsys_krn_sys_fork.تھاں);
  إذا (isemptyp(rpp) || ! isemptyp(rpc)) أرجع(EINVAL);

  أكد(!(rpp->p_misc_flags & MF_DELIVERMSG));

  /* needs to be receiving so we know where the message buffer is */
  إذا(!RTS_ISSET(rpp, RTS_RECEIVING)) {
	لکھو("kernel: fork not done synchronously?\n");
	أرجع EINVAL;
  }

  /* make sure that the FPU context is saved in parent before copy */
  save_fpu(rpp);
  /* Copy parent 'proc' struct to child. And reinitialize some fields. */
  gen = _ENDPOINT_G(rpc->p_endpoint);
#إذا defined(__i386__)
  old_fpu_save_area_p = rpc->p_seg.fpu_state;
#جے_انت
  *rpc = *rpp;				/* copy 'proc' struct */
#إذا defined(__i386__)
  rpc->p_seg.fpu_state = old_fpu_save_area_p;
  إذا(proc_ورتیا_fpu(rpp))
	حافظہ_نقل(rpc->p_seg.fpu_state, rpp->p_seg.fpu_state, FPU_XFP_ناپ);
#جے_انت
  إذا(++gen >= _ENDPOINT_MAX_GENERATION)	/* increase generation */
	gen = 1;			/* generation number wraparound */
  rpc->p_nr = m_ptr->m_lsys_krn_sys_fork.تھاں;	/* this was obliterated by copy */
  rpc->p_endpoint = _ENDPOINT(gen, rpc->p_nr);	/* new endpoint of slot */

  rpc->p_reg.retreg = 0;	/* child sees pid = 0 to know it is child */
  rpc->p_user_ویلا = 0;		/* set all the accounting times to 0 */
  rpc->p_sys_ویلا = 0;

  rpc->p_misc_flags &=
	~(MF_VIRT_TIMER | MF_PROF_TIMER | MF_SC_TRACE | MF_SPROF_تکیا | MF_قدم);
  rpc->p_virt_بایاں = 0;		/* disable, clear the process-virtual timers */
  rpc->p_prof_بایاں = 0;

  /* Mark process name as being a forked copy */
  namelen = لڑی_لمبائی(rpc->p_name);
#تعریف_کرو FORKSTR "*F"
  إذا(namelen+لڑی_لمبائی(FORKSTR) < حجم(rpc->p_name))
	strcat(rpc->p_name, FORKSTR);

  /* the child process is not runnable until it's scheduled. */
  RTS_سیٹ(rpc, RTS_NO_ویلا_حصہ);
  reset_proc_accounting(rpc);

  rpc->p_cpu_ویلا_بایاں = 0;
  rpc->p_چکراں = 0;
  rpc->p_kcall_چکراں = 0;
  rpc->p_kipc_چکراں = 0;

  rpc->p_tick_چکراں = 0;
  cpuavg_init(&rpc->p_cpuavg);

  /* If the parent is a privileged process, take away the privileges from the 
   * child process and inhibit it from running by setting the NO_PRIV flag.
   * The caller should explicitly set the new privileges before executing.
   */
  إذا (priv(rpp)->s_flags & SYS_PROC) {
      rpc->p_priv = priv_addr(USER_PRIV_ID);
      rpc->p_rts_flags |= RTS_NO_PRIV;
  }

  /* Calculate endpoint identifier, so caller knows what it is. */
  m_ptr->m_krn_lsys_sys_fork.endpt = rpc->p_endpoint;
  m_ptr->m_krn_lsys_sys_fork.msgaddr = rpp->p_delivermsg_vir;

  /* Don't schedule process in VM mode until it has a new pagetable. */
  إذا(m_ptr->m_lsys_krn_sys_fork.flags & PFF_VMINHIBIT) {
  	RTS_سیٹ(rpc, RTS_VMINHIBIT);
  }

  /* 
   * Only one in group should have RTS_SIGNALED, child doesn't inherit tracing.
   */
  RTS_UNSET(rpc, (RTS_SIGNALED | RTS_SIG_PENDING | RTS_P_STOP));
  (فراغ) sigemptyset(&rpc->p_pending);

#إذا defined(__i386__)
  rpc->p_seg.p_cr3 = 0;
  rpc->p_seg.p_cr3_v = عدم;
#وإلاإذا defined(__arm__)
  rpc->p_seg.p_ttbr = 0;
  rpc->p_seg.p_ttbr_v = عدم;
#جے_انت

  أرجع OK;
}

#جے_انت /* USE_FORK */
