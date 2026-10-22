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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
#ಅನ್ತರ್ಭಾವ "apic.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ACPI
#ಅನ್ತರ್ಭಾವ "acpi.h"
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#ವ್ಯಾಖ್ಯಾನಿಸು CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#ವ್ಯಾಖ್ಯಾನಿಸು CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#ವ್ಯಾಖ್ಯಾನಿಸು CR4_OSXMMEXCPT	(1L<<10)

ಶೂನ್ಯ * k_stacks;

ಸ್ಥಿರ ಶೂನ್ಯ ser_debug(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_vfs(ಶೂನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_proc_cpu(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !CONFIG_OXPCIE
ಸ್ಥಿರ ಶೂನ್ಯ ser_init(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ fpu_init(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	ಆದರೆ((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		ಆದರೆ(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			ಆದರೆ(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} ಇಲ್ಲದಿದ್ದರೆ {
			osfxsr_feature = 0;
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                ಹಿಂತಿರುಗಿ;
        }
}

ಶೂನ್ಯ save_local_fpu(ರಚನೆ proc *pr, ಪೂರ್ಣಾಂಕ retain)
{
	ಅಕ್ಷರ *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	ಆದರೆ(!is_fpu())
		ಹಿಂತಿರುಗಿ;

	ಪ್ರತಿಜ್ಞಾ(state);

	ಆದರೆ(osfxsr_feature) {
		fxsave(state);
	} ಇಲ್ಲದಿದ್ದರೆ {
		fnsave(state);
		ಆದರೆ (retain)
			(ಶೂನ್ಯ) frstor(state);
	}
}

ಶೂನ್ಯ save_fpu(ರಚನೆ proc *pr)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಆದರೆ (cpuid != pr->p_cpu) {
		ಪೂರ್ಣಾಂಕ stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_ವೇಳಾಪಟ್ಟಿ_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		ಆದರೆ (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		ಹಿಂತಿರುಗಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
ಸ್ಥಿರ ಅಕ್ಷರ fpu_state[NR_PROCS][FPU_XFP_ಗಾತ್ರ] __aligned(FPUALIGN);

ಶೂನ್ಯ arch_proc_reset(ರಚನೆ proc *pr)
{
	ಅಕ್ಷರ *v = ಖಾಲಿ;
	ರಚನೆ stackframe_s reg;

	ಪ್ರತಿಜ್ಞಾ(pr->p_nr < NR_PROCS);

	ಆದರೆ(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		ಪ್ರತಿಜ್ಞಾ(!((vir_ಬೈಟುಗಳು)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_ಗಾತ್ರ);
	}

	/* Clear process state. */
        memset(&reg, 0, ಗಾತ್ರ(pr->p_reg));
        ಆದರೆ(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        ಇಲ್ಲದಿದ್ದರೆ
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

ಶೂನ್ಯ arch_ಗಣ_secondary_ipc_return(ರಚನೆ proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

ಪೂರ್ಣಾಂಕ restore_fpu(ರಚನೆ proc *pr)
{
	ಪೂರ್ಣಾಂಕ failed;
	ಅಕ್ಷರ *state = pr->p_seg.fpu_state;

	ಪ್ರತಿಜ್ಞಾ(state);

	ಆದರೆ(!proc_ಬಳಸಿದ_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ(osfxsr_feature) {
			failed = fxrstor(state);
		} ಇಲ್ಲದಿದ್ದರೆ {
			failed = frstor(state);
		}

		ಆದರೆ (failed) ಹಿಂತಿರುಗಿ EINVAL;
	}

	ಹಿಂತಿರುಗಿ OK;
}

ಶೂನ್ಯ cpu_identify(ಶೂನ್ಯ)
{
	u32_t eax, ebx, ecx, edx;
	ಚಿಹ್ನೆರಹಿತ cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	ಆದರೆ (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} ಇಲ್ಲದಿದ್ದರೆ
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	ಆದರೆ (eax == 0) 
		ಹಿಂತಿರುಗಿ;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	ಆದರೆ (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	ಆದರೆ (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

ಶೂನ್ಯ arch_init(ಶೂನ್ಯ)
{
	k_stacks = (ಶೂನ್ಯ*) &k_stacks_ಆರಂಭ;
	ಪ್ರತಿಜ್ಞಾ(!((vir_ಬೈಟುಗಳು) k_stacks % K_ರಾಶಿ_ಗಾತ್ರ));

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ರಾಶಿ_ತುದಿ(0));
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !CONFIG_OXPCIE
	ser_init();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ACPI
	acpi_init();
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_APIC) && !defined(CONFIG_SMP)
	ಆದರೆ (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!apic_single_cpu_init()) {
		DEBUGBASIC(("APIC not present, using legacy PIC\n"));
	}
#ಆದರೆ_ಅಂತ್ಯ

	/* Reserve some BIOS ranges */
	cut_memmap(&kinfo, BIOS_MEM_BEGIN, BIOS_MEM_ಕೊನೆ);
	cut_memmap(&kinfo, ತಳ_MEM_ತುದಿ, ಊರ್ಧ್ವ_MEM_ಕೊನೆ);
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
ಶೂನ್ಯ do_ser_debug(ಶೂನ್ಯ)
{
	u8_t c, lsr;

#ಆದರೆ CONFIG_OXPCIE
	{
		ಪೂರ್ಣಾಂಕ oxin;
		ಆದರೆ((oxin = oxpcie_ಒಳ()) >= 0)
		ser_debug(oxin);
	}
#ಆದರೆ_ಅಂತ್ಯ

	lsr= inb(COM1_LSR);
	ಆದರೆ (!(lsr & LSR_DR))
		ಹಿಂತಿರುಗಿ;
	c = inb(COM1_RBR);
	ser_debug(c);
}

ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_ಸರತಿ_cpu(ಚಿಹ್ನೆರಹಿತ cpu)
{
	ಪೂರ್ಣಾಂಕ q;
	ರಚನೆ proc ** rdy_ತಲೆ;
	
	rdy_ತಲೆ = get_cpu_var(cpu, ಸತತ_q_ತಲೆ);

	ಗಾಗಿ(q = 0; q < NR_SCHED_QUEUES; q++) {
		ರಚನೆ proc *p;
		ಆದರೆ(rdy_ತಲೆ[q])	 {
			ಬರೆ("%2d: ", q);
			ಗಾಗಿ(p = rdy_ತಲೆ[q]; p; p = p->p_nextready) {
				ಬರೆ("%s / %d  ", p->p_name, p->p_endpoint);
			}
			ಬರೆ("\n");
		}
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_queues(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಚಿಹ್ನೆರಹಿತ cpu;

	ಬರೆ("--- run queues ---\n");
	ಗಾಗಿ (cpu = 0; cpu < ncpus; cpu++) {
		ಬರೆ("CPU %d :\n", cpu);
		ser_dump_ಸರತಿ_cpu(cpu);
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ser_dump_ಸರತಿ_cpu(0);
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
ಸ್ಥಿರ ಶೂನ್ಯ dump_bkl_usage(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ cpu;

	ಬರೆ("--- BKL usage ---\n");
	ಗಾಗಿ (cpu = 0; cpu < ncpus; cpu++) {
		ಬರೆ("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ reset_bkl_usage(ಶೂನ್ಯ)
{
	memset(kernel_ticks, 0, ಗಾತ್ರ(kernel_ticks));
	memset(bkl_ticks, 0, ಗಾತ್ರ(bkl_ticks));
	memset(bkl_tries, 0, ಗಾತ್ರ(bkl_tries));
	memset(bkl_succ, 0, ಗಾತ್ರ(bkl_succ));
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ ser_debug(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ c)
{
	serial_debug_ಸಕ್ರಿಯ = 1;

	ಆಯ್ಕೆ(c)
	{
	ಸ್ಥಿತಿ 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಸ್ಥಿತಿ 'B':
		dump_bkl_usage();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'b':
		reset_bkl_usage();
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ '1':
		ser_dump_proc();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '2':
		ser_dump_queues();
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಸ್ಥಿತಿ '4':
		ser_dump_proc_cpu();
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ '5':
		ser_dump_vfs();
		ನಿಲ್ಲಿಸು;
#ಆದರೆ DEBUG_TRACE
#ವ್ಯಾಖ್ಯಾನಿಸು TOGGLECASE(ch, flag)				\
	ಸ್ಥಿತಿ ch: {					\
		ಆದರೆ(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			ಬರೆ("%s disabled\n", #flag);	\
		} ಇಲ್ಲದಿದ್ದರೆ {				\
			verboseflags |= flag;		\
			ಬರೆ("%s enabled\n", #flag);	\
		}					\
		ನಿಲ್ಲಿಸು;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
	ಸ್ಥಿತಿ 'I':
		dump_apic_irq_state();
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	}
	serial_debug_ಸಕ್ರಿಯ = 0;
}

#ಆದರೆ DEBUG_SERIAL

ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_vfs(ಶೂನ್ಯ)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
ಸ್ಥಿರ ಶೂನ್ಯ ser_dump_proc_cpu(ಶೂನ್ಯ)
{
	ರಚನೆ proc *pp;
	ಚಿಹ್ನೆರಹಿತ cpu;

	ಗಾಗಿ (cpu = 0; cpu < ncpus; cpu++) {
		ಬರೆ("CPU %d processes : \n", cpu);
		ಗಾಗಿ (pp= BEG_USER_ADDR; pp < ಕೊನೆ_PROC_ADDR; pp++) {
			ಆದರೆ (isemptyp(pp) || pp->p_cpu != cpu)
				ಮುಂದುವರಿ;
			print_proc(pp);
		}
	}
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* DEBUG_SERIAL */

#ಆದರೆ SPROFILE

ಪೂರ್ಣಾಂಕ arch_init_profile_clock(ಸ್ಥಿರಾಂಕ u32_t freq)
{
  ಪೂರ್ಣಾಂಕ r;
  /* Set CMOS timer frequency. */
  outb(RTC_ಸೂಚ್ಯಂಕ, RTC_REG_A);
  outb(RTC_IO, RTC_A_DV_OK | freq);
  /* Enable CMOS timer interrupts. */
  outb(RTC_ಸೂಚ್ಯಂಕ, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_ಸೂಚ್ಯಂಕ, RTC_REG_B); 
  outb(RTC_IO, r | RTC_B_PIE);
  /* Mandatory read of CMOS register to enable timer interrupts. */
  outb(RTC_ಸೂಚ್ಯಂಕ, RTC_REG_C);
  inb(RTC_IO);

  ಹಿಂತಿರುಗಿ CMOS_CLOCK_IRQ;
}

ಶೂನ್ಯ arch_stop_profile_clock(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_ಸೂಚ್ಯಂಕ, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_ಸೂಚ್ಯಂಕ, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

ಶೂನ್ಯ arch_ack_profile_clock(ಶೂನ್ಯ)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_ಸೂಚ್ಯಂಕ, RTC_REG_C);
  inb(RTC_IO);
}

#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ arch_do_syscall(ರಚನೆ proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ಪ್ರತಿಜ್ಞಾ(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  ಪ್ರತಿಜ್ಞಾ(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

ರಚನೆ proc * arch_ಮುಕ್ತಾಯ_switch_ಗಮ್ಯ_user(ಶೂನ್ಯ)
{
	ಅಕ್ಷರ * stk;
	ರಚನೆ proc * p;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	stk = (ಅಕ್ಷರ *)tss[cpuid].sp0;
#ಇಲ್ಲದಿದ್ದರೆ
	stk = (ಅಕ್ಷರ *)tss[0].sp0;
#ಆದರೆ_ಅಂತ್ಯ
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_ಮುಖವಾಡ;

	/* Set TRACEBIT state properly. */
	ಆದರೆ(p->p_misc_flags & MF_ಹೆಜ್ಜೆ)
        	p->p_reg.psw |= TRACEBIT;
	ಇಲ್ಲದಿದ್ದರೆ
        	p->p_reg.psw &= ~TRACEBIT;

	ಹಿಂತಿರುಗಿ p;
}

ಶೂನ್ಯ arch_proc_setcontext(ರಚನೆ proc *p, ರಚನೆ stackframe_s *state,
	ಪೂರ್ಣಾಂಕ isuser, ಪೂರ್ಣಾಂಕ trap_style)
{
	ಆದರೆ(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(p->p_reg) == ಗಾತ್ರ(*state));
	ಆದರೆ(state != &p->p_reg) {
		ಸ್ಮರಣೆ_ನಕಲು(&p->p_reg, state, ಗಾತ್ರ(*state));
	}

	/* further code is instructed to not touch the context
	 * any more
	 */
	p->p_misc_flags |= MF_CONTEXT_ಗಣ;

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
	ಆದರೆ(!(p->p_rts_flags)) {
		ಬರೆ("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	ಆದರೆ(p->p_seg.p_kern_trap_style == KTS_NONE)
		ಬರೆ("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

ಶೂನ್ಯ restore_user_context(ರಚನೆ proc *p)
{
	ಪೂರ್ಣಾಂಕ trap_style = p->p_seg.p_kern_trap_style;
#ಆದರೆ 0
#ವ್ಯಾಖ್ಯಾನಿಸು TYPES 10
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ restores[TYPES], n = 0;

	ಆದರೆ(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	ಆದರೆ(!(n++ % 500000)) {
		ಪೂರ್ಣಾಂಕ t;
		ಗಾಗಿ(t = 0; t < TYPES; t++)
			ಆದರೆ(restores[t])
				ಬರೆ("%d: %d   ", t, restores[t]);
		ಬರೆ("\n");
	}
#ಆದರೆ_ಅಂತ್ಯ

	p->p_seg.p_kern_trap_style = KTS_NONE;

	ಆದರೆ(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	ಆದರೆ(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        ಆಯ್ಕೆ(trap_style) {
                ಸ್ಥಿತಿ KTS_NONE:
                        panic("no entry trap style known");
                ಸ್ಥಿತಿ KTS_INT_HARD:
                ಸ್ಥಿತಿ KTS_INT_UM:
                ಸ್ಥಿತಿ KTS_FULLCONTEXT:
                ಸ್ಥಿತಿ KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                ಪೂರ್ವನಿಯೋಜಿತ:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

ಶೂನ್ಯ fpu_sigcontext(ರಚನೆ proc *pr, ರಚನೆ sigframe_sigcontext *fr, ರಚನೆ sigcontext *sc)
{
	ಪೂರ್ಣಾಂಕ fp_error;

	ಆದರೆ (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_ಸ್ಥಿತಿ &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} ಇಲ್ಲದಿದ್ದರೆ {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_ಸ್ಥಿತಿ &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	ಆದರೆ (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_ಸಂಕೇತ = FPE_FLTINV;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp_error & 0x004) {
		fr->sf_ಸಂಕೇತ = FPE_FLTDIV; /* Divide by Zero */
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp_error & 0x008) {
		fr->sf_ಸಂಕೇತ = FPE_FLTOVF; /*ಅತಿಪ್ರವಾಹ*/
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp_error & 0x012) {
		fr->sf_ಸಂಕೇತ = FPE_FLTUND; /* Denormal, Underflow */
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp_error & 0x020) {
		fr->sf_ಸಂಕೇತ = FPE_FLTRES; /* Precision */
	} ಇಲ್ಲದಿದ್ದರೆ {
		fr->sf_ಸಂಕೇತ = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(ರಚನೆ proc *p) { ಹಿಂತಿರುಗಿ p->p_reg.sp; }

#ಆದರೆ !CONFIG_OXPCIE
ಸ್ಥಿರ ಶೂನ್ಯ ser_init(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ lcr;
	ಚಿಹ್ನೆರಹಿತ ಭಾಜಕ;

	/* keep BIOS settings if cttybaud is not set */
	ಆದರೆ (kinfo.serial_debug_baud <= 0) ಹಿಂತಿರುಗಿ;

	/* set DLAB to make baud accessible */
	lcr = LCR_8_ಬಿಟ್ | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	ಭಾಜಕ = UART_ತಳ_FREQ / kinfo.serial_debug_baud;
	ಆದರೆ (ಭಾಜಕ < 1) ಭಾಜಕ = 1;
	ಆದರೆ (ಭಾಜಕ > 65535) ಭಾಜಕ = 65535;
	
	outb(COM1_DLL, ಭಾಜಕ & 0xff);
	outb(COM1_DLM, (ಭಾಜಕ >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#ಆದರೆ_ಅಂತ್ಯ
