/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ <signal.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <machine/vm.h>

నిర్మాణం ex_s {
	అక్షరం *సందేశం_2;
	పూర్ణాంకం signum;
	పూర్ణాంకం minprocessor;
};

స్థిర నిర్మాణం ex_s ex_దత్తాంశం[] = {
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
	{ ఖాళీ, SIGILL, 0 },			/* probably software trap */
	{ "Coprocessor error", SIGFPE, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", SIGFPE, 386 },
};

స్థిర శూన్య inkernel_disaster(నిర్మాణం proc *saved_proc,
	నిర్మాణం exception_చట్రం *చట్రం, నిర్మాణం ex_s *ep, పూర్ణాంకం is_nested);

బాహ్య పూర్ణాంకం catch_pagefaults;

స్థిర శూన్య proc_stacktrace_execute(నిర్మాణం proc *whichproc, reg_t v_bp, reg_t pc);

స్థిర శూన్య pagefault( నిర్మాణం proc *pr,
			నిర్మాణం exception_చట్రం * చట్రం,
			పూర్ణాంకం is_nested)
{
	పూర్ణాంకం లోపల_physcopy = 0, లోపల_memset = 0;

	reg_t pagefaultcr2;
	సందేశం m_pagefault;
	పూర్ణాంకం err;

	pagefaultcr2 = read_cr2();

#ఒకవేళ 0
	రాయి("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#అయితే_ముగింపు

	లోపల_physcopy = (చట్రం->eip > (vir_బైట్లు) phys_copy) &&
	   (చట్రం->eip < (vir_బైట్లు) phys_copy_fault);

	లోపల_memset = (చట్రం->eip > (vir_బైట్లు) phys_memset) &&
	   (చట్రం->eip < (vir_బైట్లు) memset_fault);

	ఒకవేళ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (లోపల_physcopy || లోపల_memset)) {
#ఒకవేళ 0
		రాయి("pf caught! addr 0x%lx\n", pagefaultcr2);
#అయితే_ముగింపు
		ఒకవేళ (is_nested) {
			ఒకవేళ(లోపల_physcopy) {
				ప్రతిజ్ఞా(!లోపల_memset);
				చట్రం->eip = (reg_t) phys_copy_fault_లోపల_kernel;
			} లేకపోతే {
				చట్రం->eip = (reg_t) memset_fault_లోపల_kernel;
			}
		}
		లేకపోతే {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		తిరిగి;
	}

	ఒకవేళ(is_nested) {
		రాయి("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			చట్రం->eip, pagefaultcr2);
		inkernel_disaster(pr, చట్రం, ఖాళీ, is_nested);
	}

	/* VM can't handle page faults. */
	ఒకవేళ(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		రాయి("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefaultcr2, చట్రం->errcode,
			is_nested);
		proc_stacktrace(pr);
		రాయి("pc of pagefault: 0x%lx\n", చట్రం->eip);
		panic("pagefault in VM");

		తిరిగి;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_సమితి(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ఆధారం = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = చట్రం->errcode;

	ఒకవేళ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	తిరిగి;
}

స్థిర శూన్య inkernel_disaster(నిర్మాణం proc *saved_proc,
	నిర్మాణం exception_చట్రం * చట్రం, నిర్మాణం ex_s *ep,
	పూర్ణాంకం is_nested)
{
#ఒకవేళ USE_SYSDEBUG
  ఒకవేళ(ep) {
	ఒకవేళ (ep->సందేశం_2 == ఖాళీ)
		రాయి("\nIntel-reserved exception %d\n", చట్రం->సదిశ);
	  లేకపోతే
		రాయి("\n%s\n", ep->సందేశం_2);
  }

  రాయి("cpu %d is_nested = %d ", cpuid, is_nested);

  రాయి("vec_nr= %d, trap_errno= 0x%x, eip= 0x%x, "
	"cs= 0x%x, eflags= 0x%x trap_esp 0x%08x\n",
	చట్రం->సదిశ, చట్రం->errcode, చట్రం->eip,
	చట్రం->cs, చట్రం->eflags, చట్రం);
  రాయి("KERNEL registers :\n");
#నిర్వచించు REG(n) (((u32_t *)చట్రం)[-n])
  రాయి(
		  "\t%%eax 0x%08x %%ebx 0x%08x %%ecx 0x%08x %%edx 0x%08x\n"
		  "\t%%esp 0x%08x %%ebp 0x%08x %%esi 0x%08x %%edi 0x%08x\n",
		  REG(1), REG(2), REG(3), REG(4),
		  REG(5), REG(6), REG(7), REG(8));

  { 
  	reg_t k_ebp = REG(6);
  	రాయి("KERNEL stacktrace, starting with ebp = 0x%lx:\n", k_ebp);
  	proc_stacktrace_execute(proc_addr(SYSTEM), k_ebp, చట్రం->eip);
  }

  ఒకవేళ (saved_proc) {
	  రాయి("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  రాయి("pc = 0x%x\n", (గుర్తులేని) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#అయితే_ముగింపు /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
శూన్య exception_handler(పూర్ణాంకం is_nested, నిర్మాణం exception_చట్రం * చట్రం)
{
/* An exception or unexpected interrupt has occurred. */
  నమోదు నిర్మాణం ex_s *ep;
  నిర్మాణం proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_దత్తాంశం[చట్రం->సదిశ];

  ఒకవేళ (చట్రం->సదిశ == 2) {		/* spurious NMI on some machines */
	రాయి("got spurious NMI\n");
	తిరిగి;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  ఒకవేళ (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	ఒకవేళ (((శూన్య*)చట్రం->eip >= (శూన్య*)copy_సందేశం_గమ్యం_user &&
			(శూన్య*)చట్రం->eip <= (శూన్య*)__copy_msg_to_user_end) ||
			((శూన్య*)చట్రం->eip >= (శూన్య*)copy_సందేశం_from_user &&
			(శూన్య*)చట్రం->eip <= (శూన్య*)__copy_msg_from_user_end)) {
		ఎంపిక(చట్రం->సదిశ) {
		/* these error are expected */
		స్థితి పుట_FAULT_సదిశ:
		స్థితి PROTECTION_సదిశ:
			చట్రం->eip = (reg_t) __user_copy_msg_pointer_failure;
			తిరిగి;
		అప్రమేయ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	ఒకవేళ (((శూన్య*)చట్రం->eip >= (శూన్య*)fxrstor &&
			(శూన్య *)చట్రం->eip <= (శూన్య*)__fxrstor_end) ||
			((శూన్య*)చట్రం->eip >= (శూన్య*)frstor &&
			(శూన్య *)చట్రం->eip <= (శూన్య*)__frstor_end)) {
		చట్రం->eip = (reg_t) __frstor_failure;
		తిరిగి;
	}

  	ఒకవేళ(చట్రం->సదిశ == DEBUG_సదిశ
		&& (saved_proc->p_reg.psw & TRACEBIT)
		&& (saved_proc->p_seg.p_kern_trap_style == KTS_NONE)) {
		/* Getting a debug trap in the kernel is legitimate
		 * if a traced process entered the kernel using sysenter
		 * or syscall; the trap flag is not cleared then.
		 *
		 * It triggers on the first kernel entry so the trap
		 * style is still KTS_NONE.
		 */

		చట్రం->eflags &= ~TRACEBIT;

		తిరిగి;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  ఒకవేళ(చట్రం->సదిశ == పుట_FAULT_సదిశ) {
	pagefault(saved_proc, చట్రం, is_nested);
	తిరిగి;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  ఒకవేళ (is_nested == 0 && ! iskernelp(saved_proc)) {
#ఒకవేళ 0
	{

  		రాయి(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			చట్రం->సదిశ, (గుర్తులేని దీర్ఘ)చట్రం->errcode,
			(గుర్తులేని దీర్ఘ)చట్రం->eip, చట్రం->cs,
			(గుర్తులేని దీర్ఘ)చట్రం->eflags);
		proc_stacktrace(saved_proc);
	}

#అయితే_ముగింపు
	cause_sig(proc_nr(saved_proc), ep->signum);
	తిరిగి;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, చట్రం, ep, is_nested);

  panic("return from inkernel_disaster");
}

#ఒకవేళ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
స్థిర శూన్య proc_stacktrace_execute(నిర్మాణం proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	పూర్ణాంకం iskernel;
	పూర్ణాంకం n = 0;

	iskernel = iskernelp(whichproc);

	రాయి("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	అయితే(v_bp) {
		reg_t v_pc;

#నిర్వచించు PRCOPY(pr, pv, v, n) \
  (iskernel ? (జ్ఞాపకం_నకలు((అక్షరం *) v, (అక్షరం *) pv, n), OK) : \
     దత్తాంశం_copy(pr->p_endpoint, pv, KERNEL, (vir_బైట్లు) (v), n))

	        ఒకవేళ(PRCOPY(whichproc, v_bp, &v_hbp, పరిమాణం(v_hbp)) != OK) {
			రాయి("(v_bp 0x%lx ?)", v_bp);
			ఆపు;
		}
		ఒకవేళ(PRCOPY(whichproc, v_bp + పరిమాణం(v_pc), &v_pc, పరిమాణం(v_pc)) != OK) {
			రాయి("(v_pc 0x%lx ?)", v_bp + పరిమాణం(v_pc));
			ఆపు;
		}
		రాయి("0x%lx ", (గుర్తులేని దీర్ఘ) v_pc);
		ఒకవేళ(v_hbp != 0 && v_hbp <= v_bp) {
			రాయి("(hbp 0x%lx ?)", v_hbp);
			ఆపు;
		}
		v_bp = v_hbp;
		ఒకవేళ(n++ > 50) {
			రాయి("(truncated after %d steps) ", n);
			ఆపు;
		}
	}
	రాయి("\n");
}
#అయితే_ముగింపు /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
శూన్య proc_stacktrace(నిర్మాణం proc *whichproc)
{
	u32_t use_bp;

	ఒకవేళ(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		రాయి("WARNING: stacktrace of running process\n");
	}

	ఎంపిక(whichproc->p_seg.p_kern_trap_style) {
		స్థితి KTS_SYSENTER:
		స్థితి KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			ఒకవేళ(దత్తాంశం_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_బైట్లు) &use_bp,
				పరిమాణం(use_bp)) != OK) {
				రాయి("stacktrace: aborting, copy failed\n");
				తిరిగి;
			}

			ఆపు;
		}
		అప్రమేయ:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			ఆపు;
	}

#ఒకవేళ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#అయితే_ముగింపు /* USE_SYSDEBUG */
}

శూన్య enable_fpu_exception(శూన్య)
{
	u32_t cr0 = read_cr0();
	ఒకవేళ(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

శూన్య disable_fpu_exception(శూన్య)
{
	clts();
}

