/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"
#समावेश <signal.h>
#समावेश <string.h>
#समावेश <assert.h>
#समावेश "kernel/proc.h"
#समावेश "kernel/proto.h"
#समावेश <machine/vm.h>

काष्ठा ex_s {
	अक्षर *संदेश;
	पूर्णांक signum;
};

अटल काष्ठा ex_s ex_तथ्य[] = {
	{ "Reset", 0},
	{ "Undefined instruction", संक_अवैध},
	{ "Supervisor call", 0},
	{ "Prefetch Abort", संक_अवैध},
	{ "Data Abort", संक_अंश},
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

अटल व्योम inkernel_disaster(काष्ठा proc *saved_proc,
	reg_t *saved_lr, काष्ठा ex_s *ep, पूर्णांक is_nested);

बाह्य पूर्णांक catch_pagefaults;

अटल व्योम proc_stacktrace_execute(काष्ठा proc *whichproc, reg_t v_bp, reg_t pc);

अटल व्योम pagefault( काष्ठा proc *pr,
			reg_t *saved_lr,
			पूर्णांक is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_status)
{
	पूर्णांक in_physcopy = 0, in_memset = 0;

	message m_pagefault;
	पूर्णांक err;

	in_physcopy = (*saved_lr > (vir_bytes) phys_copy) &&
	   (*saved_lr < (vir_bytes) phys_copy_fault);

	in_memset = (*saved_lr > (vir_bytes) phys_memset) &&
	   (*saved_lr < (vir_bytes) memset_fault);

	अगर((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
		अगर (is_nested) {
			अगर(in_physcopy) {
				निश्चित(!in_memset);
				*saved_lr = (reg_t) phys_copy_fault_in_kernel;
			} अन्यथा {
				*saved_lr = (reg_t) memset_fault_in_kernel;
			}
		}
		अन्यथा {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		वापस;
	}

	अगर(is_nested) {
		म_लिखो("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, शून्य, is_nested);
	}

	/* VM can't handle page faults. */
	अगर(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		म_लिखो("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_status,
			is_nested);
		proc_stacktrace(pr);
		म_लिखो("pc of pagefault: 0x%lx\n", pr->p_reg.pc);
		panic("pagefault in VM");

		वापस;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_status;

	अगर ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	वापस;
}

अटल व्योम
तथ्य_abort(पूर्णांक is_nested, काष्ठा proc *pr, reg_t *saved_lr,
		       काष्ठा ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	अगर (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} अन्यथा अगर (!is_nested) {
		/* A user process caused some other kind of data abort. */
		पूर्णांक signum = संक_अंश;

		अगर (is_align_fault(fs)) {
			signum = SIGBUS;
		} अन्यथा {
			म_लिखो("KERNEL: unknown data abort by proc %d sending "
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} अन्यथा { /* is_nested */
		म_लिखो("KERNEL: inkernel data abort - disaster (dfar=0x%lx "
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

अटल व्योम inkernel_disaster(काष्ठा proc *saved_proc,
	reg_t *saved_lr, काष्ठा ex_s *ep,
	पूर्णांक is_nested)
{
#अगर USE_SYSDEBUG
  अगर(ep)
	म_लिखो("\n%s\n", ep->संदेश);

  म_लिखो("cpu %d is_nested = %d ", cpuid, is_nested);

  अगर (saved_proc) {
	  म_लिखो("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  म_लिखो("pc = 0x%x\n", (अचिन्हित) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");

#पूर्ण_अगर /* USE_SYSDEBUG */
}

व्योम exception_handler(पूर्णांक is_nested, reg_t *saved_lr, पूर्णांक vector)
{
/* An exception or unexpected interrupt has occurred. */
  काष्ठा ex_s *ep;
  काष्ठा proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_तथ्य[vector];

  निश्चित((vir_bytes) saved_lr >= kinfo.vir_kern_start);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  अगर (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	अगर (((व्योम*)*saved_lr >= (व्योम*)copy_संदेश_to_user &&
			(व्योम*)*saved_lr <= (व्योम*)__copy_msg_to_user_end) ||
			((व्योम*)*saved_lr >= (व्योम*)copy_संदेश_from_user &&
			(व्योम*)*saved_lr <= (व्योम*)__copy_msg_from_user_end)) {
		चयन(vector) {
		/* these error are expected */
		हाल तथ्य_ABORT_VECTOR:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			वापस;
		शेष:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  अगर (vector == तथ्य_ABORT_VECTOR) {
	तथ्य_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	वापस;
  }

  अगर (!is_nested && vector == PREFETCH_ABORT_VECTOR) {
	अटल पूर्णांक warned = FALSE;
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
	अगर (*saved_lr != ifar && !warned) {
		म_लिखो("KERNEL: prefetch abort with differing IFAR and LR\n");
		म_लिखो("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in "
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	वापस;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  अगर (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	वापस;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#अगर USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
अटल व्योम proc_stacktrace_execute(काष्ठा proc *whichproc, reg_t v_bp, reg_t pc)
{
	म_लिखो("%-8.8s %6d 0x%lx \n",
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#पूर्ण_अगर

व्योम proc_stacktrace(काष्ठा proc *whichproc)
{
#अगर USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#पूर्ण_अगर /* USE_SYSDEBUG */
}

व्योम enable_fpu_exception(व्योम)
{
}

व्योम disable_fpu_exception(व्योम)
{
}
