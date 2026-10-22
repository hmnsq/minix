/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ <signal.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ "kernel/proc.h"
#అన్తర్భావ "kernel/proto.h"
#అన్తర్భావ <machine/vm.h>

నిర్మాణం ex_s {
	అక్షరం *సందేశం_2;
	పూర్ణాంకం signum;
};

స్థిర నిర్మాణం ex_s ex_దత్తాంశం[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

స్థిర శూన్య inkernel_disaster(నిర్మాణం proc *saved_proc,
	reg_t *saved_lr, నిర్మాణం ex_s *ep, పూర్ణాంకం is_nested);

బాహ్య పూర్ణాంకం catch_pagefaults;

స్థిర శూన్య proc_stacktrace_execute(నిర్మాణం proc *whichproc, reg_t v_bp, reg_t pc);

స్థిర శూన్య pagefault( నిర్మాణం proc *pr,
			reg_t *saved_lr,
			పూర్ణాంకం is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_స్థితి)
{
	పూర్ణాంకం లోపల_physcopy = 0, లోపల_memset = 0;

	సందేశం m_pagefault;
	పూర్ణాంకం err;

	లోపల_physcopy = (*saved_lr > (vir_బైట్లు) phys_copy) &&
	   (*saved_lr < (vir_బైట్లు) phys_copy_fault);

	లోపల_memset = (*saved_lr > (vir_బైట్లు) phys_memset) &&
	   (*saved_lr < (vir_బైట్లు) memset_fault);

	ఒకవేళ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (లోపల_physcopy || లోపల_memset)) {
		ఒకవేళ (is_nested) {
			ఒకవేళ(లోపల_physcopy) {
				ప్రతిజ్ఞా(!లోపల_memset);
				*saved_lr = (reg_t) phys_copy_fault_లోపల_kernel;
			} లేకపోతే {
				*saved_lr = (reg_t) memset_fault_లోపల_kernel;
			}
		}
		లేకపోతే {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		తిరిగి;
	}

	ఒకవేళ(is_nested) {
		రాయి("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, ఖాళీ, is_nested);
	}

	/* VM can't handle page faults. */
	ఒకవేళ(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		రాయి("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_స్థితి,
			is_nested);
		proc_stacktrace(pr);
		రాయి("pc of pagefault: 0x%lx\n", pr->p_reg.pc);
		panic("pagefault in VM");

		తిరిగి;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_సమితి(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ఆధారం = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_స్థితి;

	ఒకవేళ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	తిరిగి;
}

స్థిర శూన్య
దత్తాంశం_abort(పూర్ణాంకం is_nested, నిర్మాణం proc *pr, reg_t *saved_lr,
		       నిర్మాణం ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	ఒకవేళ (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} లేకపోతే ఒకవేళ (!is_nested) {
		/* A user process caused some other kind of data abort. */
		పూర్ణాంకం signum = SIGSEGV;

		ఒకవేళ (is_align_fault(fs)) {
			signum = SIGBUS;
		} లేకపోతే {
			రాయి("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} లేకపోతే { /* is_nested */
		రాయి("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

స్థిర శూన్య inkernel_disaster(నిర్మాణం proc *saved_proc,
	reg_t *saved_lr, నిర్మాణం ex_s *ep,
	పూర్ణాంకం is_nested)
{
#ఒకవేళ USE_SYSDEBUG
  ఒకవేళ(ep)
	రాయి("\n%s\n", ep->సందేశం_2);

  రాయి("cpu %d is_nested = %d ", cpuid, is_nested);

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

శూన్య exception_handler(పూర్ణాంకం is_nested, reg_t *saved_lr, పూర్ణాంకం సదిశ)
{
/* An exception or unexpected interrupt has occurred. */
  నిర్మాణం ex_s *ep;
  నిర్మాణం proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_దత్తాంశం[సదిశ];

  ప్రతిజ్ఞా((vir_బైట్లు) saved_lr >= kinfo.vir_kern_ప్రారంభం);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  ఒకవేళ (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	ఒకవేళ (((శూన్య*)*saved_lr >= (శూన్య*)copy_సందేశం_గమ్యం_user &&
			(శూన్య*)*saved_lr <= (శూన్య*)__copy_msg_to_user_end) ||
			((శూన్య*)*saved_lr >= (శూన్య*)copy_సందేశం_from_user &&
			(శూన్య*)*saved_lr <= (శూన్య*)__copy_msg_from_user_end)) {
		ఎంపిక(సదిశ) {
		/* these error are expected */
		స్థితి దత్తాంశం_ABORT_సదిశ:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			తిరిగి;
		అప్రమేయ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  ఒకవేళ (సదిశ == దత్తాంశం_ABORT_సదిశ) {
	దత్తాంశం_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	తిరిగి;
  }

  ఒకవేళ (!is_nested && సదిశ == PREFETCH_ABORT_సదిశ) {
	స్థిర పూర్ణాంకం warned = FALSE;
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
	ఒకవేళ (*saved_lr != ifar && !warned) {
		రాయి("KERNEL: prefetch abort with differing IFAR and LR\n");
		రాయి("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	తిరిగి;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  ఒకవేళ (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	తిరిగి;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#ఒకవేళ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
స్థిర శూన్య proc_stacktrace_execute(నిర్మాణం proc *whichproc, reg_t v_bp, reg_t pc)
{
	రాయి("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#అయితే_ముగింపు

శూన్య proc_stacktrace(నిర్మాణం proc *whichproc)
{
#ఒకవేళ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#అయితే_ముగింపు /* USE_SYSDEBUG */
}

శూన్య enable_fpu_exception(శూన్య)
{
}

శూన్య disable_fpu_exception(శూన్య)
{
}
