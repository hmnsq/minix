/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"
#ضمن <signal.h>
#ضمن <string.h>
#ضمن <assert.h>
#ضمن <machine/vm.h>

بنية ex_s {
	حرف *پیغام_2;
	صحيح signum;
	صحيح minprocessor;
};

ساكن بنية ex_s ex_معلومات[] = {
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
	{ عدم, SIGILL, 0 },			/* probably software trap */
	{ "Coprocessor error", SIGFPE, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", SIGFPE, 386 },
};

ساكن فراغ inkernel_disaster(بنية proc *saved_proc,
	بنية exception_چوکاټ *چوکاټ, بنية ex_s *ep, صحيح is_nested);

خارجي صحيح catch_pagefaults;

ساكن فراغ proc_stacktrace_execute(بنية proc *whichproc, reg_t v_bp, reg_t pc);

ساكن فراغ pagefault( بنية proc *pr,
			بنية exception_چوکاټ * چوکاټ,
			صحيح is_nested)
{
	صحيح دننه_physcopy = 0, دننه_memset = 0;

	reg_t pagefaultcr2;
	پیغام m_pagefault;
	صحيح err;

	pagefaultcr2 = read_cr2();

#إذا 0
	چاپ_کړه("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#د_که_پای

	دننه_physcopy = (چوکاټ->eip > (vir_بایټونه) phys_copy) &&
	   (چوکاټ->eip < (vir_بایټونه) phys_copy_fault);

	دننه_memset = (چوکاټ->eip > (vir_بایټونه) phys_memset) &&
	   (چوکاټ->eip < (vir_بایټونه) memset_fault);

	إذا((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (دننه_physcopy || دننه_memset)) {
#إذا 0
		چاپ_کړه("pf caught! addr 0x%lx\n", pagefaultcr2);
#د_که_پای
		إذا (is_nested) {
			إذا(دننه_physcopy) {
				أكد(!دننه_memset);
				چوکاټ->eip = (reg_t) phys_copy_fault_دننه_kernel;
			} وإلا {
				چوکاټ->eip = (reg_t) memset_fault_دننه_kernel;
			}
		}
		وإلا {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		أرجع;
	}

	إذا(is_nested) {
		چاپ_کړه("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			چوکاټ->eip, pagefaultcr2);
		inkernel_disaster(pr, چوکاټ, عدم, is_nested);
	}

	/* VM can't handle page faults. */
	إذا(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		چاپ_کړه("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefaultcr2, چوکاټ->errcode,
			is_nested);
		proc_stacktrace(pr);
		چاپ_کړه("pc of pagefault: 0x%lx\n", چوکاټ->eip);
		panic("pagefault in VM");

		أرجع;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_سېټ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_سرچینه = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = چوکاټ->errcode;

	إذا ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	أرجع;
}

ساكن فراغ inkernel_disaster(بنية proc *saved_proc,
	بنية exception_چوکاټ * چوکاټ, بنية ex_s *ep,
	صحيح is_nested)
{
#إذا USE_SYSDEBUG
  إذا(ep) {
	إذا (ep->پیغام_2 == عدم)
		چاپ_کړه("\nIntel-reserved exception %d\n", چوکاټ->ویکټور);
	  وإلا
		چاپ_کړه("\n%s\n", ep->پیغام_2);
  }

  چاپ_کړه("cpu %d is_nested = %d ", cpuid, is_nested);

  چاپ_کړه("vec_nr= %d, trap_errno= 0x%x, eip= 0x%x, "
	"cs= 0x%x, eflags= 0x%x trap_esp 0x%08x\n",
	چوکاټ->ویکټور, چوکاټ->errcode, چوکاټ->eip,
	چوکاټ->cs, چوکاټ->eflags, چوکاټ);
  چاپ_کړه("KERNEL registers :\n");
#تعریف_کړه REG(n) (((u32_t *)چوکاټ)[-n])
  چاپ_کړه(
		  "\t%%eax 0x%08x %%ebx 0x%08x %%ecx 0x%08x %%edx 0x%08x\n"
		  "\t%%esp 0x%08x %%ebp 0x%08x %%esi 0x%08x %%edi 0x%08x\n",
		  REG(1), REG(2), REG(3), REG(4),
		  REG(5), REG(6), REG(7), REG(8));

  { 
  	reg_t k_ebp = REG(6);
  	چاپ_کړه("KERNEL stacktrace, starting with ebp = 0x%lx:\n", k_ebp);
  	proc_stacktrace_execute(proc_addr(SYSTEM), k_ebp, چوکاټ->eip);
  }

  إذا (saved_proc) {
	  چاپ_کړه("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  چاپ_کړه("pc = 0x%x\n", (بینښه) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#د_که_پای /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
فراغ exception_handler(صحيح is_nested, بنية exception_چوکاټ * چوکاټ)
{
/* An exception or unexpected interrupt has occurred. */
  سجل بنية ex_s *ep;
  بنية proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_معلومات[چوکاټ->ویکټور];

  إذا (چوکاټ->ویکټور == 2) {		/* spurious NMI on some machines */
	چاپ_کړه("got spurious NMI\n");
	أرجع;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  إذا (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	إذا (((فراغ*)چوکاټ->eip >= (فراغ*)copy_پیغام_منزل_user &&
			(فراغ*)چوکاټ->eip <= (فراغ*)__copy_msg_to_user_end) ||
			((فراغ*)چوکاټ->eip >= (فراغ*)copy_پیغام_from_user &&
			(فراغ*)چوکاټ->eip <= (فراغ*)__copy_msg_from_user_end)) {
		بدل(چوکاټ->ویکټور) {
		/* these error are expected */
		حالة پاڼه_FAULT_ویکټور:
		حالة PROTECTION_ویکټور:
			چوکاټ->eip = (reg_t) __user_copy_msg_pointer_failure;
			أرجع;
		افتراضي:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	إذا (((فراغ*)چوکاټ->eip >= (فراغ*)fxrstor &&
			(فراغ *)چوکاټ->eip <= (فراغ*)__fxrstor_end) ||
			((فراغ*)چوکاټ->eip >= (فراغ*)frstor &&
			(فراغ *)چوکاټ->eip <= (فراغ*)__frstor_end)) {
		چوکاټ->eip = (reg_t) __frstor_failure;
		أرجع;
	}

  	إذا(چوکاټ->ویکټور == DEBUG_ویکټور
		&& (saved_proc->p_reg.psw & TRACEBIT)
		&& (saved_proc->p_seg.p_kern_trap_style == KTS_NONE)) {
		/* Getting a debug trap in the kernel is legitimate
		 * if a traced process entered the kernel using sysenter
		 * or syscall; the trap flag is not cleared then.
		 *
		 * It triggers on the first kernel entry so the trap
		 * style is still KTS_NONE.
		 */

		چوکاټ->eflags &= ~TRACEBIT;

		أرجع;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  إذا(چوکاټ->ویکټور == پاڼه_FAULT_ویکټور) {
	pagefault(saved_proc, چوکاټ, is_nested);
	أرجع;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  إذا (is_nested == 0 && ! iskernelp(saved_proc)) {
#إذا 0
	{

  		چاپ_کړه(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			چوکاټ->ویکټور, (بینښه اوږد)چوکاټ->errcode,
			(بینښه اوږد)چوکاټ->eip, چوکاټ->cs,
			(بینښه اوږد)چوکاټ->eflags);
		proc_stacktrace(saved_proc);
	}

#د_که_پای
	cause_sig(proc_nr(saved_proc), ep->signum);
	أرجع;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, چوکاټ, ep, is_nested);

  panic("return from inkernel_disaster");
}

#إذا USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ساكن فراغ proc_stacktrace_execute(بنية proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	صحيح iskernel;
	صحيح n = 0;

	iskernel = iskernelp(whichproc);

	چاپ_کړه("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	طالما(v_bp) {
		reg_t v_pc;

#تعریف_کړه PRCOPY(pr, pv, v, n) \
  (iskernel ? (حافظه_نقل((حرف *) v, (حرف *) pv, n), OK) : \
     معلومات_copy(pr->p_endpoint, pv, KERNEL, (vir_بایټونه) (v), n))

	        إذا(PRCOPY(whichproc, v_bp, &v_hbp, حجم(v_hbp)) != OK) {
			چاپ_کړه("(v_bp 0x%lx ?)", v_bp);
			اكسر;
		}
		إذا(PRCOPY(whichproc, v_bp + حجم(v_pc), &v_pc, حجم(v_pc)) != OK) {
			چاپ_کړه("(v_pc 0x%lx ?)", v_bp + حجم(v_pc));
			اكسر;
		}
		چاپ_کړه("0x%lx ", (بینښه اوږد) v_pc);
		إذا(v_hbp != 0 && v_hbp <= v_bp) {
			چاپ_کړه("(hbp 0x%lx ?)", v_hbp);
			اكسر;
		}
		v_bp = v_hbp;
		إذا(n++ > 50) {
			چاپ_کړه("(truncated after %d steps) ", n);
			اكسر;
		}
	}
	چاپ_کړه("\n");
}
#د_که_پای /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
فراغ proc_stacktrace(بنية proc *whichproc)
{
	u32_t use_bp;

	إذا(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		چاپ_کړه("WARNING: stacktrace of running process\n");
	}

	بدل(whichproc->p_seg.p_kern_trap_style) {
		حالة KTS_SYSENTER:
		حالة KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			إذا(معلومات_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_بایټونه) &use_bp,
				حجم(use_bp)) != OK) {
				چاپ_کړه("stacktrace: aborting, copy failed\n");
				أرجع;
			}

			اكسر;
		}
		افتراضي:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			اكسر;
	}

#إذا USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#د_که_پای /* USE_SYSDEBUG */
}

فراغ enable_fpu_exception(فراغ)
{
	u32_t cr0 = read_cr0();
	إذا(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

فراغ disable_fpu_exception(فراغ)
{
	clts();
}

