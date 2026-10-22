/* system dependent functions for use inside the whole kernel. */

#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <machine/cmos.h>
#அந்தர்பாவ <machine/bios.h>
#அந்தர்பாவ <machine/cpu.h>
#அந்தர்பாவ <minix/portio.h>
#அந்தர்பாவ <minix/cpufeature.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <machine/vm.h>

#அந்தர்பாவ <minix/u64.h>

#அந்தர்பாவ "archconst.h"
#அந்தர்பாவ "oxpcie.h"

#அந்தர்பாவ "glo.h"

#ifdef USE_APIC
#அந்தர்பாவ "apic.h"
#endif

#ifdef USE_ACPI
#அந்தர்பாவ "acpi.h"
#endif

நிலையான முழு osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

வெறுமை * k_stacks;

நிலையான வெறுமை ser_debug(முழு c);
நிலையான வெறுமை ser_dump_vfs(வெறுமை);

#ifdef CONFIG_SMP
நிலையான வெறுமை ser_dump_proc_cpu(வெறுமை);
#endif
#எனில் !CONFIG_OXPCIE
நிலையான வெறுமை ser_init(வெறுமை);
#endif

வெறுமை fpu_init(வெறுமை)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	எனில்((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		எனில்(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			எனில்(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} இல்லையெனில் {
			osfxsr_feature = 0;
		}
	} இல்லையெனில் {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                திரும்பு;
        }
}

வெறுமை save_local_fpu(கட்டமைப்பு proc *pr, முழு retain)
{
	எழுத்து *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	எனில்(!is_fpu())
		திரும்பு;

	ப்ரதிஜ்ஞா(state);

	எனில்(osfxsr_feature) {
		fxsave(state);
	} இல்லையெனில் {
		fnsave(state);
		எனில் (retain)
			(வெறுமை) frstor(state);
	}
}

வெறுமை save_fpu(கட்டமைப்பு proc *pr)
{
#ifdef CONFIG_SMP
	எனில் (cpuid != pr->p_cpu) {
		முழு stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		எனில் (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		திரும்பு;
	}
#endif

	எனில் (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
நிலையான எழுத்து fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

வெறுமை arch_proc_reset(கட்டமைப்பு proc *pr)
{
	எழுத்து *v = வெற்று;
	கட்டமைப்பு stackframe_s reg;

	ப்ரதிஜ்ஞா(pr->p_nr < NR_PROCS);

	எனில்(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		ப்ரதிஜ்ஞா(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, அளவு(pr->p_reg));
        எனில்(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        இல்லையெனில்
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

வெறுமை arch_set_secondary_ipc_return(கட்டமைப்பு proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

முழு restore_fpu(கட்டமைப்பு proc *pr)
{
	முழு failed;
	எழுத்து *state = pr->p_seg.fpu_state;

	ப்ரதிஜ்ஞா(state);

	எனில்(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} இல்லையெனில் {
		எனில்(osfxsr_feature) {
			failed = fxrstor(state);
		} இல்லையெனில் {
			failed = frstor(state);
		}

		எனில் (failed) திரும்பு EINVAL;
	}

	திரும்பு OK;
}

வெறுமை cpu_identify(வெறுமை)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	எனில் (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} இல்லையெனில் எனில் (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} இல்லையெனில்
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	எனில் (eax == 0) 
		திரும்பு;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	எனில் (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	எனில் (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

வெறுமை arch_init(வெறுமை)
{
	k_stacks = (வெறுமை*) &k_stacks_start;
	ப்ரதிஜ்ஞா(!((vir_bytes) k_stacks % K_அடுக்கு_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_அடுக்கு_உச்சி(0));
#endif

#எனில் !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#எனில் defined(USE_APIC) && !defined(CONFIG_SMP)
	எனில் (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	இல்லையெனில் எனில் (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_உச்சி, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
வெறுமை do_ser_debug(வெறுமை)
{
	u8_t c, lsr;

#எனில் CONFIG_OXPCIE
	{
		முழு oxin;
		எனில்((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	எனில் (!(lsr & LSR_DR))
		திரும்பு;
	c = inb(COM1_RBR);
	ser_debug(c);
}

நிலையான வெறுமை ser_dump_queue_cpu(unsigned cpu)
{
	முழு q;
	கட்டமைப்பு proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	க்கு(q = 0; q < NR_SCHED_QUEUES; q++) {
		கட்டமைப்பு proc *p;
		எனில்(rdy_head[q])	 {
			printf("%2d: ", q);
			க்கு(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

நிலையான வெறுமை ser_dump_queues(வெறுமை)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	க்கு (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#இல்லையெனில்
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
நிலையான வெறுமை dump_bkl_usage(வெறுமை)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	க்கு (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

நிலையான வெறுமை reset_bkl_usage(வெறுமை)
{
	memset(kernel_ticks, 0, அளவு(kernel_ticks));
	memset(bkl_ticks, 0, அளவு(bkl_ticks));
	memset(bkl_tries, 0, அளவு(bkl_tries));
	memset(bkl_succ, 0, அளவு(bkl_succ));
}
#endif

நிலையான வெறுமை ser_debug(மாறிலி முழு c)
{
	serial_debug_active = 1;

	தேர்வு(c)
	{
	நிலை 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	நிலை 'B':
		dump_bkl_usage();
		நிறுத்து;
	நிலை 'b':
		reset_bkl_usage();
		நிறுத்து;
#endif
	நிலை '1':
		ser_dump_proc();
		நிறுத்து;
	நிலை '2':
		ser_dump_queues();
		நிறுத்து;
#ifdef CONFIG_SMP
	நிலை '4':
		ser_dump_proc_cpu();
		நிறுத்து;
#endif
	நிலை '5':
		ser_dump_vfs();
		நிறுத்து;
#எனில் DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	நிலை ch: {					\
		எனில்(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} இல்லையெனில் {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		நிறுத்து;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	நிலை 'I':
		dump_apic_irq_state();
		நிறுத்து;
#endif
	}
	serial_debug_active = 0;
}

#எனில் DEBUG_SERIAL

நிலையான வெறுமை ser_dump_vfs(வெறுமை)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
நிலையான வெறுமை ser_dump_proc_cpu(வெறுமை)
{
	கட்டமைப்பு proc *pp;
	unsigned cpu;

	க்கு (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		க்கு (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			எனில் (isemptyp(pp) || pp->p_cpu != cpu)
				தொடர்;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#எனில் SPROFILE

முழு arch_init_profile_clock(மாறிலி u32_t freq)
{
  முழு r;
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

  திரும்பு CMOS_CLOCK_IRQ;
}

வெறுமை arch_stop_profile_clock(வெறுமை)
{
  முழு r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

வெறுமை arch_ack_profile_clock(வெறுமை)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

வெறுமை arch_do_syscall(கட்டமைப்பு proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ப்ரதிஜ்ஞா(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  ப்ரதிஜ்ஞா(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

கட்டமைப்பு proc * arch_finish_switch_to_user(வெறுமை)
{
	எழுத்து * stk;
	கட்டமைப்பு proc * p;

#ifdef CONFIG_SMP
	stk = (எழுத்து *)tss[cpuid].sp0;
#இல்லையெனில்
	stk = (எழுத்து *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	எனில்(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	இல்லையெனில்
        	p->p_reg.psw &= ~TRACEBIT;

	திரும்பு p;
}

வெறுமை arch_proc_setcontext(கட்டமைப்பு proc *p, கட்டமைப்பு stackframe_s *state,
	முழு isuser, முழு trap_style)
{
	எனில்(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	ப்ரதிஜ்ஞா(அளவு(p->p_reg) == அளவு(*state));
	எனில்(state != &p->p_reg) {
		memcpy(&p->p_reg, state, அளவு(*state));
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
	எனில்(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	எனில்(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

வெறுமை restore_user_context(கட்டமைப்பு proc *p)
{
	முழு trap_style = p->p_seg.p_kern_trap_style;
#எனில் 0
#define TYPES 10
	நிலையான முழு restores[TYPES], n = 0;

	எனில்(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	எனில்(!(n++ % 500000)) {
		முழு t;
		க்கு(t = 0; t < TYPES; t++)
			எனில்(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	எனில்(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	எனில்(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        தேர்வு(trap_style) {
                நிலை KTS_NONE:
                        panic("no entry trap style known");
                நிலை KTS_INT_HARD:
                நிலை KTS_INT_UM:
                நிலை KTS_FULLCONTEXT:
                நிலை KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                இயல்பு:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

வெறுமை fpu_sigcontext(கட்டமைப்பு proc *pr, கட்டமைப்பு sigframe_sigcontext *fr, கட்டமைப்பு sigcontext *sc)
{
	முழு fp_error;

	எனில் (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} இல்லையெனில் {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	எனில் (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} இல்லையெனில் எனில் (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} இல்லையெனில் எனில் (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} இல்லையெனில் எனில் (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} இல்லையெனில் எனில் (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} இல்லையெனில் {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(கட்டமைப்பு proc *p) { திரும்பு p->p_reg.sp; }

#எனில் !CONFIG_OXPCIE
நிலையான வெறுமை ser_init(வெறுமை)
{
	unsigned எழுத்து lcr;
	unsigned வகுஎண்;

	/* keep BIOS settings if cttybaud is not set */
	எனில் (kinfo.serial_debug_baud <= 0) திரும்பு;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	வகுஎண் = UART_BASE_FREQ / kinfo.serial_debug_baud;
	எனில் (வகுஎண் < 1) வகுஎண் = 1;
	எனில் (வகுஎண் > 65535) வகுஎண் = 65535;
	
	outb(COM1_DLL, வகுஎண் & 0xff);
	outb(COM1_DLM, (வகுஎண் >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
