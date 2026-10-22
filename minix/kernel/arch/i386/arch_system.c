/* system dependent functions for use inside the whole kernel. */

#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <machine/cmos.h>
#અન્તર્ભાવ <machine/bios.h>
#અન્તર્ભાવ <machine/cpu.h>
#અન્તર્ભાવ <minix/portio.h>
#અન્તર્ભાવ <minix/cpufeature.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <machine/vm.h>

#અન્તર્ભાવ <minix/u64.h>

#અન્તર્ભાવ "archconst.h"
#અન્તર્ભાવ "oxpcie.h"

#અન્તર્ભાવ "glo.h"

#ifdef USE_APIC
#અન્તર્ભાવ "apic.h"
#endif

#ifdef USE_ACPI
#અન્તર્ભાવ "acpi.h"
#endif

સ્થિર પૂર્ણાંક osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

શૂન્ય * k_stacks;

સ્થિર શૂન્ય ser_debug(પૂર્ણાંક c);
સ્થિર શૂન્ય ser_dump_vfs(શૂન્ય);

#ifdef CONFIG_SMP
સ્થિર શૂન્ય ser_dump_proc_cpu(શૂન્ય);
#endif
#જો !CONFIG_OXPCIE
સ્થિર શૂન્ય ser_init(શૂન્ય);
#endif

શૂન્ય fpu_init(શૂન્ય)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	જો((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		જો(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			જો(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} નહીંતર {
			osfxsr_feature = 0;
		}
	} નહીંતર {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                પાછા;
        }
}

શૂન્ય save_local_fpu(સંરચના proc *pr, પૂર્ણાંક retain)
{
	અક્ષર *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	જો(!is_fpu())
		પાછા;

	પ્રતિજ્ઞા(state);

	જો(osfxsr_feature) {
		fxsave(state);
	} નહીંતર {
		fnsave(state);
		જો (retain)
			(શૂન્ય) frstor(state);
	}
}

શૂન્ય save_fpu(સંરચના proc *pr)
{
#ifdef CONFIG_SMP
	જો (cpuid != pr->p_cpu) {
		પૂર્ણાંક stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		જો (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		પાછા;
	}
#endif

	જો (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
સ્થિર અક્ષર fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

શૂન્ય arch_proc_reset(સંરચના proc *pr)
{
	અક્ષર *v = ખાલી;
	સંરચના stackframe_s reg;

	પ્રતિજ્ઞા(pr->p_nr < NR_PROCS);

	જો(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		પ્રતિજ્ઞા(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, કદ(pr->p_reg));
        જો(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        નહીંતર
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

શૂન્ય arch_set_secondary_ipc_return(સંરચના proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

પૂર્ણાંક restore_fpu(સંરચના proc *pr)
{
	પૂર્ણાંક failed;
	અક્ષર *state = pr->p_seg.fpu_state;

	પ્રતિજ્ઞા(state);

	જો(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} નહીંતર {
		જો(osfxsr_feature) {
			failed = fxrstor(state);
		} નહીંતર {
			failed = frstor(state);
		}

		જો (failed) પાછા EINVAL;
	}

	પાછા OK;
}

શૂન્ય cpu_identify(શૂન્ય)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	જો (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} નહીંતર જો (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} નહીંતર
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	જો (eax == 0) 
		પાછા;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	જો (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	જો (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

શૂન્ય arch_init(શૂન્ય)
{
	k_stacks = (શૂન્ય*) &k_stacks_start;
	પ્રતિજ્ઞા(!((vir_bytes) k_stacks % K_ઢગલો_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ઢગલો_ટોચ(0));
#endif

#જો !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#જો defined(USE_APIC) && !defined(CONFIG_SMP)
	જો (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	નહીંતર જો (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_ટોચ, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
શૂન્ય do_ser_debug(શૂન્ય)
{
	u8_t c, lsr;

#જો CONFIG_OXPCIE
	{
		પૂર્ણાંક oxin;
		જો((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	જો (!(lsr & LSR_DR))
		પાછા;
	c = inb(COM1_RBR);
	ser_debug(c);
}

સ્થિર શૂન્ય ser_dump_queue_cpu(unsigned cpu)
{
	પૂર્ણાંક q;
	સંરચના proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	માટે(q = 0; q < NR_SCHED_QUEUES; q++) {
		સંરચના proc *p;
		જો(rdy_head[q])	 {
			printf("%2d: ", q);
			માટે(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

સ્થિર શૂન્ય ser_dump_queues(શૂન્ય)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	માટે (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#નહીંતર
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
સ્થિર શૂન્ય dump_bkl_usage(શૂન્ય)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	માટે (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

સ્થિર શૂન્ય reset_bkl_usage(શૂન્ય)
{
	memset(kernel_ticks, 0, કદ(kernel_ticks));
	memset(bkl_ticks, 0, કદ(bkl_ticks));
	memset(bkl_tries, 0, કદ(bkl_tries));
	memset(bkl_succ, 0, કદ(bkl_succ));
}
#endif

સ્થિર શૂન્ય ser_debug(અચલ પૂર્ણાંક c)
{
	serial_debug_active = 1;

	પસંદગી(c)
	{
	સ્થિતિ 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	સ્થિતિ 'B':
		dump_bkl_usage();
		તોડો;
	સ્થિતિ 'b':
		reset_bkl_usage();
		તોડો;
#endif
	સ્થિતિ '1':
		ser_dump_proc();
		તોડો;
	સ્થિતિ '2':
		ser_dump_queues();
		તોડો;
#ifdef CONFIG_SMP
	સ્થિતિ '4':
		ser_dump_proc_cpu();
		તોડો;
#endif
	સ્થિતિ '5':
		ser_dump_vfs();
		તોડો;
#જો DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	સ્થિતિ ch: {					\
		જો(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} નહીંતર {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		તોડો;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	સ્થિતિ 'I':
		dump_apic_irq_state();
		તોડો;
#endif
	}
	serial_debug_active = 0;
}

#જો DEBUG_SERIAL

સ્થિર શૂન્ય ser_dump_vfs(શૂન્ય)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
સ્થિર શૂન્ય ser_dump_proc_cpu(શૂન્ય)
{
	સંરચના proc *pp;
	unsigned cpu;

	માટે (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		માટે (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			જો (isemptyp(pp) || pp->p_cpu != cpu)
				ચાલુ;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#જો SPROFILE

પૂર્ણાંક arch_init_profile_clock(અચલ u32_t freq)
{
  પૂર્ણાંક r;
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

  પાછા CMOS_CLOCK_IRQ;
}

શૂન્ય arch_stop_profile_clock(શૂન્ય)
{
  પૂર્ણાંક r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

શૂન્ય arch_ack_profile_clock(શૂન્ય)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

શૂન્ય arch_do_syscall(સંરચના proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  પ્રતિજ્ઞા(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  પ્રતિજ્ઞા(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

સંરચના proc * arch_finish_switch_to_user(શૂન્ય)
{
	અક્ષર * stk;
	સંરચના proc * p;

#ifdef CONFIG_SMP
	stk = (અક્ષર *)tss[cpuid].sp0;
#નહીંતર
	stk = (અક્ષર *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	જો(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	નહીંતર
        	p->p_reg.psw &= ~TRACEBIT;

	પાછા p;
}

શૂન્ય arch_proc_setcontext(સંરચના proc *p, સંરચના stackframe_s *state,
	પૂર્ણાંક isuser, પૂર્ણાંક trap_style)
{
	જો(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	પ્રતિજ્ઞા(કદ(p->p_reg) == કદ(*state));
	જો(state != &p->p_reg) {
		memcpy(&p->p_reg, state, કદ(*state));
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
	જો(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	જો(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

શૂન્ય restore_user_context(સંરચના proc *p)
{
	પૂર્ણાંક trap_style = p->p_seg.p_kern_trap_style;
#જો 0
#define TYPES 10
	સ્થિર પૂર્ણાંક restores[TYPES], n = 0;

	જો(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	જો(!(n++ % 500000)) {
		પૂર્ણાંક t;
		માટે(t = 0; t < TYPES; t++)
			જો(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	જો(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	જો(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        પસંદગી(trap_style) {
                સ્થિતિ KTS_NONE:
                        panic("no entry trap style known");
                સ્થિતિ KTS_INT_HARD:
                સ્થિતિ KTS_INT_UM:
                સ્થિતિ KTS_FULLCONTEXT:
                સ્થિતિ KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                મૂળભૂત:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

શૂન્ય fpu_sigcontext(સંરચના proc *pr, સંરચના sigframe_sigcontext *fr, સંરચના sigcontext *sc)
{
	પૂર્ણાંક fp_error;

	જો (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} નહીંતર {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	જો (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} નહીંતર જો (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} નહીંતર જો (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} નહીંતર જો (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} નહીંતર જો (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} નહીંતર {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(સંરચના proc *p) { પાછા p->p_reg.sp; }

#જો !CONFIG_OXPCIE
સ્થિર શૂન્ય ser_init(શૂન્ય)
{
	unsigned અક્ષર lcr;
	unsigned ભાજક;

	/* keep BIOS settings if cttybaud is not set */
	જો (kinfo.serial_debug_baud <= 0) પાછા;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	ભાજક = UART_BASE_FREQ / kinfo.serial_debug_baud;
	જો (ભાજક < 1) ભાજક = 1;
	જો (ભાજક > 65535) ભાજક = 65535;
	
	outb(COM1_DLL, ભાજક & 0xff);
	outb(COM1_DLM, (ભાજક >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
