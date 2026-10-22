/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"
#समावेश <signal.h>
#समावेश <string.h>
#समावेश <assert.h>
#समावेश <machine/vm.h>

काष्ठा ex_s {
	अक्षर *संदेश;
	पूर्णांक signum;
	पूर्णांक minprocessor;
};

अटल काष्ठा ex_s ex_तथ्य[] = {
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
	{ शून्य, संक_अवैध, 0 },			/* probably software trap */
	{ "Coprocessor error", संक_भ_त्रुटि, 386 },
	{ "Alignment check", SIGBUS, 386 },
	{ "Machine check", SIGBUS, 386 },
	{ "SIMD exception", संक_भ_त्रुटि, 386 },
};

अटल व्योम inkernel_disaster(काष्ठा proc *saved_proc,
	काष्ठा exception_frame *frame, काष्ठा ex_s *ep, पूर्णांक is_nested);

बाह्य पूर्णांक catch_pagefaults;

अटल व्योम proc_stacktrace_execute(काष्ठा proc *whichproc, reg_t v_bp, reg_t pc);

अटल व्योम pagefault( काष्ठा proc *pr,
			काष्ठा exception_frame * frame,
			पूर्णांक is_nested)
{
	पूर्णांक in_physcopy = 0, in_memset = 0;

	reg_t pagefaultcr2;
	message m_pagefault;
	पूर्णांक err;

	pagefaultcr2 = read_cr2();

#अगर 0
	म_लिखो("kernel: pagefault in pr %d, addr 0x%lx, his cr3 0x%lx, actual cr3 0x%lx\n",
		pr->p_endpoint, pagefaultcr2, pr->p_seg.p_cr3, read_cr3());
#पूर्ण_अगर

	in_physcopy = (frame->eip > (vir_bytes) phys_copy) &&
	   (frame->eip < (vir_bytes) phys_copy_fault);

	in_memset = (frame->eip > (vir_bytes) phys_memset) &&
	   (frame->eip < (vir_bytes) memset_fault);

	अगर((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
#अगर 0
		म_लिखो("pf caught! addr 0x%lx\n", pagefaultcr2);
#पूर्ण_अगर
		अगर (is_nested) {
			अगर(in_physcopy) {
				निश्चित(!in_memset);
				frame->eip = (reg_t) phys_copy_fault_in_kernel;
			} अन्यथा {
				frame->eip = (reg_t) memset_fault_in_kernel;
			}
		}
		अन्यथा {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefaultcr2;
		}
	
		वापस;
	}

	अगर(is_nested) {
		म_लिखो("pagefault in kernel at pc 0x%lx address 0x%lx\n",
			frame->eip, pagefaultcr2);
		inkernel_disaster(pr, frame, शून्य, is_nested);
	}

	/* VM can't handle page faults. */
	अगर(pr->p_endpoint == VM_PROC_NR) {
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

		वापस;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefaultcr2;
	m_pagefault.VPF_FLAGS = frame->errcode;

	अगर ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	वापस;
}

अटल व्योम inkernel_disaster(काष्ठा proc *saved_proc,
	काष्ठा exception_frame * frame, काष्ठा ex_s *ep,
	पूर्णांक is_nested)
{
#अगर USE_SYSDEBUG
  अगर(ep) {
	अगर (ep->संदेश == शून्य)
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
#घोषणा REG(n) (((u32_t *)frame)[-n])
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

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
व्योम exception_handler(पूर्णांक is_nested, काष्ठा exception_frame * frame)
{
/* An exception or unexpected interrupt has occurred. */
  रेजिस्टर काष्ठा ex_s *ep;
  काष्ठा proc *saved_proc;

  /* Save proc_ptr, because it may be changed by debug statements. */
  saved_proc = get_cpulocal_var(proc_ptr);
  
  ep = &ex_तथ्य[frame->vector];

  अगर (frame->vector == 2) {		/* spurious NMI on some machines */
	म_लिखो("got spurious NMI\n");
	वापस;
  }

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  अगर (is_nested) {
	/*
	 * if a problem occured while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	अगर (((व्योम*)frame->eip >= (व्योम*)copy_संदेश_to_user &&
			(व्योम*)frame->eip <= (व्योम*)__copy_msg_to_user_end) ||
			((व्योम*)frame->eip >= (व्योम*)copy_संदेश_from_user &&
			(व्योम*)frame->eip <= (व्योम*)__copy_msg_from_user_end)) {
		चयन(frame->vector) {
		/* these error are expected */
		हाल PAGE_FAULT_VECTOR:
		हाल PROTECTION_VECTOR:
			frame->eip = (reg_t) __user_copy_msg_pointer_failure;
			वापस;
		शेष:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}

	/* Pass any error resulting from restoring FPU state, as a FPU
	 * exception to the process.
	 */
	अगर (((व्योम*)frame->eip >= (व्योम*)fxrstor &&
			(व्योम *)frame->eip <= (व्योम*)__fxrstor_end) ||
			((व्योम*)frame->eip >= (व्योम*)frstor &&
			(व्योम *)frame->eip <= (व्योम*)__frstor_end)) {
		frame->eip = (reg_t) __frstor_failure;
		वापस;
	}

  	अगर(frame->vector == DEBUG_VECTOR
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

		वापस;

		/* If control passes, this case is not recognized as legitimate
		 * and we panic later on after all.
		 */
	}
  }

  अगर(frame->vector == PAGE_FAULT_VECTOR) {
	pagefault(saved_proc, frame, is_nested);
	वापस;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  अगर (is_nested == 0 && ! iskernelp(saved_proc)) {
#अगर 0
	{

  		म_लिखो(
  "vec_nr= %d, trap_errno= 0x%lx, eip= 0x%lx, cs= 0x%x, eflags= 0x%lx\n",
			frame->vector, (अचिन्हित दीर्घ)frame->errcode,
			(अचिन्हित दीर्घ)frame->eip, frame->cs,
			(अचिन्हित दीर्घ)frame->eflags);
		proc_stacktrace(saved_proc);
	}

#पूर्ण_अगर
	cause_sig(proc_nr(saved_proc), ep->signum);
	वापस;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, frame, ep, is_nested);

  panic("return from inkernel_disaster");
}

#अगर USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
अटल व्योम proc_stacktrace_execute(काष्ठा proc *whichproc, reg_t v_bp, reg_t pc)
{
	reg_t v_hbp;
	पूर्णांक iskernel;
	पूर्णांक n = 0;

	iskernel = iskernelp(whichproc);

	म_लिखो("%-8.8s %6d 0x%lx ",
		whichproc->p_name, whichproc->p_endpoint, pc);

	जबतक(v_bp) {
		reg_t v_pc;

#घोषणा PRCOPY(pr, pv, v, n) \
  (iskernel ? (स_नकल((अक्षर *) v, (अक्षर *) pv, n), OK) : \
     तथ्य_copy(pr->p_endpoint, pv, KERNEL, (vir_bytes) (v), n))

	        अगर(PRCOPY(whichproc, v_bp, &v_hbp, माप(v_hbp)) != OK) {
			म_लिखो("(v_bp 0x%lx ?)", v_bp);
			अवरोध;
		}
		अगर(PRCOPY(whichproc, v_bp + माप(v_pc), &v_pc, माप(v_pc)) != OK) {
			म_लिखो("(v_pc 0x%lx ?)", v_bp + माप(v_pc));
			अवरोध;
		}
		म_लिखो("0x%lx ", (अचिन्हित दीर्घ) v_pc);
		अगर(v_hbp != 0 && v_hbp <= v_bp) {
			म_लिखो("(hbp 0x%lx ?)", v_hbp);
			अवरोध;
		}
		v_bp = v_hbp;
		अगर(n++ > 50) {
			म_लिखो("(truncated after %d steps) ", n);
			अवरोध;
		}
	}
	म_लिखो("\n");
}
#पूर्ण_अगर /* USE_SYSDEBUG */

/*===========================================================================*
 *				proc_stacktrace			     *
 *===========================================================================*/
व्योम proc_stacktrace(काष्ठा proc *whichproc)
{
	u32_t use_bp;

	अगर(whichproc->p_seg.p_kern_trap_style == KTS_NONE) {
		म_लिखो("WARNING: stacktrace of running process\n");
	}

	चयन(whichproc->p_seg.p_kern_trap_style) {
		हाल KTS_SYSENTER:
		हाल KTS_SYSCALL:
		{
			u32_t sp = whichproc->p_reg.sp;

			/* Full context is not available in the p_reg
			 * struct. Obtain it from the user's stack.
			 * The use stack pointer is always available.
			 * The fact that it's there, and the 16 byte offset,
			 * is a dependency on the trap code in
			 * kernel/arch/i386/usermapped_glo_ipc.S.
			 */

			अगर(तथ्य_copy(whichproc->p_endpoint, sp+16,
			  KERNEL, (vir_bytes) &use_bp,
				माप(use_bp)) != OK) {
				म_लिखो("stacktrace: aborting, copy failed\n");
				वापस;
			}

			अवरोध;
		}
		शेष:
			/* Full context is available; use the stored ebp */
			use_bp = whichproc->p_reg.fp;
			अवरोध;
	}

#अगर USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, use_bp, whichproc->p_reg.pc);
#पूर्ण_अगर /* USE_SYSDEBUG */
}

व्योम enable_fpu_exception(व्योम)
{
	u32_t cr0 = read_cr0();
	अगर(!(cr0 & I386_CR0_TS))
		write_cr0(cr0 | I386_CR0_TS);
}

व्योम disable_fpu_exception(व्योम)
{
	clts();
}

