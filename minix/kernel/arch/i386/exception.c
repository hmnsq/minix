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
	حرف *نياپو;
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
	بنية exception_چوکٺو *چوکٺو, بنية ex_s *ep, صحيح is_nested);

خارجي صحيح catch_pagefaults;

ساكن فراغ proc_stacktrace_execute(بنية proc *whichproc, reg_t v_bp, reg_t pc);

ساكن فراغ pagefault( بنية proc *pr,
			بنية exception_چوکٺو * چوکٺو,
			صحيح is_nested)
{
	صحيح اندريون_physcopy = 0, اندريون_memset = 0;

	reg_t pagefaultcr2;
	پيغام m_pagefault;
	صحيح err;

	pagefaultcr2 = read_cr2();

#إذا 0
	لکو("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#جيڪڏهن_پورو

	اندريون_physcopy = (چوکٺو->eip > (vir_بائيٽون) phys_copy) &&
	   (چوکٺو->eip < (vir_بائيٽون) phys_copy_fault);

	اندريون_memset = (چوکٺو->eip > (vir_بائيٽون) phys_memset) &&
	   (چوکٺو->eip < (vir_بائيٽون) memset_fault);

	إذا((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (اندريون_physcopy || اندريون_memset)) {
#إذا 0
		لکو("pf caught! addr 0x%lx\n", pagefaultcr2);
#جيڪڏهن_پورو
		إذا (is_nested) {
			إذا(اندريون_physcopy) {
				أكد(!اندريون_memset);
				چوکٺو->eip = (reg_t) phys_copy_fault_اندريون_kernel;
			} وإلا {
				چوکٺو->eip = (reg_t) memset_fault_اندريون_kernel;
			}
		}
		وإلا {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		أرجع;
	}

	إذا(is_nested) {
		لکو("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			چوکٺو->eip, pagefaultcr2);
		inkernel_disaster(pr, چوکٺو, عدم, is_nested);
	}

	/* VM can't handle page faults. */
	إذا(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		لکو("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefaultcr2, چوکٺو->errcode,
			is_nested);
		proc_stacktrace(pr);
		لکو("pc of pagefault: 0x%lx\n", چوکٺو->eip);
		panic("pagefault in VM");

		أرجع;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_سيٽ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ذريعو = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = چوکٺو->errcode;

	إذا ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	أرجع;
}

ساكن فراغ inkernel_disaster(بنية proc *saved_proc,
	بنية exception_چوکٺو * چوکٺو, بنية ex_s *ep,
	صحيح is_nested)
{
#إذا USE_SYSDEBUG
  إذا(ep) {
	إذا (ep->نياپو == عدم)
		لکو("\nIntel-reserved exception %d\n", چوکٺو->ويڪٽر);
	  وإلا
		لکو("\n%s\n", ep->نياپو);
  }

  لکو("cpu %d is_nested = %d ", cpuid, is_nested);

  لکو("vec_nr= %d, trap_errno= 0x%x, eip= 0x%x, "
	"cs= 0x%x, eflags= 0x%x trap_esp 0x%08x\n",
	چوکٺو->ويڪٽر, چوکٺو->errcode, چوکٺو->eip,
	چوکٺو->cs, چوکٺو->eflags, چوکٺو);
  لکو("KERNEL registers :\n");
#وصف_ڪريو REG(n) (((u32_t *)چوکٺو)[-n])
  لکو(
		  "\t%%eax 0x%08x %%ebx 0x%08x %%ecx 0x%08x %%edx 0x%08x\n"
		  "\t%%esp 0x%08x %%ebp 0x%08x %%esi 0x%08x %%edi 0x%08x\n",
		  REG(1), REG(2), REG(3), REG(4),
		  REG(5), REG(6), REG(7), REG(8));

  { 
  	reg_t k_ebp = REG(6);
  	لکو("KERNEL stacktrace, starting with ebp = 0x%lx:\n", k_ebp);
  	proc_stacktrace_execute(proc_addr(SYSTEM), k_ebp, چوکٺو->eip);
  }

  إذا (saved_proc) {
	  لکو("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  لکو("pc = 0x%x\n", (بنانشان) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#جيڪڏهن_پورو /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
فراغ exception_handler(صحيح is_nested, بنية exception_چوکٺو * چوکٺو)
{
/* An exception or unexpected interrupt has occurred. */
  سجل بنية ex_s *ep;
  بنية proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_معلومات[چوکٺو->ويڪٽر];

  إذا (چوکٺو->ويڪٽر == 2) {		/* spurious NMI on some machines */
	لکو("got spurious NMI\n");
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
	إذا (((فراغ*)چوکٺو->eip >= (فراغ*)copy_نياپو_منزل_user &&
			(فراغ*)چوکٺو->eip <= (فراغ*)__copy_msg_to_user_end) ||
			((فراغ*)چوکٺو->eip >= (فراغ*)copy_نياپو_from_user &&
			(فراغ*)چوکٺو->eip <= (فراغ*)__copy_msg_from_user_end)) {
		بدل(چوکٺو->ويڪٽر) {
		/* these error are expected */
		حالة صفحو_FAULT_ويڪٽر:
		حالة PROTECTION_ويڪٽر:
			چوکٺو->eip = (reg_t) __user_copy_msg_pointer_failure;
			أرجع;
		افتراضي:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	إذا (((فراغ*)چوکٺو->eip >= (فراغ*)fxrstor &&
			(فراغ *)چوکٺو->eip <= (فراغ*)__fxrstor_end) ||
			((فراغ*)چوکٺو->eip >= (فراغ*)frstor &&
			(فراغ *)چوکٺو->eip <= (فراغ*)__frstor_end)) {
		چوکٺو->eip = (reg_t) __frstor_failure;
		أرجع;
	}

  	إذا(چوکٺو->ويڪٽر == DEBUG_ويڪٽر
		&& (saved_proc->p_reg.psw & TRACEBIT)
		&& (saved_proc->p_seg.p_kern_trap_style == KTS_NONE)) {
		/* Getting a debug trap in the kernel is legitimate
		 * if a traced process entered the kernel using sysenter
		 * or syscall; the trap flag is not cleared then.
		 *
		 * It triggers on the first kernel entry so the trap
		 * style is still KTS_NONE.
		 */

		چوکٺو->eflags &= ~TRACEBIT;

		أرجع;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  إذا(چوکٺو->ويڪٽر == صفحو_FAULT_ويڪٽر) {
	pagefault(saved_proc, چوکٺو, is_nested);
	أرجع;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  إذا (is_nested == 0 && ! iskernelp(saved_proc)) {
#إذا 0
	{

  		لکو(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			چوکٺو->ويڪٽر, (بنانشان ڊگهو)چوکٺو->errcode,
			(بنانشان ڊگهو)چوکٺو->eip, چوکٺو->cs,
			(بنانشان ڊگهو)چوکٺو->eflags);
		proc_stacktrace(saved_proc);
	}

#جيڪڏهن_پورو
	cause_sig(proc_nr(saved_proc), ep->signum);
	أرجع;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, چوکٺو, ep, is_nested);

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

	لکو("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	طالما(v_bp) {
		reg_t v_pc;

#وصف_ڪريو PRCOPY(pr, pv, v, n) \
  (iskernel ? (يادگيري_نقل((حرف *) v, (حرف *) pv, n), OK) : \
     معلومات_copy(pr->p_endpoint, pv, KERNEL, (vir_بائيٽون) (v), n))

	        إذا(PRCOPY(whichproc, v_bp, &v_hbp, حجم(v_hbp)) != OK) {
			لکو("(v_bp 0x%lx ?)", v_bp);
			اكسر;
		}
		إذا(PRCOPY(whichproc, v_bp + حجم(v_pc), &v_pc, حجم(v_pc)) != OK) {
			لکو("(v_pc 0x%lx ?)", v_bp + حجم(v_pc));
			اكسر;
		}
		لکو("0x%lx ", (بنانشان ڊگهو) v_pc);
		إذا(v_hbp != 0 && v_hbp <= v_bp) {
			لکو("(hbp 0x%lx ?)", v_hbp);
			اكسر;
		}
		v_bp = v_hbp;
		إذا(n++ > 50) {
			لکو("(truncated after %d steps) ", n);
			اكسر;
		}
	}
	لکو("\n");
}
#جيڪڏهن_پورو /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
فراغ proc_stacktrace(بنية proc *whichproc)
{
	u32_t use_bp;

	إذا(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		لکو("WARNING: stacktrace of running process\n");
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
			  KERNEL, (vir_بائيٽون) &use_bp,
				حجم(use_bp)) != OK) {
				لکو("stacktrace: aborting, copy failed\n");
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
#جيڪڏهن_پورو /* USE_SYSDEBUG */
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

