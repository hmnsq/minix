/* The kernel call that is implemented in this file:
 *	m_type: SYS_SIGSEND
 *
 * The parameters for this kernel call are:
 * 	m_sigcalls.endpt	# process to call signal handler
 *	m_sigcalls.sigctx	# pointer to sigcontext structure
 *
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <signal.h>
#अन्तर्भाव <string.h>

#यदि USE_SIGSEND

/*===========================================================================*
 *			      do_sigsend				     *
 *===========================================================================*/
पूर्णाङ्क do_sigsend(संरचना proc * caller, message * m_ptr)
{
/* Handle sys_sigsend, POSIX-style signal handling. */

  संरचना sigmsg smsg;
  पञ्जिका संरचना proc *rp;
  संरचना sigframe_sigcontext fr, *frp;
  पूर्णाङ्क proc_nr, r;
#यदि defined(__i386__)
  reg_t new_fp;
#endif

  यदि (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr)) फर्काऊ EINVAL;
  यदि (iskerneln(proc_nr)) फर्काऊ EPERM;
  rp = proc_addr(proc_nr);

  /* Get the sigmsg structure into our address space.  */
  यदि ((r = data_copy_vmcheck(caller, caller->p_endpoint,
		(vir_bytes)m_ptr->m_sigcalls.sigctx, KERNEL,
		(vir_bytes)&smsg, (phys_bytes) परिमाण(संरचना sigmsg))) != OK)
	फर्काऊ r;

  /* WARNING: the following code may be run more than once even for a single
   * signal delivery. Do not change registers here. See the comment below.
   */

  /* Compute the user stack pointer where sigframe will start. */
  smsg.sm_stkptr = arch_get_sp(rp);
  frp = (संरचना sigframe_sigcontext *) smsg.sm_stkptr - 1;

  /* Copy the registers to the sigcontext structure. */
  स_रखो(&fr, 0, परिमाण(fr));
  fr.sf_scp = &frp->sf_sc;

#यदि defined(__i386__)
  fr.sf_sc.sc_gs = rp->p_reg.gs;
  fr.sf_sc.sc_fs = rp->p_reg.fs;
  fr.sf_sc.sc_es = rp->p_reg.es;
  fr.sf_sc.sc_ds = rp->p_reg.ds;
  fr.sf_sc.sc_edi = rp->p_reg.di;
  fr.sf_sc.sc_esi = rp->p_reg.si;
  fr.sf_sc.sc_ebp = rp->p_reg.fp;
  fr.sf_sc.sc_ebx = rp->p_reg.bx;
  fr.sf_sc.sc_edx = rp->p_reg.dx;
  fr.sf_sc.sc_ecx = rp->p_reg.cx;
  fr.sf_sc.sc_eax = rp->p_reg.retreg;
  fr.sf_sc.sc_eip = rp->p_reg.pc;
  fr.sf_sc.sc_cs = rp->p_reg.cs;
  fr.sf_sc.sc_eflags = rp->p_reg.psw;
  fr.sf_sc.sc_esp = rp->p_reg.sp;
  fr.sf_sc.sc_ss = rp->p_reg.ss;
  fr.sf_fp = rp->p_reg.fp;
  fr.sf_signum = smsg.sm_signo;
  new_fp = (reg_t) &frp->sf_fp;
  fr.sf_scpcopy = fr.sf_scp;
  fr.sf_ra_sigreturn = smsg.sm_sigreturn;
  fr.sf_ra= rp->p_reg.pc;

  fr.sf_sc.trap_style = rp->p_seg.p_kern_trap_style;

  यदि (fr.sf_sc.trap_style == KTS_NONE) {
  	म_लिखो("do_sigsend: sigsend an unsaved process\n");
	फर्काऊ EINVAL;
  }

  यदि (proc_used_fpu(rp)) {
	/* save the FPU context before saving it to the sig context */
	save_fpu(rp);
	स_नकल(&fr.sf_sc.sc_fpu_state, rp->p_seg.fpu_state, FPU_XFP_SIZE);
  }
#endif

#यदि defined(__arm__)
  fr.sf_sc.sc_spsr = rp->p_reg.psr;
  fr.sf_sc.sc_r0 = rp->p_reg.retreg;
  fr.sf_sc.sc_r1 = rp->p_reg.r1;
  fr.sf_sc.sc_r2 = rp->p_reg.r2;
  fr.sf_sc.sc_r3 = rp->p_reg.r3;
  fr.sf_sc.sc_r4 = rp->p_reg.r4;
  fr.sf_sc.sc_r5 = rp->p_reg.r5;
  fr.sf_sc.sc_r6 = rp->p_reg.r6;
  fr.sf_sc.sc_r7 = rp->p_reg.r7;
  fr.sf_sc.sc_r8 = rp->p_reg.r8;
  fr.sf_sc.sc_r9 = rp->p_reg.r9;
  fr.sf_sc.sc_r10 = rp->p_reg.r10;
  fr.sf_sc.sc_r11 = rp->p_reg.fp;
  fr.sf_sc.sc_r12 = rp->p_reg.r12;
  fr.sf_sc.sc_usr_sp = rp->p_reg.sp;
  fr.sf_sc.sc_usr_lr = rp->p_reg.lr;
  fr.sf_sc.sc_svc_lr = 0;	/* ? */
  fr.sf_sc.sc_pc = rp->p_reg.pc;	/* R15 */
#endif

  /* Finish the sigcontext initialization. */
  fr.sf_sc.sc_mask = smsg.sm_mask;
  fr.sf_sc.sc_flags = rp->p_misc_flags & MF_FPU_INITIALIZED;
  fr.sf_sc.sc_magic = SC_MAGIC;

  /* Initialize the sigframe structure. */
  fpu_sigcontext(rp, &fr, &fr.sf_sc);

  /* Copy the sigframe structure to the user's stack. */
  यदि ((r = data_copy_vmcheck(caller, KERNEL, (vir_bytes)&fr,
		m_ptr->m_sigcalls.endpt, (vir_bytes)frp,
		(vir_bytes)परिमाण(संरचना sigframe_sigcontext))) != OK)
      फर्काऊ r;

  /* WARNING: up to the statement above, the code may run multiple times, since
   * copying out the frame/context may fail with VMSUSPEND the first time. For
   * that reason, changes to process registers *MUST* be deferred until after
   * this last copy -- otherwise, these changes will be made several times,
   * possibly leading to corrupted process state.
   */

  /* Reset user registers to execute the signal handler. */
  rp->p_reg.sp = (reg_t) frp;
  rp->p_reg.pc = (reg_t) smsg.sm_sighandler;

#यदि defined(__i386__)
  rp->p_reg.fp = new_fp;
#अथवायदि defined(__arm__)
  /* use the ARM link register to set the return address from the signal
   * handler
   */
  rp->p_reg.lr = (reg_t) smsg.sm_sigreturn;
  यदि(rp->p_reg.lr & 1) { म_लिखो("sigsend: LSB LR makes no sense.\n"); }

  /* pass signal handler parameters in registers */
  rp->p_reg.retreg = (reg_t) smsg.sm_signo;
  rp->p_reg.r1 = 0;	/* sf_code */
  rp->p_reg.r2 = (reg_t) fr.sf_scp;
  rp->p_misc_flags |= MF_CONTEXT_SET;
#endif

  /* Signal handler should get clean FPU. */
  rp->p_misc_flags &= ~MF_FPU_INITIALIZED;

  यदि(!RTS_ISSET(rp, RTS_PROC_STOP)) {
	म_लिखो("system: warning: sigsend a running process\n");
	म_लिखो("caller stack: ");
	proc_stacktrace(caller);
  }

  फर्काऊ OK;
}

#endif /* USE_SIGSEND */

