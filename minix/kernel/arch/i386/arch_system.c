/* system dependent functions for use inside the whole kernel. */

#شامل <unistd.h>
#شامل <ctype.h>
#شامل <string.h>
#شامل <machine/cmos.h>
#شامل <machine/bios.h>
#شامل <machine/cpu.h>
#شامل <minix/portio.h>
#شامل <minix/cpufeature.h>
#شامل <assert.h>
#شامل <signal.h>
#شامل <machine/vm.h>

#شامل <minix/u64.h>

#شامل "archconst.h"
#شامل "oxpcie.h"

#شامل "glo.h"

#ifdef USE_APIC
#شامل "apic.h"
#endif

#ifdef USE_ACPI
#شامل "acpi.h"
#endif

ایستا صحیح osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

تهی * k_stacks;

ایستا تهی ser_debug(صحیح c);
ایستا تهی ser_dump_vfs(تهی);

#ifdef CONFIG_SMP
ایستا تهی ser_dump_proc_cpu(تهی);
#endif
#اگر !CONFIG_OXPCIE
ایستا تهی ser_init(تهی);
#endif

تهی fpu_init(تهی)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	اگر((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		اگر(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			اگر(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} وگرنه {
			osfxsr_feature = 0;
		}
	} وگرنه {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                بازگشت;
        }
}

تهی save_local_fpu(ساختار proc *pr, صحیح retain)
{
	نویسه *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	اگر(!is_fpu())
		بازگشت;

	أكد(state);

	اگر(osfxsr_feature) {
		fxsave(state);
	} وگرنه {
		fnsave(state);
		اگر (retain)
			(تهی) frstor(state);
	}
}

تهی save_fpu(ساختار proc *pr)
{
#ifdef CONFIG_SMP
	اگر (cpuid != pr->p_cpu) {
		صحیح stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		اگر (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		بازگشت;
	}
#endif

	اگر (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
ایستا نویسه fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

تهی arch_proc_reset(ساختار proc *pr)
{
	نویسه *v = پوچ;
	ساختار stackframe_s reg;

	أكد(pr->p_nr < NR_PROCS);

	اگر(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		أكد(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, اندازه(pr->p_reg));
        اگر(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        وگرنه
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

تهی arch_set_secondary_ipc_return(ساختار proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

صحیح restore_fpu(ساختار proc *pr)
{
	صحیح failed;
	نویسه *state = pr->p_seg.fpu_state;

	أكد(state);

	اگر(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} وگرنه {
		اگر(osfxsr_feature) {
			failed = fxrstor(state);
		} وگرنه {
			failed = frstor(state);
		}

		اگر (failed) بازگشت EINVAL;
	}

	بازگشت OK;
}

تهی cpu_identify(تهی)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	اگر (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} وگرنه اگر (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} وگرنه
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	اگر (eax == 0) 
		بازگشت;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	اگر (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	اگر (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

تهی arch_init(تهی)
{
	k_stacks = (تهی*) &k_stacks_start;
	أكد(!((vir_bytes) k_stacks % K_پشته_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_پشته_بالاترین(0));
#endif

#اگر !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#اگر defined(USE_APIC) && !defined(CONFIG_SMP)
	اگر (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	وگرنه اگر (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#endif

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_END);
	cut_memmap(&kinfo, BASE_MEM_بالاترین, UPPER_MEM_END);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
تهی do_ser_debug(تهی)
{
	u8_t c, lsr;

#اگر CONFIG_OXPCIE
	{
		صحیح oxin;
		اگر((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	اگر (!(lsr & LSR_DR))
		بازگشت;
	c = inb(COM1_RBR);
	ser_debug(c);
}

ایستا تهی ser_dump_queue_cpu(unsigned cpu)
{
	صحیح q;
	ساختار proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	برای(q = 0; q < NR_SCHED_QUEUES; q++) {
		ساختار proc *p;
		اگر(rdy_head[q])	 {
			printf("%2d: ", q);
			برای(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

ایستا تهی ser_dump_queues(تهی)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	برای (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#وگرنه
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
ایستا تهی dump_bkl_usage(تهی)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	برای (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

ایستا تهی reset_bkl_usage(تهی)
{
	memset(kernel_ticks, 0, اندازه(kernel_ticks));
	memset(bkl_ticks, 0, اندازه(bkl_ticks));
	memset(bkl_tries, 0, اندازه(bkl_tries));
	memset(bkl_succ, 0, اندازه(bkl_succ));
}
#endif

ایستا تهی ser_debug(ثابت صحیح c)
{
	serial_debug_active = 1;

	انتخاب(c)
	{
	حالت 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	حالت 'B':
		dump_bkl_usage();
		بشکن;
	حالت 'b':
		reset_bkl_usage();
		بشکن;
#endif
	حالت '1':
		ser_dump_proc();
		بشکن;
	حالت '2':
		ser_dump_queues();
		بشکن;
#ifdef CONFIG_SMP
	حالت '4':
		ser_dump_proc_cpu();
		بشکن;
#endif
	حالت '5':
		ser_dump_vfs();
		بشکن;
#اگر DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	حالت ch: {					\
		اگر(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} وگرنه {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		بشکن;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	حالت 'I':
		dump_apic_irq_state();
		بشکن;
#endif
	}
	serial_debug_active = 0;
}

#اگر DEBUG_SERIAL

ایستا تهی ser_dump_vfs(تهی)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
ایستا تهی ser_dump_proc_cpu(تهی)
{
	ساختار proc *pp;
	unsigned cpu;

	برای (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		برای (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			اگر (isemptyp(pp) || pp->p_cpu != cpu)
				ادامه;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#اگر SPROFILE

صحیح arch_init_profile_clock(ثابت u32_t freq)
{
  صحیح r;
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

  بازگشت CMOS_CLOCK_IRQ;
}

تهی arch_stop_profile_clock(تهی)
{
  صحیح r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

تهی arch_ack_profile_clock(تهی)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

تهی arch_do_syscall(ساختار proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  أكد(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  أكد(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

ساختار proc * arch_finish_switch_to_user(تهی)
{
	نویسه * stk;
	ساختار proc * p;

#ifdef CONFIG_SMP
	stk = (نویسه *)tss[cpuid].sp0;
#وگرنه
	stk = (نویسه *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	اگر(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	وگرنه
        	p->p_reg.psw &= ~TRACEBIT;

	بازگشت p;
}

تهی arch_proc_setcontext(ساختار proc *p, ساختار stackframe_s *state,
	صحیح isuser, صحیح trap_style)
{
	اگر(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	أكد(اندازه(p->p_reg) == اندازه(*state));
	اگر(state != &p->p_reg) {
		memcpy(&p->p_reg, state, اندازه(*state));
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
	اگر(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	اگر(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

تهی restore_user_context(ساختار proc *p)
{
	صحیح trap_style = p->p_seg.p_kern_trap_style;
#اگر 0
#define TYPES 10
	ایستا صحیح restores[TYPES], n = 0;

	اگر(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	اگر(!(n++ % 500000)) {
		صحیح t;
		برای(t = 0; t < TYPES; t++)
			اگر(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	اگر(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	اگر(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        انتخاب(trap_style) {
                حالت KTS_NONE:
                        panic("no entry trap style known");
                حالت KTS_INT_HARD:
                حالت KTS_INT_UM:
                حالت KTS_FULLCONTEXT:
                حالت KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                پیشفرض:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

تهی fpu_sigcontext(ساختار proc *pr, ساختار sigframe_sigcontext *fr, ساختار sigcontext *sc)
{
	صحیح fp_error;

	اگر (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} وگرنه {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	اگر (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} وگرنه اگر (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} وگرنه اگر (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} وگرنه اگر (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} وگرنه اگر (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} وگرنه {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(ساختار proc *p) { بازگشت p->p_reg.sp; }

#اگر !CONFIG_OXPCIE
ایستا تهی ser_init(تهی)
{
	unsigned نویسه lcr;
	unsigned مقسوم_علیه;

	/* keep BIOS settings if cttybaud is not set */
	اگر (kinfo.serial_debug_baud <= 0) بازگشت;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	مقسوم_علیه = UART_BASE_FREQ / kinfo.serial_debug_baud;
	اگر (مقسوم_علیه < 1) مقسوم_علیه = 1;
	اگر (مقسوم_علیه > 65535) مقسوم_علیه = 65535;
	
	outb(COM1_DLL, مقسوم_علیه & 0xff);
	outb(COM1_DLM, (مقسوم_علیه >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
