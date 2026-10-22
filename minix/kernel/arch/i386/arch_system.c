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

स्थिर पूर्णाङ्क osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

शून्य * k_stacks;

स्थिर शून्य ser_debug(पूर्णाङ्क c);
स्थिर शून्य ser_dump_vfs(शून्य);

#ifdef CONFIG_SMP
स्थिर शून्य ser_dump_proc_cpu(शून्य);
#endif
#जइ !CONFIG_OXPCIE
स्थिर शून्य ser_init(शून्य);
#endif

शून्य fpu_init(शून्य)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	जइ((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		जइ(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			जइ(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} अण्णहा {
			osfxsr_feature = 0;
		}
	} अण्णहा {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                प्रत्यागम;
        }
}

शून्य save_local_fpu(संरचना proc *pr, पूर्णाङ्क retain)
{
	वर्ण *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	जइ(!is_fpu())
		प्रत्यागम;

	प्रतिज्ञा(state);

	जइ(osfxsr_feature) {
		fxsave(state);
	} अण्णहा {
		fnsave(state);
		जइ (retain)
			(शून्य) frstor(state);
	}
}

शून्य save_fpu(संरचना proc *pr)
{
#ifdef CONFIG_SMP
	जइ (cpuid != pr->p_cpu) {
		पूर्णाङ्क stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		जइ (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		प्रत्यागम;
	}
#endif

	जइ (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
स्थिर वर्ण fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

शून्य arch_proc_reset(संरचना proc *pr)
{
	वर्ण *v = रिक्त;
	संरचना stackframe_s reg;

	प्रतिज्ञा(pr->p_nr < NR_PROCS);

	जइ(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		प्रतिज्ञा(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, परिमाण(pr->p_reg));
        जइ(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        अण्णहा
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

पूर्णाङ्क restore_fpu(संरचना proc *pr)
{
	पूर्णाङ्क failed;
	वर्ण *state = pr->p_seg.fpu_state;

	प्रतिज्ञा(state);

	जइ(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} अण्णहा {
		जइ(osfxsr_feature) {
			failed = fxrstor(state);
		} अण्णहा {
			failed = frstor(state);
		}

		जइ (failed) प्रत्यागम EINVAL;
	}

	प्रत्यागम OK;
}

शून्य cpu_identify(शून्य)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	जइ (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} अण्णहा जइ (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} अण्णहा
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	जइ (eax == 0) 
		प्रत्यागम;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	जइ (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	जइ (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

शून्य arch_init(शून्य)
{
	k_stacks = (शून्य*) &k_stacks_start;
	प्रतिज्ञा(!((vir_bytes) k_stacks % K_राशिः_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_राशिः_शिखरम्(0));
#endif

#जइ !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#जइ defined(USE_APIC) && !defined(CONFIG_SMP)
	जइ (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	अण्णहा जइ (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_शिखरम्, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
शून्य do_ser_debug(शून्य)
{
	u8_t c, lsr;

#जइ CONFIG_OXPCIE
	{
		पूर्णाङ्क oxin;
		जइ((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	जइ (!(lsr & LSR_DR))
		प्रत्यागम;
	c = inb(COM1_RBR);
	ser_debug(c);
}

स्थिर शून्य ser_dump_queue_cpu(unsigned cpu)
{
	पूर्णाङ्क q;
	संरचना proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	प्रति(q = 0; q < NR_SCHED_QUEUES; q++) {
		संरचना proc *p;
		जइ(rdy_head[q])	 {
			printf("%2d: ", q);
			प्रति(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

स्थिर शून्य ser_dump_queues(शून्य)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	प्रति (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#अण्णहा
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
स्थिर शून्य dump_bkl_usage(शून्य)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	प्रति (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

स्थिर शून्य reset_bkl_usage(शून्य)
{
	memset(kernel_ticks, 0, परिमाण(kernel_ticks));
	memset(bkl_ticks, 0, परिमाण(bkl_ticks));
	memset(bkl_tries, 0, परिमाण(bkl_tries));
	memset(bkl_succ, 0, परिमाण(bkl_succ));
}
#endif

स्थिर शून्य ser_debug(ध्रुव पूर्णाङ्क c)
{
	serial_debug_active = 1;

	विकल्प(c)
	{
	पक्ष 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	पक्ष 'B':
		dump_bkl_usage();
		भङ्ग;
	पक्ष 'b':
		reset_bkl_usage();
		भङ्ग;
#endif
	पक्ष '1':
		ser_dump_proc();
		भङ्ग;
	पक्ष '2':
		ser_dump_queues();
		भङ्ग;
#ifdef CONFIG_SMP
	पक्ष '4':
		ser_dump_proc_cpu();
		भङ्ग;
#endif
	पक्ष '5':
		ser_dump_vfs();
		भङ्ग;
#जइ DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	पक्ष ch: {					\
		जइ(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} अण्णहा {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		भङ्ग;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	पक्ष 'I':
		dump_apic_irq_state();
		भङ्ग;
#endif
	}
	serial_debug_active = 0;
}

#जइ DEBUG_SERIAL

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
	unsigned cpu;

	प्रति (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		प्रति (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			जइ (isemptyp(pp) || pp->p_cpu != cpu)
				अनुवर्त;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#जइ SPROFILE

पूर्णाङ्क arch_init_profile_clock(ध्रुव u32_t freq)
{
  पूर्णाङ्क r;
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

  प्रत्यागम CMOS_CLOCK_IRQ;
}

शून्य arch_stop_profile_clock(शून्य)
{
  पूर्णाङ्क r;
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
	वर्ण * stk;
	संरचना proc * p;

#ifdef CONFIG_SMP
	stk = (वर्ण *)tss[cpuid].sp0;
#अण्णहा
	stk = (वर्ण *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	जइ(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	अण्णहा
        	p->p_reg.psw &= ~TRACEBIT;

	प्रत्यागम p;
}

शून्य arch_proc_setcontext(संरचना proc *p, संरचना stackframe_s *state,
	पूर्णाङ्क isuser, पूर्णाङ्क trap_style)
{
	जइ(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	प्रतिज्ञा(परिमाण(p->p_reg) == परिमाण(*state));
	जइ(state != &p->p_reg) {
		memcpy(&p->p_reg, state, परिमाण(*state));
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
	जइ(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	जइ(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

शून्य restore_user_context(संरचना proc *p)
{
	पूर्णाङ्क trap_style = p->p_seg.p_kern_trap_style;
#जइ 0
#define TYPES 10
	स्थिर पूर्णाङ्क restores[TYPES], n = 0;

	जइ(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	जइ(!(n++ % 500000)) {
		पूर्णाङ्क t;
		प्रति(t = 0; t < TYPES; t++)
			जइ(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	जइ(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	जइ(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        विकल्प(trap_style) {
                पक्ष KTS_NONE:
                        panic("no entry trap style known");
                पक्ष KTS_INT_HARD:
                पक्ष KTS_INT_UM:
                पक्ष KTS_FULLCONTEXT:
                पक्ष KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                स्वभाव:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

शून्य fpu_sigcontext(संरचना proc *pr, संरचना sigframe_sigcontext *fr, संरचना sigcontext *sc)
{
	पूर्णाङ्क fp_error;

	जइ (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} अण्णहा {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	जइ (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} अण्णहा जइ (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} अण्णहा जइ (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} अण्णहा जइ (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} अण्णहा जइ (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} अण्णहा {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(संरचना proc *p) { प्रत्यागम p->p_reg.sp; }

#जइ !CONFIG_OXPCIE
स्थिर शून्य ser_init(शून्य)
{
	unsigned वर्ण lcr;
	unsigned भाजकः;

	/* keep BIOS settings if cttybaud is not set */
	जइ (kinfo.serial_debug_baud <= 0) प्रत्यागम;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	भाजकः = UART_BASE_FREQ / kinfo.serial_debug_baud;
	जइ (भाजकः < 1) भाजकः = 1;
	जइ (भाजकः > 65535) भाजकः = 65535;
	
	outb(COM1_DLL, भाजकः & 0xff);
	outb(COM1_DLM, (भाजकः >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
