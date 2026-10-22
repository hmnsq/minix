/* The kernel call that is implemented in this file:
 *	m_type: SYS_SIGRETURN
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endp		# process returning from handler
 *	m_sigcalls.sigctx	# pointer to sigcontext structure
 *
 */

#समावेश "kernel/system.h"
#समावेश <string.h>
#समावेश <machine/cpu.h>

#अगर USE_SIGRETURN 

/*===========================================================================*
 *			      do_sigreturn				     *
 *===========================================================================*/
पूर्णांक do_sigreturn(काष्ठा proc * caller, message * m_ptr)
{
/* POSIX style signals require sys_sigreturn to put things in order before 
 * the signalled process can resume execution
 */
  काष्ठा sigcontext sc;
  रेजिस्टर काष्ठा proc *rp;
  पूर्णांक proc_nr, r;

  अगर (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr)) वापस EINVAL;
  अगर (iskerneln(proc_nr)) वापस EPERM;
  rp = proc_addr(proc_nr);

  /* Copy in the sigcontext structure. */
  अगर ((r = तथ्य_copy(m_ptr->m_sigcalls.endpt,
		 (vir_bytes)m_ptr->m_sigcalls.sigctx, KERNEL,
		 (vir_bytes)&sc, माप(काष्ठा sigcontext))) != OK)
	वापस r;

#अगर defined(__i386__)
  /* Restore user bits of psw from sc, maintain system bits from proc. */
  sc.sc_eflags  =  (sc.sc_eflags & X86_FLAGS_USER) |
                (rp->p_reg.psw & ~X86_FLAGS_USER);
#पूर्ण_अगर

#अगर defined(__i386__)
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
#पूर्ण_अगर

#अगर defined(__arm__)
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
#पूर्ण_अगर

  /* Restore the registers. */
  arch_proc_setcontext(rp, &rp->p_reg, 1, sc.trap_style);

  अगर(sc.sc_magic != SC_MAGIC) { म_लिखो("kernel sigreturn: corrupt signal context\n"); }

#अगर defined(__i386__)
  अगर (sc.sc_flags & MF_FPU_INITIALIZED)
  {
	स_नकल(rp->p_seg.fpu_state, &sc.sc_fpu_state, FPU_XFP_आकार);
	rp->p_misc_flags |=  MF_FPU_INITIALIZED; /* Restore math usage flag. */
	/* force reloading FPU */
	release_fpu(rp);
  }
#पूर्ण_अगर

  वापस OK;
}
#पूर्ण_अगर /* USE_SIGRETURN */

