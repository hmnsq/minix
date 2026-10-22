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

ಸಂರಚನಾ ex_s {
	ವರ್ಣ *ಸಂದೇಶ;
	ಪೂರ್ಣಾಙ್ಕ signum;
	ಪೂರ್ಣಾಙ್ಕ minprocessor;
};

ಸ್ಥಿರ ಸಂರಚನಾ ex_s ex_data[] = {
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
	{ ರಿಕ್ತ, SIGILL, 0 },			/* probably software trap */
	{ "Coprocessor error", SIGFPE, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", SIGFPE, 386 },
};

ಸ್ಥಿರ ಶೂನ್ಯ inkernel_disaster(ಸಂರಚನಾ proc *saved_proc,
	ಸಂರಚನಾ exception_frame *frame, ಸಂರಚನಾ ex_s *ep, ಪೂರ್ಣಾಙ್ಕ is_nested);

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ catch_pagefaults;

ಸ್ಥಿರ ಶೂನ್ಯ proc_stacktrace_execute(ಸಂರಚನಾ proc *whichproc, reg_t v_bp, reg_t pc);

ಸ್ಥಿರ ಶೂನ್ಯ pagefault( ಸಂರಚನಾ proc *pr,
			ಸಂರಚನಾ exception_frame * frame,
			ಪೂರ್ಣಾಙ್ಕ is_nested)
{
	ಪೂರ್ಣಾಙ್ಕ in_physcopy = 0, in_memset = 0;

	reg_t pagefaultcr2;
	message m_pagefault;
	ಪೂರ್ಣಾಙ್ಕ err;

	pagefaultcr2 = read_cr2();

#ಯದಿ 0
	printf("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#endif

	in_physcopy = (frame->eip > (vir_bytes) phys_copy) &&
	   (frame->eip < (vir_bytes) phys_copy_fault);

	in_memset = (frame->eip > (vir_bytes) phys_memset) &&
	   (frame->eip < (vir_bytes) memset_fault);

	ಯದಿ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
#ಯದಿ 0
		printf("pf caught! addr 0x%lx\n", pagefaultcr2);
#endif
		ಯದಿ (is_nested) {
			ಯದಿ(in_physcopy) {
				ಪ್ರತಿಜ್ಞಾ(!in_memset);
				frame->eip = (reg_t) phys_copy_fault_in_kernel;
			} ಅನ್ಯಥಾ {
				frame->eip = (reg_t) memset_fault_in_kernel;
			}
		}
		ಅನ್ಯಥಾ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ(is_nested) {
		printf("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			frame->eip, pagefaultcr2);
		inkernel_disaster(pr, frame, ರಿಕ್ತ, is_nested);
	}

	/* VM can't handle page faults. */
	ಯದಿ(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		printf("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefaultcr2, frame->errcode,
			is_nested);
		proc_stacktrace(pr);
		printf("pc of pagefault: 0x%lx\n", frame->eip);
		panic("pagefault in VM");

		ಪ್ರತ್ಯಾಗಮ;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = frame->errcode;

	ಯದಿ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	ಪ್ರತ್ಯಾಗಮ;
}

ಸ್ಥಿರ ಶೂನ್ಯ inkernel_disaster(ಸಂರಚನಾ proc *saved_proc,
	ಸಂರಚನಾ exception_frame * frame, ಸಂರಚನಾ ex_s *ep,
	ಪೂರ್ಣಾಙ್ಕ is_nested)
{
#ಯದಿ USE_SYSDEBUG
  ಯದಿ(ep) {
	ಯದಿ (ep->ಸಂದೇಶ == ರಿಕ್ತ)
		printf("\nIntel-reserved exception %d\n", frame->vector);
	  ಅನ್ಯಥಾ
		printf("\n%s\n", ep->ಸಂದೇಶ);
  }

  printf("cpu %d is_nested = %d ", cpuid, is_nested);

  printf("vec_nr= %d, trap_errno= 0x%x, eip= 0x%x, "
	"cs= 0x%x, eflags= 0x%x trap_esp 0x%08x\n",
	frame->vector, frame->errcode, frame->eip,
	frame->cs, frame->eflags, frame);
  printf("KERNEL registers :\n");
#define REG(n) (((u32_t *)frame)[-n])
  printf(
		  "\t%%eax 0x%08x %%ebx 0x%08x %%ecx 0x%08x %%edx 0x%08x\n"
		  "\t%%esp 0x%08x %%ebp 0x%08x %%esi 0x%08x %%edi 0x%08x\n",
		  REG(1), REG(2), REG(3), REG(4),
		  REG(5), REG(6), REG(7), REG(8));

  { 
  	reg_t k_ebp = REG(6);
  	printf("KERNEL stacktrace, starting with ebp = 0x%lx:\n", k_ebp);
  	proc_stacktrace_execute(proc_addr(SYSTEM), k_ebp, frame->eip);
  }

  ಯದಿ (saved_proc) {
	  printf("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  printf("pc = 0x%x\n", (unsigned) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#endif /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
ಶೂನ್ಯ exception_handler(ಪೂರ್ಣಾಙ್ಕ is_nested, ಸಂರಚನಾ exception_frame * frame)
{
/* An exception or unexpected interrupt has occurred. */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ ex_s *ep;
  ಸಂರಚನಾ proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_data[frame->vector];

  ಯದಿ (frame->vector == 2) {		/* spurious NMI on some machines */
	printf("got spurious NMI\n");
	ಪ್ರತ್ಯಾಗಮ;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  ಯದಿ (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	ಯದಿ (((ಶೂನ್ಯ*)frame->eip >= (ಶೂನ್ಯ*)copy_ಸಂದೇಶ_to_user &&
			(ಶೂನ್ಯ*)frame->eip <= (ಶೂನ್ಯ*)__copy_msg_to_user_end) ||
			((ಶೂನ್ಯ*)frame->eip >= (ಶೂನ್ಯ*)copy_ಸಂದೇಶ_from_user &&
			(ಶೂನ್ಯ*)frame->eip <= (ಶೂನ್ಯ*)__copy_msg_from_user_end)) {
		ವಿಕಲ್ಪ(frame->vector) {
		/* these error are expected */
		ಪಕ್ಷ PAGE_FAULT_VECTOR:
		ಪಕ್ಷ PROTECTION_VECTOR:
			frame->eip = (reg_t) __user_copy_msg_pointer_failure;
			ಪ್ರತ್ಯಾಗಮ;
		ಸ್ವಭಾವ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	ಯದಿ (((ಶೂನ್ಯ*)frame->eip >= (ಶೂನ್ಯ*)fxrstor &&
			(ಶೂನ್ಯ *)frame->eip <= (ಶೂನ್ಯ*)__fxrstor_end) ||
			((ಶೂನ್ಯ*)frame->eip >= (ಶೂನ್ಯ*)frstor &&
			(ಶೂನ್ಯ *)frame->eip <= (ಶೂನ್ಯ*)__frstor_end)) {
		frame->eip = (reg_t) __frstor_failure;
		ಪ್ರತ್ಯಾಗಮ;
	}

  	ಯದಿ(frame->vector == DEBUG_VECTOR
		&& (saved_proc->p_reg.psw & TRACEBIT)
		&& (saved_proc->p_seg.p_kern_trap_style == KTS_NONE)) {
		/* Getting a debug trap in the kernel is legitimate
		 * if a traced process entered the kernel using sysenter
		 * or syscall; the trap flag is not cleared then.
		 *
		 * It triggers on the first kernel entry so the trap
		 * style is still KTS_NONE.
		 */

		frame->eflags &= ~TRACEBIT;

		ಪ್ರತ್ಯಾಗಮ;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  ಯದಿ(frame->vector == PAGE_FAULT_VECTOR) {
	pagefault(saved_proc, frame, is_nested);
	ಪ್ರತ್ಯಾಗಮ;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  ಯದಿ (is_nested == 0 && ! iskernelp(saved_proc)) {
#ಯದಿ 0
	{

  		printf(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			frame->vector, (unsigned long)frame->errcode,
			(unsigned long)frame->eip, frame->cs,
			(unsigned long)frame->eflags);
		proc_stacktrace(saved_proc);
	}

#endif
	cause_sig(proc_nr(saved_proc), ep->signum);
	ಪ್ರತ್ಯಾಗಮ;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, frame, ep, is_nested);

  panic("return from inkernel_disaster");
}

#ಯದಿ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ proc_stacktrace_execute(ಸಂರಚನಾ proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	ಪೂರ್ಣಾಙ್ಕ iskernel;
	ಪೂರ್ಣಾಙ್ಕ n = 0;

	iskernel = iskernelp(whichproc);

	printf("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	ಯಾವತ್(v_bp) {
		reg_t v_pc;

#define PRCOPY(pr, pv, v, n) \
  (iskernel ? (memcpy((ವರ್ಣ *) v, (ವರ್ಣ *) pv, n), OK) : \
     data_copy(pr->p_endpoint, pv, KERNEL, (vir_bytes) (v), n))

	        ಯದಿ(PRCOPY(whichproc, v_bp, &v_hbp, ಪರಿಮಾಣ(v_hbp)) != OK) {
			printf("(v_bp 0x%lx ?)", v_bp);
			ಭಙ್ಗ;
		}
		ಯದಿ(PRCOPY(whichproc, v_bp + ಪರಿಮಾಣ(v_pc), &v_pc, ಪರಿಮಾಣ(v_pc)) != OK) {
			printf("(v_pc 0x%lx ?)", v_bp + ಪರಿಮಾಣ(v_pc));
			ಭಙ್ಗ;
		}
		printf("0x%lx ", (unsigned long) v_pc);
		ಯದಿ(v_hbp != 0 && v_hbp <= v_bp) {
			printf("(hbp 0x%lx ?)", v_hbp);
			ಭಙ್ಗ;
		}
		v_bp = v_hbp;
		ಯದಿ(n++ > 50) {
			printf("(truncated after %d steps) ", n);
			ಭಙ್ಗ;
		}
	}
	printf("\n");
}
#endif /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
ಶೂನ್ಯ proc_stacktrace(ಸಂರಚನಾ proc *whichproc)
{
	u32_t use_bp;

	ಯದಿ(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		printf("WARNING: stacktrace of running process\n");
	}

	ವಿಕಲ್ಪ(whichproc->p_seg.p_kern_trap_style) {
		ಪಕ್ಷ KTS_SYSENTER:
		ಪಕ್ಷ KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			ಯದಿ(data_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_bytes) &use_bp,
				ಪರಿಮಾಣ(use_bp)) != OK) {
				printf("stacktrace: aborting, copy failed\n");
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಭಙ್ಗ;
		}
		ಸ್ವಭಾವ:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			ಭಙ್ಗ;
	}

#ಯದಿ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#endif /* USE_SYSDEBUG */
}

ಶೂನ್ಯ enable_fpu_exception(ಶೂನ್ಯ)
{
	u32_t cr0 = read_cr0();
	ಯದಿ(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

ಶೂನ್ಯ disable_fpu_exception(ಶೂನ್ಯ)
{
	clts();
}

