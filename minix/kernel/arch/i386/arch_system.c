/* system dependent functions for use inside the whole kernel. */

#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <machine/cmos.h>
#ଅନ୍ତର୍ଭାଵ <machine/bios.h>
#ଅନ୍ତର୍ଭାଵ <machine/cpu.h>
#ଅନ୍ତର୍ଭାଵ <minix/portio.h>
#ଅନ୍ତର୍ଭାଵ <minix/cpufeature.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <machine/vm.h>

#ଅନ୍ତର୍ଭାଵ <minix/u64.h>

#ଅନ୍ତର୍ଭାଵ "archconst.h"
#ଅନ୍ତର୍ଭାଵ "oxpcie.h"

#ଅନ୍ତର୍ଭାଵ "glo.h"

#ifdef USE_APIC
#ଅନ୍ତର୍ଭାଵ "apic.h"
#endif

#ifdef USE_ACPI
#ଅନ୍ତର୍ଭାଵ "acpi.h"
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

ଶୂନ୍ୟ * k_stacks;

ସ୍ଥିର ଶୂନ୍ୟ ser_debug(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ଶୂନ୍ୟ ser_dump_vfs(ଶୂନ୍ୟ);

#ifdef CONFIG_SMP
ସ୍ଥିର ଶୂନ୍ୟ ser_dump_proc_cpu(ଶୂନ୍ୟ);
#endif
#ଯଦି !CONFIG_OXPCIE
ସ୍ଥିର ଶୂନ୍ୟ ser_init(ଶୂନ୍ୟ);
#endif

ଶୂନ୍ୟ fpu_init(ଶୂନ୍ୟ)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	ଯଦି((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		ଯଦି(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			ଯଦି(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} ଅନ୍ୟଥା {
			osfxsr_feature = 0;
		}
	} ଅନ୍ୟଥା {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                ଫେରନ୍ତୁ;
        }
}

ଶୂନ୍ୟ save_local_fpu(ସଂରଚନା proc *pr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain)
{
	ଅକ୍ଷର *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	ଯଦି(!is_fpu())
		ଫେରନ୍ତୁ;

	ପ୍ରତିଜ୍ଞା(state);

	ଯଦି(osfxsr_feature) {
		fxsave(state);
	} ଅନ୍ୟଥା {
		fnsave(state);
		ଯଦି (retain)
			(ଶୂନ୍ୟ) frstor(state);
	}
}

ଶୂନ୍ୟ save_fpu(ସଂରଚନା proc *pr)
{
#ifdef CONFIG_SMP
	ଯଦି (cpuid != pr->p_cpu) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		ଯଦି (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		ଫେରନ୍ତୁ;
	}
#endif

	ଯଦି (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
ସ୍ଥିର ଅକ୍ଷର fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

ଶୂନ୍ୟ arch_proc_reset(ସଂରଚନା proc *pr)
{
	ଅକ୍ଷର *v = ଖାଲି;
	ସଂରଚନା stackframe_s reg;

	ପ୍ରତିଜ୍ଞା(pr->p_nr < NR_PROCS);

	ଯଦି(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		ପ୍ରତିଜ୍ଞା(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, ଆକାର(pr->p_reg));
        ଯଦି(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        ଅନ୍ୟଥା
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

ଶୂନ୍ୟ arch_set_secondary_ipc_return(ସଂରଚନା proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା restore_fpu(ସଂରଚନା proc *pr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା failed;
	ଅକ୍ଷର *state = pr->p_seg.fpu_state;

	ପ୍ରତିଜ୍ଞା(state);

	ଯଦି(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} ଅନ୍ୟଥା {
		ଯଦି(osfxsr_feature) {
			failed = fxrstor(state);
		} ଅନ୍ୟଥା {
			failed = frstor(state);
		}

		ଯଦି (failed) ଫେରନ୍ତୁ EINVAL;
	}

	ଫେରନ୍ତୁ OK;
}

ଶୂନ୍ୟ cpu_identify(ଶୂନ୍ୟ)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	ଯଦି (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} ଅନ୍ୟଥା ଯଦି (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} ଅନ୍ୟଥା
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	ଯଦି (eax == 0) 
		ଫେରନ୍ତୁ;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	ଯଦି (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	ଯଦି (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

ଶୂନ୍ୟ arch_init(ଶୂନ୍ୟ)
{
	k_stacks = (ଶୂନ୍ୟ*) &k_stacks_start;
	ପ୍ରତିଜ୍ଞା(!((vir_bytes) k_stacks % K_ଗଦା_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ଗଦା_ଶୀର୍ଷ(0));
#endif

#ଯଦି !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#ଯଦି defined(USE_APIC) && !defined(CONFIG_SMP)
	ଯଦି (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	ଅନ୍ୟଥା ଯଦି (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_ଶୀର୍ଷ, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
ଶୂନ୍ୟ do_ser_debug(ଶୂନ୍ୟ)
{
	u8_t c, lsr;

#ଯଦି CONFIG_OXPCIE
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା oxin;
		ଯଦି((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	ଯଦି (!(lsr & LSR_DR))
		ଫେରନ୍ତୁ;
	c = inb(COM1_RBR);
	ser_debug(c);
}

ସ୍ଥିର ଶୂନ୍ୟ ser_dump_queue_cpu(unsigned cpu)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା q;
	ସଂରଚନା proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	ପାଇଁ(q = 0; q < NR_SCHED_QUEUES; q++) {
		ସଂରଚନା proc *p;
		ଯଦି(rdy_head[q])	 {
			printf("%2d: ", q);
			ପାଇଁ(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

ସ୍ଥିର ଶୂନ୍ୟ ser_dump_queues(ଶୂନ୍ୟ)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	ପାଇଁ (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#ଅନ୍ୟଥା
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
ସ୍ଥିର ଶୂନ୍ୟ dump_bkl_usage(ଶୂନ୍ୟ)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	ପାଇଁ (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

ସ୍ଥିର ଶୂନ୍ୟ reset_bkl_usage(ଶୂନ୍ୟ)
{
	memset(kernel_ticks, 0, ଆକାର(kernel_ticks));
	memset(bkl_ticks, 0, ଆକାର(bkl_ticks));
	memset(bkl_tries, 0, ଆକାର(bkl_tries));
	memset(bkl_succ, 0, ଆକାର(bkl_succ));
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ ser_debug(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
	serial_debug_active = 1;

	ବାଛ(c)
	{
	ସ୍ଥିତି 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	ସ୍ଥିତି 'B':
		dump_bkl_usage();
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 'b':
		reset_bkl_usage();
		ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି '1':
		ser_dump_proc();
		ଭାଙ୍ଗ;
	ସ୍ଥିତି '2':
		ser_dump_queues();
		ଭାଙ୍ଗ;
#ifdef CONFIG_SMP
	ସ୍ଥିତି '4':
		ser_dump_proc_cpu();
		ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି '5':
		ser_dump_vfs();
		ଭାଙ୍ଗ;
#ଯଦି DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	ସ୍ଥିତି ch: {					\
		ଯଦି(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} ଅନ୍ୟଥା {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		ଭାଙ୍ଗ;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	ସ୍ଥିତି 'I':
		dump_apic_irq_state();
		ଭାଙ୍ଗ;
#endif
	}
	serial_debug_active = 0;
}

#ଯଦି DEBUG_SERIAL

ସ୍ଥିର ଶୂନ୍ୟ ser_dump_vfs(ଶୂନ୍ୟ)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
ସ୍ଥିର ଶୂନ୍ୟ ser_dump_proc_cpu(ଶୂନ୍ୟ)
{
	ସଂରଚନା proc *pp;
	unsigned cpu;

	ପାଇଁ (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		ପାଇଁ (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			ଯଦି (isemptyp(pp) || pp->p_cpu != cpu)
				ଜାରି;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#ଯଦି SPROFILE

ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_init_profile_clock(ଅଚଳ u32_t freq)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
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

  ଫେରନ୍ତୁ CMOS_CLOCK_IRQ;
}

ଶୂନ୍ୟ arch_stop_profile_clock(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

ଶୂନ୍ୟ arch_ack_profile_clock(ଶୂନ୍ୟ)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

ଶୂନ୍ୟ arch_do_syscall(ସଂରଚନା proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ପ୍ରତିଜ୍ଞା(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  ପ୍ରତିଜ୍ଞା(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

ସଂରଚନା proc * arch_finish_switch_to_user(ଶୂନ୍ୟ)
{
	ଅକ୍ଷର * stk;
	ସଂରଚନା proc * p;

#ifdef CONFIG_SMP
	stk = (ଅକ୍ଷର *)tss[cpuid].sp0;
#ଅନ୍ୟଥା
	stk = (ଅକ୍ଷର *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	ଯଦି(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	ଅନ୍ୟଥା
        	p->p_reg.psw &= ~TRACEBIT;

	ଫେରନ୍ତୁ p;
}

ଶୂନ୍ୟ arch_proc_setcontext(ସଂରଚନା proc *p, ସଂରଚନା stackframe_s *state,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା isuser, ପୂର୍ଣ୍ଣସଂଖ୍ୟା trap_style)
{
	ଯଦି(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	ପ୍ରତିଜ୍ଞା(ଆକାର(p->p_reg) == ଆକାର(*state));
	ଯଦି(state != &p->p_reg) {
		memcpy(&p->p_reg, state, ଆକାର(*state));
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
	ଯଦି(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	ଯଦି(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

ଶୂନ୍ୟ restore_user_context(ସଂରଚନା proc *p)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା trap_style = p->p_seg.p_kern_trap_style;
#ଯଦି 0
#define TYPES 10
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା restores[TYPES], n = 0;

	ଯଦି(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	ଯଦି(!(n++ % 500000)) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା t;
		ପାଇଁ(t = 0; t < TYPES; t++)
			ଯଦି(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	ଯଦି(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	ଯଦି(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        ବାଛ(trap_style) {
                ସ୍ଥିତି KTS_NONE:
                        panic("no entry trap style known");
                ସ୍ଥିତି KTS_INT_HARD:
                ସ୍ଥିତି KTS_INT_UM:
                ସ୍ଥିତି KTS_FULLCONTEXT:
                ସ୍ଥିତି KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

ଶୂନ୍ୟ fpu_sigcontext(ସଂରଚନା proc *pr, ସଂରଚନା sigframe_sigcontext *fr, ସଂରଚନା sigcontext *sc)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fp_error;

	ଯଦି (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} ଅନ୍ୟଥା {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	ଯଦି (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} ଅନ୍ୟଥା ଯଦି (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} ଅନ୍ୟଥା ଯଦି (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} ଅନ୍ୟଥା ଯଦି (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} ଅନ୍ୟଥା ଯଦି (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} ଅନ୍ୟଥା {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(ସଂରଚନା proc *p) { ଫେରନ୍ତୁ p->p_reg.sp; }

#ଯଦି !CONFIG_OXPCIE
ସ୍ଥିର ଶୂନ୍ୟ ser_init(ଶୂନ୍ୟ)
{
	unsigned ଅକ୍ଷର lcr;
	unsigned ଭାଜକ;

	/* keep BIOS settings if cttybaud is not set */
	ଯଦି (kinfo.serial_debug_baud <= 0) ଫେରନ୍ତୁ;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	ଭାଜକ = UART_BASE_FREQ / kinfo.serial_debug_baud;
	ଯଦି (ଭାଜକ < 1) ଭାଜକ = 1;
	ଯଦି (ଭାଜକ > 65535) ଭାଜକ = 65535;
	
	outb(COM1_DLL, ଭାଜକ & 0xff);
	outb(COM1_DLM, (ଭାଜକ >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
