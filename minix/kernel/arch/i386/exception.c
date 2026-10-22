/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <machine/vm.h>

ರಚನೆ ex_s {
	ಅಕ್ಷರ *ಸಂದೇಶ_2;
	ಪೂರ್ಣಾಂಕ signum;
	ಪೂರ್ಣಾಂಕ minprocessor;
};

ಸ್ಥಿರ ರಚನೆ ex_s ex_ದತ್ತಾಂಶ[] = {
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
	{ ಖಾಲಿ, SIGILL, 0 },			/* probably software trap */
	{ "Coprocessor error", SIGFPE, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", SIGFPE, 386 },
};

ಸ್ಥಿರ ಶೂನ್ಯ inkernel_disaster(ರಚನೆ proc *saved_proc,
	ರಚನೆ exception_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು, ರಚನೆ ex_s *ep, ಪೂರ್ಣಾಂಕ is_nested);

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ catch_pagefaults;

ಸ್ಥಿರ ಶೂನ್ಯ proc_stacktrace_execute(ರಚನೆ proc *whichproc, reg_t v_bp, reg_t pc);

ಸ್ಥಿರ ಶೂನ್ಯ pagefault( ರಚನೆ proc *pr,
			ರಚನೆ exception_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು,
			ಪೂರ್ಣಾಂಕ is_nested)
{
	ಪೂರ್ಣಾಂಕ ಒಳ_physcopy = 0, ಒಳ_memset = 0;

	reg_t pagefaultcr2;
	ಸಂದೇಶ m_pagefault;
	ಪೂರ್ಣಾಂಕ err;

	pagefaultcr2 = read_cr2();

#ಆದರೆ 0
	ಬರೆ("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#ಆದರೆ_ಅಂತ್ಯ

	ಒಳ_physcopy = (ಚೌಕಟ್ಟು->eip > (vir_ಬೈಟುಗಳು) phys_copy) &&
	   (ಚೌಕಟ್ಟು->eip < (vir_ಬೈಟುಗಳು) phys_copy_fault);

	ಒಳ_memset = (ಚೌಕಟ್ಟು->eip > (vir_ಬೈಟುಗಳು) phys_memset) &&
	   (ಚೌಕಟ್ಟು->eip < (vir_ಬೈಟುಗಳು) memset_fault);

	ಆದರೆ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (ಒಳ_physcopy || ಒಳ_memset)) {
#ಆದರೆ 0
		ಬರೆ("pf caught! addr 0x%lx\n", pagefaultcr2);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (is_nested) {
			ಆದರೆ(ಒಳ_physcopy) {
				ಪ್ರತಿಜ್ಞಾ(!ಒಳ_memset);
				ಚೌಕಟ್ಟು->eip = (reg_t) phys_copy_fault_ಒಳ_kernel;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಚೌಕಟ್ಟು->eip = (reg_t) memset_fault_ಒಳ_kernel;
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ(is_nested) {
		ಬರೆ("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			ಚೌಕಟ್ಟು->eip, pagefaultcr2);
		inkernel_disaster(pr, ಚೌಕಟ್ಟು, ಖಾಲಿ, is_nested);
	}

	/* VM can't handle page faults. */
	ಆದರೆ(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		ಬರೆ("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefaultcr2, ಚೌಕಟ್ಟು->errcode,
			is_nested);
		proc_stacktrace(pr);
		ಬರೆ("pc of pagefault: 0x%lx\n", ಚೌಕಟ್ಟು->eip);
		panic("pagefault in VM");

		ಹಿಂತಿರುಗಿ;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_ಗಣ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ಮೂಲ = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = ಚೌಕಟ್ಟು->errcode;

	ಆದರೆ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ inkernel_disaster(ರಚನೆ proc *saved_proc,
	ರಚನೆ exception_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು, ರಚನೆ ex_s *ep,
	ಪೂರ್ಣಾಂಕ is_nested)
{
#ಆದರೆ USE_SYSDEBUG
  ಆದರೆ(ep) {
	ಆದರೆ (ep->ಸಂದೇಶ_2 == ಖಾಲಿ)
		ಬರೆ("\nIntel-reserved exception %d\n", ಚೌಕಟ್ಟು->ಸದಿಶ);
	  ಇಲ್ಲದಿದ್ದರೆ
		ಬರೆ("\n%s\n", ep->ಸಂದೇಶ_2);
  }

  ಬರೆ("cpu %d is_nested = %d ", cpuid, is_nested);

  ಬರೆ("vec_nr= %d, trap_errno= 0x%x, eip= 0x%x, "
	"cs= 0x%x, eflags= 0x%x trap_esp 0x%08x\n",
	ಚೌಕಟ್ಟು->ಸದಿಶ, ಚೌಕಟ್ಟು->errcode, ಚೌಕಟ್ಟು->eip,
	ಚೌಕಟ್ಟು->cs, ಚೌಕಟ್ಟು->eflags, ಚೌಕಟ್ಟು);
  ಬರೆ("KERNEL registers :\n");
#ವ್ಯಾಖ್ಯಾನಿಸು REG(n) (((u32_t *)ಚೌಕಟ್ಟು)[-n])
  ಬರೆ(
		  "\t%%eax 0x%08x %%ebx 0x%08x %%ecx 0x%08x %%edx 0x%08x\n"
		  "\t%%esp 0x%08x %%ebp 0x%08x %%esi 0x%08x %%edi 0x%08x\n",
		  REG(1), REG(2), REG(3), REG(4),
		  REG(5), REG(6), REG(7), REG(8));

  { 
  	reg_t k_ebp = REG(6);
  	ಬರೆ("KERNEL stacktrace, starting with ebp = 0x%lx:\n", k_ebp);
  	proc_stacktrace_execute(proc_addr(SYSTEM), k_ebp, ಚೌಕಟ್ಟು->eip);
  }

  ಆದರೆ (saved_proc) {
	  ಬರೆ("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  ಬರೆ("pc = 0x%x\n", (ಚಿಹ್ನೆರಹಿತ) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#ಆದರೆ_ಅಂತ್ಯ /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
ಶೂನ್ಯ exception_handler(ಪೂರ್ಣಾಂಕ is_nested, ರಚನೆ exception_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು)
{
/* An exception or unexpected interrupt has occurred. */
  ನೋಂದಣಿ ರಚನೆ ex_s *ep;
  ರಚನೆ proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_ದತ್ತಾಂಶ[ಚೌಕಟ್ಟು->ಸದಿಶ];

  ಆದರೆ (ಚೌಕಟ್ಟು->ಸದಿಶ == 2) {		/* spurious NMI on some machines */
	ಬರೆ("got spurious NMI\n");
	ಹಿಂತಿರುಗಿ;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  ಆದರೆ (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	ಆದರೆ (((ಶೂನ್ಯ*)ಚೌಕಟ್ಟು->eip >= (ಶೂನ್ಯ*)copy_ಸಂದೇಶ_ಗಮ್ಯ_user &&
			(ಶೂನ್ಯ*)ಚೌಕಟ್ಟು->eip <= (ಶೂನ್ಯ*)__copy_msg_to_user_end) ||
			((ಶೂನ್ಯ*)ಚೌಕಟ್ಟು->eip >= (ಶೂನ್ಯ*)copy_ಸಂದೇಶ_from_user &&
			(ಶೂನ್ಯ*)ಚೌಕಟ್ಟು->eip <= (ಶೂನ್ಯ*)__copy_msg_from_user_end)) {
		ಆಯ್ಕೆ(ಚೌಕಟ್ಟು->ಸದಿಶ) {
		/* these error are expected */
		ಸ್ಥಿತಿ ಪುಟ_FAULT_ಸದಿಶ:
		ಸ್ಥಿತಿ PROTECTION_ಸದಿಶ:
			ಚೌಕಟ್ಟು->eip = (reg_t) __user_copy_msg_pointer_failure;
			ಹಿಂತಿರುಗಿ;
		ಪೂರ್ವನಿಯೋಜಿತ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	ಆದರೆ (((ಶೂನ್ಯ*)ಚೌಕಟ್ಟು->eip >= (ಶೂನ್ಯ*)fxrstor &&
			(ಶೂನ್ಯ *)ಚೌಕಟ್ಟು->eip <= (ಶೂನ್ಯ*)__fxrstor_end) ||
			((ಶೂನ್ಯ*)ಚೌಕಟ್ಟು->eip >= (ಶೂನ್ಯ*)frstor &&
			(ಶೂನ್ಯ *)ಚೌಕಟ್ಟು->eip <= (ಶೂನ್ಯ*)__frstor_end)) {
		ಚೌಕಟ್ಟು->eip = (reg_t) __frstor_failure;
		ಹಿಂತಿರುಗಿ;
	}

  	ಆದರೆ(ಚೌಕಟ್ಟು->ಸದಿಶ == DEBUG_ಸದಿಶ
		&& (saved_proc->p_reg.psw & TRACEBIT)
		&& (saved_proc->p_seg.p_kern_trap_style == KTS_NONE)) {
		/* Getting a debug trap in the kernel is legitimate
		 * if a traced process entered the kernel using sysenter
		 * or syscall; the trap flag is not cleared then.
		 *
		 * It triggers on the first kernel entry so the trap
		 * style is still KTS_NONE.
		 */

		ಚೌಕಟ್ಟು->eflags &= ~TRACEBIT;

		ಹಿಂತಿರುಗಿ;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  ಆದರೆ(ಚೌಕಟ್ಟು->ಸದಿಶ == ಪುಟ_FAULT_ಸದಿಶ) {
	pagefault(saved_proc, ಚೌಕಟ್ಟು, is_nested);
	ಹಿಂತಿರುಗಿ;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  ಆದರೆ (is_nested == 0 && ! iskernelp(saved_proc)) {
#ಆದರೆ 0
	{

  		ಬರೆ(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			ಚೌಕಟ್ಟು->ಸದಿಶ, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಚೌಕಟ್ಟು->errcode,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಚೌಕಟ್ಟು->eip, ಚೌಕಟ್ಟು->cs,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಚೌಕಟ್ಟು->eflags);
		proc_stacktrace(saved_proc);
	}

#ಆದರೆ_ಅಂತ್ಯ
	cause_sig(proc_nr(saved_proc), ep->signum);
	ಹಿಂತಿರುಗಿ;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, ಚೌಕಟ್ಟು, ep, is_nested);

  panic("return from inkernel_disaster");
}

#ಆದರೆ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ proc_stacktrace_execute(ರಚನೆ proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	ಪೂರ್ಣಾಂಕ iskernel;
	ಪೂರ್ಣಾಂಕ n = 0;

	iskernel = iskernelp(whichproc);

	ಬರೆ("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	ಆಗ(v_bp) {
		reg_t v_pc;

#ವ್ಯಾಖ್ಯಾನಿಸು PRCOPY(pr, pv, v, n) \
  (iskernel ? (ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *) v, (ಅಕ್ಷರ *) pv, n), OK) : \
     ದತ್ತಾಂಶ_copy(pr->p_endpoint, pv, KERNEL, (vir_ಬೈಟುಗಳು) (v), n))

	        ಆದರೆ(PRCOPY(whichproc, v_bp, &v_hbp, ಗಾತ್ರ(v_hbp)) != OK) {
			ಬರೆ("(v_bp 0x%lx ?)", v_bp);
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ(PRCOPY(whichproc, v_bp + ಗಾತ್ರ(v_pc), &v_pc, ಗಾತ್ರ(v_pc)) != OK) {
			ಬರೆ("(v_pc 0x%lx ?)", v_bp + ಗಾತ್ರ(v_pc));
			ನಿಲ್ಲಿಸು;
		}
		ಬರೆ("0x%lx ", (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) v_pc);
		ಆದರೆ(v_hbp != 0 && v_hbp <= v_bp) {
			ಬರೆ("(hbp 0x%lx ?)", v_hbp);
			ನಿಲ್ಲಿಸು;
		}
		v_bp = v_hbp;
		ಆದರೆ(n++ > 50) {
			ಬರೆ("(truncated after %d steps) ", n);
			ನಿಲ್ಲಿಸು;
		}
	}
	ಬರೆ("\n");
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
ಶೂನ್ಯ proc_stacktrace(ರಚನೆ proc *whichproc)
{
	u32_t use_bp;

	ಆದರೆ(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		ಬರೆ("WARNING: stacktrace of running process\n");
	}

	ಆಯ್ಕೆ(whichproc->p_seg.p_kern_trap_style) {
		ಸ್ಥಿತಿ KTS_SYSENTER:
		ಸ್ಥಿತಿ KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			ಆದರೆ(ದತ್ತಾಂಶ_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_ಬೈಟುಗಳು) &use_bp,
				ಗಾತ್ರ(use_bp)) != OK) {
				ಬರೆ("stacktrace: aborting, copy failed\n");
				ಹಿಂತಿರುಗಿ;
			}

			ನಿಲ್ಲಿಸು;
		}
		ಪೂರ್ವನಿಯೋಜಿತ:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			ನಿಲ್ಲಿಸು;
	}

#ಆದರೆ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#ಆದರೆ_ಅಂತ್ಯ /* USE_SYSDEBUG */
}

ಶೂನ್ಯ enable_fpu_exception(ಶೂನ್ಯ)
{
	u32_t cr0 = read_cr0();
	ಆದರೆ(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

ಶೂನ್ಯ disable_fpu_exception(ಶೂನ್ಯ)
{
	clts();
}

