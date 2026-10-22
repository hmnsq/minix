/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

ਬਣਤਰ ex_s {
	ਅੱਖਰ *ਸੁਨੇਹਾ_2;
	ਪੂਰਨ signum;
	ਪੂਰਨ minprocessor;
};

ਸਥਿਰ ਬਣਤਰ ex_s ex_ਅੰਕੜੇ[] = {
	{ "Divide error", SIGFPE, 86 },
	{ "Debug exception", SIGTRAP, 86 },
	{ "Nonmaskable interrupt", SIGBUS, 86 },
	{ "Breakpoint", SIGEMT, 86 },
	{ "Overflow", SIGFPE, 86 },
	{ "Bounds check", SIGFPE, 186 },
	{ "Invalid opcode", SIGILL, 186 },
	{ "Coprocessor not available", SIGFPE, 186 },
	{ "Double fault", SIGBUS, 286 },
	{ "Coprocessor segment overrun", SIGSEGV, 286 },
	{ "Invalid TSS", SIGSEGV, 286 },
	{ "Segment not present", SIGSEGV, 286 },
	{ "Stack exception", SIGSEGV, 286 },	/* STACK_FAULT already used */
	{ "General protection", SIGSEGV, 286 },
	{ "Page fault", SIGSEGV, 386 },		/* not close */
	{ ਨਿਰਾ, SIGILL, 0 },			/* probably software trap */
	{ "Coprocessor error", SIGFPE, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", SIGFPE, 386 },
};

ਸਥਿਰ ਖਾਲੀ inkernel_disaster(ਬਣਤਰ proc *saved_proc,
	ਬਣਤਰ exception_ਚੌਖਟਾ *ਚੌਖਟਾ, ਬਣਤਰ ex_s *ep, ਪੂਰਨ is_nested);

ਬਾਹਰੀ ਪੂਰਨ catch_pagefaults;

ਸਥਿਰ ਖਾਲੀ proc_stacktrace_execute(ਬਣਤਰ proc *whichproc, reg_t v_bp, reg_t pc);

ਸਥਿਰ ਖਾਲੀ pagefault( ਬਣਤਰ proc *pr,
			ਬਣਤਰ exception_ਚੌਖਟਾ * ਚੌਖਟਾ,
			ਪੂਰਨ is_nested)
{
	ਪੂਰਨ ਵਿੱਚ_physcopy = 0, ਵਿੱਚ_memset = 0;

	reg_t pagefaultcr2;
	ਸੁਨੇਹਾ m_pagefault;
	ਪੂਰਨ err;

	pagefaultcr2 = read_cr2();

#ਜੇ 0
	ਲਿਖੋ("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#ਜੇ_ਅੰਤ

	ਵਿੱਚ_physcopy = (ਚੌਖਟਾ->eip > (vir_ਬਾਈਟਾਂ) phys_copy) &&
	   (ਚੌਖਟਾ->eip < (vir_ਬਾਈਟਾਂ) phys_copy_fault);

	ਵਿੱਚ_memset = (ਚੌਖਟਾ->eip > (vir_ਬਾਈਟਾਂ) phys_memset) &&
	   (ਚੌਖਟਾ->eip < (vir_ਬਾਈਟਾਂ) memset_fault);

	ਜੇ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (ਵਿੱਚ_physcopy || ਵਿੱਚ_memset)) {
#ਜੇ 0
		ਲਿਖੋ("pf caught! addr 0x%lx\n", pagefaultcr2);
#ਜੇ_ਅੰਤ
		ਜੇ (is_nested) {
			ਜੇ(ਵਿੱਚ_physcopy) {
				ਪ੍ਰਤਿਜ੍ਞਾ(!ਵਿੱਚ_memset);
				ਚੌਖਟਾ->eip = (reg_t) phys_copy_fault_ਵਿੱਚ_kernel;
			} ਨਹੀਂਤਾਂ {
				ਚੌਖਟਾ->eip = (reg_t) memset_fault_ਵਿੱਚ_kernel;
			}
		}
		ਨਹੀਂਤਾਂ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		ਵਾਪਸ;
	}

	ਜੇ(is_nested) {
		ਲਿਖੋ("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			ਚੌਖਟਾ->eip, pagefaultcr2);
		inkernel_disaster(pr, ਚੌਖਟਾ, ਨਿਰਾ, is_nested);
	}

	/* VM can't handle page faults. */
	ਜੇ(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		ਲਿਖੋ("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefaultcr2, ਚੌਖਟਾ->errcode,
			is_nested);
		proc_stacktrace(pr);
		ਲਿਖੋ("pc of pagefault: 0x%lx\n", ਚੌਖਟਾ->eip);
		panic("pagefault in VM");

		ਵਾਪਸ;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_ਸੈੱਟ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ਸਰੋਤ = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = ਚੌਖਟਾ->errcode;

	ਜੇ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	ਵਾਪਸ;
}

ਸਥਿਰ ਖਾਲੀ inkernel_disaster(ਬਣਤਰ proc *saved_proc,
	ਬਣਤਰ exception_ਚੌਖਟਾ * ਚੌਖਟਾ, ਬਣਤਰ ex_s *ep,
	ਪੂਰਨ is_nested)
{
#ਜੇ USE_SYSDEBUG
  ਜੇ(ep) {
	ਜੇ (ep->ਸੁਨੇਹਾ_2 == ਨਿਰਾ)
		ਲਿਖੋ("\nIntel-reserved exception %d\n", ਚੌਖਟਾ->ਸਦਿਸ਼);
	  ਨਹੀਂਤਾਂ
		ਲਿਖੋ("\n%s\n", ep->ਸੁਨੇਹਾ_2);
  }

  ਲਿਖੋ("cpu %d is_nested = %d ", cpuid, is_nested);

  ਲਿਖੋ("vec_nr= %d, trap_errno= 0x%x, eip= 0x%x, "
	"cs= 0x%x, eflags= 0x%x trap_esp 0x%08x\n",
	ਚੌਖਟਾ->ਸਦਿਸ਼, ਚੌਖਟਾ->errcode, ਚੌਖਟਾ->eip,
	ਚੌਖਟਾ->cs, ਚੌਖਟਾ->eflags, ਚੌਖਟਾ);
  ਲਿਖੋ("KERNEL registers :\n");
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG(n) (((u32_t *)ਚੌਖਟਾ)[-n])
  ਲਿਖੋ(
		  "\t%%eax 0x%08x %%ebx 0x%08x %%ecx 0x%08x %%edx 0x%08x\n"
		  "\t%%esp 0x%08x %%ebp 0x%08x %%esi 0x%08x %%edi 0x%08x\n",
		  REG(1), REG(2), REG(3), REG(4),
		  REG(5), REG(6), REG(7), REG(8));

  { 
  	reg_t k_ebp = REG(6);
  	ਲਿਖੋ("KERNEL stacktrace, starting with ebp = 0x%lx:\n", k_ebp);
  	proc_stacktrace_execute(proc_addr(SYSTEM), k_ebp, ਚੌਖਟਾ->eip);
  }

  ਜੇ (saved_proc) {
	  ਲਿਖੋ("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  ਲਿਖੋ("pc = 0x%x\n", (ਬਿਨਾਨਿਸ਼ਾਨੀ) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#ਜੇ_ਅੰਤ /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
ਖਾਲੀ exception_handler(ਪੂਰਨ is_nested, ਬਣਤਰ exception_ਚੌਖਟਾ * ਚੌਖਟਾ)
{
/* An exception or unexpected interrupt has occurred. */
  ਰਜਿਸਟਰ ਬਣਤਰ ex_s *ep;
  ਬਣਤਰ proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_ਅੰਕੜੇ[ਚੌਖਟਾ->ਸਦਿਸ਼];

  ਜੇ (ਚੌਖਟਾ->ਸਦਿਸ਼ == 2) {		/* spurious NMI on some machines */
	ਲਿਖੋ("got spurious NMI\n");
	ਵਾਪਸ;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  ਜੇ (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	ਜੇ (((ਖਾਲੀ*)ਚੌਖਟਾ->eip >= (ਖਾਲੀ*)copy_ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_user &&
			(ਖਾਲੀ*)ਚੌਖਟਾ->eip <= (ਖਾਲੀ*)__copy_msg_to_user_end) ||
			((ਖਾਲੀ*)ਚੌਖਟਾ->eip >= (ਖਾਲੀ*)copy_ਸੁਨੇਹਾ_from_user &&
			(ਖਾਲੀ*)ਚੌਖਟਾ->eip <= (ਖਾਲੀ*)__copy_msg_from_user_end)) {
		ਚੋਣ(ਚੌਖਟਾ->ਸਦਿਸ਼) {
		/* these error are expected */
		ਹਾਲਤ ਪੰਨਾ_FAULT_ਸਦਿਸ਼:
		ਹਾਲਤ PROTECTION_ਸਦਿਸ਼:
			ਚੌਖਟਾ->eip = (reg_t) __user_copy_msg_pointer_failure;
			ਵਾਪਸ;
		ਮੂਲ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	ਜੇ (((ਖਾਲੀ*)ਚੌਖਟਾ->eip >= (ਖਾਲੀ*)fxrstor &&
			(ਖਾਲੀ *)ਚੌਖਟਾ->eip <= (ਖਾਲੀ*)__fxrstor_end) ||
			((ਖਾਲੀ*)ਚੌਖਟਾ->eip >= (ਖਾਲੀ*)frstor &&
			(ਖਾਲੀ *)ਚੌਖਟਾ->eip <= (ਖਾਲੀ*)__frstor_end)) {
		ਚੌਖਟਾ->eip = (reg_t) __frstor_failure;
		ਵਾਪਸ;
	}

  	ਜੇ(ਚੌਖਟਾ->ਸਦਿਸ਼ == DEBUG_ਸਦਿਸ਼
		&& (saved_proc->p_reg.psw & TRACEBIT)
		&& (saved_proc->p_seg.p_kern_trap_style == KTS_NONE)) {
		/* Getting a debug trap in the kernel is legitimate
		 * if a traced process entered the kernel using sysenter
		 * or syscall; the trap flag is not cleared then.
		 *
		 * It triggers on the first kernel entry so the trap
		 * style is still KTS_NONE.
		 */

		ਚੌਖਟਾ->eflags &= ~TRACEBIT;

		ਵਾਪਸ;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  ਜੇ(ਚੌਖਟਾ->ਸਦਿਸ਼ == ਪੰਨਾ_FAULT_ਸਦਿਸ਼) {
	pagefault(saved_proc, ਚੌਖਟਾ, is_nested);
	ਵਾਪਸ;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  ਜੇ (is_nested == 0 && ! iskernelp(saved_proc)) {
#ਜੇ 0
	{

  		ਲਿਖੋ(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			ਚੌਖਟਾ->ਸਦਿਸ਼, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਚੌਖਟਾ->errcode,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਚੌਖਟਾ->eip, ਚੌਖਟਾ->cs,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਚੌਖਟਾ->eflags);
		proc_stacktrace(saved_proc);
	}

#ਜੇ_ਅੰਤ
	cause_sig(proc_nr(saved_proc), ep->signum);
	ਵਾਪਸ;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, ਚੌਖਟਾ, ep, is_nested);

  panic("return from inkernel_disaster");
}

#ਜੇ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ proc_stacktrace_execute(ਬਣਤਰ proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	ਪੂਰਨ iskernel;
	ਪੂਰਨ n = 0;

	iskernel = iskernelp(whichproc);

	ਲਿਖੋ("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	ਜਦੋਂ(v_bp) {
		reg_t v_pc;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRCOPY(pr, pv, v, n) \
  (iskernel ? (ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *) v, (ਅੱਖਰ *) pv, n), OK) : \
     ਅੰਕੜੇ_copy(pr->p_endpoint, pv, KERNEL, (vir_ਬਾਈਟਾਂ) (v), n))

	        ਜੇ(PRCOPY(whichproc, v_bp, &v_hbp, ਆਕਾਰ(v_hbp)) != OK) {
			ਲਿਖੋ("(v_bp 0x%lx ?)", v_bp);
			ਰੋਕੋ;
		}
		ਜੇ(PRCOPY(whichproc, v_bp + ਆਕਾਰ(v_pc), &v_pc, ਆਕਾਰ(v_pc)) != OK) {
			ਲਿਖੋ("(v_pc 0x%lx ?)", v_bp + ਆਕਾਰ(v_pc));
			ਰੋਕੋ;
		}
		ਲਿਖੋ("0x%lx ", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) v_pc);
		ਜੇ(v_hbp != 0 && v_hbp <= v_bp) {
			ਲਿਖੋ("(hbp 0x%lx ?)", v_hbp);
			ਰੋਕੋ;
		}
		v_bp = v_hbp;
		ਜੇ(n++ > 50) {
			ਲਿਖੋ("(truncated after %d steps) ", n);
			ਰੋਕੋ;
		}
	}
	ਲਿਖੋ("\n");
}
#ਜੇ_ਅੰਤ /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
ਖਾਲੀ proc_stacktrace(ਬਣਤਰ proc *whichproc)
{
	u32_t use_bp;

	ਜੇ(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		ਲਿਖੋ("WARNING: stacktrace of running process\n");
	}

	ਚੋਣ(whichproc->p_seg.p_kern_trap_style) {
		ਹਾਲਤ KTS_SYSENTER:
		ਹਾਲਤ KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			ਜੇ(ਅੰਕੜੇ_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_ਬਾਈਟਾਂ) &use_bp,
				ਆਕਾਰ(use_bp)) != OK) {
				ਲਿਖੋ("stacktrace: aborting, copy failed\n");
				ਵਾਪਸ;
			}

			ਰੋਕੋ;
		}
		ਮੂਲ:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			ਰੋਕੋ;
	}

#ਜੇ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#ਜੇ_ਅੰਤ /* USE_SYSDEBUG */
}

ਖਾਲੀ enable_fpu_exception(ਖਾਲੀ)
{
	u32_t cr0 = read_cr0();
	ਜੇ(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

ਖਾਲੀ disable_fpu_exception(ਖਾਲੀ)
{
	clts();
}

