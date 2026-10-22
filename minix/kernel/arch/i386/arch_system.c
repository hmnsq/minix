/* system dependent functions for use inside the whole kernel. */

#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <machine/cmos.h>
#ಅನ್ತರ್ಭಾವ <machine/bios.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <minix/portio.h>
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ಅನ್ತರ್ಭಾವ <minix/u64.h>

#ಅನ್ತರ್ಭಾವ "archconst.h"
#ಅನ್ತರ್ಭಾವ "oxpcie.h"

#ಅನ್ತರ್ಭಾವ "glo.h"

#ifdef USE_APIC
#ಅನ್ತರ್ಭಾವ "apic.h"
#endif

#ifdef USE_ACPI
#ಅನ್ತರ್ಭಾವ "acpi.h"
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

ಶೂನ್ಯ * k_stacks;

ಸ್ಥಿರ ಶೂನ್ಯ ser_debug(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_vfs(ಶೂನ್ಯ);

#ifdef CONFIG_SMP
ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_proc_cpu(ಶೂನ್ಯ);
#endif
#ಯದಿ !CONFIG_OXPCIE
ಸ್ಥಿರ ಶೂನ್ಯ ser_init(ಶೂನ್ಯ);
#endif

ಶೂನ್ಯ fpu_init(ಶೂನ್ಯ)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	ಯದಿ((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		ಯದಿ(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			ಯದಿ(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} ಅನ್ಯಥಾ {
			osfxsr_feature = 0;
		}
	} ಅನ್ಯಥಾ {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                ಪ್ರತ್ಯಾಗಮ;
        }
}

ಶೂನ್ಯ save_local_fpu(ಸಂರಚನಾ proc *pr, ಪೂರ್ಣಾಙ್ಕ retain)
{
	ವರ್ಣ *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	ಯದಿ(!is_fpu())
		ಪ್ರತ್ಯಾಗಮ;

	ಪ್ರತಿಜ್ಞಾ(state);

	ಯದಿ(osfxsr_feature) {
		fxsave(state);
	} ಅನ್ಯಥಾ {
		fnsave(state);
		ಯದಿ (retain)
			(ಶೂನ್ಯ) frstor(state);
	}
}

ಶೂನ್ಯ save_fpu(ಸಂರಚನಾ proc *pr)
{
#ifdef CONFIG_SMP
	ಯದಿ (cpuid != pr->p_cpu) {
		ಪೂರ್ಣಾಙ್ಕ stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		ಯದಿ (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		ಪ್ರತ್ಯಾಗಮ;
	}
#endif

	ಯದಿ (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
ಸ್ಥಿರ ವರ್ಣ fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

ಶೂನ್ಯ arch_proc_reset(ಸಂರಚನಾ proc *pr)
{
	ವರ್ಣ *v = ರಿಕ್ತ;
	ಸಂರಚನಾ stackframe_s reg;

	ಪ್ರತಿಜ್ಞಾ(pr->p_nr < NR_PROCS);

	ಯದಿ(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		ಪ್ರತಿಜ್ಞಾ(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, ಪರಿಮಾಣ(pr->p_reg));
        ಯದಿ(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        ಅನ್ಯಥಾ
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

ಶೂನ್ಯ arch_set_secondary_ipc_return(ಸಂರಚನಾ proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

ಪೂರ್ಣಾಙ್ಕ restore_fpu(ಸಂರಚನಾ proc *pr)
{
	ಪೂರ್ಣಾಙ್ಕ failed;
	ವರ್ಣ *state = pr->p_seg.fpu_state;

	ಪ್ರತಿಜ್ಞಾ(state);

	ಯದಿ(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} ಅನ್ಯಥಾ {
		ಯದಿ(osfxsr_feature) {
			failed = fxrstor(state);
		} ಅನ್ಯಥಾ {
			failed = frstor(state);
		}

		ಯದಿ (failed) ಪ್ರತ್ಯಾಗಮ EINVAL;
	}

	ಪ್ರತ್ಯಾಗಮ OK;
}

ಶೂನ್ಯ cpu_identify(ಶೂನ್ಯ)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	ಯದಿ (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} ಅನ್ಯಥಾ ಯದಿ (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} ಅನ್ಯಥಾ
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	ಯದಿ (eax == 0) 
		ಪ್ರತ್ಯಾಗಮ;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	ಯದಿ (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	ಯದಿ (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

ಶೂನ್ಯ arch_init(ಶೂನ್ಯ)
{
	k_stacks = (ಶೂನ್ಯ*) &k_stacks_start;
	ಪ್ರತಿಜ್ಞಾ(!((vir_bytes) k_stacks % K_ರಾಶಿ_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ರಾಶಿ_ತುದಿ(0));
#endif

#ಯದಿ !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#ಯದಿ defined(USE_APIC) && !defined(CONFIG_SMP)
	ಯದಿ (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	ಅನ್ಯಥಾ ಯದಿ (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_ತುದಿ, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
ಶೂನ್ಯ do_ser_debug(ಶೂನ್ಯ)
{
	u8_t c, lsr;

#ಯದಿ CONFIG_OXPCIE
	{
		ಪೂರ್ಣಾಙ್ಕ oxin;
		ಯದಿ((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	ಯದಿ (!(lsr & LSR_DR))
		ಪ್ರತ್ಯಾಗಮ;
	c = inb(COM1_RBR);
	ser_debug(c);
}

ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_queue_cpu(unsigned cpu)
{
	ಪೂರ್ಣಾಙ್ಕ q;
	ಸಂರಚನಾ proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	ಪ್ರತಿ(q = 0; q < NR_SCHED_QUEUES; q++) {
		ಸಂರಚನಾ proc *p;
		ಯದಿ(rdy_head[q])	 {
			printf("%2d: ", q);
			ಪ್ರತಿ(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_queues(ಶೂನ್ಯ)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	ಪ್ರತಿ (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#ಅನ್ಯಥಾ
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
ಸ್ಥಿರ ಶೂನ್ಯ dump_bkl_usage(ಶೂನ್ಯ)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	ಪ್ರತಿ (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ reset_bkl_usage(ಶೂನ್ಯ)
{
	memset(kernel_ticks, 0, ಪರಿಮಾಣ(kernel_ticks));
	memset(bkl_ticks, 0, ಪರಿಮಾಣ(bkl_ticks));
	memset(bkl_tries, 0, ಪರಿಮಾಣ(bkl_tries));
	memset(bkl_succ, 0, ಪರಿಮಾಣ(bkl_succ));
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ ser_debug(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ c)
{
	serial_debug_active = 1;

	ವಿಕಲ್ಪ(c)
	{
	ಪಕ್ಷ 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	ಪಕ್ಷ 'B':
		dump_bkl_usage();
		ಭಙ್ಗ;
	ಪಕ್ಷ 'b':
		reset_bkl_usage();
		ಭಙ್ಗ;
#endif
	ಪಕ್ಷ '1':
		ser_dump_proc();
		ಭಙ್ಗ;
	ಪಕ್ಷ '2':
		ser_dump_queues();
		ಭಙ್ಗ;
#ifdef CONFIG_SMP
	ಪಕ್ಷ '4':
		ser_dump_proc_cpu();
		ಭಙ್ಗ;
#endif
	ಪಕ್ಷ '5':
		ser_dump_vfs();
		ಭಙ್ಗ;
#ಯದಿ DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	ಪಕ್ಷ ch: {					\
		ಯದಿ(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} ಅನ್ಯಥಾ {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		ಭಙ್ಗ;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	ಪಕ್ಷ 'I':
		dump_apic_irq_state();
		ಭಙ್ಗ;
#endif
	}
	serial_debug_active = 0;
}

#ಯದಿ DEBUG_SERIAL

ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_vfs(ಶೂನ್ಯ)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_proc_cpu(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ proc *pp;
	unsigned cpu;

	ಪ್ರತಿ (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		ಪ್ರತಿ (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			ಯದಿ (isemptyp(pp) || pp->p_cpu != cpu)
				ಅನುವರ್ತ;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#ಯದಿ SPROFILE

ಪೂರ್ಣಾಙ್ಕ arch_init_profile_clock(ಧ್ರುವ u32_t freq)
{
  ಪೂರ್ಣಾಙ್ಕ r;
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

  ಪ್ರತ್ಯಾಗಮ CMOS_CLOCK_IRQ;
}

ಶೂನ್ಯ arch_stop_profile_clock(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

ಶೂನ್ಯ arch_ack_profile_clock(ಶೂನ್ಯ)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

ಶೂನ್ಯ arch_do_syscall(ಸಂರಚನಾ proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ಪ್ರತಿಜ್ಞಾ(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  ಪ್ರತಿಜ್ಞಾ(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

ಸಂರಚನಾ proc * arch_finish_switch_to_user(ಶೂನ್ಯ)
{
	ವರ್ಣ * stk;
	ಸಂರಚನಾ proc * p;

#ifdef CONFIG_SMP
	stk = (ವರ್ಣ *)tss[cpuid].sp0;
#ಅನ್ಯಥಾ
	stk = (ವರ್ಣ *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	ಯದಿ(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	ಅನ್ಯಥಾ
        	p->p_reg.psw &= ~TRACEBIT;

	ಪ್ರತ್ಯಾಗಮ p;
}

ಶೂನ್ಯ arch_proc_setcontext(ಸಂರಚನಾ proc *p, ಸಂರಚನಾ stackframe_s *state,
	ಪೂರ್ಣಾಙ್ಕ isuser, ಪೂರ್ಣಾಙ್ಕ trap_style)
{
	ಯದಿ(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(p->p_reg) == ಪರಿಮಾಣ(*state));
	ಯದಿ(state != &p->p_reg) {
		memcpy(&p->p_reg, state, ಪರಿಮಾಣ(*state));
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
	ಯದಿ(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	ಯದಿ(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

ಶೂನ್ಯ restore_user_context(ಸಂರಚನಾ proc *p)
{
	ಪೂರ್ಣಾಙ್ಕ trap_style = p->p_seg.p_kern_trap_style;
#ಯದಿ 0
#define TYPES 10
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ restores[TYPES], n = 0;

	ಯದಿ(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	ಯದಿ(!(n++ % 500000)) {
		ಪೂರ್ಣಾಙ್ಕ t;
		ಪ್ರತಿ(t = 0; t < TYPES; t++)
			ಯದಿ(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	ಯದಿ(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	ಯದಿ(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        ವಿಕಲ್ಪ(trap_style) {
                ಪಕ್ಷ KTS_NONE:
                        panic("no entry trap style known");
                ಪಕ್ಷ KTS_INT_HARD:
                ಪಕ್ಷ KTS_INT_UM:
                ಪಕ್ಷ KTS_FULLCONTEXT:
                ಪಕ್ಷ KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                ಸ್ವಭಾವ:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

ಶೂನ್ಯ fpu_sigcontext(ಸಂರಚನಾ proc *pr, ಸಂರಚನಾ sigframe_sigcontext *fr, ಸಂರಚನಾ sigcontext *sc)
{
	ಪೂರ್ಣಾಙ್ಕ fp_error;

	ಯದಿ (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} ಅನ್ಯಥಾ {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	ಯದಿ (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} ಅನ್ಯಥಾ ಯದಿ (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} ಅನ್ಯಥಾ ಯದಿ (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} ಅನ್ಯಥಾ ಯದಿ (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} ಅನ್ಯಥಾ ಯದಿ (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} ಅನ್ಯಥಾ {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(ಸಂರಚನಾ proc *p) { ಪ್ರತ್ಯಾಗಮ p->p_reg.sp; }

#ಯದಿ !CONFIG_OXPCIE
ಸ್ಥಿರ ಶೂನ್ಯ ser_init(ಶೂನ್ಯ)
{
	unsigned ವರ್ಣ lcr;
	unsigned ಭಾಜಕ;

	/* keep BIOS settings if cttybaud is not set */
	ಯದಿ (kinfo.serial_debug_baud <= 0) ಪ್ರತ್ಯಾಗಮ;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	ಭಾಜಕ = UART_BASE_FREQ / kinfo.serial_debug_baud;
	ಯದಿ (ಭಾಜಕ < 1) ಭಾಜಕ = 1;
	ಯದಿ (ಭಾಜಕ > 65535) ಭಾಜಕ = 65535;
	
	outb(COM1_DLL, ಭಾಜಕ & 0xff);
	outb(COM1_DLM, (ಭಾಜಕ >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
