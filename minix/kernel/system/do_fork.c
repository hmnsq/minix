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

#شامل "kernel/system.h"
#شامل "kernel/vm.h"
#شامل <signal.h>
#شامل <string.h>
#شامل <assert.h>

#شامل <minix/endpoint.h>
#شامل <minix/u64.h>

#اگر USE_FORK

/*===========================================================================*
 *				do_fork					     *
 *===========================================================================*/
صحیح do_fork(ساخت proc * caller, پیغام * m_ptr)
{
/* Handle sys_fork().
 * m_lsys_krn_sys_fork.endpt has forked.
 * The child is m_lsys_krn_sys_fork.slot.
 */
#اگر defined(__i386__)
  حرف *old_fpu_save_area_p;
#اگر_ختم
  اندراج ساخت proc *rpc;		/* child process pointer */
  ساخت proc *rpp;			/* parent process pointer */
  صحیح gen;
  صحیح p_proc;
  صحیح namelen;

  اگر(!isokendpt(m_ptr->m_lsys_krn_sys_fork.endpt, &p_proc))
	واپس EINVAL;

  rpp = proc_addr(p_proc);
  rpc = proc_addr(m_ptr->m_lsys_krn_sys_fork.خانہ);
  اگر (isemptyp(rpp) || ! isemptyp(rpc)) واپس(EINVAL);

  أكد(!(rpp->p_misc_flags & MF_DELIVERMSG));

  /* needs to be receiving so we know where the message buffer is */
  اگر(!RTS_ISSET(rpp, RTS_RECEIVING)) {
	لکھو("kernel: fork not done synchronously?\n");
	واپس EINVAL;
  }

  /* make sure that the FPU context is saved in parent before copy */
  save_fpu(rpp);
  /* Copy parent 'proc' struct to child. And reinitialize some fields. */
  gen = _ENDPOINT_G(rpc->p_endpoint);
#اگر defined(__i386__)
  old_fpu_save_area_p = rpc->p_seg.fpu_state;
#اگر_ختم
  *rpc = *rpp;				/* copy 'proc' struct */
#اگر defined(__i386__)
  rpc->p_seg.fpu_state = old_fpu_save_area_p;
  اگر(proc_مستعمل_fpu(rpp))
	حافظہ_نقل(rpc->p_seg.fpu_state, rpp->p_seg.fpu_state, FPU_XFP_سائز);
#اگر_ختم
  اگر(++gen >= _ENDPOINT_MAX_GENERATION)	/* increase generation */
	gen = 1;			/* generation number wraparound */
  rpc->p_nr = m_ptr->m_lsys_krn_sys_fork.خانہ;	/* this was obliterated by copy */
  rpc->p_endpoint = _ENDPOINT(gen, rpc->p_nr);	/* new endpoint of slot */

  rpc->p_reg.retreg = 0;	/* child sees pid = 0 to know it is child */
  rpc->p_user_وقت = 0;		/* set all the accounting times to 0 */
  rpc->p_sys_وقت = 0;

  rpc->p_misc_flags &=
	~(MF_VIRT_TIMER | MF_PROF_TIMER | MF_SC_TRACE | MF_SPROF_نظر_آیا | MF_قدم);
  rpc->p_virt_بایاں = 0;		/* disable, clear the process-virtual timers */
  rpc->p_prof_بایاں = 0;

  /* Mark process name as being a forked copy */
  namelen = لڑی_لمبائی(rpc->p_name);
#تعریف_کرو FORKSTR "*F"
  اگر(namelen+لڑی_لمبائی(FORKSTR) < حجم(rpc->p_name))
	strcat(rpc->p_name, FORKSTR);

  /* the child process is not runnable until it's scheduled. */
  RTS_سیٹ(rpc, RTS_NO_وقتی_حصہ);
  reset_proc_accounting(rpc);

  rpc->p_cpu_وقت_بایاں = 0;
  rpc->p_ادوار = 0;
  rpc->p_kcall_ادوار = 0;
  rpc->p_kipc_ادوار = 0;

  rpc->p_tick_ادوار = 0;
  cpuavg_init(&rpc->p_cpuavg);

  /* If the parent is a privileged process, take away the privileges from the 
   * child process and inhibit it from running by setting the NO_PRIV flag.
   * The caller should explicitly set the new privileges before executing.
   */
  اگر (priv(rpp)->s_flags & SYS_PROC) {
      rpc->p_priv = priv_addr(USER_PRIV_ID);
      rpc->p_rts_flags |= RTS_NO_PRIV;
  }

  /* Calculate endpoint identifier, so caller knows what it is. */
  m_ptr->m_krn_lsys_sys_fork.endpt = rpc->p_endpoint;
  m_ptr->m_krn_lsys_sys_fork.msgaddr = rpp->p_delivermsg_vir;

  /* Don't schedule process in VM mode until it has a new pagetable. */
  اگر(m_ptr->m_lsys_krn_sys_fork.flags & PFF_VMINHIBIT) {
  	RTS_سیٹ(rpc, RTS_VMINHIBIT);
  }

  /* 
   * Only one in group should have RTS_SIGNALED, child doesn't inherit tracing.
   */
  RTS_UNSET(rpc, (RTS_SIGNALED | RTS_SIG_PENDING | RTS_P_STOP));
  (خالی) sigemptyset(&rpc->p_pending);

#اگر defined(__i386__)
  rpc->p_seg.p_cr3 = 0;
  rpc->p_seg.p_cr3_v = تہی;
#وگرنہ_اگر defined(__arm__)
  rpc->p_seg.p_ttbr = 0;
  rpc->p_seg.p_ttbr_v = تہی;
#اگر_ختم

  واپس OK;
}

#اگر_ختم /* USE_FORK */
