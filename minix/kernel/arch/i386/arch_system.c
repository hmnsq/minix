/* system dependent functions for use inside the whole kernel. */

#अन्तर्भाव <unistd.h>
#अन्तर्भाव <ctype.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <machine/cmos.h>
#अन्तर्भाव <machine/bios.h>
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <minix/portio.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <machine/vm.h>

#अन्तर्भाव <minix/u64.h>

#अन्तर्भाव "archconst.h"
#अन्तर्भाव "oxpcie.h"

#अन्तर्भाव "glo.h"

#ifdef USE_APIC
#अन्तर्भाव "apic.h"
#endif

#ifdef USE_ACPI
#अन्तर्भाव "acpi.h"
#endif

स्थिर पूर्णांक osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

शून्य * k_stacks;

स्थिर शून्य ser_debug(पूर्णांक c);
स्थिर शून्य ser_dump_vfs(शून्य);

#ifdef CONFIG_SMP
स्थिर शून्य ser_dump_proc_cpu(शून्य);
#endif
#जर !CONFIG_OXPCIE
स्थिर शून्य ser_init(शून्य);
#endif

शून्य fpu_init(शून्य)
{
	अचिन्हित लघु cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	जर((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		जर(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			जर(_cpufeature(_CPUF_I386_SSE))
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
                परत;
        }
}

शून्य save_local_fpu(संरचना proc *pr, पूर्णांक retain)
{
	अक्षर *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	जर(!is_fpu())
		परत;

	प्रतिज्ञा(state);

	जर(osfxsr_feature) {
		fxsave(state);
	} अन्यथा {
		fnsave(state);
		जर (retain)
			(शून्य) frstor(state);
	}
}

शून्य save_fpu(संरचना proc *pr)
{
#ifdef CONFIG_SMP
	जर (cpuid != pr->p_cpu) {
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
		जर (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		परत;
	}
#endif

	जर (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
स्थिर अक्षर fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

शून्य arch_proc_reset(संरचना proc *pr)
{
	अक्षर *v = रिक्त;
	संरचना stackframe_s reg;

	प्रतिज्ञा(pr->p_nr < NR_PROCS);

	जर(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		प्रतिज्ञा(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		स_रखो(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        स_रखो(&reg, 0, आकार(pr->p_reg));
        जर(iskerneln(pr->p_nr))
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

शून्य arch_set_secondary_ipc_return(संरचना proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

पूर्णांक restore_fpu(संरचना proc *pr)
{
	पूर्णांक failed;
	अक्षर *state = pr->p_seg.fpu_state;

	प्रतिज्ञा(state);

	जर(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} अन्यथा {
		जर(osfxsr_feature) {
			failed = fxrstor(state);
		} अन्यथा {
			failed = frstor(state);
		}

		जर (failed) परत EINVAL;
	}

	परत OK;
}

शून्य cpu_identify(शून्य)
{
	u32_t eax, ebx, ecx, edx;
	अचिन्हित cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	जर (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} अन्यथा जर (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} अन्यथा
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	जर (eax == 0) 
		परत;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	जर (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	जर (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

शून्य arch_init(शून्य)
{
	k_stacks = (शून्य*) &k_stacks_start;
	प्रतिज्ञा(!((vir_bytes) k_stacks % K_चळत_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_चळत_शीर्ष(0));
#endif

#जर !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#जर defined(USE_APIC) && !defined(CONFIG_SMP)
	जर (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	अन्यथा जर (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_शीर्ष, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
शून्य do_ser_debug(शून्य)
{
	u8_t c, lsr;

#जर CONFIG_OXPCIE
	{
		पूर्णांक oxin;
		जर((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	जर (!(lsr & LSR_DR))
		परत;
	c = inb(COM1_RBR);
	ser_debug(c);
}

स्थिर शून्य ser_dump_queue_cpu(अचिन्हित cpu)
{
	पूर्णांक q;
	संरचना proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	साठी(q = 0; q < NR_SCHED_QUEUES; q++) {
		संरचना proc *p;
		जर(rdy_head[q])	 {
			म_लिखो("%2d: ", q);
			साठी(p = rdy_head[q]; p; p = p->p_nextready) {
				म_लिखो("%s / %d  ", p->p_name, p->p_endpoint);
			}
			म_लिखो("\n");
		}
	}
}

स्थिर शून्य ser_dump_queues(शून्य)
{
#ifdef CONFIG_SMP
	अचिन्हित cpu;

	म_लिखो("--- run queues ---\n");
	साठी (cpu = 0; cpu < ncpus; cpu++) {
		म_लिखो("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#अन्यथा
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
स्थिर शून्य dump_bkl_usage(शून्य)
{
	अचिन्हित cpu;

	म_लिखो("--- BKL usage ---\n");
	साठी (cpu = 0; cpu < ncpus; cpu++) {
		म_लिखो("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

स्थिर शून्य reset_bkl_usage(शून्य)
{
	स_रखो(kernel_ticks, 0, आकार(kernel_ticks));
	स_रखो(bkl_ticks, 0, आकार(bkl_ticks));
	स_रखो(bkl_tries, 0, आकार(bkl_tries));
	स_रखो(bkl_succ, 0, आकार(bkl_succ));
}
#endif

स्थिर शून्य ser_debug(स्थिरांक पूर्णांक c)
{
	serial_debug_active = 1;

	निवड(c)
	{
	स्थिती 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	स्थिती 'B':
		dump_bkl_usage();
		थांबा;
	स्थिती 'b':
		reset_bkl_usage();
		थांबा;
#endif
	स्थिती '1':
		ser_dump_proc();
		थांबा;
	स्थिती '2':
		ser_dump_queues();
		थांबा;
#ifdef CONFIG_SMP
	स्थिती '4':
		ser_dump_proc_cpu();
		थांबा;
#endif
	स्थिती '5':
		ser_dump_vfs();
		थांबा;
#जर DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	स्थिती ch: {					\
		जर(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			म_लिखो("%s disabled\n", #flag);	\
		} अन्यथा {				\
			verboseflags |= flag;		\
			म_लिखो("%s enabled\n", #flag);	\
		}					\
		थांबा;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	स्थिती 'I':
		dump_apic_irq_state();
		थांबा;
#endif
	}
	serial_debug_active = 0;
}

#जर DEBUG_SERIAL

स्थिर शून्य ser_dump_vfs(शून्य)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
स्थिर शून्य ser_dump_proc_cpu(शून्य)
{
	संरचना proc *pp;
	अचिन्हित cpu;

	साठी (cpu = 0; cpu < ncpus; cpu++) {
		म_लिखो("CPU %d processes : \n", cpu);
		साठी (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			जर (isemptyp(pp) || pp->p_cpu != cpu)
				सुरूठेवा;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#जर SPROFILE

पूर्णांक arch_init_profile_clock(स्थिरांक u32_t freq)
{
  पूर्णांक r;
  /* Set CMOS timer frequency. */
  outb(RTC_INDEX, RTC_REG_A);
  outb(RTC_IO, RTC_A_DV_OK | freq);
  /* Enable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B); 
  outb(RTC_IO, r | RTC_B_PIE);
  /* Mandatory read of CMOS register to enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);

  परत CMOS_CLOCK_IRQ;
}

शून्य arch_stop_profile_clock(शून्य)
{
  पूर्णांक r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

शून्य arch_ack_profile_clock(शून्य)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

शून्य arch_do_syscall(संरचना proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  प्रतिज्ञा(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  प्रतिज्ञा(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

संरचना proc * arch_finish_switch_to_user(शून्य)
{
	अक्षर * stk;
	संरचना proc * p;

#ifdef CONFIG_SMP
	stk = (अक्षर *)tss[cpuid].sp0;
#अन्यथा
	stk = (अक्षर *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	जर(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	अन्यथा
        	p->p_reg.psw &= ~TRACEBIT;

	परत p;
}

शून्य arch_proc_setcontext(संरचना proc *p, संरचना stackframe_s *state,
	पूर्णांक isuser, पूर्णांक trap_style)
{
	जर(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	प्रतिज्ञा(आकार(p->p_reg) == आकार(*state));
	जर(state != &p->p_reg) {
		स_नकल(&p->p_reg, state, आकार(*state));
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
	जर(!(p->p_rts_flags)) {
		म_लिखो("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	जर(p->p_seg.p_kern_trap_style == KTS_NONE)
		म_लिखो("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

शून्य restore_user_context(संरचना proc *p)
{
	पूर्णांक trap_style = p->p_seg.p_kern_trap_style;
#जर 0
#define TYPES 10
	स्थिर पूर्णांक restores[TYPES], n = 0;

	जर(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	जर(!(n++ % 500000)) {
		पूर्णांक t;
		साठी(t = 0; t < TYPES; t++)
			जर(restores[t])
				म_लिखो("%d: %d   ", t, restores[t]);
		म_लिखो("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	जर(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	जर(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        निवड(trap_style) {
                स्थिती KTS_NONE:
                        panic("no entry trap style known");
                स्थिती KTS_INT_HARD:
                स्थिती KTS_INT_UM:
                स्थिती KTS_FULLCONTEXT:
                स्थिती KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                पूर्वनिर्धारित:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

शून्य fpu_sigcontext(संरचना proc *pr, संरचना sigframe_sigcontext *fr, संरचना sigcontext *sc)
{
	पूर्णांक fp_error;

	जर (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} अन्यथा {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	जर (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} अन्यथा जर (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} अन्यथा जर (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} अन्यथा जर (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} अन्यथा जर (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} अन्यथा {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(संरचना proc *p) { परत p->p_reg.sp; }

#जर !CONFIG_OXPCIE
स्थिर शून्य ser_init(शून्य)
{
	अचिन्हित अक्षर lcr;
	अचिन्हित भाजक;

	/* keep BIOS settings if cttybaud is not set */
	जर (kinfo.serial_debug_baud <= 0) परत;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	भाजक = UART_BASE_FREQ / kinfo.serial_debug_baud;
	जर (भाजक < 1) भाजक = 1;
	जर (भाजक > 65535) भाजक = 65535;
	
	outb(COM1_DLL, भाजक & 0xff);
	outb(COM1_DLM, (भाजक >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
