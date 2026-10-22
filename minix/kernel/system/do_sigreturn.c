/* The kernel call that is implemented in this file:
 *	m_type: SYS_SIGRETURN
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endp		# process returning from handler
 *	m_sigcalls.sigctx	# pointer to sigcontext structure
 *
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <string.h>
#अन्तर्भाव <machine/cpu.h>

#यदि USE_SIGRETURN 

/*===========================================================================*
 *			      do_sigreturn				     *
 *===========================================================================*/
पूर्णाङ्क do_sigreturn(संरचना proc * caller, सन्देशः * m_ptr)
{
/* POSIX style signals require sys_sigreturn to put things in order before 
 * the signalled process can resume execution
 */
  संरचना sigcontext sc;
  पञ्जिका संरचना proc *rp;
  पूर्णाङ्क proc_nr, r;

  यदि (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr)) प्रत्यागम EINVAL;
  यदि (iskerneln(proc_nr)) प्रत्यागम EPERM;
  rp = proc_addr(proc_nr);

  /* Copy in the sigcontext structure. */
  यदि ((r = दत्त_copy(m_ptr->m_sigcalls.endpt,
		 (vir_अष्टकानि)m_ptr->m_sigcalls.sigctx, KERNEL,
		 (vir_अष्टकानि)&sc, परिमाण(संरचना sigcontext))) != OK)
	प्रत्यागम r;

#यदि defined(__i386__)
  /* Restore user bits of psw from sc, maintain system bits from proc. */
  sc.sc_eflags  =  (sc.sc_eflags & X86_FLAGS_USER) |
                (rp->p_reg.psw & ~X86_FLAGS_USER);
#यदि_अन्तः

#यदि defined(__i386__)
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
#यदि_अन्तः

#यदि defined(__arm__)
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
#यदि_अन्तः

  /* Restore the registers. */
  arch_proc_setcontext(rp, &rp->p_reg, 1, sc.trap_style);

  यदि(sc.sc_magic != SC_MAGIC) { प्रारूप_लिख("kernel sigreturn: corrupt signal context\n"); }

#यदि defined(__i386__)
  यदि (sc.sc_flags & MF_FPU_INITIALIZED)
  {
	स्मृतिं_प्रतिलिख(rp->p_seg.fpu_state, &sc.sc_fpu_state, FPU_XFP_आकार);
	rp->p_misc_flags |=  MF_FPU_INITIALIZED; /* Restore math usage flag. */
	/* force reloading FPU */
	release_fpu(rp);
  }
#यदि_अन्तः

  प्रत्यागम OK;
}
#यदि_अन्तः /* USE_SIGRETURN */

