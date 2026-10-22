/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ "kernel/proc.h"
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"
#ಅನ್ತರ್ಭಾವ <machine/vm.h>

ರಚನೆ ex_s {
	ಅಕ್ಷರ *ಸಂದೇಶ_2;
	ಪೂರ್ಣಾಂಕ signum;
};

ಸ್ಥಿರ ರಚನೆ ex_s ex_ದತ್ತಾಂಶ[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

ಸ್ಥಿರ ಶೂನ್ಯ inkernel_disaster(ರಚನೆ proc *saved_proc,
	reg_t *saved_lr, ರಚನೆ ex_s *ep, ಪೂರ್ಣಾಂಕ is_nested);

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ catch_pagefaults;

ಸ್ಥಿರ ಶೂನ್ಯ proc_stacktrace_execute(ರಚನೆ proc *whichproc, reg_t v_bp, reg_t pc);

ಸ್ಥಿರ ಶೂನ್ಯ pagefault( ರಚನೆ proc *pr,
			reg_t *saved_lr,
			ಪೂರ್ಣಾಂಕ is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_ಸ್ಥಿತಿ)
{
	ಪೂರ್ಣಾಂಕ ಒಳ_physcopy = 0, ಒಳ_memset = 0;

	ಸಂದೇಶ m_pagefault;
	ಪೂರ್ಣಾಂಕ err;

	ಒಳ_physcopy = (*saved_lr > (vir_ಬೈಟುಗಳು) phys_copy) &&
	   (*saved_lr < (vir_ಬೈಟುಗಳು) phys_copy_fault);

	ಒಳ_memset = (*saved_lr > (vir_ಬೈಟುಗಳು) phys_memset) &&
	   (*saved_lr < (vir_ಬೈಟುಗಳು) memset_fault);

	ಆದರೆ((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (ಒಳ_physcopy || ಒಳ_memset)) {
		ಆದರೆ (is_nested) {
			ಆದರೆ(ಒಳ_physcopy) {
				ಪ್ರತಿಜ್ಞಾ(!ಒಳ_memset);
				*saved_lr = (reg_t) phys_copy_fault_ಒಳ_kernel;
			} ಇಲ್ಲದಿದ್ದರೆ {
				*saved_lr = (reg_t) memset_fault_ಒಳ_kernel;
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ(is_nested) {
		ಬರೆ("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, ಖಾಲಿ, is_nested);
	}

	/* VM can't handle page faults. */
	ಆದರೆ(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		ಬರೆ("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_ಸ್ಥಿತಿ,
			is_nested);
		proc_stacktrace(pr);
		ಬರೆ("pc of pagefault: 0x%lx\n", pr->p_reg.pc);
		panic("pagefault in VM");

		ಹಿಂತಿರುಗಿ;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_ಗಣ(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_ಮೂಲ = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_ಸ್ಥಿತಿ;

	ಆದರೆ ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ
ದತ್ತಾಂಶ_abort(ಪೂರ್ಣಾಂಕ is_nested, ರಚನೆ proc *pr, reg_t *saved_lr,
		       ರಚನೆ ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	ಆದರೆ (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_nested) {
		/* A user process caused some other kind of data abort. */
		ಪೂರ್ಣಾಂಕ signum = SIGSEGV;

		ಆದರೆ (is_align_fault(fs)) {
			signum = SIGBUS;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಬರೆ("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} ಇಲ್ಲದಿದ್ದರೆ { /* is_nested */
		ಬರೆ("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ inkernel_disaster(ರಚನೆ proc *saved_proc,
	reg_t *saved_lr, ರಚನೆ ex_s *ep,
	ಪೂರ್ಣಾಂಕ is_nested)
{
#ಆದರೆ USE_SYSDEBUG
  ಆದರೆ(ep)
	ಬರೆ("\n%s\n", ep->ಸಂದೇಶ_2);

  ಬರೆ("cpu %d is_nested = %d ", cpuid, is_nested);

  ಆದರೆ (saved_proc) {
	  ಬರೆ("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  ಬರೆ("pc = 0x%x\n", (ಚಿಹ್ನೆರಹಿತ) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");

#ಆದರೆ_ಅಂತ್ಯ /* USE_SYSDEBUG */
}

ಶೂನ್ಯ exception_handler(ಪೂರ್ಣಾಂಕ is_nested, reg_t *saved_lr, ಪೂರ್ಣಾಂಕ ಸದಿಶ)
{
/* An exception or unexpected interrupt has occurred. */
  ರಚನೆ ex_s *ep;
  ರಚನೆ proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_ದತ್ತಾಂಶ[ಸದಿಶ];

  ಪ್ರತಿಜ್ಞಾ((vir_ಬೈಟುಗಳು) saved_lr >= kinfo.vir_kern_ಆರಂಭ);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  ಆದರೆ (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	ಆದರೆ (((ಶೂನ್ಯ*)*saved_lr >= (ಶೂನ್ಯ*)copy_ಸಂದೇಶ_ಗಮ್ಯ_user &&
			(ಶೂನ್ಯ*)*saved_lr <= (ಶೂನ್ಯ*)__copy_msg_to_user_end) ||
			((ಶೂನ್ಯ*)*saved_lr >= (ಶೂನ್ಯ*)copy_ಸಂದೇಶ_from_user &&
			(ಶೂನ್ಯ*)*saved_lr <= (ಶೂನ್ಯ*)__copy_msg_from_user_end)) {
		ಆಯ್ಕೆ(ಸದಿಶ) {
		/* these error are expected */
		ಸ್ಥಿತಿ ದತ್ತಾಂಶ_ABORT_ಸದಿಶ:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			ಹಿಂತಿರುಗಿ;
		ಪೂರ್ವನಿಯೋಜಿತ:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  ಆದರೆ (ಸದಿಶ == ದತ್ತಾಂಶ_ABORT_ಸದಿಶ) {
	ದತ್ತಾಂಶ_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ (!is_nested && ಸದಿಶ == PREFETCH_ABORT_ಸದಿಶ) {
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ warned = FALSE;
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
	ಆದರೆ (*saved_lr != ifar && !warned) {
		ಬರೆ("KERNEL: prefetch abort with differing IFAR and LR\n");
		ಬರೆ("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	ಹಿಂತಿರುಗಿ;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  ಆದರೆ (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	ಹಿಂತಿರುಗಿ;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#ಆದರೆ USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ proc_stacktrace_execute(ರಚನೆ proc *whichproc, reg_t v_bp, reg_t pc)
{
	ಬರೆ("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ proc_stacktrace(ರಚನೆ proc *whichproc)
{
#ಆದರೆ USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#ಆದರೆ_ಅಂತ್ಯ /* USE_SYSDEBUG */
}

ಶೂನ್ಯ enable_fpu_exception(ಶೂನ್ಯ)
{
}

ಶೂನ್ಯ disable_fpu_exception(ಶೂನ್ಯ)
{
}
