/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_proto.h"
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <machine/vm.h>

ഘടന ex_s {
	അക്ഷരം *സന്ദേശം;
	പൂർണ്ണസംഖ്യ signum;
	പൂർണ്ണസംഖ്യ minprocessor;
};

സ്ഥിതം ഘടന ex_s ex_data[] = {
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
	{ ശൂന്യ, SIGILL, 0 },			/* probably software trap */
	{ "Coprocessor error", SIGFPE, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", SIGFPE, 386 },
};

സ്ഥിതം ശൂന്യം inkernel_disaster(ഘടന proc *saved_proc,
	ഘടന exception_frame *frame, ഘടന ex_s *ep, പൂർണ്ണസംഖ്യ is_nested);

ബാഹ്യ പൂർണ്ണസംഖ്യ catch_pagefaults;

സ്ഥിതം ശൂന്യം proc_stacktrace_execute(ഘടന proc *whichproc, reg_t v_bp, reg_t pc);

സ്ഥിതം ശൂന്യം pagefault( ഘടന proc *pr,
			ഘടന exception_frame * frame,
			പൂർണ്ണസംഖ്യ is_nested)
{
	പൂർണ്ണസംഖ്യ in_physcopy = 0, in_memset = 0;

	reg_t pagefaultcr2;
	message m_pagefault;
	പൂർണ്ണസംഖ്യ err;

	pagefaultcr2 = read_cr2();

#എങ്കിൽ 0
	printf("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#endif

	in_physcopy = (frame->eip > (vir_bytes) phys_copy) &&
	   (frame->eip < (vir_bytes) phys_copy_fault);

	in_memset = (frame->eip > (vir_bytes) phys_memset) &&
	   (frame->eip < (vir_bytes) memset_fault);

	എങ്കിൽ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
#എങ്കിൽ 0
		printf("pf caught! addr 0x%lx\n", pagefaultcr2);
#endif
		എങ്കിൽ (is_nested) {
			എങ്കിൽ(in_physcopy) {
				പ്രതിജ്ഞാ(!in_memset);
				frame->eip = (reg_t) phys_copy_fault_in_kernel;
			} അല്ലെങ്കിൽ {
				frame->eip = (reg_t) memset_fault_in_kernel;
			}
		}
		അല്ലെങ്കിൽ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		തിരികെ;
	}

	എങ്കിൽ(is_nested) {
		printf("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			frame->eip, pagefaultcr2);
		inkernel_disaster(pr, frame, ശൂന്യ, is_nested);
	}

	/* VM can't handle page faults. */
	എങ്കിൽ(pr->p_endpoint == VM_PROC_NR) {
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

		തിരികെ;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = frame->errcode;

	എങ്കിൽ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	തിരികെ;
}

സ്ഥിതം ശൂന്യം inkernel_disaster(ഘടന proc *saved_proc,
	ഘടന exception_frame * frame, ഘടന ex_s *ep,
	പൂർണ്ണസംഖ്യ is_nested)
{
#എങ്കിൽ USE_SYSDEBUG
  എങ്കിൽ(ep) {
	എങ്കിൽ (ep->സന്ദേശം == ശൂന്യ)
		printf("\nIntel-reserved exception %d\n", frame->vector);
	  അല്ലെങ്കിൽ
		printf("\n%s\n", ep->സന്ദേശം);
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

  എങ്കിൽ (saved_proc) {
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
ശൂന്യം exception_handler(പൂർണ്ണസംഖ്യ is_nested, ഘടന exception_frame * frame)
{
/* An exception or unexpected interrupt has occurred. */
  രജിസ്റ്റർ ഘടന ex_s *ep;
  ഘടന proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_data[frame->vector];

  എങ്കിൽ (frame->vector == 2) {		/* spurious NMI on some machines */
	printf("got spurious NMI\n");
	തിരികെ;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  എങ്കിൽ (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	എങ്കിൽ (((ശൂന്യം*)frame->eip >= (ശൂന്യം*)copy_സന്ദേശം_to_user &&
			(ശൂന്യം*)frame->eip <= (ശൂന്യം*)__copy_msg_to_user_end) ||
			((ശൂന്യം*)frame->eip >= (ശൂന്യം*)copy_സന്ദേശം_from_user &&
			(ശൂന്യം*)frame->eip <= (ശൂന്യം*)__copy_msg_from_user_end)) {
		തിരഞ്ഞെടുപ്പ്(frame->vector) {
		/* these error are expected */
		അവസ്ഥ PAGE_FAULT_VECTOR:
		അവസ്ഥ PROTECTION_VECTOR:
			frame->eip = (reg_t) __user_copy_msg_pointer_failure;
			തിരികെ;
		സ്ഥിരസ്ഥിതി:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	എങ്കിൽ (((ശൂന്യം*)frame->eip >= (ശൂന്യം*)fxrstor &&
			(ശൂന്യം *)frame->eip <= (ശൂന്യം*)__fxrstor_end) ||
			((ശൂന്യം*)frame->eip >= (ശൂന്യം*)frstor &&
			(ശൂന്യം *)frame->eip <= (ശൂന്യം*)__frstor_end)) {
		frame->eip = (reg_t) __frstor_failure;
		തിരികെ;
	}

  	എങ്കിൽ(frame->vector == DEBUG_VECTOR
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

		തിരികെ;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  എങ്കിൽ(frame->vector == PAGE_FAULT_VECTOR) {
	pagefault(saved_proc, frame, is_nested);
	തിരികെ;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  എങ്കിൽ (is_nested == 0 && ! iskernelp(saved_proc)) {
#എങ്കിൽ 0
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
	തിരികെ;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, frame, ep, is_nested);

  panic("return from inkernel_disaster");
}

#എങ്കിൽ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
സ്ഥിതം ശൂന്യം proc_stacktrace_execute(ഘടന proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	പൂർണ്ണസംഖ്യ iskernel;
	പൂർണ്ണസംഖ്യ n = 0;

	iskernel = iskernelp(whichproc);

	printf("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	ആകുമ്പോൾ(v_bp) {
		reg_t v_pc;

#define PRCOPY(pr, pv, v, n) \
  (iskernel ? (memcpy((അക്ഷരം *) v, (അക്ഷരം *) pv, n), OK) : \
     data_copy(pr->p_endpoint, pv, KERNEL, (vir_bytes) (v), n))

	        എങ്കിൽ(PRCOPY(whichproc, v_bp, &v_hbp, വലുപ്പം(v_hbp)) != OK) {
			printf("(v_bp 0x%lx ?)", v_bp);
			നിർത്തുക;
		}
		എങ്കിൽ(PRCOPY(whichproc, v_bp + വലുപ്പം(v_pc), &v_pc, വലുപ്പം(v_pc)) != OK) {
			printf("(v_pc 0x%lx ?)", v_bp + വലുപ്പം(v_pc));
			നിർത്തുക;
		}
		printf("0x%lx ", (unsigned long) v_pc);
		എങ്കിൽ(v_hbp != 0 && v_hbp <= v_bp) {
			printf("(hbp 0x%lx ?)", v_hbp);
			നിർത്തുക;
		}
		v_bp = v_hbp;
		എങ്കിൽ(n++ > 50) {
			printf("(truncated after %d steps) ", n);
			നിർത്തുക;
		}
	}
	printf("\n");
}
#endif /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
ശൂന്യം proc_stacktrace(ഘടന proc *whichproc)
{
	u32_t use_bp;

	എങ്കിൽ(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		printf("WARNING: stacktrace of running process\n");
	}

	തിരഞ്ഞെടുപ്പ്(whichproc->p_seg.p_kern_trap_style) {
		അവസ്ഥ KTS_SYSENTER:
		അവസ്ഥ KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			എങ്കിൽ(data_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_bytes) &use_bp,
				വലുപ്പം(use_bp)) != OK) {
				printf("stacktrace: aborting, copy failed\n");
				തിരികെ;
			}

			നിർത്തുക;
		}
		സ്ഥിരസ്ഥിതി:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			നിർത്തുക;
	}

#എങ്കിൽ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#endif /* USE_SYSDEBUG */
}

ശൂന്യം enable_fpu_exception(ശൂന്യം)
{
	u32_t cr0 = read_cr0();
	എങ്കിൽ(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

ശൂന്യം disable_fpu_exception(ശൂന്യം)
{
	clts();
}

