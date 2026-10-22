/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <machine/vm.h>

ସଂରଚନା ex_s {
	ଅକ୍ଷର *ସନ୍ଦେଶ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା signum;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା minprocessor;
};

ସ୍ଥିର ସଂରଚନା ex_s ex_data[] = {
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
	{ ଖାଲି, SIGILL, 0 },			/* probably software trap */
	{ "Coprocessor error", SIGFPE, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", SIGFPE, 386 },
};

ସ୍ଥିର ଶୂନ୍ୟ inkernel_disaster(ସଂରଚନା proc *saved_proc,
	ସଂରଚନା exception_frame *frame, ସଂରଚନା ex_s *ep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_nested);

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା catch_pagefaults;

ସ୍ଥିର ଶୂନ୍ୟ proc_stacktrace_execute(ସଂରଚନା proc *whichproc, reg_t v_bp, reg_t pc);

ସ୍ଥିର ଶୂନ୍ୟ pagefault( ସଂରଚନା proc *pr,
			ସଂରଚନା exception_frame * frame,
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_nested)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_physcopy = 0, in_memset = 0;

	reg_t pagefaultcr2;
	message m_pagefault;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	pagefaultcr2 = read_cr2();

#ଯଦି 0
	printf("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#endif

	in_physcopy = (frame->eip > (vir_bytes) phys_copy) &&
	   (frame->eip < (vir_bytes) phys_copy_fault);

	in_memset = (frame->eip > (vir_bytes) phys_memset) &&
	   (frame->eip < (vir_bytes) memset_fault);

	ଯଦି((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
#ଯଦି 0
		printf("pf caught! addr 0x%lx\n", pagefaultcr2);
#endif
		ଯଦି (is_nested) {
			ଯଦି(in_physcopy) {
				ପ୍ରତିଜ୍ଞା(!in_memset);
				frame->eip = (reg_t) phys_copy_fault_in_kernel;
			} ଅନ୍ୟଥା {
				frame->eip = (reg_t) memset_fault_in_kernel;
			}
		}
		ଅନ୍ୟଥା {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		ଫେରନ୍ତୁ;
	}

	ଯଦି(is_nested) {
		printf("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			frame->eip, pagefaultcr2);
		inkernel_disaster(pr, frame, ଖାଲି, is_nested);
	}

	/* VM can't handle page faults. */
	ଯଦି(pr->p_endpoint == VM_PROC_NR) {
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

		ଫେରନ୍ତୁ;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = frame->errcode;

	ଯଦି ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	ଫେରନ୍ତୁ;
}

ସ୍ଥିର ଶୂନ୍ୟ inkernel_disaster(ସଂରଚନା proc *saved_proc,
	ସଂରଚନା exception_frame * frame, ସଂରଚନା ex_s *ep,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_nested)
{
#ଯଦି USE_SYSDEBUG
  ଯଦି(ep) {
	ଯଦି (ep->ସନ୍ଦେଶ == ଖାଲି)
		printf("\nIntel-reserved exception %d\n", frame->vector);
	  ଅନ୍ୟଥା
		printf("\n%s\n", ep->ସନ୍ଦେଶ);
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

  ଯଦି (saved_proc) {
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
ଶୂନ୍ୟ exception_handler(ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_nested, ସଂରଚନା exception_frame * frame)
{
/* An exception or unexpected interrupt has occurred. */
  ପଞ୍ଜିକରଣ ସଂରଚନା ex_s *ep;
  ସଂରଚନା proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_data[frame->vector];

  ଯଦି (frame->vector == 2) {		/* spurious NMI on some machines */
	printf("got spurious NMI\n");
	ଫେରନ୍ତୁ;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  ଯଦି (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	ଯଦି (((ଶୂନ୍ୟ*)frame->eip >= (ଶୂନ୍ୟ*)copy_ସନ୍ଦେଶ_to_user &&
			(ଶୂନ୍ୟ*)frame->eip <= (ଶୂନ୍ୟ*)__copy_msg_to_user_end) ||
			((ଶୂନ୍ୟ*)frame->eip >= (ଶୂନ୍ୟ*)copy_ସନ୍ଦେଶ_from_user &&
			(ଶୂନ୍ୟ*)frame->eip <= (ଶୂନ୍ୟ*)__copy_msg_from_user_end)) {
		ବାଛ(frame->vector) {
		/* these error are expected */
		ସ୍ଥିତି PAGE_FAULT_VECTOR:
		ସ୍ଥିତି PROTECTION_VECTOR:
			frame->eip = (reg_t) __user_copy_msg_pointer_failure;
			ଫେରନ୍ତୁ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	ଯଦି (((ଶୂନ୍ୟ*)frame->eip >= (ଶୂନ୍ୟ*)fxrstor &&
			(ଶୂନ୍ୟ *)frame->eip <= (ଶୂନ୍ୟ*)__fxrstor_end) ||
			((ଶୂନ୍ୟ*)frame->eip >= (ଶୂନ୍ୟ*)frstor &&
			(ଶୂନ୍ୟ *)frame->eip <= (ଶୂନ୍ୟ*)__frstor_end)) {
		frame->eip = (reg_t) __frstor_failure;
		ଫେରନ୍ତୁ;
	}

  	ଯଦି(frame->vector == DEBUG_VECTOR
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

		ଫେରନ୍ତୁ;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  ଯଦି(frame->vector == PAGE_FAULT_VECTOR) {
	pagefault(saved_proc, frame, is_nested);
	ଫେରନ୍ତୁ;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  ଯଦି (is_nested == 0 && ! iskernelp(saved_proc)) {
#ଯଦି 0
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
	ଫେରନ୍ତୁ;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, frame, ep, is_nested);

  panic("return from inkernel_disaster");
}

#ଯଦି USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ସ୍ଥିର ଶୂନ୍ୟ proc_stacktrace_execute(ସଂରଚନା proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା iskernel;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;

	iskernel = iskernelp(whichproc);

	printf("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	ଯେତେବେଳେ(v_bp) {
		reg_t v_pc;

#define PRCOPY(pr, pv, v, n) \
  (iskernel ? (memcpy((ଅକ୍ଷର *) v, (ଅକ୍ଷର *) pv, n), OK) : \
     data_copy(pr->p_endpoint, pv, KERNEL, (vir_bytes) (v), n))

	        ଯଦି(PRCOPY(whichproc, v_bp, &v_hbp, ଆକାର(v_hbp)) != OK) {
			printf("(v_bp 0x%lx ?)", v_bp);
			ଭାଙ୍ଗ;
		}
		ଯଦି(PRCOPY(whichproc, v_bp + ଆକାର(v_pc), &v_pc, ଆକାର(v_pc)) != OK) {
			printf("(v_pc 0x%lx ?)", v_bp + ଆକାର(v_pc));
			ଭାଙ୍ଗ;
		}
		printf("0x%lx ", (unsigned long) v_pc);
		ଯଦି(v_hbp != 0 && v_hbp <= v_bp) {
			printf("(hbp 0x%lx ?)", v_hbp);
			ଭାଙ୍ଗ;
		}
		v_bp = v_hbp;
		ଯଦି(n++ > 50) {
			printf("(truncated after %d steps) ", n);
			ଭାଙ୍ଗ;
		}
	}
	printf("\n");
}
#endif /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
ଶୂନ୍ୟ proc_stacktrace(ସଂରଚନା proc *whichproc)
{
	u32_t use_bp;

	ଯଦି(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		printf("WARNING: stacktrace of running process\n");
	}

	ବାଛ(whichproc->p_seg.p_kern_trap_style) {
		ସ୍ଥିତି KTS_SYSENTER:
		ସ୍ଥିତି KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			ଯଦି(data_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_bytes) &use_bp,
				ଆକାର(use_bp)) != OK) {
				printf("stacktrace: aborting, copy failed\n");
				ଫେରନ୍ତୁ;
			}

			ଭାଙ୍ଗ;
		}
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			ଭାଙ୍ଗ;
	}

#ଯଦି USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#endif /* USE_SYSDEBUG */
}

ଶୂନ୍ୟ enable_fpu_exception(ଶୂନ୍ୟ)
{
	u32_t cr0 = read_cr0();
	ଯଦି(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

ଶୂନ୍ୟ disable_fpu_exception(ଶୂନ୍ୟ)
{
	clts();
}

