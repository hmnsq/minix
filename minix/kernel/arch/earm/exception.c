/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव <signal.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव "kernel/proc.h"
#अन्तर्भाव "kernel/proto.h"
#अन्तर्भाव <machine/vm.h>

संरचना ex_s {
	वर्ण *सन्देशः_2;
	पूर्णाङ्क signum;
};

स्थिर संरचना ex_s ex_दत्त[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL},
	{ "Data Abort", SIGSEGV},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

स्थिर शून्य inkernel_disaster(संरचना proc *saved_proc,
	reg_t *saved_lr, संरचना ex_s *ep, पूर्णाङ्क is_nested);

बाह्य पूर्णाङ्क catch_pagefaults;

स्थिर शून्य proc_stacktrace_execute(संरचना proc *whichproc, reg_t v_bp, reg_t pc);

स्थिर शून्य pagefault( संरचना proc *pr,
			reg_t *saved_lr,
			पूर्णाङ्क is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_स्थितिः)
{
	पूर्णाङ्क अन्तर्_physcopy = 0, अन्तर्_memset = 0;

	सन्देशः m_pagefault;
	पूर्णाङ्क err;

	अन्तर्_physcopy = (*saved_lr > (vir_अष्टकानि) phys_copy) &&
	   (*saved_lr < (vir_अष्टकानि) phys_copy_fault);

	अन्तर्_memset = (*saved_lr > (vir_अष्टकानि) phys_memset) &&
	   (*saved_lr < (vir_अष्टकानि) memset_fault);

	यदि((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (अन्तर्_physcopy || अन्तर्_memset)) {
		यदि (is_nested) {
			यदि(अन्तर्_physcopy) {
				प्रतिज्ञा(!अन्तर्_memset);
				*saved_lr = (reg_t) phys_copy_fault_अन्तर्_kernel;
			} अन्यथा {
				*saved_lr = (reg_t) memset_fault_अन्तर्_kernel;
			}
		}
		अन्यथा {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		प्रत्यागम;
	}

	यदि(is_nested) {
		प्रारूप_लिख("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, रिक्त, is_nested);
	}

	/* VM can't handle page faults. */
	यदि(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		प्रारूप_लिख("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_स्थितिः,
			is_nested);
		proc_stacktrace(pr);
		प्रारूप_लिख("pc of pagefault: 0x%lx\n", pr->p_reg.pc);
		panic("pagefault in VM");

		प्रत्यागम;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_समुच्चयः(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_स्थितिः;

	यदि ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	प्रत्यागम;
}

स्थिर शून्य
दत्त_abort(पूर्णाङ्क is_nested, संरचना proc *pr, reg_t *saved_lr,
		       संरचना ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	यदि (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} अन्यथा यदि (!is_nested) {
		/* A user process caused some other kind of data abort. */
		पूर्णाङ्क signum = SIGSEGV;

		यदि (is_align_fault(fs)) {
			signum = SIGBUS;
		} अन्यथा {
			प्रारूप_लिख("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} अन्यथा { /* is_nested */
		प्रारूप_लिख("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

स्थिर शून्य inkernel_disaster(संरचना proc *saved_proc,
	reg_t *saved_lr, संरचना ex_s *ep,
	पूर्णाङ्क is_nested)
{
#यदि USE_SYSDEBUG
  यदि(ep)
	प्रारूप_लिख("\n%s\n", ep->सन्देशः_2);

  प्रारूप_लिख("cpu %d is_nested = %d ", cpuid, is_nested);

  यदि (saved_proc) {
	  प्रारूप_लिख("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  प्रारूप_लिख("pc = 0x%x\n", (अचिह्नित) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");

#यदि_अन्तः /* USE_SYSDEBUG */
}

शून्य exception_handler(पूर्णाङ्क is_nested, reg_t *saved_lr, पूर्णाङ्क सदिशम्)
{
/* An exception or unexpected interrupt has occurred. */
  संरचना ex_s *ep;
  संरचना proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_दत्त[सदिशम्];

  प्रतिज्ञा((vir_अष्टकानि) saved_lr >= kinfo.vir_kern_start);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  यदि (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	यदि (((शून्य*)*saved_lr >= (शून्य*)copy_सन्देशः_गन्तव्यम्_user &&
			(शून्य*)*saved_lr <= (शून्य*)__copy_msg_to_user_end) ||
			((शून्य*)*saved_lr >= (शून्य*)copy_सन्देशः_from_user &&
			(शून्य*)*saved_lr <= (शून्य*)__copy_msg_from_user_end)) {
		विकल्प(सदिशम्) {
		/* these error are expected */
		पक्ष दत्त_ABORT_सदिशम्:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			प्रत्यागम;
		स्वभाव:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  यदि (सदिशम् == दत्त_ABORT_सदिशम्) {
	दत्त_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	प्रत्यागम;
  }

  यदि (!is_nested && सदिशम् == PREFETCH_ABORT_सदिशम्) {
	स्थिर पूर्णाङ्क warned = FALSE;
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
	यदि (*saved_lr != ifar && !warned) {
		प्रारूप_लिख("KERNEL: prefetch abort with differing IFAR and LR\n");
		प्रारूप_लिख("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	प्रत्यागम;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  यदि (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	प्रत्यागम;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#यदि USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
स्थिर शून्य proc_stacktrace_execute(संरचना proc *whichproc, reg_t v_bp, reg_t pc)
{
	प्रारूप_लिख("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#यदि_अन्तः

शून्य proc_stacktrace(संरचना proc *whichproc)
{
#यदि USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#यदि_अन्तः /* USE_SYSDEBUG */
}

शून्य enable_fpu_exception(शून्य)
{
}

शून्य disable_fpu_exception(शून्य)
{
}
