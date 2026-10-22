/* system dependent functions for use inside the whole kernel. */

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cmos.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/bios.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/portio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "archconst.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "oxpcie.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
#ਅਨ੍ਤਰ੍ਭਾਵ "apic.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ACPI
#ਅਨ੍ਤਰ੍ਭਾਵ "acpi.h"
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CR4_OSXMMEXCPT	(1L<<10)

ਖਾਲੀ * k_stacks;

ਸਥਿਰ ਖਾਲੀ ser_debug(ਪੂਰਨ c);
ਸਥਿਰ ਖਾਲੀ ser_dump_vfs(ਖਾਲੀ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
ਸਥਿਰ ਖਾਲੀ ser_dump_proc_cpu(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
#ਜੇ !CONFIG_OXPCIE
ਸਥਿਰ ਖਾਲੀ ser_init(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

ਖਾਲੀ fpu_init(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	ਜੇ((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		ਜੇ(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			ਜੇ(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} ਨਹੀਂਤਾਂ {
			osfxsr_feature = 0;
		}
	} ਨਹੀਂਤਾਂ {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                ਵਾਪਸ;
        }
}

ਖਾਲੀ save_local_fpu(ਬਣਤਰ proc *pr, ਪੂਰਨ retain)
{
	ਅੱਖਰ *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	ਜੇ(!is_fpu())
		ਵਾਪਸ;

	ਪ੍ਰਤਿਜ੍ਞਾ(state);

	ਜੇ(osfxsr_feature) {
		fxsave(state);
	} ਨਹੀਂਤਾਂ {
		fnsave(state);
		ਜੇ (retain)
			(ਖਾਲੀ) frstor(state);
	}
}

ਖਾਲੀ save_fpu(ਬਣਤਰ proc *pr)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਜੇ (cpuid != pr->p_cpu) {
		ਪੂਰਨ stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_ਸਮਾਂ_ਸਾਰਣੀ_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		ਜੇ (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		ਵਾਪਸ;
	}
#ਜੇ_ਅੰਤ

	ਜੇ (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
ਸਥਿਰ ਅੱਖਰ fpu_state[NR_PROCS][FPU_XFP_ਮਾਪ] __aligned(FPUALIGN);

ਖਾਲੀ arch_proc_reset(ਬਣਤਰ proc *pr)
{
	ਅੱਖਰ *v = ਨਿਰਾ;
	ਬਣਤਰ stackframe_s reg;

	ਪ੍ਰਤਿਜ੍ਞਾ(pr->p_nr < NR_PROCS);

	ਜੇ(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		ਪ੍ਰਤਿਜ੍ਞਾ(!((vir_ਬਾਈਟਾਂ)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_ਮਾਪ);
	}

	/* Clear process state. */
        memset(&reg, 0, ਆਕਾਰ(pr->p_reg));
        ਜੇ(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        ਨਹੀਂਤਾਂ
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

ਖਾਲੀ arch_ਸੈੱਟ_secondary_ipc_return(ਬਣਤਰ proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

ਪੂਰਨ restore_fpu(ਬਣਤਰ proc *pr)
{
	ਪੂਰਨ failed;
	ਅੱਖਰ *state = pr->p_seg.fpu_state;

	ਪ੍ਰਤਿਜ੍ਞਾ(state);

	ਜੇ(!proc_ਵਰਤਿਆ_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} ਨਹੀਂਤਾਂ {
		ਜੇ(osfxsr_feature) {
			failed = fxrstor(state);
		} ਨਹੀਂਤਾਂ {
			failed = frstor(state);
		}

		ਜੇ (failed) ਵਾਪਸ EINVAL;
	}

	ਵਾਪਸ OK;
}

ਖਾਲੀ cpu_identify(ਖਾਲੀ)
{
	u32_t eax, ebx, ecx, edx;
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	ਜੇ (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} ਨਹੀਂਤਾਂ ਜੇ (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} ਨਹੀਂਤਾਂ
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	ਜੇ (eax == 0) 
		ਵਾਪਸ;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	ਜੇ (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	ਜੇ (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

ਖਾਲੀ arch_init(ਖਾਲੀ)
{
	k_stacks = (ਖਾਲੀ*) &k_stacks_ਸ਼ੁਰੂ;
	ਪ੍ਰਤਿਜ੍ਞਾ(!((vir_ਬਾਈਟਾਂ) k_stacks % K_ਢੇਰ_ਮਾਪ));

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ਢੇਰ_ਸਿਖਰ(0));
#ਜੇ_ਅੰਤ

#ਜੇ !CONFIG_OXPCIE
	ser_init();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ACPI
	acpi_init();
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_APIC) && !defined(CONFIG_SMP)
	ਜੇ (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	ਨਹੀਂਤਾਂ ਜੇ (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#ਜੇ_ਅੰਤ

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_ਸਿਰਾ);
	cut_memmap(&kinfo, ਅਧਾਰ_MEM_ਸਿਖਰ, ਉੱਤਲਾ_MEM_ਸਿਰਾ);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
ਖਾਲੀ do_ser_debug(ਖਾਲੀ)
{
	u8_t c, lsr;

#ਜੇ CONFIG_OXPCIE
	{
		ਪੂਰਨ oxin;
		ਜੇ((oxin = oxpcie_ਵਿੱਚ()) >= 0)
		ser_debug(oxin);
	}
#ਜੇ_ਅੰਤ

	lsr= inb(COM1_LSR);
	ਜੇ (!(lsr & LSR_DR))
		ਵਾਪਸ;
	c = inb(COM1_RBR);
	ser_debug(c);
}

ਸਥਿਰ ਖਾਲੀ ser_dump_ਕਤਾਰ_cpu(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
	ਪੂਰਨ q;
	ਬਣਤਰ proc ** rdy_ਸਿਰ;
	
	rdy_ਸਿਰ = get_cpu_var(cpu, ਲੜੀਵਾਰ_q_ਸਿਰ);

	ਲਈ(q = 0; q < NR_SCHED_QUEUES; q++) {
		ਬਣਤਰ proc *p;
		ਜੇ(rdy_ਸਿਰ[q])	 {
			ਲਿਖੋ("%2d: ", q);
			ਲਈ(p = rdy_ਸਿਰ[q]; p; p = p->p_nextready) {
				ਲਿਖੋ("%s / %d  ", p->p_name, p->p_endpoint);
			}
			ਲਿਖੋ("\n");
		}
	}
}

ਸਥਿਰ ਖਾਲੀ ser_dump_queues(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu;

	ਲਿਖੋ("--- run queues ---\n");
	ਲਈ (cpu = 0; cpu < ncpus; cpu++) {
		ਲਿਖੋ("CPU %d :\n", cpu);
		ser_dump_ਕਤਾਰ_cpu(cpu);
	}
#ਨਹੀਂਤਾਂ
	ser_dump_ਕਤਾਰ_cpu(0);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
ਸਥਿਰ ਖਾਲੀ dump_bkl_usage(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu;

	ਲਿਖੋ("--- BKL usage ---\n");
	ਲਈ (cpu = 0; cpu < ncpus; cpu++) {
		ਲਿਖੋ("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

ਸਥਿਰ ਖਾਲੀ reset_bkl_usage(ਖਾਲੀ)
{
	memset(kernel_ticks, 0, ਆਕਾਰ(kernel_ticks));
	memset(bkl_ticks, 0, ਆਕਾਰ(bkl_ticks));
	memset(bkl_tries, 0, ਆਕਾਰ(bkl_tries));
	memset(bkl_succ, 0, ਆਕਾਰ(bkl_succ));
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ ser_debug(ਅਚਲ ਪੂਰਨ c)
{
	serial_debug_ਸਰਗਰਮ = 1;

	ਚੋਣ(c)
	{
	ਹਾਲਤ 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਹਾਲਤ 'B':
		dump_bkl_usage();
		ਰੋਕੋ;
	ਹਾਲਤ 'b':
		reset_bkl_usage();
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ '1':
		ser_dump_proc();
		ਰੋਕੋ;
	ਹਾਲਤ '2':
		ser_dump_queues();
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਹਾਲਤ '4':
		ser_dump_proc_cpu();
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ '5':
		ser_dump_vfs();
		ਰੋਕੋ;
#ਜੇ DEBUG_TRACE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TOGGLECASE(ch, flag)				\
	ਹਾਲਤ ch: {					\
		ਜੇ(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			ਲਿਖੋ("%s disabled\n", #flag);	\
		} ਨਹੀਂਤਾਂ {				\
			verboseflags |= flag;		\
			ਲਿਖੋ("%s enabled\n", #flag);	\
		}					\
		ਰੋਕੋ;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
	ਹਾਲਤ 'I':
		dump_apic_irq_state();
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	}
	serial_debug_ਸਰਗਰਮ = 0;
}

#ਜੇ DEBUG_SERIAL

ਸਥਿਰ ਖਾਲੀ ser_dump_vfs(ਖਾਲੀ)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
ਸਥਿਰ ਖਾਲੀ ser_dump_proc_cpu(ਖਾਲੀ)
{
	ਬਣਤਰ proc *pp;
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu;

	ਲਈ (cpu = 0; cpu < ncpus; cpu++) {
		ਲਿਖੋ("CPU %d processes : \n", cpu);
		ਲਈ (pp= BEG_USER_ADDR; pp < ਸਿਰਾ_PROC_ADDR; pp++) {
			ਜੇ (isemptyp(pp) || pp->p_cpu != cpu)
				ਜਾਰੀ;
			print_proc(pp);
		}
	}
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* DEBUG_SERIAL */

#ਜੇ SPROFILE

ਪੂਰਨ arch_init_profile_clock(ਅਚਲ u32_t freq)
{
  ਪੂਰਨ r;
  /* Set CMOS timer frequency. */
  outb(RTC_ਸੂਚਕਾਂਕ, RTC_REG_A);
  outb(RTC_IO, RTC_A_DV_OK | freq);
  /* Enable CMOS timer interrupts. */
  outb(RTC_ਸੂਚਕਾਂਕ, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_ਸੂਚਕਾਂਕ, RTC_REG_B); 
  outb(RTC_IO, r | RTC_B_PIE);
  /* Mandatory read of CMOS register to enable timer interrupts. */
  outb(RTC_ਸੂਚਕਾਂਕ, RTC_REG_C);
  inb(RTC_IO);

  ਵਾਪਸ CMOS_CLOCK_IRQ;
}

ਖਾਲੀ arch_stop_profile_clock(ਖਾਲੀ)
{
  ਪੂਰਨ r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_ਸੂਚਕਾਂਕ, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_ਸੂਚਕਾਂਕ, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

ਖਾਲੀ arch_ack_profile_clock(ਖਾਲੀ)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_ਸੂਚਕਾਂਕ, RTC_REG_C);
  inb(RTC_IO);
}

#ਜੇ_ਅੰਤ

ਖਾਲੀ arch_do_syscall(ਬਣਤਰ proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ਪ੍ਰਤਿਜ੍ਞਾ(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  ਪ੍ਰਤਿਜ੍ਞਾ(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

ਬਣਤਰ proc * arch_ਅੰਤ_switch_ਮੰਜ਼ਿਲ_user(ਖਾਲੀ)
{
	ਅੱਖਰ * stk;
	ਬਣਤਰ proc * p;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	stk = (ਅੱਖਰ *)tss[cpuid].sp0;
#ਨਹੀਂਤਾਂ
	stk = (ਅੱਖਰ *)tss[0].sp0;
#ਜੇ_ਅੰਤ
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_ਨਕਾਬ;

	/* Set TRACEBIT state properly. */
	ਜੇ(p->p_misc_flags & MF_ਕਦਮ)
        	p->p_reg.psw |= TRACEBIT;
	ਨਹੀਂਤਾਂ
        	p->p_reg.psw &= ~TRACEBIT;

	ਵਾਪਸ p;
}

ਖਾਲੀ arch_proc_setcontext(ਬਣਤਰ proc *p, ਬਣਤਰ stackframe_s *state,
	ਪੂਰਨ isuser, ਪੂਰਨ trap_style)
{
	ਜੇ(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(p->p_reg) == ਆਕਾਰ(*state));
	ਜੇ(state != &p->p_reg) {
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&p->p_reg, state, ਆਕਾਰ(*state));
	}

	/* further code is instructed to not touch the context
	 * any more
	 */
	p->p_misc_flags |= MF_CONTEXT_ਸੈੱਟ;

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
	ਜੇ(!(p->p_rts_flags)) {
		ਲਿਖੋ("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	ਜੇ(p->p_seg.p_kern_trap_style == KTS_NONE)
		ਲਿਖੋ("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

ਖਾਲੀ restore_user_context(ਬਣਤਰ proc *p)
{
	ਪੂਰਨ trap_style = p->p_seg.p_kern_trap_style;
#ਜੇ 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TYPES 10
	ਸਥਿਰ ਪੂਰਨ restores[TYPES], n = 0;

	ਜੇ(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	ਜੇ(!(n++ % 500000)) {
		ਪੂਰਨ t;
		ਲਈ(t = 0; t < TYPES; t++)
			ਜੇ(restores[t])
				ਲਿਖੋ("%d: %d   ", t, restores[t]);
		ਲਿਖੋ("\n");
	}
#ਜੇ_ਅੰਤ

	p->p_seg.p_kern_trap_style = KTS_NONE;

	ਜੇ(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	ਜੇ(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        ਚੋਣ(trap_style) {
                ਹਾਲਤ KTS_NONE:
                        panic("no entry trap style known");
                ਹਾਲਤ KTS_INT_HARD:
                ਹਾਲਤ KTS_INT_UM:
                ਹਾਲਤ KTS_FULLCONTEXT:
                ਹਾਲਤ KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                ਮੂਲ:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

ਖਾਲੀ fpu_sigcontext(ਬਣਤਰ proc *pr, ਬਣਤਰ sigframe_sigcontext *fr, ਬਣਤਰ sigcontext *sc)
{
	ਪੂਰਨ fp_error;

	ਜੇ (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_ਸਥਿਤੀ &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} ਨਹੀਂਤਾਂ {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_ਸਥਿਤੀ &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	ਜੇ (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_ਕੋਡ = FPE_FLTINV;
	} ਨਹੀਂਤਾਂ ਜੇ (fp_error & 0x004) {
		fr->sf_ਕੋਡ = FPE_FLTDIV; /* Divide by Zero */
	} ਨਹੀਂਤਾਂ ਜੇ (fp_error & 0x008) {
		fr->sf_ਕੋਡ = FPE_FLTOVF; /*ਉਛਾਲ*/
	} ਨਹੀਂਤਾਂ ਜੇ (fp_error & 0x012) {
		fr->sf_ਕੋਡ = FPE_FLTUND; /* Denormal, Underflow */
	} ਨਹੀਂਤਾਂ ਜੇ (fp_error & 0x020) {
		fr->sf_ਕੋਡ = FPE_FLTRES; /* Precision */
	} ਨਹੀਂਤਾਂ {
		fr->sf_ਕੋਡ = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(ਬਣਤਰ proc *p) { ਵਾਪਸ p->p_reg.sp; }

#ਜੇ !CONFIG_OXPCIE
ਸਥਿਰ ਖਾਲੀ ser_init(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ lcr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਭਾਜਕ;

	/* keep BIOS settings if cttybaud is not set */
	ਜੇ (kinfo.serial_debug_baud <= 0) ਵਾਪਸ;

	/* set DLAB to make baud accessible */
	lcr = LCR_8_ਦੋਆਧਾਰੀ_ਅੰਕ | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	ਭਾਜਕ = UART_ਅਧਾਰ_FREQ / kinfo.serial_debug_baud;
	ਜੇ (ਭਾਜਕ < 1) ਭਾਜਕ = 1;
	ਜੇ (ਭਾਜਕ > 65535) ਭਾਜਕ = 65535;
	
	outb(COM1_DLL, ਭਾਜਕ & 0xff);
	outb(COM1_DLM, (ਭਾਜਕ >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#ਜੇ_ਅੰਤ
