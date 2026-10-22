/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#شامل "kernel/kernel.h"
#شامل "arch_proto.h"
#شامل <signal.h>
#شامل <string.h>
#شامل <assert.h>
#شامل "kernel/proc.h"
#شامل "kernel/proto.h"
#شامل <machine/vm.h>

ساخت ex_s {
	حرف *پیغام_2;
	صحیح signum;
};

ساکن ساخت ex_s ex_ڈیٹا[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

ساکن خالی inkernel_disaster(ساخت proc *saved_proc,
	reg_t *saved_lr, ساخت ex_s *ep, صحیح is_nested);

خارجی صحیح catch_pagefaults;

ساکن خالی proc_stacktrace_execute(ساخت proc *whichproc, reg_t v_bp, reg_t pc);

ساکن خالی pagefault( ساخت proc *pr,
			reg_t *saved_lr,
			صحیح is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_حالت)
{
	صحیح اندر_physcopy = 0, اندر_memset = 0;

	پیغام m_pagefault;
	صحیح err;

	اندر_physcopy = (*saved_lr > (vir_بائٹیں) phys_copy) &&
	   (*saved_lr < (vir_بائٹیں) phys_copy_fault);

	اندر_memset = (*saved_lr > (vir_بائٹیں) phys_memset) &&
	   (*saved_lr < (vir_بائٹیں) memset_fault);

	اگر((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (اندر_physcopy || اندر_memset)) {
		اگر (is_nested) {
			اگر(اندر_physcopy) {
				أكد(!اندر_memset);
				*saved_lr = (reg_t) phys_copy_fault_اندر_kernel;
			} ورنہ {
				*saved_lr = (reg_t) memset_fault_اندر_kernel;
			}
		}
		ورنہ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		واپس;
	}

	اگر(is_nested) {
		لکھو("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, تہی, is_nested);
	}

	/* VM can't handle page faults. */
	اگر(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		لکھو("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_حالت,
			is_nested);
		proc_stacktrace(pr);
		لکھو("pc of pagefault: 0x%lx\n", pr->p_reg.pc);
		panic("pagefault in VM");

		واپس;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_سیٹ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ماخذ = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_حالت;

	اگر ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	واپس;
}

ساکن خالی
ڈیٹا_abort(صحیح is_nested, ساخت proc *pr, reg_t *saved_lr,
		       ساخت ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	اگر (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} ورنہ اگر (!is_nested) {
		/* A user process caused some other kind of data abort. */
		صحیح signum = SIGSEGV;

		اگر (is_align_fault(fs)) {
			signum = SIGBUS;
		} ورنہ {
			لکھو("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} ورنہ { /* is_nested */
		لکھو("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

ساکن خالی inkernel_disaster(ساخت proc *saved_proc,
	reg_t *saved_lr, ساخت ex_s *ep,
	صحیح is_nested)
{
#اگر USE_SYSDEBUG
  اگر(ep)
	لکھو("\n%s\n", ep->پیغام_2);

  لکھو("cpu %d is_nested = %d ", cpuid, is_nested);

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

خالی exception_handler(صحیح is_nested, reg_t *saved_lr, صحیح سمتیہ)
{
/* An exception or unexpected interrupt has occurred. */
  ساخت ex_s *ep;
  ساخت proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_ڈیٹا[سمتیہ];

  أكد((vir_بائٹیں) saved_lr >= kinfo.vir_kern_آغاز);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  اگر (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	اگر (((خالی*)*saved_lr >= (خالی*)copy_پیغام_مقصد_user &&
			(خالی*)*saved_lr <= (خالی*)__copy_msg_to_user_end) ||
			((خالی*)*saved_lr >= (خالی*)copy_پیغام_from_user &&
			(خالی*)*saved_lr <= (خالی*)__copy_msg_from_user_end)) {
		انتخاب(سمتیہ) {
		/* these error are expected */
		حالت ڈیٹا_ABORT_سمتیہ:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			واپس;
		طےشدہ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  اگر (سمتیہ == ڈیٹا_ABORT_سمتیہ) {
	ڈیٹا_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	واپس;
  }

  اگر (!is_nested && سمتیہ == PREFETCH_ABORT_سمتیہ) {
	ساکن صحیح warned = FALSE;
	reg_t ifar = read_ifar(), ifsr = read_ifsr();

	/* The saved_lr is the instruction we're going to execute after
	 * the fault is handled; IFAR is the address that pagefaulted
	 * while fetching the instruction. As far as we know the two
	 * should be the same, if not this assumption will lead to very
	 * hard to debug problems (instruction executing being off by one)
	 * and this assumption needs re-examining.
	 *
	 * UPDATE: at least qemu-linaro does in fact sometimes generate faults
	 * with LR and IFAR differing by as many as 64 bytes.  While the page
	 * fault resolution code below handles this case just fine, the cause
	 * of this behavior is unknown.  We have not yet seen the same on
	 * actual hardware, which is why we warn about this problem once.
	 */
	اگر (*saved_lr != ifar && !warned) {
		لکھو("KERNEL: prefetch abort with differing IFAR and LR\n");
		لکھو("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	واپس;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  اگر (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	واپس;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#اگر USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ساکن خالی proc_stacktrace_execute(ساخت proc *whichproc, reg_t v_bp, reg_t pc)
{
	لکھو("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#اگر_ختم

خالی proc_stacktrace(ساخت proc *whichproc)
{
#اگر USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#اگر_ختم /* USE_SYSDEBUG */
}

خالی enable_fpu_exception(خالی)
{
}

خالی disable_fpu_exception(خالی)
{
}
