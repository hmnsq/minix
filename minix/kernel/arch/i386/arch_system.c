/* system dependent functions for use inside the whole kernel. */

#ضمن <unistd.h>
#ضمن <ctype.h>
#ضمن <string.h>
#ضمن <machine/cmos.h>
#ضمن <machine/bios.h>
#ضمن <machine/cpu.h>
#ضمن <minix/portio.h>
#ضمن <minix/cpufeature.h>
#ضمن <assert.h>
#ضمن <signal.h>
#ضمن <machine/vm.h>

#ضمن <minix/u64.h>

#ضمن "archconst.h"
#ضمن "oxpcie.h"

#ضمن "glo.h"

#ifdef USE_APIC
#ضمن "apic.h"
#endif

#ifdef USE_ACPI
#ضمن "acpi.h"
#endif

ساكن صحيح osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

فراغ * k_stacks;

ساكن فراغ ser_debug(صحيح c);
ساكن فراغ ser_dump_vfs(فراغ);

#ifdef CONFIG_SMP
ساكن فراغ ser_dump_proc_cpu(فراغ);
#endif
#𐡄𐡍 !CONFIG_OXPCIE
ساكن فراغ ser_init(فراغ);
#endif

فراغ fpu_init(فراغ)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	𐡄𐡍((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		𐡄𐡍(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			𐡄𐡍(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} وإلا {
			osfxsr_feature = 0;
		}
	} وإلا {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                أرجع;
        }
}

فراغ save_local_fpu(بنية proc *pr, صحيح retain)
{
	حرف *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	𐡄𐡍(!is_fpu())
		أرجع;

	أكد(state);

	𐡄𐡍(osfxsr_feature) {
		fxsave(state);
	} وإلا {
		fnsave(state);
		𐡄𐡍 (retain)
			(فراغ) frstor(state);
	}
}

فراغ save_fpu(بنية proc *pr)
{
#ifdef CONFIG_SMP
	𐡄𐡍 (cpuid != pr->p_cpu) {
		صحيح stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		𐡄𐡍 (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		أرجع;
	}
#endif

	𐡄𐡍 (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
ساكن حرف fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

فراغ arch_proc_reset(بنية proc *pr)
{
	حرف *v = عدم;
	بنية stackframe_s reg;

	أكد(pr->p_nr < NR_PROCS);

	𐡄𐡍(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		أكد(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, حجم(pr->p_reg));
        𐡄𐡍(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        وإلا
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

فراغ arch_set_secondary_ipc_return(بنية proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

صحيح restore_fpu(بنية proc *pr)
{
	صحيح failed;
	حرف *state = pr->p_seg.fpu_state;

	أكد(state);

	𐡄𐡍(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} وإلا {
		𐡄𐡍(osfxsr_feature) {
			failed = fxrstor(state);
		} وإلا {
			failed = frstor(state);
		}

		𐡄𐡍 (failed) أرجع EINVAL;
	}

	أرجع OK;
}

فراغ cpu_identify(فراغ)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	𐡄𐡍 (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} وإلا 𐡄𐡍 (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} وإلا
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	𐡄𐡍 (eax == 0) 
		أرجع;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	𐡄𐡍 (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	𐡄𐡍 (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

فراغ arch_init(فراغ)
{
	k_stacks = (فراغ*) &k_stacks_start;
	أكد(!((vir_bytes) k_stacks % K_STACK_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_stack_top(0));
#endif

#𐡄𐡍 !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#𐡄𐡍 defined(USE_APIC) && !defined(CONFIG_SMP)
	𐡄𐡍 (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	وإلا 𐡄𐡍 (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_TOP, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
فراغ do_ser_debug(فراغ)
{
	u8_t c, lsr;

#𐡄𐡍 CONFIG_OXPCIE
	{
		صحيح oxin;
		𐡄𐡍((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	𐡄𐡍 (!(lsr & LSR_DR))
		أرجع;
	c = inb(COM1_RBR);
	ser_debug(c);
}

ساكن فراغ ser_dump_queue_cpu(unsigned cpu)
{
	صحيح q;
	بنية proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	لكل(q = 0; q < NR_SCHED_QUEUES; q++) {
		بنية proc *p;
		𐡄𐡍(rdy_head[q])	 {
			printf("%2d: ", q);
			لكل(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

ساكن فراغ ser_dump_queues(فراغ)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	لكل (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#وإلا
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
ساكن فراغ dump_bkl_usage(فراغ)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	لكل (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

ساكن فراغ reset_bkl_usage(فراغ)
{
	memset(kernel_ticks, 0, حجم(kernel_ticks));
	memset(bkl_ticks, 0, حجم(bkl_ticks));
	memset(bkl_tries, 0, حجم(bkl_tries));
	memset(bkl_succ, 0, حجم(bkl_succ));
}
#endif

ساكن فراغ ser_debug(ثابت صحيح c)
{
	serial_debug_active = 1;

	بدل(c)
	{
	حالة 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	حالة 'B':
		dump_bkl_usage();
		اكسر;
	حالة 'b':
		reset_bkl_usage();
		اكسر;
#endif
	حالة '1':
		ser_dump_proc();
		اكسر;
	حالة '2':
		ser_dump_queues();
		اكسر;
#ifdef CONFIG_SMP
	حالة '4':
		ser_dump_proc_cpu();
		اكسر;
#endif
	حالة '5':
		ser_dump_vfs();
		اكسر;
#𐡄𐡍 DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	حالة ch: {					\
		𐡄𐡍(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} وإلا {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		اكسر;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	حالة 'I':
		dump_apic_irq_state();
		اكسر;
#endif
	}
	serial_debug_active = 0;
}

#𐡄𐡍 DEBUG_SERIAL

ساكن فراغ ser_dump_vfs(فراغ)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
ساكن فراغ ser_dump_proc_cpu(فراغ)
{
	بنية proc *pp;
	unsigned cpu;

	لكل (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		لكل (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			𐡄𐡍 (isemptyp(pp) || pp->p_cpu != cpu)
				استمر;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#𐡄𐡍 SPROFILE

صحيح arch_init_profile_clock(ثابت u32_t freq)
{
  صحيح r;
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

  أرجع CMOS_CLOCK_IRQ;
}

فراغ arch_stop_profile_clock(فراغ)
{
  صحيح r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

فراغ arch_ack_profile_clock(فراغ)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

فراغ arch_do_syscall(بنية proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  أكد(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  أكد(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

بنية proc * arch_finish_switch_to_user(فراغ)
{
	حرف * stk;
	بنية proc * p;

#ifdef CONFIG_SMP
	stk = (حرف *)tss[cpuid].sp0;
#وإلا
	stk = (حرف *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	𐡄𐡍(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	وإلا
        	p->p_reg.psw &= ~TRACEBIT;

	أرجع p;
}

فراغ arch_proc_setcontext(بنية proc *p, بنية stackframe_s *state,
	صحيح isuser, صحيح trap_style)
{
	𐡄𐡍(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	أكد(حجم(p->p_reg) == حجم(*state));
	𐡄𐡍(state != &p->p_reg) {
		memcpy(&p->p_reg, state, حجم(*state));
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
	𐡄𐡍(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	𐡄𐡍(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

فراغ restore_user_context(بنية proc *p)
{
	صحيح trap_style = p->p_seg.p_kern_trap_style;
#𐡄𐡍 0
#define TYPES 10
	ساكن صحيح restores[TYPES], n = 0;

	𐡄𐡍(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	𐡄𐡍(!(n++ % 500000)) {
		صحيح t;
		لكل(t = 0; t < TYPES; t++)
			𐡄𐡍(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	𐡄𐡍(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	𐡄𐡍(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        بدل(trap_style) {
                حالة KTS_NONE:
                        panic("no entry trap style known");
                حالة KTS_INT_HARD:
                حالة KTS_INT_UM:
                حالة KTS_FULLCONTEXT:
                حالة KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                افتراضي:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

فراغ fpu_sigcontext(بنية proc *pr, بنية sigframe_sigcontext *fr, بنية sigcontext *sc)
{
	صحيح fp_error;

	𐡄𐡍 (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} وإلا {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	𐡄𐡍 (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} وإلا 𐡄𐡍 (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} وإلا 𐡄𐡍 (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} وإلا 𐡄𐡍 (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} وإلا 𐡄𐡍 (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} وإلا {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(بنية proc *p) { أرجع p->p_reg.sp; }

#𐡄𐡍 !CONFIG_OXPCIE
ساكن فراغ ser_init(فراغ)
{
	unsigned حرف lcr;
	unsigned divisor;

	/* keep BIOS settings if cttybaud is not set */
	𐡄𐡍 (kinfo.serial_debug_baud <= 0) أرجع;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	divisor = UART_BASE_FREQ / kinfo.serial_debug_baud;
	𐡄𐡍 (divisor < 1) divisor = 1;
	𐡄𐡍 (divisor > 65535) divisor = 65535;
	
	outb(COM1_DLL, divisor & 0xff);
	outb(COM1_DLM, (divisor >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
