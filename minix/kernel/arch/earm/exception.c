/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

ਬਣਤਰ ex_s {
	ਅੱਖਰ *ਸੁਨੇਹਾ_2;
	ਪੂਰਨ signum;
};

ਸਥਿਰ ਬਣਤਰ ex_s ex_ਅੰਕੜੇ[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

ਸਥਿਰ ਖਾਲੀ inkernel_disaster(ਬਣਤਰ proc *saved_proc,
	reg_t *saved_lr, ਬਣਤਰ ex_s *ep, ਪੂਰਨ is_nested);

ਬਾਹਰੀ ਪੂਰਨ catch_pagefaults;

ਸਥਿਰ ਖਾਲੀ proc_stacktrace_execute(ਬਣਤਰ proc *whichproc, reg_t v_bp, reg_t pc);

ਸਥਿਰ ਖਾਲੀ pagefault( ਬਣਤਰ proc *pr,
			reg_t *saved_lr,
			ਪੂਰਨ is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_ਸਥਿਤੀ)
{
	ਪੂਰਨ ਵਿੱਚ_physcopy = 0, ਵਿੱਚ_memset = 0;

	ਸੁਨੇਹਾ m_pagefault;
	ਪੂਰਨ err;

	ਵਿੱਚ_physcopy = (*saved_lr > (vir_ਬਾਈਟਾਂ) phys_copy) &&
	   (*saved_lr < (vir_ਬਾਈਟਾਂ) phys_copy_fault);

	ਵਿੱਚ_memset = (*saved_lr > (vir_ਬਾਈਟਾਂ) phys_memset) &&
	   (*saved_lr < (vir_ਬਾਈਟਾਂ) memset_fault);

	ਜੇ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (ਵਿੱਚ_physcopy || ਵਿੱਚ_memset)) {
		ਜੇ (is_nested) {
			ਜੇ(ਵਿੱਚ_physcopy) {
				ਪ੍ਰਤਿਜ੍ਞਾ(!ਵਿੱਚ_memset);
				*saved_lr = (reg_t) phys_copy_fault_ਵਿੱਚ_kernel;
			} ਨਹੀਂਤਾਂ {
				*saved_lr = (reg_t) memset_fault_ਵਿੱਚ_kernel;
			}
		}
		ਨਹੀਂਤਾਂ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		ਵਾਪਸ;
	}

	ਜੇ(is_nested) {
		ਲਿਖੋ("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, ਨਿਰਾ, is_nested);
	}

	/* VM can't handle page faults. */
	ਜੇ(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		ਲਿਖੋ("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_ਸਥਿਤੀ,
			is_nested);
		proc_stacktrace(pr);
		ਲਿਖੋ("pc of pagefault: 0x%lx\n", pr->p_reg.pc);
		panic("pagefault in VM");

		ਵਾਪਸ;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_ਸੈੱਟ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ਸਰੋਤ = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_ਸਥਿਤੀ;

	ਜੇ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	ਵਾਪਸ;
}

ਸਥਿਰ ਖਾਲੀ
ਅੰਕੜੇ_abort(ਪੂਰਨ is_nested, ਬਣਤਰ proc *pr, reg_t *saved_lr,
		       ਬਣਤਰ ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	ਜੇ (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} ਨਹੀਂਤਾਂ ਜੇ (!is_nested) {
		/* A user process caused some other kind of data abort. */
		ਪੂਰਨ signum = SIGSEGV;

		ਜੇ (is_align_fault(fs)) {
			signum = SIGBUS;
		} ਨਹੀਂਤਾਂ {
			ਲਿਖੋ("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} ਨਹੀਂਤਾਂ { /* is_nested */
		ਲਿਖੋ("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

ਸਥਿਰ ਖਾਲੀ inkernel_disaster(ਬਣਤਰ proc *saved_proc,
	reg_t *saved_lr, ਬਣਤਰ ex_s *ep,
	ਪੂਰਨ is_nested)
{
#ਜੇ USE_SYSDEBUG
  ਜੇ(ep)
	ਲਿਖੋ("\n%s\n", ep->ਸੁਨੇਹਾ_2);

  ਲਿਖੋ("cpu %d is_nested = %d ", cpuid, is_nested);

  ਜੇ (saved_proc) {
	  ਲਿਖੋ("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  ਲਿਖੋ("pc = 0x%x\n", (ਬਿਨਾਨਿਸ਼ਾਨੀ) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");

#ਜੇ_ਅੰਤ /* USE_SYSDEBUG */
}

ਖਾਲੀ exception_handler(ਪੂਰਨ is_nested, reg_t *saved_lr, ਪੂਰਨ ਸਦਿਸ਼)
{
/* An exception or unexpected interrupt has occurred. */
  ਬਣਤਰ ex_s *ep;
  ਬਣਤਰ proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_ਅੰਕੜੇ[ਸਦਿਸ਼];

  ਪ੍ਰਤਿਜ੍ਞਾ((vir_ਬਾਈਟਾਂ) saved_lr >= kinfo.vir_kern_ਸ਼ੁਰੂ);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  ਜੇ (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	ਜੇ (((ਖਾਲੀ*)*saved_lr >= (ਖਾਲੀ*)copy_ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_user &&
			(ਖਾਲੀ*)*saved_lr <= (ਖਾਲੀ*)__copy_msg_to_user_end) ||
			((ਖਾਲੀ*)*saved_lr >= (ਖਾਲੀ*)copy_ਸੁਨੇਹਾ_from_user &&
			(ਖਾਲੀ*)*saved_lr <= (ਖਾਲੀ*)__copy_msg_from_user_end)) {
		ਚੋਣ(ਸਦਿਸ਼) {
		/* these error are expected */
		ਹਾਲਤ ਅੰਕੜੇ_ABORT_ਸਦਿਸ਼:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			ਵਾਪਸ;
		ਮੂਲ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  ਜੇ (ਸਦਿਸ਼ == ਅੰਕੜੇ_ABORT_ਸਦਿਸ਼) {
	ਅੰਕੜੇ_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	ਵਾਪਸ;
  }

  ਜੇ (!is_nested && ਸਦਿਸ਼ == PREFETCH_ABORT_ਸਦਿਸ਼) {
	ਸਥਿਰ ਪੂਰਨ warned = FALSE;
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
	ਜੇ (*saved_lr != ifar && !warned) {
		ਲਿਖੋ("KERNEL: prefetch abort with differing IFAR and LR\n");
		ਲਿਖੋ("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	ਵਾਪਸ;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  ਜੇ (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	ਵਾਪਸ;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#ਜੇ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ proc_stacktrace_execute(ਬਣਤਰ proc *whichproc, reg_t v_bp, reg_t pc)
{
	ਲਿਖੋ("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ proc_stacktrace(ਬਣਤਰ proc *whichproc)
{
#ਜੇ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#ਜੇ_ਅੰਤ /* USE_SYSDEBUG */
}

ਖਾਲੀ enable_fpu_exception(ਖਾਲੀ)
{
}

ਖਾਲੀ disable_fpu_exception(ਖਾਲੀ)
{
}
