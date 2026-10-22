/* system dependent functions for use inside the whole kernel. */

#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <ctype.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <machine/cmos.h>
#অন্তর্ভাৱ <machine/bios.h>
#অন্তর্ভাৱ <machine/cpu.h>
#অন্তর্ভাৱ <minix/portio.h>
#অন্তর্ভাৱ <minix/cpufeature.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <machine/vm.h>

#অন্তর্ভাৱ <minix/u64.h>

#অন্তর্ভাৱ "archconst.h"
#অন্তর্ভাৱ "oxpcie.h"

#অন্তর্ভাৱ "glo.h"

#ifdef USE_APIC
#অন্তর্ভাৱ "apic.h"
#endif

#ifdef USE_ACPI
#অন্তর্ভাৱ "acpi.h"
#endif

স্থির পূর্ণাঙ্ক osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

শূন্য * k_stacks;

স্থির শূন্য ser_debug(পূর্ণাঙ্ক c);
স্থির শূন্য ser_dump_vfs(শূন্য);

#ifdef CONFIG_SMP
স্থির শূন্য ser_dump_proc_cpu(শূন্য);
#endif
#যদি !CONFIG_OXPCIE
স্থির শূন্য ser_init(শূন্য);
#endif

শূন্য fpu_init(শূন্য)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	যদি((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		যদি(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			যদি(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} অন্যথা {
			osfxsr_feature = 0;
		}
	} অন্যথা {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                ঘূৰাই;
        }
}

শূন্য save_local_fpu(সংরচনা proc *pr, পূর্ণাঙ্ক retain)
{
	ৱর্ণ *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	যদি(!is_fpu())
		ঘূৰাই;

	প্রতিজ্ঞা(state);

	যদি(osfxsr_feature) {
		fxsave(state);
	} অন্যথা {
		fnsave(state);
		যদি (retain)
			(শূন্য) frstor(state);
	}
}

শূন্য save_fpu(সংরচনা proc *pr)
{
#ifdef CONFIG_SMP
	যদি (cpuid != pr->p_cpu) {
		পূর্ণাঙ্ক stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		যদি (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		ঘূৰাই;
	}
#endif

	যদি (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
স্থির ৱর্ণ fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

শূন্য arch_proc_reset(সংরচনা proc *pr)
{
	ৱর্ণ *v = রিক্ত;
	সংরচনা stackframe_s reg;

	প্রতিজ্ঞা(pr->p_nr < NR_PROCS);

	যদি(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		প্রতিজ্ঞা(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, পরিমাণ(pr->p_reg));
        যদি(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        অন্যথা
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

শূন্য arch_set_secondary_ipc_return(সংরচনা proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

পূর্ণাঙ্ক restore_fpu(সংরচনা proc *pr)
{
	পূর্ণাঙ্ক failed;
	ৱর্ণ *state = pr->p_seg.fpu_state;

	প্রতিজ্ঞা(state);

	যদি(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} অন্যথা {
		যদি(osfxsr_feature) {
			failed = fxrstor(state);
		} অন্যথা {
			failed = frstor(state);
		}

		যদি (failed) ঘূৰাই EINVAL;
	}

	ঘূৰাই OK;
}

শূন্য cpu_identify(শূন্য)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	যদি (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} অন্যথা যদি (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} অন্যথা
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	যদি (eax == 0) 
		ঘূৰাই;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	যদি (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	যদি (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

শূন্য arch_init(শূন্য)
{
	k_stacks = (শূন্য*) &k_stacks_start;
	প্রতিজ্ঞা(!((vir_bytes) k_stacks % K_স্তূপ_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_স্তূপ_শীৰ্ষ(0));
#endif

#যদি !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#যদি defined(USE_APIC) && !defined(CONFIG_SMP)
	যদি (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	অন্যথা যদি (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_শীৰ্ষ, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
শূন্য do_ser_debug(শূন্য)
{
	u8_t c, lsr;

#যদি CONFIG_OXPCIE
	{
		পূর্ণাঙ্ক oxin;
		যদি((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	যদি (!(lsr & LSR_DR))
		ঘূৰাই;
	c = inb(COM1_RBR);
	ser_debug(c);
}

স্থির শূন্য ser_dump_queue_cpu(unsigned cpu)
{
	পূর্ণাঙ্ক q;
	সংরচনা proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	বাবে(q = 0; q < NR_SCHED_QUEUES; q++) {
		সংরচনা proc *p;
		যদি(rdy_head[q])	 {
			printf("%2d: ", q);
			বাবে(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

স্থির শূন্য ser_dump_queues(শূন্য)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	বাবে (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#অন্যথা
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
স্থির শূন্য dump_bkl_usage(শূন্য)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	বাবে (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

স্থির শূন্য reset_bkl_usage(শূন্য)
{
	memset(kernel_ticks, 0, পরিমাণ(kernel_ticks));
	memset(bkl_ticks, 0, পরিমাণ(bkl_ticks));
	memset(bkl_tries, 0, পরিমাণ(bkl_tries));
	memset(bkl_succ, 0, পরিমাণ(bkl_succ));
}
#endif

স্থির শূন্য ser_debug(ধ্রুৱ পূর্ণাঙ্ক c)
{
	serial_debug_active = 1;

	ৱিকল্প(c)
	{
	পক্ষ 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	পক্ষ 'B':
		dump_bkl_usage();
		ভঙ্গ;
	পক্ষ 'b':
		reset_bkl_usage();
		ভঙ্গ;
#endif
	পক্ষ '1':
		ser_dump_proc();
		ভঙ্গ;
	পক্ষ '2':
		ser_dump_queues();
		ভঙ্গ;
#ifdef CONFIG_SMP
	পক্ষ '4':
		ser_dump_proc_cpu();
		ভঙ্গ;
#endif
	পক্ষ '5':
		ser_dump_vfs();
		ভঙ্গ;
#যদি DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	পক্ষ ch: {					\
		যদি(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} অন্যথা {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		ভঙ্গ;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	পক্ষ 'I':
		dump_apic_irq_state();
		ভঙ্গ;
#endif
	}
	serial_debug_active = 0;
}

#যদি DEBUG_SERIAL

স্থির শূন্য ser_dump_vfs(শূন্য)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
স্থির শূন্য ser_dump_proc_cpu(শূন্য)
{
	সংরচনা proc *pp;
	unsigned cpu;

	বাবে (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		বাবে (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			যদি (isemptyp(pp) || pp->p_cpu != cpu)
				অনুৱর্ত;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#যদি SPROFILE

পূর্ণাঙ্ক arch_init_profile_clock(ধ্রুৱ u32_t freq)
{
  পূর্ণাঙ্ক r;
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

  ঘূৰাই CMOS_CLOCK_IRQ;
}

শূন্য arch_stop_profile_clock(শূন্য)
{
  পূর্ণাঙ্ক r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

শূন্য arch_ack_profile_clock(শূন্য)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

শূন্য arch_do_syscall(সংরচনা proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  প্রতিজ্ঞা(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  প্রতিজ্ঞা(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

সংরচনা proc * arch_finish_switch_to_user(শূন্য)
{
	ৱর্ণ * stk;
	সংরচনা proc * p;

#ifdef CONFIG_SMP
	stk = (ৱর্ণ *)tss[cpuid].sp0;
#অন্যথা
	stk = (ৱর্ণ *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	যদি(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	অন্যথা
        	p->p_reg.psw &= ~TRACEBIT;

	ঘূৰাই p;
}

শূন্য arch_proc_setcontext(সংরচনা proc *p, সংরচনা stackframe_s *state,
	পূর্ণাঙ্ক isuser, পূর্ণাঙ্ক trap_style)
{
	যদি(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	প্রতিজ্ঞা(পরিমাণ(p->p_reg) == পরিমাণ(*state));
	যদি(state != &p->p_reg) {
		memcpy(&p->p_reg, state, পরিমাণ(*state));
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
	যদি(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	যদি(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

শূন্য restore_user_context(সংরচনা proc *p)
{
	পূর্ণাঙ্ক trap_style = p->p_seg.p_kern_trap_style;
#যদি 0
#define TYPES 10
	স্থির পূর্ণাঙ্ক restores[TYPES], n = 0;

	যদি(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	যদি(!(n++ % 500000)) {
		পূর্ণাঙ্ক t;
		বাবে(t = 0; t < TYPES; t++)
			যদি(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	যদি(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	যদি(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        ৱিকল্প(trap_style) {
                পক্ষ KTS_NONE:
                        panic("no entry trap style known");
                পক্ষ KTS_INT_HARD:
                পক্ষ KTS_INT_UM:
                পক্ষ KTS_FULLCONTEXT:
                পক্ষ KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                স্ৱভাৱ:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

শূন্য fpu_sigcontext(সংরচনা proc *pr, সংরচনা sigframe_sigcontext *fr, সংরচনা sigcontext *sc)
{
	পূর্ণাঙ্ক fp_error;

	যদি (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} অন্যথা {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	যদি (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} অন্যথা যদি (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} অন্যথা যদি (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} অন্যথা যদি (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} অন্যথা যদি (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} অন্যথা {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(সংরচনা proc *p) { ঘূৰাই p->p_reg.sp; }

#যদি !CONFIG_OXPCIE
স্থির শূন্য ser_init(শূন্য)
{
	unsigned ৱর্ণ lcr;
	unsigned ভাজক;

	/* keep BIOS settings if cttybaud is not set */
	যদি (kinfo.serial_debug_baud <= 0) ঘূৰাই;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	ভাজক = UART_BASE_FREQ / kinfo.serial_debug_baud;
	যদি (ভাজক < 1) ভাজক = 1;
	যদি (ভাজক > 65535) ভাজক = 65535;
	
	outb(COM1_DLL, ভাজক & 0xff);
	outb(COM1_DLM, (ভাজক >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
