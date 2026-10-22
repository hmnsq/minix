/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "arch_proto.h"
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব "kernel/proc.h"
#অন্তর্ভাব "kernel/proto.h"
#অন্তর্ভাব <machine/vm.h>

কাঠামো ex_s {
	অক্ষর *বার্তা;
	পূর্ণসংখ্যা signum;
};

স্থির কাঠামো ex_s ex_তথ্য[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

স্থির শূন্য inkernel_disaster(কাঠামো proc *saved_proc,
	reg_t *saved_lr, কাঠামো ex_s *ep, পূর্ণসংখ্যা is_nested);

বাহ্যিক পূর্ণসংখ্যা catch_pagefaults;

স্থির শূন্য proc_stacktrace_execute(কাঠামো proc *whichproc, reg_t v_bp, reg_t pc);

স্থির শূন্য pagefault( কাঠামো proc *pr,
			reg_t *saved_lr,
			পূর্ণসংখ্যা is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_status)
{
	পূর্ণসংখ্যা in_physcopy = 0, in_memset = 0;

	message m_pagefault;
	পূর্ণসংখ্যা err;

	in_physcopy = (*saved_lr > (vir_bytes) phys_copy) &&
	   (*saved_lr < (vir_bytes) phys_copy_fault);

	in_memset = (*saved_lr > (vir_bytes) phys_memset) &&
	   (*saved_lr < (vir_bytes) memset_fault);

	যদি((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
		যদি (is_nested) {
			যদি(in_physcopy) {
				প্রতিজ্ঞা(!in_memset);
				*saved_lr = (reg_t) phys_copy_fault_in_kernel;
			} অন্যথা {
				*saved_lr = (reg_t) memset_fault_in_kernel;
			}
		}
		অন্যথা {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		ফেরত;
	}

	যদি(is_nested) {
		printf("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, ফাঁকা, is_nested);
	}

	/* VM can't handle page faults. */
	যদি(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		printf("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_status,
			is_nested);
		proc_stacktrace(pr);
		printf("pc of pagefault: 0x%lx\n", pr->p_reg.pc);
		panic("pagefault in VM");

		ফেরত;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_status;

	যদি ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	ফেরত;
}

স্থির শূন্য
তথ্য_abort(পূর্ণসংখ্যা is_nested, কাঠামো proc *pr, reg_t *saved_lr,
		       কাঠামো ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	যদি (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} অন্যথা যদি (!is_nested) {
		/* A user process caused some other kind of data abort. */
		পূর্ণসংখ্যা signum = SIGSEGV;

		যদি (is_align_fault(fs)) {
			signum = SIGBUS;
		} অন্যথা {
			printf("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} অন্যথা { /* is_nested */
		printf("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

স্থির শূন্য inkernel_disaster(কাঠামো proc *saved_proc,
	reg_t *saved_lr, কাঠামো ex_s *ep,
	পূর্ণসংখ্যা is_nested)
{
#যদি USE_SYSDEBUG
  যদি(ep)
	printf("\n%s\n", ep->বার্তা);

  printf("cpu %d is_nested = %d ", cpuid, is_nested);

  যদি (saved_proc) {
	  printf("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  printf("pc = 0x%x\n", (unsigned) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");

#endif /* USE_SYSDEBUG */
}

শূন্য exception_handler(পূর্ণসংখ্যা is_nested, reg_t *saved_lr, পূর্ণসংখ্যা vector)
{
/* An exception or unexpected interrupt has occurred. */
  কাঠামো ex_s *ep;
  কাঠামো proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_তথ্য[vector];

  প্রতিজ্ঞা((vir_bytes) saved_lr >= kinfo.vir_kern_start);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  যদি (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	যদি (((শূন্য*)*saved_lr >= (শূন্য*)copy_বার্তা_to_user &&
			(শূন্য*)*saved_lr <= (শূন্য*)__copy_msg_to_user_end) ||
			((শূন্য*)*saved_lr >= (শূন্য*)copy_বার্তা_from_user &&
			(শূন্য*)*saved_lr <= (শূন্য*)__copy_msg_from_user_end)) {
		নির্বাচন(vector) {
		/* these error are expected */
		ক্ষেত্র তথ্য_ABORT_VECTOR:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			ফেরত;
		পূর্বনির্ধারিত:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  যদি (vector == তথ্য_ABORT_VECTOR) {
	তথ্য_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	ফেরত;
  }

  যদি (!is_nested && vector == PREFETCH_ABORT_VECTOR) {
	স্থির পূর্ণসংখ্যা warned = FALSE;
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
	যদি (*saved_lr != ifar && !warned) {
		printf("KERNEL: prefetch abort with differing IFAR and LR\n");
		printf("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	ফেরত;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  যদি (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	ফেরত;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#যদি USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
স্থির শূন্য proc_stacktrace_execute(কাঠামো proc *whichproc, reg_t v_bp, reg_t pc)
{
	printf("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#endif

শূন্য proc_stacktrace(কাঠামো proc *whichproc)
{
#যদি USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#endif /* USE_SYSDEBUG */
}

শূন্য enable_fpu_exception(শূন্য)
{
}

শূন্য disable_fpu_exception(শূন্য)
{
}
