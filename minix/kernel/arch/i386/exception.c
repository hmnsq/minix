/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव <signal.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <machine/vm.h>

संरचना ex_s {
	वर्ण *संदेश;
	पूर्णाङ्क signum;
	पूर्णाङ्क minprocessor;
};

स्थिर संरचना ex_s ex_data[] = {
	{ "Divide error", संक_भ_त्रुटि, 86 },
	{ "Debug exception", SIGTRAP, 86 },
	{ "Nonmaskable interrupt", SIGBUS, 86 },
	{ "Breakpoint", SIGEMT, 86 },
	{ "Overflow", संक_भ_त्रुटि, 86 },
	{ "Bounds check", संक_भ_त्रुटि, 186 },
	{ "Invalid opcode", संक_अवैध, 186 },
	{ "Coprocessor not available", संक_भ_त्रुटि, 186 },
	{ "Double fault", SIGBUS, 286 },
	{ "Coprocessor segment overrun", संक_अंश, 286 },
	{ "Invalid TSS", संक_अंश, 286 },
	{ "Segment not present", संक_अंश, 286 },
	{ "Stack exception", संक_अंश, 286 },	/* STACK_FAULT already used */
	{ "General protection", संक_अंश, 286 },
	{ "Page fault", संक_अंश, 386 },		/* not close */
	{ रिक्त, संक_अवैध, 0 },			/* probably software trap */
	{ "Coprocessor error", संक_भ_त्रुटि, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", संक_भ_त्रुटि, 386 },
};

स्थिर शून्य inkernel_disaster(संरचना proc *saved_proc,
	संरचना exception_frame *frame, संरचना ex_s *ep, पूर्णाङ्क is_nested);

बाह्य पूर्णाङ्क catch_pagefaults;

स्थिर शून्य proc_stacktrace_execute(संरचना proc *whichproc, reg_t v_bp, reg_t pc);

स्थिर शून्य pagefault( संरचना proc *pr,
			संरचना exception_frame * frame,
			पूर्णाङ्क is_nested)
{
	पूर्णाङ्क in_physcopy = 0, in_memset = 0;

	reg_t pagefaultcr2;
	message m_pagefault;
	पूर्णाङ्क err;

	pagefaultcr2 = read_cr2();

#यदि 0
	म_लिखो("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#endif

	in_physcopy = (frame->eip > (vir_bytes) phys_copy) &&
	   (frame->eip < (vir_bytes) phys_copy_fault);

	in_memset = (frame->eip > (vir_bytes) phys_memset) &&
	   (frame->eip < (vir_bytes) memset_fault);

	यदि((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
#यदि 0
		म_लिखो("pf caught! addr 0x%lx\n", pagefaultcr2);
#endif
		यदि (is_nested) {
			यदि(in_physcopy) {
				प्रतिज्ञा(!in_memset);
				frame->eip = (reg_t) phys_copy_fault_in_kernel;
			} अन्यथा {
				frame->eip = (reg_t) memset_fault_in_kernel;
			}
		}
		अन्यथा {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		प्रत्यागम;
	}

	यदि(is_nested) {
		म_लिखो("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			frame->eip, pagefaultcr2);
		inkernel_disaster(pr, frame, रिक्त, is_nested);
	}

	/* VM can't handle page faults. */
	यदि(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		म_लिखो("pagefault for VM on CPU %d, "
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefaultcr2, frame->errcode,
			is_nested);
		proc_stacktrace(pr);
		म_लिखो("pc of pagefault: 0x%lx\n", frame->eip);
		panic("pagefault in VM");

		प्रत्यागम;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = frame->errcode;

	यदि ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	प्रत्यागम;
}

स्थिर शून्य inkernel_disaster(संरचना proc *saved_proc,
	संरचना exception_frame * frame, संरचना ex_s *ep,
	पूर्णाङ्क is_nested)
{
#यदि USE_SYSDEBUG
  यदि(ep) {
	यदि (ep->संदेश == रिक्त)
		म_लिखो("\nIntel-reserved exception %d\n", frame->vector);
	  अन्यथा
		म_लिखो("\n%s\n", ep->संदेश);
  }

  म_लिखो("cpu %d is_nested = %d ", cpuid, is_nested);

  म_लिखो("vec_nr= %d, trap_errno= 0x%x, eip= 0x%x, "
	"cs= 0x%x, eflags= 0x%x trap_esp 0x%08x\n",
	frame->vector, frame->errcode, frame->eip,
	frame->cs, frame->eflags, frame);
  म_लिखो("KERNEL registers :\n");
#define REG(n) (((u32_t *)frame)[-n])
  म_लिखो(
		  "\t%%eax 0x%08x %%ebx 0x%08x %%ecx 0x%08x %%edx 0x%08x\n"
		  "\t%%esp 0x%08x %%ebp 0x%08x %%esi 0x%08x %%edi 0x%08x\n",
		  REG(1), REG(2), REG(3), REG(4),
		  REG(5), REG(6), REG(7), REG(8));

  { 
  	reg_t k_ebp = REG(6);
  	म_लिखो("KERNEL stacktrace, starting with ebp = 0x%lx:\n", k_ebp);
  	proc_stacktrace_execute(proc_addr(SYSTEM), k_ebp, frame->eip);
  }

  यदि (saved_proc) {
	  म_लिखो("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name);
	  म_लिखो("pc = 0x%x\n", (अचिन्हित) saved_proc->p_reg.pc);
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");
#endif /* USE_SYSDEBUG */
}

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
शून्य exception_handler(पूर्णाङ्क is_nested, संरचना exception_frame * frame)
{
/* An exception or unexpected interrupt has occurred. */
  पञ्जिका संरचना ex_s *ep;
  संरचना proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_data[frame->vector];

  यदि (frame->vector == 2) {		/* spurious NMI on some machines */
	म_लिखो("got spurious NMI\n");
	प्रत्यागम;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  यदि (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	यदि (((शून्य*)frame->eip >= (शून्य*)copy_संदेश_to_user &&
			(शून्य*)frame->eip <= (शून्य*)__copy_msg_to_user_end) ||
			((शून्य*)frame->eip >= (शून्य*)copy_संदेश_from_user &&
			(शून्य*)frame->eip <= (शून्य*)__copy_msg_from_user_end)) {
		विकल्प(frame->vector) {
		/* these error are expected */
		पक्ष PAGE_FAULT_VECTOR:
		पक्ष PROTECTION_VECTOR:
			frame->eip = (reg_t) __user_copy_msg_pointer_failure;
			प्रत्यागम;
		स्वभाव:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	यदि (((शून्य*)frame->eip >= (शून्य*)fxrstor &&
			(शून्य *)frame->eip <= (शून्य*)__fxrstor_end) ||
			((शून्य*)frame->eip >= (शून्य*)frstor &&
			(शून्य *)frame->eip <= (शून्य*)__frstor_end)) {
		frame->eip = (reg_t) __frstor_failure;
		प्रत्यागम;
	}

  	यदि(frame->vector == DEBUG_VECTOR
		&& (saved_proc->p_reg.psw & TRACEBIT)
		&& (saved_proc->p_seg.p_kern_trap_style == KTS_NONE)) {
		/* Getting a debug trap in the kernel is legitimate
		 * if a traced process entered the kernel using sysenter
		 * or syscall; the trap flag is not cleared then.
		 *
		 * It triggers on the first kernel entry so the trap
		 * style is still KTS_NONE.
		 */

		frame->eflags &= ~TRACEBIT;

		प्रत्यागम;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  यदि(frame->vector == PAGE_FAULT_VECTOR) {
	pagefault(saved_proc, frame, is_nested);
	प्रत्यागम;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  यदि (is_nested == 0 && ! iskernelp(saved_proc)) {
#यदि 0
	{

  		म_लिखो(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			frame->vector, (अचिन्हित दीर्घ)frame->errcode,
			(अचिन्हित दीर्घ)frame->eip, frame->cs,
			(अचिन्हित दीर्घ)frame->eflags);
		proc_stacktrace(saved_proc);
	}

#endif
	cause_sig(proc_nr(saved_proc), ep->signum);
	प्रत्यागम;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, frame, ep, is_nested);

  panic("return from inkernel_disaster");
}

#यदि USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
स्थिर शून्य proc_stacktrace_execute(संरचना proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	पूर्णाङ्क iskernel;
	पूर्णाङ्क n = 0;

	iskernel = iskernelp(whichproc);

	म_लिखो("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	यावत्(v_bp) {
		reg_t v_pc;

#define PRCOPY(pr, pv, v, n) \
  (iskernel ? (स_नकल((वर्ण *) v, (वर्ण *) pv, n), OK) : \
     data_copy(pr->p_endpoint, pv, KERNEL, (vir_bytes) (v), n))

	        यदि(PRCOPY(whichproc, v_bp, &v_hbp, परिमाण(v_hbp)) != OK) {
			म_लिखो("(v_bp 0x%lx ?)", v_bp);
			भङ्ग;
		}
		यदि(PRCOPY(whichproc, v_bp + परिमाण(v_pc), &v_pc, परिमाण(v_pc)) != OK) {
			म_लिखो("(v_pc 0x%lx ?)", v_bp + परिमाण(v_pc));
			भङ्ग;
		}
		म_लिखो("0x%lx ", (अचिन्हित दीर्घ) v_pc);
		यदि(v_hbp != 0 && v_hbp <= v_bp) {
			म_लिखो("(hbp 0x%lx ?)", v_hbp);
			भङ्ग;
		}
		v_bp = v_hbp;
		यदि(n++ > 50) {
			म_लिखो("(truncated after %d steps) ", n);
			भङ्ग;
		}
	}
	म_लिखो("\n");
}
#endif /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
शून्य proc_stacktrace(संरचना proc *whichproc)
{
	u32_t use_bp;

	यदि(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		म_लिखो("WARNING: stacktrace of running process\n");
	}

	विकल्प(whichproc->p_seg.p_kern_trap_style) {
		पक्ष KTS_SYSENTER:
		पक्ष KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			यदि(data_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_bytes) &use_bp,
				परिमाण(use_bp)) != OK) {
				म_लिखो("stacktrace: aborting, copy failed\n");
				प्रत्यागम;
			}

			भङ्ग;
		}
		स्वभाव:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			भङ्ग;
	}

#यदि USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#endif /* USE_SYSDEBUG */
}

शून्य enable_fpu_exception(शून्य)
{
	u32_t cr0 = read_cr0();
	यदि(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

शून्य disable_fpu_exception(शून्य)
{
	clts();
}

