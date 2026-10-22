/* The kernel call that is implemented in this file:
 *	m_type: SYS_SIGRETURN
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endp		# process returning from handler
 *	m_sigcalls.sigctx	# pointer to sigcontext structure
 *
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <machine/cpu.h>

#যদি USE_SIGRETURN 

/*===========================================================================*
 *			      do_sigreturn				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_sigreturn(কাঠামো proc * caller, message * m_ptr)
{
/* POSIX style signals require sys_sigreturn to put things in order before 
 * the signalled process can resume execution
 */
  কাঠামো sigcontext sc;
  নিবন্ধ কাঠামো proc *rp;
  পূর্ণসংখ্যা proc_nr, r;

  যদি (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr)) ফেরত EINVAL;
  যদি (iskerneln(proc_nr)) ফেরত EPERM;
  rp = proc_addr(proc_nr);

  /* Copy in the sigcontext structure. */
  যদি ((r = তথ্য_copy(m_ptr->m_sigcalls.endpt,
		 (vir_bytes)m_ptr->m_sigcalls.sigctx, KERNEL,
		 (vir_bytes)&sc, আকার(কাঠামো sigcontext))) != OK)
	ফেরত r;

#যদি defined(__i386__)
  /* Restore user bits of psw from sc, maintain system bits from proc. */
  sc.sc_eflags  =  (sc.sc_eflags & X86_FLAGS_USER) |
                (rp->p_reg.psw & ~X86_FLAGS_USER);
#endif

#যদি defined(__i386__)
  /* Write back registers we allow to be restored, i.e.
   * not the segment ones.
   */
  rp->p_reg.di = sc.sc_edi;
  rp->p_reg.si = sc.sc_esi;
  rp->p_reg.fp = sc.sc_ebp;
  rp->p_reg.bx = sc.sc_ebx;
  rp->p_reg.dx = sc.sc_edx;
  rp->p_reg.cx = sc.sc_ecx;
  rp->p_reg.retreg = sc.sc_eax;
  rp->p_reg.pc = sc.sc_eip;
  rp->p_reg.psw = sc.sc_eflags;
  rp->p_reg.sp = sc.sc_esp;
#endif

#যদি defined(__arm__)
  rp->p_reg.psr = sc.sc_spsr;
  rp->p_reg.retreg = sc.sc_r0;
  rp->p_reg.r1 = sc.sc_r1;
  rp->p_reg.r2 = sc.sc_r2;
  rp->p_reg.r3 = sc.sc_r3;
  rp->p_reg.r4 = sc.sc_r4;
  rp->p_reg.r5 = sc.sc_r5;
  rp->p_reg.r6 = sc.sc_r6;
  rp->p_reg.r7 = sc.sc_r7;
  rp->p_reg.r8 = sc.sc_r8;
  rp->p_reg.r9 = sc.sc_r9;
  rp->p_reg.r10 = sc.sc_r10;
  rp->p_reg.fp = sc.sc_r11;
  rp->p_reg.r12 = sc.sc_r12;
  rp->p_reg.sp = sc.sc_usr_sp;
  rp->p_reg.lr = sc.sc_usr_lr;
  rp->p_reg.pc = sc.sc_pc;
#endif

  /* Restore the registers. */
  arch_proc_setcontext(rp, &rp->p_reg, 1, sc.trap_style);

  যদি(sc.sc_magic != SC_MAGIC) { printf("kernel sigreturn: corrupt signal context\n"); }

#যদি defined(__i386__)
  যদি (sc.sc_flags & MF_FPU_INITIALIZED)
  {
	memcpy(rp->p_seg.fpu_state, &sc.sc_fpu_state, FPU_XFP_আকার);
	rp->p_misc_flags |=  MF_FPU_INITIALIZED; /* Restore math usage flag. */
	/* force reloading FPU */
	release_fpu(rp);
  }
#endif

  ফেরত OK;
}
#endif /* USE_SIGRETURN */

