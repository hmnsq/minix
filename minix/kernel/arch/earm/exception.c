/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"
#ضمن <signal.h>
#ضمن <string.h>
#ضمن <assert.h>
#ضمن "kernel/proc.h"
#ضمن "kernel/proto.h"
#ضمن <machine/vm.h>

بنية ex_s {
	حرف *پیغام_2;
	صحيح signum;
};

ساكن بنية ex_s ex_معلومات[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

ساكن فراغ inkernel_disaster(بنية proc *saved_proc,
	reg_t *saved_lr, بنية ex_s *ep, صحيح is_nested);

خارجي صحيح catch_pagefaults;

ساكن فراغ proc_stacktrace_execute(بنية proc *whichproc, reg_t v_bp, reg_t pc);

ساكن فراغ pagefault( بنية proc *pr,
			reg_t *saved_lr,
			صحيح is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_حالت)
{
	صحيح دننه_physcopy = 0, دننه_memset = 0;

	پیغام m_pagefault;
	صحيح err;

	دننه_physcopy = (*saved_lr > (vir_بایټونه) phys_copy) &&
	   (*saved_lr < (vir_بایټونه) phys_copy_fault);

	دننه_memset = (*saved_lr > (vir_بایټونه) phys_memset) &&
	   (*saved_lr < (vir_بایټونه) memset_fault);

	إذا((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (دننه_physcopy || دننه_memset)) {
		إذا (is_nested) {
			إذا(دننه_physcopy) {
				أكد(!دننه_memset);
				*saved_lr = (reg_t) phys_copy_fault_دننه_kernel;
			} وإلا {
				*saved_lr = (reg_t) memset_fault_دننه_kernel;
			}
		}
		وإلا {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		أرجع;
	}

	إذا(is_nested) {
		چاپ_کړه("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, عدم, is_nested);
	}

	/* VM can't handle page faults. */
	إذا(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		چاپ_کړه("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_حالت,
			is_nested);
		proc_stacktrace(pr);
		چاپ_کړه("pc of pagefault: 0x%lx\n", pr->p_reg.pc);
		panic("pagefault in VM");

		أرجع;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_سېټ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_سرچینه = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_حالت;

	إذا ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	أرجع;
}

ساكن فراغ
معلومات_abort(صحيح is_nested, بنية proc *pr, reg_t *saved_lr,
		       بنية ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	إذا (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} وإلا إذا (!is_nested) {
		/* A user process caused some other kind of data abort. */
		صحيح signum = SIGSEGV;

		إذا (is_align_fault(fs)) {
			signum = SIGBUS;
		} وإلا {
			چاپ_کړه("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} وإلا { /* is_nested */
		چاپ_کړه("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

ساكن فراغ inkernel_disaster(بنية proc *saved_proc,
	reg_t *saved_lr, بنية ex_s *ep,
	صحيح is_nested)
{
#إذا USE_SYSDEBUG
  إذا(ep)
	چاپ_کړه("\n%s\n", ep->پیغام_2);

  چاپ_کړه("cpu %d is_nested = %d ", cpuid, is_nested);

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

فراغ exception_handler(صحيح is_nested, reg_t *saved_lr, صحيح ویکټور)
{
/* An exception or unexpected interrupt has occurred. */
  بنية ex_s *ep;
  بنية proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_معلومات[ویکټور];

  أكد((vir_بایټونه) saved_lr >= kinfo.vir_kern_پیل);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  إذا (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	إذا (((فراغ*)*saved_lr >= (فراغ*)copy_پیغام_منزل_user &&
			(فراغ*)*saved_lr <= (فراغ*)__copy_msg_to_user_end) ||
			((فراغ*)*saved_lr >= (فراغ*)copy_پیغام_from_user &&
			(فراغ*)*saved_lr <= (فراغ*)__copy_msg_from_user_end)) {
		بدل(ویکټور) {
		/* these error are expected */
		حالة معلومات_ABORT_ویکټور:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			أرجع;
		افتراضي:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  إذا (ویکټور == معلومات_ABORT_ویکټور) {
	معلومات_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	أرجع;
  }

  إذا (!is_nested && ویکټور == PREFETCH_ABORT_ویکټور) {
	ساكن صحيح warned = FALSE;
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
	إذا (*saved_lr != ifar && !warned) {
		چاپ_کړه("KERNEL: prefetch abort with differing IFAR and LR\n");
		چاپ_کړه("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	أرجع;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  إذا (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	أرجع;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#إذا USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ساكن فراغ proc_stacktrace_execute(بنية proc *whichproc, reg_t v_bp, reg_t pc)
{
	چاپ_کړه("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#د_که_پای

فراغ proc_stacktrace(بنية proc *whichproc)
{
#إذا USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#د_که_پای /* USE_SYSDEBUG */
}

فراغ enable_fpu_exception(فراغ)
{
}

فراغ disable_fpu_exception(فراغ)
{
}
