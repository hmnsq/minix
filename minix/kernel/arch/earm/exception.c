/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_proto.h"
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ "kernel/proc.h"
#അന്തര്ഭാവ "kernel/proto.h"
#അന്തര്ഭാവ <machine/vm.h>

ഘടന ex_s {
	അക്ഷരം *സന്ദേശം;
	പൂർണ്ണസംഖ്യ signum;
};

സ്ഥിതം ഘടന ex_s ex_data[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

സ്ഥിതം ശൂന്യം inkernel_disaster(ഘടന proc *saved_proc,
	reg_t *saved_lr, ഘടന ex_s *ep, പൂർണ്ണസംഖ്യ is_nested);

ബാഹ്യ പൂർണ്ണസംഖ്യ catch_pagefaults;

സ്ഥിതം ശൂന്യം proc_stacktrace_execute(ഘടന proc *whichproc, reg_t v_bp, reg_t pc);

സ്ഥിതം ശൂന്യം pagefault( ഘടന proc *pr,
			reg_t *saved_lr,
			പൂർണ്ണസംഖ്യ is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_status)
{
	പൂർണ്ണസംഖ്യ in_physcopy = 0, in_memset = 0;

	message m_pagefault;
	പൂർണ്ണസംഖ്യ err;

	in_physcopy = (*saved_lr > (vir_bytes) phys_copy) &&
	   (*saved_lr < (vir_bytes) phys_copy_fault);

	in_memset = (*saved_lr > (vir_bytes) phys_memset) &&
	   (*saved_lr < (vir_bytes) memset_fault);

	എങ്കിൽ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
		എങ്കിൽ (is_nested) {
			എങ്കിൽ(in_physcopy) {
				പ്രതിജ്ഞാ(!in_memset);
				*saved_lr = (reg_t) phys_copy_fault_in_kernel;
			} അല്ലെങ്കിൽ {
				*saved_lr = (reg_t) memset_fault_in_kernel;
			}
		}
		അല്ലെങ്കിൽ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		തിരികെ;
	}

	എങ്കിൽ(is_nested) {
		printf("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, ശൂന്യ, is_nested);
	}

	/* VM can't handle page faults. */
	എങ്കിൽ(pr->p_endpoint == VM_PROC_NR) {
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

		തിരികെ;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_status;

	എങ്കിൽ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	തിരികെ;
}

സ്ഥിതം ശൂന്യം
data_abort(പൂർണ്ണസംഖ്യ is_nested, ഘടന proc *pr, reg_t *saved_lr,
		       ഘടന ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	എങ്കിൽ (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} അല്ലെങ്കിൽ എങ്കിൽ (!is_nested) {
		/* A user process caused some other kind of data abort. */
		പൂർണ്ണസംഖ്യ signum = SIGSEGV;

		എങ്കിൽ (is_align_fault(fs)) {
			signum = SIGBUS;
		} അല്ലെങ്കിൽ {
			printf("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} അല്ലെങ്കിൽ { /* is_nested */
		printf("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

സ്ഥിതം ശൂന്യം inkernel_disaster(ഘടന proc *saved_proc,
	reg_t *saved_lr, ഘടന ex_s *ep,
	പൂർണ്ണസംഖ്യ is_nested)
{
#എങ്കിൽ USE_SYSDEBUG
  എങ്കിൽ(ep)
	printf("\n%s\n", ep->സന്ദേശം);

  printf("cpu %d is_nested = %d ", cpuid, is_nested);

  എങ്കിൽ (saved_proc) {
	  printf("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  printf("pc = 0x%x\n", (unsigned) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");

#endif /* USE_SYSDEBUG */
}

ശൂന്യം exception_handler(പൂർണ്ണസംഖ്യ is_nested, reg_t *saved_lr, പൂർണ്ണസംഖ്യ vector)
{
/* An exception or unexpected interrupt has occurred. */
  ഘടന ex_s *ep;
  ഘടന proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_data[vector];

  പ്രതിജ്ഞാ((vir_bytes) saved_lr >= kinfo.vir_kern_start);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  എങ്കിൽ (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	എങ്കിൽ (((ശൂന്യം*)*saved_lr >= (ശൂന്യം*)copy_സന്ദേശം_to_user &&
			(ശൂന്യം*)*saved_lr <= (ശൂന്യം*)__copy_msg_to_user_end) ||
			((ശൂന്യം*)*saved_lr >= (ശൂന്യം*)copy_സന്ദേശം_from_user &&
			(ശൂന്യം*)*saved_lr <= (ശൂന്യം*)__copy_msg_from_user_end)) {
		തിരഞ്ഞെടുപ്പ്(vector) {
		/* these error are expected */
		അവസ്ഥ DATA_ABORT_VECTOR:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			തിരികെ;
		സ്ഥിരസ്ഥിതി:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  എങ്കിൽ (vector == DATA_ABORT_VECTOR) {
	data_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	തിരികെ;
  }

  എങ്കിൽ (!is_nested && vector == PREFETCH_ABORT_VECTOR) {
	സ്ഥിതം പൂർണ്ണസംഖ്യ warned = FALSE;
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
	എങ്കിൽ (*saved_lr != ifar && !warned) {
		printf("KERNEL: prefetch abort with differing IFAR and LR\n");
		printf("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	തിരികെ;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  എങ്കിൽ (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	തിരികെ;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#എങ്കിൽ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
സ്ഥിതം ശൂന്യം proc_stacktrace_execute(ഘടന proc *whichproc, reg_t v_bp, reg_t pc)
{
	printf("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#endif

ശൂന്യം proc_stacktrace(ഘടന proc *whichproc)
{
#എങ്കിൽ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#endif /* USE_SYSDEBUG */
}

ശൂന്യം enable_fpu_exception(ശൂന്യം)
{
}

ശൂന്യം disable_fpu_exception(ശൂന്യം)
{
}
