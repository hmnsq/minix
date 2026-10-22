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
	حرف *پیغام;
	صحيح signum;
	صحيح minprocessor;
};

ساكن بنية ex_s ex_انکڑے[] = {
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
	بنية exception_چوکھٹا *چوکھٹا, بنية ex_s *ep, صحيح is_nested);

خارجي صحيح catch_pagefaults;

ساكن فراغ proc_stacktrace_execute(بنية proc *whichproc, reg_t v_bp, reg_t pc);

ساكن فراغ pagefault( بنية proc *pr,
			بنية exception_چوکھٹا * چوکھٹا,
			صحيح is_nested)
{
	صحيح وچ_physcopy = 0, وچ_memset = 0;

	reg_t pagefaultcr2;
	سنیہا m_pagefault;
	صحيح err;

	pagefaultcr2 = read_cr2();

#إذا 0
	لکھو("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#جے_انت

	وچ_physcopy = (چوکھٹا->eip > (vir_بائٹاں) phys_copy) &&
	   (چوکھٹا->eip < (vir_بائٹاں) phys_copy_fault);

	وچ_memset = (چوکھٹا->eip > (vir_بائٹاں) phys_memset) &&
	   (چوکھٹا->eip < (vir_بائٹاں) memset_fault);

	إذا((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (وچ_physcopy || وچ_memset)) {
#إذا 0
		لکھو("pf caught! addr 0x%lx\n", pagefaultcr2);
#جے_انت
		إذا (is_nested) {
			إذا(وچ_physcopy) {
				أكد(!وچ_memset);
				چوکھٹا->eip = (reg_t) phys_copy_fault_وچ_kernel;
			} وإلا {
				چوکھٹا->eip = (reg_t) memset_fault_وچ_kernel;
			}
		}
		وإلا {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		أرجع;
	}

	إذا(is_nested) {
		لکھو("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			چوکھٹا->eip, pagefaultcr2);
		inkernel_disaster(pr, چوکھٹا, عدم, is_nested);
	}

	/* VM can't handle page faults. */
	إذا(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		لکھو("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefaultcr2, چوکھٹا->errcode,
			is_nested);
		proc_stacktrace(pr);
		لکھو("pc of pagefault: 0x%lx\n", چوکھٹا->eip);
		panic("pagefault in VM");

		أرجع;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_سیٹ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_سروت = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = چوکھٹا->errcode;

	إذا ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	أرجع;
}

ساكن فراغ inkernel_disaster(بنية proc *saved_proc,
	بنية exception_چوکھٹا * چوکھٹا, بنية ex_s *ep,
	صحيح is_nested)
{
#إذا USE_SYSDEBUG
  إذا(ep) {
	إذا (ep->پیغام == عدم)
		لکھو("\nIntel-reserved exception %d\n", چوکھٹا->سمتیہ);
	  وإلا
		لکھو("\n%s\n", ep->پیغام);
  }

  لکھو("cpu %d is_nested = %d ", cpuid, is_nested);

  لکھو("vec_nr= %d, trap_errno= 0x%x, eip= 0x%x, "
	"cs= 0x%x, eflags= 0x%x trap_esp 0x%08x\n",
	چوکھٹا->سمتیہ, چوکھٹا->errcode, چوکھٹا->eip,
	چوکھٹا->cs, چوکھٹا->eflags, چوکھٹا);
  لکھو("KERNEL registers :\n");
#تعریف_کرو REG(n) (((u32_t *)چوکھٹا)[-n])
  لکھو(
		  "\t%%eax 0x%08x %%ebx 0x%08x %%ecx 0x%08x %%edx 0x%08x\n"
		  "\t%%esp 0x%08x %%ebp 0x%08x %%esi 0x%08x %%edi 0x%08x\n",
		  REG(1), REG(2), REG(3), REG(4),
		  REG(5), REG(6), REG(7), REG(8));

  { 
  	reg_t k_ebp = REG(6);
  	لکھو("KERNEL stacktrace, starting with ebp = 0x%lx:\n", k_ebp);
  	proc_stacktrace_execute(proc_addr(SYSTEM), k_ebp, چوکھٹا->eip);
  }

  إذا (saved_proc) {
	  لکھو("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  لکھو("pc = 0x%x\n", (بنانشانی) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#جے_انت /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
فراغ exception_handler(صحيح is_nested, بنية exception_چوکھٹا * چوکھٹا)
{
/* An exception or unexpected interrupt has occurred. */
  سجل بنية ex_s *ep;
  بنية proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_انکڑے[چوکھٹا->سمتیہ];

  إذا (چوکھٹا->سمتیہ == 2) {		/* spurious NMI on some machines */
	لکھو("got spurious NMI\n");
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
	إذا (((فراغ*)چوکھٹا->eip >= (فراغ*)copy_پیغام_منزل_user &&
			(فراغ*)چوکھٹا->eip <= (فراغ*)__copy_msg_to_user_end) ||
			((فراغ*)چوکھٹا->eip >= (فراغ*)copy_پیغام_from_user &&
			(فراغ*)چوکھٹا->eip <= (فراغ*)__copy_msg_from_user_end)) {
		بدل(چوکھٹا->سمتیہ) {
		/* these error are expected */
		حالة ورقہ_FAULT_سمتیہ:
		حالة PROTECTION_سمتیہ:
			چوکھٹا->eip = (reg_t) __user_copy_msg_pointer_failure;
			أرجع;
		افتراضي:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	إذا (((فراغ*)چوکھٹا->eip >= (فراغ*)fxrstor &&
			(فراغ *)چوکھٹا->eip <= (فراغ*)__fxrstor_end) ||
			((فراغ*)چوکھٹا->eip >= (فراغ*)frstor &&
			(فراغ *)چوکھٹا->eip <= (فراغ*)__frstor_end)) {
		چوکھٹا->eip = (reg_t) __frstor_failure;
		أرجع;
	}

  	إذا(چوکھٹا->سمتیہ == DEBUG_سمتیہ
		&& (saved_proc->p_reg.psw & TRACEBIT)
		&& (saved_proc->p_seg.p_kern_trap_style == KTS_NONE)) {
		/* Getting a debug trap in the kernel is legitimate
		 * if a traced process entered the kernel using sysenter
		 * or syscall; the trap flag is not cleared then.
		 *
		 * It triggers on the first kernel entry so the trap
		 * style is still KTS_NONE.
		 */

		چوکھٹا->eflags &= ~TRACEBIT;

		أرجع;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  إذا(چوکھٹا->سمتیہ == ورقہ_FAULT_سمتیہ) {
	pagefault(saved_proc, چوکھٹا, is_nested);
	أرجع;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  إذا (is_nested == 0 && ! iskernelp(saved_proc)) {
#إذا 0
	{

  		لکھو(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			چوکھٹا->سمتیہ, (بنانشانی لما)چوکھٹا->errcode,
			(بنانشانی لما)چوکھٹا->eip, چوکھٹا->cs,
			(بنانشانی لما)چوکھٹا->eflags);
		proc_stacktrace(saved_proc);
	}

#جے_انت
	cause_sig(proc_nr(saved_proc), ep->signum);
	أرجع;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, چوکھٹا, ep, is_nested);

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

	لکھو("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	طالما(v_bp) {
		reg_t v_pc;

#تعریف_کرو PRCOPY(pr, pv, v, n) \
  (iskernel ? (حافظہ_نقل((حرف *) v, (حرف *) pv, n), OK) : \
     انکڑے_copy(pr->p_endpoint, pv, KERNEL, (vir_بائٹاں) (v), n))

	        إذا(PRCOPY(whichproc, v_bp, &v_hbp, حجم(v_hbp)) != OK) {
			لکھو("(v_bp 0x%lx ?)", v_bp);
			اكسر;
		}
		إذا(PRCOPY(whichproc, v_bp + حجم(v_pc), &v_pc, حجم(v_pc)) != OK) {
			لکھو("(v_pc 0x%lx ?)", v_bp + حجم(v_pc));
			اكسر;
		}
		لکھو("0x%lx ", (بنانشانی لما) v_pc);
		إذا(v_hbp != 0 && v_hbp <= v_bp) {
			لکھو("(hbp 0x%lx ?)", v_hbp);
			اكسر;
		}
		v_bp = v_hbp;
		إذا(n++ > 50) {
			لکھو("(truncated after %d steps) ", n);
			اكسر;
		}
	}
	لکھو("\n");
}
#جے_انت /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
فراغ proc_stacktrace(بنية proc *whichproc)
{
	u32_t use_bp;

	إذا(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		لکھو("WARNING: stacktrace of running process\n");
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

			إذا(انکڑے_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_بائٹاں) &use_bp,
				حجم(use_bp)) != OK) {
				لکھو("stacktrace: aborting, copy failed\n");
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
#جے_انت /* USE_SYSDEBUG */
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

