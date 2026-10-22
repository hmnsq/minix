/* system dependent functions for use inside the whole kernel. */

#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <machine/cmos.h>
#അന്തര്ഭാവ <machine/bios.h>
#അന്തര്ഭാവ <machine/cpu.h>
#അന്തര്ഭാവ <minix/portio.h>
#അന്തര്ഭാവ <minix/cpufeature.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <machine/vm.h>

#അന്തര്ഭാവ <minix/u64.h>

#അന്തര്ഭാവ "archconst.h"
#അന്തര്ഭാവ "oxpcie.h"

#അന്തര്ഭാവ "glo.h"

#ifdef USE_APIC
#അന്തര്ഭാവ "apic.h"
#endif

#ifdef USE_ACPI
#അന്തര്ഭാവ "acpi.h"
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

ശൂന്യം * k_stacks;

സ്ഥിതം ശൂന്യം ser_debug(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം ശൂന്യം ser_dump_vfs(ശൂന്യം);

#ifdef CONFIG_SMP
സ്ഥിതം ശൂന്യം ser_dump_proc_cpu(ശൂന്യം);
#endif
#എങ്കിൽ !CONFIG_OXPCIE
സ്ഥിതം ശൂന്യം ser_init(ശൂന്യം);
#endif

ശൂന്യം fpu_init(ശൂന്യം)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	എങ്കിൽ((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		എങ്കിൽ(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			എങ്കിൽ(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} അല്ലെങ്കിൽ {
			osfxsr_feature = 0;
		}
	} അല്ലെങ്കിൽ {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                തിരികെ;
        }
}

ശൂന്യം save_local_fpu(ഘടന proc *pr, പൂർണ്ണസംഖ്യ retain)
{
	അക്ഷരം *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	എങ്കിൽ(!is_fpu())
		തിരികെ;

	പ്രതിജ്ഞാ(state);

	എങ്കിൽ(osfxsr_feature) {
		fxsave(state);
	} അല്ലെങ്കിൽ {
		fnsave(state);
		എങ്കിൽ (retain)
			(ശൂന്യം) frstor(state);
	}
}

ശൂന്യം save_fpu(ഘടന proc *pr)
{
#ifdef CONFIG_SMP
	എങ്കിൽ (cpuid != pr->p_cpu) {
		പൂർണ്ണസംഖ്യ stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		എങ്കിൽ (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		തിരികെ;
	}
#endif

	എങ്കിൽ (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
സ്ഥിതം അക്ഷരം fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

ശൂന്യം arch_proc_reset(ഘടന proc *pr)
{
	അക്ഷരം *v = ശൂന്യ;
	ഘടന stackframe_s reg;

	പ്രതിജ്ഞാ(pr->p_nr < NR_PROCS);

	എങ്കിൽ(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		പ്രതിജ്ഞാ(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, വലുപ്പം(pr->p_reg));
        എങ്കിൽ(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        അല്ലെങ്കിൽ
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

ശൂന്യം arch_set_secondary_ipc_return(ഘടന proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

പൂർണ്ണസംഖ്യ restore_fpu(ഘടന proc *pr)
{
	പൂർണ്ണസംഖ്യ failed;
	അക്ഷരം *state = pr->p_seg.fpu_state;

	പ്രതിജ്ഞാ(state);

	എങ്കിൽ(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} അല്ലെങ്കിൽ {
		എങ്കിൽ(osfxsr_feature) {
			failed = fxrstor(state);
		} അല്ലെങ്കിൽ {
			failed = frstor(state);
		}

		എങ്കിൽ (failed) തിരികെ EINVAL;
	}

	തിരികെ OK;
}

ശൂന്യം cpu_identify(ശൂന്യം)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	എങ്കിൽ (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} അല്ലെങ്കിൽ എങ്കിൽ (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} അല്ലെങ്കിൽ
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	എങ്കിൽ (eax == 0) 
		തിരികെ;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	എങ്കിൽ (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	എങ്കിൽ (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

ശൂന്യം arch_init(ശൂന്യം)
{
	k_stacks = (ശൂന്യം*) &k_stacks_start;
	പ്രതിജ്ഞാ(!((vir_bytes) k_stacks % K_അട്ടി_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_അട്ടി_മുകൾ(0));
#endif

#എങ്കിൽ !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#എങ്കിൽ defined(USE_APIC) && !defined(CONFIG_SMP)
	എങ്കിൽ (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_മുകൾ, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
ശൂന്യം do_ser_debug(ശൂന്യം)
{
	u8_t c, lsr;

#എങ്കിൽ CONFIG_OXPCIE
	{
		പൂർണ്ണസംഖ്യ oxin;
		എങ്കിൽ((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	എങ്കിൽ (!(lsr & LSR_DR))
		തിരികെ;
	c = inb(COM1_RBR);
	ser_debug(c);
}

സ്ഥിതം ശൂന്യം ser_dump_queue_cpu(unsigned cpu)
{
	പൂർണ്ണസംഖ്യ q;
	ഘടന proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	വേണ്ടി(q = 0; q < NR_SCHED_QUEUES; q++) {
		ഘടന proc *p;
		എങ്കിൽ(rdy_head[q])	 {
			printf("%2d: ", q);
			വേണ്ടി(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

സ്ഥിതം ശൂന്യം ser_dump_queues(ശൂന്യം)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	വേണ്ടി (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#അല്ലെങ്കിൽ
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
സ്ഥിതം ശൂന്യം dump_bkl_usage(ശൂന്യം)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	വേണ്ടി (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

സ്ഥിതം ശൂന്യം reset_bkl_usage(ശൂന്യം)
{
	memset(kernel_ticks, 0, വലുപ്പം(kernel_ticks));
	memset(bkl_ticks, 0, വലുപ്പം(bkl_ticks));
	memset(bkl_tries, 0, വലുപ്പം(bkl_tries));
	memset(bkl_succ, 0, വലുപ്പം(bkl_succ));
}
#endif

സ്ഥിതം ശൂന്യം ser_debug(സ്ഥിരം പൂർണ്ണസംഖ്യ c)
{
	serial_debug_active = 1;

	തിരഞ്ഞെടുപ്പ്(c)
	{
	അവസ്ഥ 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	അവസ്ഥ 'B':
		dump_bkl_usage();
		നിർത്തുക;
	അവസ്ഥ 'b':
		reset_bkl_usage();
		നിർത്തുക;
#endif
	അവസ്ഥ '1':
		ser_dump_proc();
		നിർത്തുക;
	അവസ്ഥ '2':
		ser_dump_queues();
		നിർത്തുക;
#ifdef CONFIG_SMP
	അവസ്ഥ '4':
		ser_dump_proc_cpu();
		നിർത്തുക;
#endif
	അവസ്ഥ '5':
		ser_dump_vfs();
		നിർത്തുക;
#എങ്കിൽ DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	അവസ്ഥ ch: {					\
		എങ്കിൽ(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} അല്ലെങ്കിൽ {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		നിർത്തുക;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	അവസ്ഥ 'I':
		dump_apic_irq_state();
		നിർത്തുക;
#endif
	}
	serial_debug_active = 0;
}

#എങ്കിൽ DEBUG_SERIAL

സ്ഥിതം ശൂന്യം ser_dump_vfs(ശൂന്യം)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
സ്ഥിതം ശൂന്യം ser_dump_proc_cpu(ശൂന്യം)
{
	ഘടന proc *pp;
	unsigned cpu;

	വേണ്ടി (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		വേണ്ടി (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			എങ്കിൽ (isemptyp(pp) || pp->p_cpu != cpu)
				തുടരുക;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#എങ്കിൽ SPROFILE

പൂർണ്ണസംഖ്യ arch_init_profile_clock(സ്ഥിരം u32_t freq)
{
  പൂർണ്ണസംഖ്യ r;
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

  തിരികെ CMOS_CLOCK_IRQ;
}

ശൂന്യം arch_stop_profile_clock(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

ശൂന്യം arch_ack_profile_clock(ശൂന്യം)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

ശൂന്യം arch_do_syscall(ഘടന proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  പ്രതിജ്ഞാ(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  പ്രതിജ്ഞാ(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

ഘടന proc * arch_finish_switch_to_user(ശൂന്യം)
{
	അക്ഷരം * stk;
	ഘടന proc * p;

#ifdef CONFIG_SMP
	stk = (അക്ഷരം *)tss[cpuid].sp0;
#അല്ലെങ്കിൽ
	stk = (അക്ഷരം *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	എങ്കിൽ(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	അല്ലെങ്കിൽ
        	p->p_reg.psw &= ~TRACEBIT;

	തിരികെ p;
}

ശൂന്യം arch_proc_setcontext(ഘടന proc *p, ഘടന stackframe_s *state,
	പൂർണ്ണസംഖ്യ isuser, പൂർണ്ണസംഖ്യ trap_style)
{
	എങ്കിൽ(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	പ്രതിജ്ഞാ(വലുപ്പം(p->p_reg) == വലുപ്പം(*state));
	എങ്കിൽ(state != &p->p_reg) {
		memcpy(&p->p_reg, state, വലുപ്പം(*state));
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
	എങ്കിൽ(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	എങ്കിൽ(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

ശൂന്യം restore_user_context(ഘടന proc *p)
{
	പൂർണ്ണസംഖ്യ trap_style = p->p_seg.p_kern_trap_style;
#എങ്കിൽ 0
#define TYPES 10
	സ്ഥിതം പൂർണ്ണസംഖ്യ restores[TYPES], n = 0;

	എങ്കിൽ(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	എങ്കിൽ(!(n++ % 500000)) {
		പൂർണ്ണസംഖ്യ t;
		വേണ്ടി(t = 0; t < TYPES; t++)
			എങ്കിൽ(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	എങ്കിൽ(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	എങ്കിൽ(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        തിരഞ്ഞെടുപ്പ്(trap_style) {
                അവസ്ഥ KTS_NONE:
                        panic("no entry trap style known");
                അവസ്ഥ KTS_INT_HARD:
                അവസ്ഥ KTS_INT_UM:
                അവസ്ഥ KTS_FULLCONTEXT:
                അവസ്ഥ KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                സ്ഥിരസ്ഥിതി:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

ശൂന്യം fpu_sigcontext(ഘടന proc *pr, ഘടന sigframe_sigcontext *fr, ഘടന sigcontext *sc)
{
	പൂർണ്ണസംഖ്യ fp_error;

	എങ്കിൽ (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} അല്ലെങ്കിൽ {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	എങ്കിൽ (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} അല്ലെങ്കിൽ എങ്കിൽ (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} അല്ലെങ്കിൽ എങ്കിൽ (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} അല്ലെങ്കിൽ എങ്കിൽ (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} അല്ലെങ്കിൽ എങ്കിൽ (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} അല്ലെങ്കിൽ {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(ഘടന proc *p) { തിരികെ p->p_reg.sp; }

#എങ്കിൽ !CONFIG_OXPCIE
സ്ഥിതം ശൂന്യം ser_init(ശൂന്യം)
{
	unsigned അക്ഷരം lcr;
	unsigned ഹാരകം;

	/* keep BIOS settings if cttybaud is not set */
	എങ്കിൽ (kinfo.serial_debug_baud <= 0) തിരികെ;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	ഹാരകം = UART_BASE_FREQ / kinfo.serial_debug_baud;
	എങ്കിൽ (ഹാരകം < 1) ഹാരകം = 1;
	എങ്കിൽ (ഹാരകം > 65535) ഹാരകം = 65535;
	
	outb(COM1_DLL, ഹാരകം & 0xff);
	outb(COM1_DLM, (ഹാരകം >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
