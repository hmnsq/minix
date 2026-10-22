/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#شامل "kernel/kernel.h"
#شامل "arch_proto.h"
#شامل <signal.h>
#شامل <string.h>
#شامل <assert.h>
#شامل <machine/vm.h>

ساخت ex_s {
	حرف *پیغام_2;
	صحیح signum;
	صحیح minprocessor;
};

ساکن ساخت ex_s ex_ڈیٹا[] = {
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
	{ تہی, SIGILL, 0 },			/* probably software trap */
	{ "Coprocessor error", SIGFPE, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", SIGFPE, 386 },
};

ساکن خالی inkernel_disaster(ساخت proc *saved_proc,
	ساخت exception_چوکھٹا *چوکھٹا, ساخت ex_s *ep, صحیح is_nested);

خارجی صحیح catch_pagefaults;

ساکن خالی proc_stacktrace_execute(ساخت proc *whichproc, reg_t v_bp, reg_t pc);

ساکن خالی pagefault( ساخت proc *pr,
			ساخت exception_چوکھٹا * چوکھٹا,
			صحیح is_nested)
{
	صحیح اندر_physcopy = 0, اندر_memset = 0;

	reg_t pagefaultcr2;
	پیغام m_pagefault;
	صحیح err;

	pagefaultcr2 = read_cr2();

#اگر 0
	لکھو("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#اگر_ختم

	اندر_physcopy = (چوکھٹا->eip > (vir_بائٹیں) phys_copy) &&
	   (چوکھٹا->eip < (vir_بائٹیں) phys_copy_fault);

	اندر_memset = (چوکھٹا->eip > (vir_بائٹیں) phys_memset) &&
	   (چوکھٹا->eip < (vir_بائٹیں) memset_fault);

	اگر((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (اندر_physcopy || اندر_memset)) {
#اگر 0
		لکھو("pf caught! addr 0x%lx\n", pagefaultcr2);
#اگر_ختم
		اگر (is_nested) {
			اگر(اندر_physcopy) {
				أكد(!اندر_memset);
				چوکھٹا->eip = (reg_t) phys_copy_fault_اندر_kernel;
			} ورنہ {
				چوکھٹا->eip = (reg_t) memset_fault_اندر_kernel;
			}
		}
		ورنہ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		واپس;
	}

	اگر(is_nested) {
		لکھو("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			چوکھٹا->eip, pagefaultcr2);
		inkernel_disaster(pr, چوکھٹا, تہی, is_nested);
	}

	/* VM can't handle page faults. */
	اگر(pr->p_endpoint == VM_PROC_NR) {
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

		واپس;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_سیٹ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ماخذ = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = چوکھٹا->errcode;

	اگر ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	واپس;
}

ساکن خالی inkernel_disaster(ساخت proc *saved_proc,
	ساخت exception_چوکھٹا * چوکھٹا, ساخت ex_s *ep,
	صحیح is_nested)
{
#اگر USE_SYSDEBUG
  اگر(ep) {
	اگر (ep->پیغام_2 == تہی)
		لکھو("\nIntel-reserved exception %d\n", چوکھٹا->سمتیہ);
	  ورنہ
		لکھو("\n%s\n", ep->پیغام_2);
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

  اگر (saved_proc) {
	  لکھو("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  لکھو("pc = 0x%x\n", (غیرعلامتی) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#اگر_ختم /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
خالی exception_handler(صحیح is_nested, ساخت exception_چوکھٹا * چوکھٹا)
{
/* An exception or unexpected interrupt has occurred. */
  اندراج ساخت ex_s *ep;
  ساخت proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_ڈیٹا[چوکھٹا->سمتیہ];

  اگر (چوکھٹا->سمتیہ == 2) {		/* spurious NMI on some machines */
	لکھو("got spurious NMI\n");
	واپس;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  اگر (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	اگر (((خالی*)چوکھٹا->eip >= (خالی*)copy_پیغام_مقصد_user &&
			(خالی*)چوکھٹا->eip <= (خالی*)__copy_msg_to_user_end) ||
			((خالی*)چوکھٹا->eip >= (خالی*)copy_پیغام_from_user &&
			(خالی*)چوکھٹا->eip <= (خالی*)__copy_msg_from_user_end)) {
		انتخاب(چوکھٹا->سمتیہ) {
		/* these error are expected */
		حالت صفحہ_FAULT_سمتیہ:
		حالت PROTECTION_سمتیہ:
			چوکھٹا->eip = (reg_t) __user_copy_msg_pointer_failure;
			واپس;
		طےشدہ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	اگر (((خالی*)چوکھٹا->eip >= (خالی*)fxrstor &&
			(خالی *)چوکھٹا->eip <= (خالی*)__fxrstor_end) ||
			((خالی*)چوکھٹا->eip >= (خالی*)frstor &&
			(خالی *)چوکھٹا->eip <= (خالی*)__frstor_end)) {
		چوکھٹا->eip = (reg_t) __frstor_failure;
		واپس;
	}

  	اگر(چوکھٹا->سمتیہ == DEBUG_سمتیہ
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

		واپس;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  اگر(چوکھٹا->سمتیہ == صفحہ_FAULT_سمتیہ) {
	pagefault(saved_proc, چوکھٹا, is_nested);
	واپس;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  اگر (is_nested == 0 && ! iskernelp(saved_proc)) {
#اگر 0
	{

  		لکھو(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			چوکھٹا->سمتیہ, (غیرعلامتی طویل)چوکھٹا->errcode,
			(غیرعلامتی طویل)چوکھٹا->eip, چوکھٹا->cs,
			(غیرعلامتی طویل)چوکھٹا->eflags);
		proc_stacktrace(saved_proc);
	}

#اگر_ختم
	cause_sig(proc_nr(saved_proc), ep->signum);
	واپس;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, چوکھٹا, ep, is_nested);

  panic("return from inkernel_disaster");
}

#اگر USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ساکن خالی proc_stacktrace_execute(ساخت proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	صحیح iskernel;
	صحیح n = 0;

	iskernel = iskernelp(whichproc);

	لکھو("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	جب(v_bp) {
		reg_t v_pc;

#تعریف_کرو PRCOPY(pr, pv, v, n) \
  (iskernel ? (حافظہ_نقل((حرف *) v, (حرف *) pv, n), OK) : \
     ڈیٹا_copy(pr->p_endpoint, pv, KERNEL, (vir_بائٹیں) (v), n))

	        اگر(PRCOPY(whichproc, v_bp, &v_hbp, حجم(v_hbp)) != OK) {
			لکھو("(v_bp 0x%lx ?)", v_bp);
			توڑو;
		}
		اگر(PRCOPY(whichproc, v_bp + حجم(v_pc), &v_pc, حجم(v_pc)) != OK) {
			لکھو("(v_pc 0x%lx ?)", v_bp + حجم(v_pc));
			توڑو;
		}
		لکھو("0x%lx ", (غیرعلامتی طویل) v_pc);
		اگر(v_hbp != 0 && v_hbp <= v_bp) {
			لکھو("(hbp 0x%lx ?)", v_hbp);
			توڑو;
		}
		v_bp = v_hbp;
		اگر(n++ > 50) {
			لکھو("(truncated after %d steps) ", n);
			توڑو;
		}
	}
	لکھو("\n");
}
#اگر_ختم /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
خالی proc_stacktrace(ساخت proc *whichproc)
{
	u32_t use_bp;

	اگر(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		لکھو("WARNING: stacktrace of running process\n");
	}

	انتخاب(whichproc->p_seg.p_kern_trap_style) {
		حالت KTS_SYSENTER:
		حالت KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			اگر(ڈیٹا_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_بائٹیں) &use_bp,
				حجم(use_bp)) != OK) {
				لکھو("stacktrace: aborting, copy failed\n");
				واپس;
			}

			توڑو;
		}
		طےشدہ:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			توڑو;
	}

#اگر USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#اگر_ختم /* USE_SYSDEBUG */
}

خالی enable_fpu_exception(خالی)
{
	u32_t cr0 = read_cr0();
	اگر(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

خالی disable_fpu_exception(خالی)
{
	clts();
}

