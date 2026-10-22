/* system dependent functions for use inside the whole kernel. */

#అన్తర్భావ <unistd.h>
#అన్తర్భావ <ctype.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <machine/cmos.h>
#అన్తర్భావ <machine/bios.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <minix/portio.h>
#అన్తర్భావ <minix/cpufeature.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <machine/vm.h>

#అన్తర్భావ <minix/u64.h>

#అన్తర్భావ "archconst.h"
#అన్తర్భావ "oxpcie.h"

#అన్తర్భావ "glo.h"

#నిర్వచితమైతే USE_APIC
#అన్తర్భావ "apic.h"
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ACPI
#అన్తర్భావ "acpi.h"
#అయితే_ముగింపు

స్థిర పూర్ణాంకం osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#నిర్వచించు CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#నిర్వచించు CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#నిర్వచించు CR4_OSXMMEXCPT	(1L<<10)

శూన్య * k_stacks;

స్థిర శూన్య ser_debug(పూర్ణాంకం c);
స్థిర శూన్య ser_dump_vfs(శూన్య);

#నిర్వచితమైతే CONFIG_SMP
స్థిర శూన్య ser_dump_proc_cpu(శూన్య);
#అయితే_ముగింపు
#ఒకవేళ !CONFIG_OXPCIE
స్థిర శూన్య ser_init(శూన్య);
#అయితే_ముగింపు

శూన్య fpu_init(శూన్య)
{
	గుర్తులేని పొట్టి cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	ఒకవేళ((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		ఒకవేళ(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			ఒకవేళ(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} లేకపోతే {
			osfxsr_feature = 0;
		}
	} లేకపోతే {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                తిరిగి;
        }
}

శూన్య save_local_fpu(నిర్మాణం proc *pr, పూర్ణాంకం retain)
{
	అక్షరం *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	ఒకవేళ(!is_fpu())
		తిరిగి;

	ప్రతిజ్ఞా(state);

	ఒకవేళ(osfxsr_feature) {
		fxsave(state);
	} లేకపోతే {
		fnsave(state);
		ఒకవేళ (retain)
			(శూన్య) frstor(state);
	}
}

శూన్య save_fpu(నిర్మాణం proc *pr)
{
#నిర్వచితమైతే CONFIG_SMP
	ఒకవేళ (cpuid != pr->p_cpu) {
		పూర్ణాంకం stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_కాలపట్టిక_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		ఒకవేళ (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		తిరిగి;
	}
#అయితే_ముగింపు

	ఒకవేళ (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
స్థిర అక్షరం fpu_state[NR_PROCS][FPU_XFP_కొలత] __aligned(FPUALIGN);

శూన్య arch_proc_reset(నిర్మాణం proc *pr)
{
	అక్షరం *v = ఖాళీ;
	నిర్మాణం stackframe_s reg;

	ప్రతిజ్ఞా(pr->p_nr < NR_PROCS);

	ఒకవేళ(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		ప్రతిజ్ఞా(!((vir_బైట్లు)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_కొలత);
	}

	/* Clear process state. */
        memset(&reg, 0, పరిమాణం(pr->p_reg));
        ఒకవేళ(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        లేకపోతే
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

శూన్య arch_సమితి_secondary_ipc_return(నిర్మాణం proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

పూర్ణాంకం restore_fpu(నిర్మాణం proc *pr)
{
	పూర్ణాంకం failed;
	అక్షరం *state = pr->p_seg.fpu_state;

	ప్రతిజ్ఞా(state);

	ఒకవేళ(!proc_వాడిన_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} లేకపోతే {
		ఒకవేళ(osfxsr_feature) {
			failed = fxrstor(state);
		} లేకపోతే {
			failed = frstor(state);
		}

		ఒకవేళ (failed) తిరిగి EINVAL;
	}

	తిరిగి OK;
}

శూన్య cpu_identify(శూన్య)
{
	u32_t eax, ebx, ecx, edx;
	గుర్తులేని cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	ఒకవేళ (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} లేకపోతే ఒకవేళ (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} లేకపోతే
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	ఒకవేళ (eax == 0) 
		తిరిగి;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	ఒకవేళ (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	ఒకవేళ (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

శూన్య arch_init(శూన్య)
{
	k_stacks = (శూన్య*) &k_stacks_ప్రారంభం;
	ప్రతిజ్ఞా(!((vir_బైట్లు) k_stacks % K_దొంతర_కొలత));

#నిర్వచితం_కాకపోతే CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_దొంతర_శిఖరం(0));
#అయితే_ముగింపు

#ఒకవేళ !CONFIG_OXPCIE
	ser_init();
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ACPI
	acpi_init();
#అయితే_ముగింపు

#ఒకవేళ defined(USE_APIC) && !defined(CONFIG_SMP)
	ఒకవేళ (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	లేకపోతే ఒకవేళ (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#అయితే_ముగింపు

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_చివర);
	cut_memmap(&kinfo, పునాది_MEM_శిఖరం, పై_MEM_చివర);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
శూన్య do_ser_debug(శూన్య)
{
	u8_t c, lsr;

#ఒకవేళ CONFIG_OXPCIE
	{
		పూర్ణాంకం oxin;
		ఒకవేళ((oxin = oxpcie_లోపల()) >= 0)
		ser_debug(oxin);
	}
#అయితే_ముగింపు

	lsr= inb(COM1_LSR);
	ఒకవేళ (!(lsr & LSR_DR))
		తిరిగి;
	c = inb(COM1_RBR);
	ser_debug(c);
}

స్థిర శూన్య ser_dump_వరుస_cpu(గుర్తులేని cpu)
{
	పూర్ణాంకం q;
	నిర్మాణం proc ** rdy_తల;
	
	rdy_తల = get_cpu_var(cpu, నిరంతరం_q_తల);

	కొరకు(q = 0; q < NR_SCHED_QUEUES; q++) {
		నిర్మాణం proc *p;
		ఒకవేళ(rdy_తల[q])	 {
			రాయి("%2d: ", q);
			కొరకు(p = rdy_తల[q]; p; p = p->p_nextready) {
				రాయి("%s / %d  ", p->p_name, p->p_endpoint);
			}
			రాయి("\n");
		}
	}
}

స్థిర శూన్య ser_dump_queues(శూన్య)
{
#నిర్వచితమైతే CONFIG_SMP
	గుర్తులేని cpu;

	రాయి("--- run queues ---\n");
	కొరకు (cpu = 0; cpu < ncpus; cpu++) {
		రాయి("CPU %d :\n", cpu);
		ser_dump_వరుస_cpu(cpu);
	}
#లేకపోతే
	ser_dump_వరుస_cpu(0);
#అయితే_ముగింపు
}

#నిర్వచితమైతే CONFIG_SMP
స్థిర శూన్య dump_bkl_usage(శూన్య)
{
	గుర్తులేని cpu;

	రాయి("--- BKL usage ---\n");
	కొరకు (cpu = 0; cpu < ncpus; cpu++) {
		రాయి("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

స్థిర శూన్య reset_bkl_usage(శూన్య)
{
	memset(kernel_ticks, 0, పరిమాణం(kernel_ticks));
	memset(bkl_ticks, 0, పరిమాణం(bkl_ticks));
	memset(bkl_tries, 0, పరిమాణం(bkl_tries));
	memset(bkl_succ, 0, పరిమాణం(bkl_succ));
}
#అయితే_ముగింపు

స్థిర శూన్య ser_debug(స్థిరరాశి పూర్ణాంకం c)
{
	serial_debug_చురుకైన = 1;

	ఎంపిక(c)
	{
	స్థితి 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#నిర్వచితమైతే CONFIG_SMP
	స్థితి 'B':
		dump_bkl_usage();
		ఆపు;
	స్థితి 'b':
		reset_bkl_usage();
		ఆపు;
#అయితే_ముగింపు
	స్థితి '1':
		ser_dump_proc();
		ఆపు;
	స్థితి '2':
		ser_dump_queues();
		ఆపు;
#నిర్వచితమైతే CONFIG_SMP
	స్థితి '4':
		ser_dump_proc_cpu();
		ఆపు;
#అయితే_ముగింపు
	స్థితి '5':
		ser_dump_vfs();
		ఆపు;
#ఒకవేళ DEBUG_TRACE
#నిర్వచించు TOGGLECASE(ch, flag)				\
	స్థితి ch: {					\
		ఒకవేళ(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			రాయి("%s disabled\n", #flag);	\
		} లేకపోతే {				\
			verboseflags |= flag;		\
			రాయి("%s enabled\n", #flag);	\
		}					\
		ఆపు;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#అయితే_ముగింపు
#నిర్వచితమైతే USE_APIC
	స్థితి 'I':
		dump_apic_irq_state();
		ఆపు;
#అయితే_ముగింపు
	}
	serial_debug_చురుకైన = 0;
}

#ఒకవేళ DEBUG_SERIAL

స్థిర శూన్య ser_dump_vfs(శూన్య)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#నిర్వచితమైతే CONFIG_SMP
స్థిర శూన్య ser_dump_proc_cpu(శూన్య)
{
	నిర్మాణం proc *pp;
	గుర్తులేని cpu;

	కొరకు (cpu = 0; cpu < ncpus; cpu++) {
		రాయి("CPU %d processes : \n", cpu);
		కొరకు (pp= BEG_USER_ADDR; pp < చివర_PROC_ADDR; pp++) {
			ఒకవేళ (isemptyp(pp) || pp->p_cpu != cpu)
				కొనసాగు;
			print_proc(pp);
		}
	}
}
#అయితే_ముగింపు

#అయితే_ముగింపు /* DEBUG_SERIAL */

#ఒకవేళ SPROFILE

పూర్ణాంకం arch_init_profile_clock(స్థిరరాశి u32_t freq)
{
  పూర్ణాంకం r;
  /* Set CMOS timer frequency. */
  outb(RTC_సూచీ, RTC_REG_A);
  outb(RTC_IO, RTC_A_DV_OK | freq);
  /* Enable CMOS timer interrupts. */
  outb(RTC_సూచీ, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_సూచీ, RTC_REG_B); 
  outb(RTC_IO, r | RTC_B_PIE);
  /* Mandatory read of CMOS register to enable timer interrupts. */
  outb(RTC_సూచీ, RTC_REG_C);
  inb(RTC_IO);

  తిరిగి CMOS_CLOCK_IRQ;
}

శూన్య arch_stop_profile_clock(శూన్య)
{
  పూర్ణాంకం r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_సూచీ, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_సూచీ, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

శూన్య arch_ack_profile_clock(శూన్య)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_సూచీ, RTC_REG_C);
  inb(RTC_IO);
}

#అయితే_ముగింపు

శూన్య arch_do_syscall(నిర్మాణం proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ప్రతిజ్ఞా(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  ప్రతిజ్ఞా(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

నిర్మాణం proc * arch_ముగింపు_switch_గమ్యం_user(శూన్య)
{
	అక్షరం * stk;
	నిర్మాణం proc * p;

#నిర్వచితమైతే CONFIG_SMP
	stk = (అక్షరం *)tss[cpuid].sp0;
#లేకపోతే
	stk = (అక్షరం *)tss[0].sp0;
#అయితే_ముగింపు
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_ముసుగు;

	/* Set TRACEBIT state properly. */
	ఒకవేళ(p->p_misc_flags & MF_అడుగు)
        	p->p_reg.psw |= TRACEBIT;
	లేకపోతే
        	p->p_reg.psw &= ~TRACEBIT;

	తిరిగి p;
}

శూన్య arch_proc_setcontext(నిర్మాణం proc *p, నిర్మాణం stackframe_s *state,
	పూర్ణాంకం isuser, పూర్ణాంకం trap_style)
{
	ఒకవేళ(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	ప్రతిజ్ఞా(పరిమాణం(p->p_reg) == పరిమాణం(*state));
	ఒకవేళ(state != &p->p_reg) {
		జ్ఞాపకం_నకలు(&p->p_reg, state, పరిమాణం(*state));
	}

	/* further code is instructed to not touch the context
	 * any more
	 */
	p->p_misc_flags |= MF_CONTEXT_సమితి;

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
	ఒకవేళ(!(p->p_rts_flags)) {
		రాయి("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	ఒకవేళ(p->p_seg.p_kern_trap_style == KTS_NONE)
		రాయి("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

శూన్య restore_user_context(నిర్మాణం proc *p)
{
	పూర్ణాంకం trap_style = p->p_seg.p_kern_trap_style;
#ఒకవేళ 0
#నిర్వచించు TYPES 10
	స్థిర పూర్ణాంకం restores[TYPES], n = 0;

	ఒకవేళ(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	ఒకవేళ(!(n++ % 500000)) {
		పూర్ణాంకం t;
		కొరకు(t = 0; t < TYPES; t++)
			ఒకవేళ(restores[t])
				రాయి("%d: %d   ", t, restores[t]);
		రాయి("\n");
	}
#అయితే_ముగింపు

	p->p_seg.p_kern_trap_style = KTS_NONE;

	ఒకవేళ(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	ఒకవేళ(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        ఎంపిక(trap_style) {
                స్థితి KTS_NONE:
                        panic("no entry trap style known");
                స్థితి KTS_INT_HARD:
                స్థితి KTS_INT_UM:
                స్థితి KTS_FULLCONTEXT:
                స్థితి KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                అప్రమేయ:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

శూన్య fpu_sigcontext(నిర్మాణం proc *pr, నిర్మాణం sigframe_sigcontext *fr, నిర్మాణం sigcontext *sc)
{
	పూర్ణాంకం fp_error;

	ఒకవేళ (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_స్థితి &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} లేకపోతే {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_స్థితి &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	ఒకవేళ (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_సంకేతం = FPE_FLTINV;
	} లేకపోతే ఒకవేళ (fp_error & 0x004) {
		fr->sf_సంకేతం = FPE_FLTDIV; /* Divide by Zero */
	} లేకపోతే ఒకవేళ (fp_error & 0x008) {
		fr->sf_సంకేతం = FPE_FLTOVF; /*అధికప్రవాహం*/
	} లేకపోతే ఒకవేళ (fp_error & 0x012) {
		fr->sf_సంకేతం = FPE_FLTUND; /* Denormal, Underflow */
	} లేకపోతే ఒకవేళ (fp_error & 0x020) {
		fr->sf_సంకేతం = FPE_FLTRES; /* Precision */
	} లేకపోతే {
		fr->sf_సంకేతం = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(నిర్మాణం proc *p) { తిరిగి p->p_reg.sp; }

#ఒకవేళ !CONFIG_OXPCIE
స్థిర శూన్య ser_init(శూన్య)
{
	గుర్తులేని అక్షరం lcr;
	గుర్తులేని భాజకం;

	/* keep BIOS settings if cttybaud is not set */
	ఒకవేళ (kinfo.serial_debug_baud <= 0) తిరిగి;

	/* set DLAB to make baud accessible */
	lcr = LCR_8_బిట్టు | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	భాజకం = UART_పునాది_FREQ / kinfo.serial_debug_baud;
	ఒకవేళ (భాజకం < 1) భాజకం = 1;
	ఒకవేళ (భాజకం > 65535) భాజకం = 65535;
	
	outb(COM1_DLL, భాజకం & 0xff);
	outb(COM1_DLM, (భాజకం >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#అయితే_ముగింపు
