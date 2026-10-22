/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ "kernel/proc.h"
#અન્તર્ભાવ "kernel/proto.h"
#અન્તર્ભાવ <machine/vm.h>

સંરચના ex_s {
	અક્ષર *સંદેશ;
	પૂર્ણાંક signum;
};

સ્થિર સંરચના ex_s ex_data[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

સ્થિર શૂન્ય inkernel_disaster(સંરચના proc *saved_proc,
	reg_t *saved_lr, સંરચના ex_s *ep, પૂર્ણાંક is_nested);

બાહ્ય પૂર્ણાંક catch_pagefaults;

સ્થિર શૂન્ય proc_stacktrace_execute(સંરચના proc *whichproc, reg_t v_bp, reg_t pc);

સ્થિર શૂન્ય pagefault( સંરચના proc *pr,
			reg_t *saved_lr,
			પૂર્ણાંક is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_status)
{
	પૂર્ણાંક in_physcopy = 0, in_memset = 0;

	message m_pagefault;
	પૂર્ણાંક err;

	in_physcopy = (*saved_lr > (vir_bytes) phys_copy) &&
	   (*saved_lr < (vir_bytes) phys_copy_fault);

	in_memset = (*saved_lr > (vir_bytes) phys_memset) &&
	   (*saved_lr < (vir_bytes) memset_fault);

	જો((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
		જો (is_nested) {
			જો(in_physcopy) {
				પ્રતિજ્ઞા(!in_memset);
				*saved_lr = (reg_t) phys_copy_fault_in_kernel;
			} નહીંતર {
				*saved_lr = (reg_t) memset_fault_in_kernel;
			}
		}
		નહીંતર {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		પાછા;
	}

	જો(is_nested) {
		printf("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, ખાલી, is_nested);
	}

	/* VM can't handle page faults. */
	જો(pr->p_endpoint == VM_PROC_NR) {
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

		પાછા;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_status;

	જો ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	પાછા;
}

સ્થિર શૂન્ય
data_abort(પૂર્ણાંક is_nested, સંરચના proc *pr, reg_t *saved_lr,
		       સંરચના ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	જો (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} નહીંતર જો (!is_nested) {
		/* A user process caused some other kind of data abort. */
		પૂર્ણાંક signum = SIGSEGV;

		જો (is_align_fault(fs)) {
			signum = SIGBUS;
		} નહીંતર {
			printf("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} નહીંતર { /* is_nested */
		printf("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

સ્થિર શૂન્ય inkernel_disaster(સંરચના proc *saved_proc,
	reg_t *saved_lr, સંરચના ex_s *ep,
	પૂર્ણાંક is_nested)
{
#જો USE_SYSDEBUG
  જો(ep)
	printf("\n%s\n", ep->સંદેશ);

  printf("cpu %d is_nested = %d ", cpuid, is_nested);

  જો (saved_proc) {
	  printf("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  printf("pc = 0x%x\n", (unsigned) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");

#endif /* USE_SYSDEBUG */
}

શૂન્ય exception_handler(પૂર્ણાંક is_nested, reg_t *saved_lr, પૂર્ણાંક vector)
{
/* An exception or unexpected interrupt has occurred. */
  સંરચના ex_s *ep;
  સંરચના proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_data[vector];

  પ્રતિજ્ઞા((vir_bytes) saved_lr >= kinfo.vir_kern_start);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  જો (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	જો (((શૂન્ય*)*saved_lr >= (શૂન્ય*)copy_સંદેશ_to_user &&
			(શૂન્ય*)*saved_lr <= (શૂન્ય*)__copy_msg_to_user_end) ||
			((શૂન્ય*)*saved_lr >= (શૂન્ય*)copy_સંદેશ_from_user &&
			(શૂન્ય*)*saved_lr <= (શૂન્ય*)__copy_msg_from_user_end)) {
		પસંદગી(vector) {
		/* these error are expected */
		સ્થિતિ DATA_ABORT_VECTOR:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			પાછા;
		મૂળભૂત:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  જો (vector == DATA_ABORT_VECTOR) {
	data_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	પાછા;
  }

  જો (!is_nested && vector == PREFETCH_ABORT_VECTOR) {
	સ્થિર પૂર્ણાંક warned = FALSE;
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
	જો (*saved_lr != ifar && !warned) {
		printf("KERNEL: prefetch abort with differing IFAR and LR\n");
		printf("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	પાછા;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  જો (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	પાછા;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#જો USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
સ્થિર શૂન્ય proc_stacktrace_execute(સંરચના proc *whichproc, reg_t v_bp, reg_t pc)
{
	printf("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#endif

શૂન્ય proc_stacktrace(સંરચના proc *whichproc)
{
#જો USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#endif /* USE_SYSDEBUG */
}

શૂન્ય enable_fpu_exception(શૂન્ય)
{
}

શૂન્ય disable_fpu_exception(શૂન્ય)
{
}
