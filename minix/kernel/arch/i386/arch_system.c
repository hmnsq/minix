/* system dependent functions for use inside the whole kernel. */

#समावेश <unistd.h>
#समावेश <ctype.h>
#समावेश <string.h>
#समावेश <machine/cmos.h>
#समावेश <machine/bios.h>
#समावेश <machine/cpu.h>
#समावेश <minix/portio.h>
#समावेश <minix/cpufeature.h>
#समावेश <assert.h>
#समावेश <signal.h>
#समावेश <machine/vm.h>

#समावेश <minix/u64.h>

#समावेश "archconst.h"
#समावेश "oxpcie.h"

#समावेश "glo.h"

#अगर_घोषित USE_APIC
#समावेश "apic.h"
#पूर्ण_अगर

#अगर_घोषित USE_ACPI
#समावेश "acpi.h"
#पूर्ण_अगर

अटल पूर्णांक osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#घोषणा CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#घोषणा CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#घोषणा CR4_OSXMMEXCPT	(1L<<10)

व्योम * k_stacks;

अटल व्योम ser_debug(पूर्णांक c);
अटल व्योम ser_dump_vfs(व्योम);

#अगर_घोषित CONFIG_SMP
अटल व्योम ser_dump_proc_cpu(व्योम);
#पूर्ण_अगर
#अगर !CONFIG_OXPCIE
अटल व्योम ser_init(व्योम);
#पूर्ण_अगर

व्योम fpu_init(व्योम)
{
	अचिन्हित लघु cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	अगर((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		अगर(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			अगर(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} अन्यथा {
			osfxsr_feature = 0;
		}
	} अन्यथा {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                वापस;
        }
}

व्योम save_local_fpu(काष्ठा proc *pr, पूर्णांक retain)
{
	अक्षर *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	अगर(!is_fpu())
		वापस;

	निश्चित(state);

	अगर(osfxsr_feature) {
		fxsave(state);
	} अन्यथा {
		fnsave(state);
		अगर (retain)
			(व्योम) frstor(state);
	}
}

व्योम save_fpu(काष्ठा proc *pr)
{
#अगर_घोषित CONFIG_SMP
	अगर (cpuid != pr->p_cpu) {
		पूर्णांक stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		अगर (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		वापस;
	}
#पूर्ण_अगर

	अगर (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
अटल अक्षर fpu_state[NR_PROCS][FPU_XFP_आकार] __aligned(FPUALIGN);

व्योम arch_proc_reset(काष्ठा proc *pr)
{
	अक्षर *v = शून्य;
	काष्ठा stackframe_s reg;

	निश्चित(pr->p_nr < NR_PROCS);

	अगर(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		निश्चित(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		स_रखो(v, 0, FPU_XFP_आकार);
	}

	/* Clear process state. */
        स_रखो(&reg, 0, माप(pr->p_reg));
        अगर(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        अन्यथा
        	reg.psw = INIT_PSW;

	pr->p_seg.fpu_state = v;

	/* Initialize the fundamentals that are (initially) the same for all
	 * processes - the segment selectors it gets to use.
	 */
	pr->p_reg.cs = USER_CS_SELECTOR;
	pr->p_reg.gs = 
	pr->p_reg.fs = 
	pr->p_reg.ss = 
	pr->p_reg.es = 
	pr->p_reg.ds = USER_DS_SELECTOR;

	/* set full context and make sure it gets restored */
	arch_proc_setcontext(pr, &reg, 0, KTS_FULLCONTEXT);
}

व्योम arch_set_secondary_ipc_return(काष्ठा proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

पूर्णांक restore_fpu(काष्ठा proc *pr)
{
	पूर्णांक failed;
	अक्षर *state = pr->p_seg.fpu_state;

	निश्चित(state);

	अगर(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} अन्यथा {
		अगर(osfxsr_feature) {
			failed = fxrstor(state);
		} अन्यथा {
			failed = frstor(state);
		}

		अगर (failed) वापस EINVAL;
	}

	वापस OK;
}

व्योम cpu_identify(व्योम)
{
	u32_t eax, ebx, ecx, edx;
	अचिन्हित cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	अगर (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} अन्यथा अगर (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} अन्यथा
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	अगर (eax == 0) 
		वापस;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	अगर (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	अगर (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

व्योम arch_init(व्योम)
{
	k_stacks = (व्योम*) &k_stacks_start;
	निश्चित(!((vir_bytes) k_stacks % K_ढेर_आकार));

#अगर_अघोषित CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ढेर_शीर्ष(0));
#पूर्ण_अगर

#अगर !CONFIG_OXPCIE
	ser_init();
#पूर्ण_अगर

#अगर_घोषित USE_ACPI
	acpi_init();
#पूर्ण_अगर

#अगर defined(USE_APIC) && !defined(CONFIG_SMP)
	अगर (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	अन्यथा अगर (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#पूर्ण_अगर

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_शीर्ष, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
व्योम do_ser_debug(व्योम)
{
	u8_t c, lsr;

#अगर CONFIG_OXPCIE
	{
		पूर्णांक oxin;
		अगर((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#पूर्ण_अगर

	lsr= inb(COM1_LSR);
	अगर (!(lsr & LSR_DR))
		वापस;
	c = inb(COM1_RBR);
	ser_debug(c);
}

अटल व्योम ser_dump_queue_cpu(अचिन्हित cpu)
{
	पूर्णांक q;
	काष्ठा proc ** rdy_शीर्ष;
	
	rdy_शीर्ष = get_cpu_var(cpu, run_q_शीर्ष);

	क्रम(q = 0; q < NR_SCHED_QUEUES; q++) {
		काष्ठा proc *p;
		अगर(rdy_शीर्ष[q])	 {
			म_लिखो("%2d: ", q);
			क्रम(p = rdy_शीर्ष[q]; p; p = p->p_nextready) {
				म_लिखो("%s / %d  ", p->p_name, p->p_endpoint);
			}
			म_लिखो("\n");
		}
	}
}

अटल व्योम ser_dump_queues(व्योम)
{
#अगर_घोषित CONFIG_SMP
	अचिन्हित cpu;

	म_लिखो("--- run queues ---\n");
	क्रम (cpu = 0; cpu < ncpus; cpu++) {
		म_लिखो("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#अन्यथा
	ser_dump_queue_cpu(0);
#पूर्ण_अगर
}

#अगर_घोषित CONFIG_SMP
अटल व्योम dump_bkl_usage(व्योम)
{
	अचिन्हित cpu;

	म_लिखो("--- BKL usage ---\n");
	क्रम (cpu = 0; cpu < ncpus; cpu++) {
		म_लिखो("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

अटल व्योम reset_bkl_usage(व्योम)
{
	स_रखो(kernel_ticks, 0, माप(kernel_ticks));
	स_रखो(bkl_ticks, 0, माप(bkl_ticks));
	स_रखो(bkl_tries, 0, माप(bkl_tries));
	स_रखो(bkl_succ, 0, माप(bkl_succ));
}
#पूर्ण_अगर

अटल व्योम ser_debug(स्थिर पूर्णांक c)
{
	serial_debug_active = 1;

	चयन(c)
	{
	हाल 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#अगर_घोषित CONFIG_SMP
	हाल 'B':
		dump_bkl_usage();
		अवरोध;
	हाल 'b':
		reset_bkl_usage();
		अवरोध;
#पूर्ण_अगर
	हाल '1':
		ser_dump_proc();
		अवरोध;
	हाल '2':
		ser_dump_queues();
		अवरोध;
#अगर_घोषित CONFIG_SMP
	हाल '4':
		ser_dump_proc_cpu();
		अवरोध;
#पूर्ण_अगर
	हाल '5':
		ser_dump_vfs();
		अवरोध;
#अगर DEBUG_TRACE
#घोषणा TOGGLECASE(ch, flag)				\
	हाल ch: {					\
		अगर(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			म_लिखो("%s disabled\n", #flag);	\
		} अन्यथा {				\
			verboseflags |= flag;		\
			म_लिखो("%s enabled\n", #flag);	\
		}					\
		अवरोध;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#पूर्ण_अगर
#अगर_घोषित USE_APIC
	हाल 'I':
		dump_apic_irq_state();
		अवरोध;
#पूर्ण_अगर
	}
	serial_debug_active = 0;
}

#अगर DEBUG_SERIAL

अटल व्योम ser_dump_vfs(व्योम)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#अगर_घोषित CONFIG_SMP
अटल व्योम ser_dump_proc_cpu(व्योम)
{
	काष्ठा proc *pp;
	अचिन्हित cpu;

	क्रम (cpu = 0; cpu < ncpus; cpu++) {
		म_लिखो("CPU %d processes : \n", cpu);
		क्रम (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			अगर (isemptyp(pp) || pp->p_cpu != cpu)
				जारी;
			print_proc(pp);
		}
	}
}
#पूर्ण_अगर

#पूर्ण_अगर /* DEBUG_SERIAL */

#अगर SPROFILE

पूर्णांक arch_init_profile_clock(स्थिर u32_t freq)
{
  पूर्णांक r;
  /* Set CMOS timer frequency. */
  outb(RTC_सूचकांक, RTC_REG_A);
  outb(RTC_IO, RTC_A_DV_OK | freq);
  /* Enable CMOS timer interrupts. */
  outb(RTC_सूचकांक, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_सूचकांक, RTC_REG_B); 
  outb(RTC_IO, r | RTC_B_PIE);
  /* Mandatory read of CMOS register to enable timer interrupts. */
  outb(RTC_सूचकांक, RTC_REG_C);
  inb(RTC_IO);

  वापस CMOS_CLOCK_IRQ;
}

व्योम arch_stop_profile_clock(व्योम)
{
  पूर्णांक r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_सूचकांक, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_सूचकांक, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

व्योम arch_ack_profile_clock(व्योम)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_सूचकांक, RTC_REG_C);
  inb(RTC_IO);
}

#पूर्ण_अगर

व्योम arch_do_syscall(काष्ठा proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  निश्चित(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  निश्चित(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

काष्ठा proc * arch_finish_switch_to_user(व्योम)
{
	अक्षर * stk;
	काष्ठा proc * p;

#अगर_घोषित CONFIG_SMP
	stk = (अक्षर *)tss[cpuid].sp0;
#अन्यथा
	stk = (अक्षर *)tss[0].sp0;
#पूर्ण_अगर
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	अगर(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	अन्यथा
        	p->p_reg.psw &= ~TRACEBIT;

	वापस p;
}

व्योम arch_proc_setcontext(काष्ठा proc *p, काष्ठा stackframe_s *state,
	पूर्णांक isuser, पूर्णांक trap_style)
{
	अगर(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	निश्चित(माप(p->p_reg) == माप(*state));
	अगर(state != &p->p_reg) {
		स_नकल(&p->p_reg, state, माप(*state));
	}

	/* further code is instructed to not touch the context
	 * any more
	 */
	p->p_misc_flags |= MF_CONTEXT_SET;

	/* on x86 this requires returning using iret (KTS_INT)
	 * so that the full context is restored instead of relying on
	 * the userspace doing it (as it would do on SYSEXIT).
	 * as ESP and EIP are also reset, userspace won't try to
	 * restore bogus context after returning.
	 *
	 * if the process is not blocked, or the kernel will ignore
	 * our trap style, we needn't panic but things will probably
	 * not go well for the process (restored context will be ignored)
	 * and the situation should be debugged.
	 */
	अगर(!(p->p_rts_flags)) {
		म_लिखो("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	अगर(p->p_seg.p_kern_trap_style == KTS_NONE)
		म_लिखो("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

व्योम restore_user_context(काष्ठा proc *p)
{
	पूर्णांक trap_style = p->p_seg.p_kern_trap_style;
#अगर 0
#घोषणा TYPES 10
	अटल पूर्णांक restores[TYPES], n = 0;

	अगर(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	अगर(!(n++ % 500000)) {
		पूर्णांक t;
		क्रम(t = 0; t < TYPES; t++)
			अगर(restores[t])
				म_लिखो("%d: %d   ", t, restores[t]);
		म_लिखो("\n");
	}
#पूर्ण_अगर

	p->p_seg.p_kern_trap_style = KTS_NONE;

	अगर(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	अगर(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        चयन(trap_style) {
                हाल KTS_NONE:
                        panic("no entry trap style known");
                हाल KTS_INT_HARD:
                हाल KTS_INT_UM:
                हाल KTS_FULLCONTEXT:
                हाल KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                शेष:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

व्योम fpu_sigcontext(काष्ठा proc *pr, काष्ठा sigframe_sigcontext *fr, काष्ठा sigcontext *sc)
{
	पूर्णांक fp_error;

	अगर (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} अन्यथा {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	अगर (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} अन्यथा अगर (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} अन्यथा अगर (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} अन्यथा अगर (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} अन्यथा अगर (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} अन्यथा {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(काष्ठा proc *p) { वापस p->p_reg.sp; }

#अगर !CONFIG_OXPCIE
अटल व्योम ser_init(व्योम)
{
	अचिन्हित अक्षर lcr;
	अचिन्हित भाजक;

	/* keep BIOS settings if cttybaud is not set */
	अगर (kinfo.serial_debug_baud <= 0) वापस;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	भाजक = UART_BASE_FREQ / kinfo.serial_debug_baud;
	अगर (भाजक < 1) भाजक = 1;
	अगर (भाजक > 65535) भाजक = 65535;
	
	outb(COM1_DLL, भाजक & 0xff);
	outb(COM1_DLM, (भाजक >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#पूर्ण_अगर
