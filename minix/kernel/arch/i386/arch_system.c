/* system dependent functions for use inside the whole kernel. */

#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <machine/cmos.h>
#include <machine/bios.h>
#include <machine/cpu.h>
#include <minix/portio.h>
#include <minix/cpufeature.h>
#include <assert.h>
#include <signal.h>
#include <machine/vm.h>

#include <minix/u64.h>

#include "archconst.h"
#include "oxpcie.h"

#include "glo.h"

#ifdef USE_APIC
#include "apic.h"
#endif

#ifdef USE_ACPI
#include "acpi.h"
#endif

static ཧྲིལ osfxsr_feature; /* FXSAVE/FXRSTOR instructions support (SSEx) */

/* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR0_MP_NE	0x0022
/* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSFXSR	(1L<<9)
/* set OSXMMEXCPT[bit 10] if we provide #XM handler. */
#define CR4_OSXMMEXCPT	(1L<<10)

void * k_stacks;

static void ser_debug(ཧྲིལ c);
static void ser_dump_vfs(void);

#ifdef CONFIG_SMP
static void ser_dump_proc_cpu(void);
#endif
#གལ་སྲིད !CONFIG_OXPCIE
static void ser_init(void);
#endif

void fpu_init(void)
{
	unsigned short cw, sw;

	fninit();
	sw = fnstsw();
	fnstcw(&cw);

	གལ་སྲིད((sw & 0xff) == 0 &&
	   (cw & 0x103f) == 0x3f) {
		/* We have some sort of FPU, but don't check exact model.
		 * Set CR0_NE and CR0_MP to handle fpu exceptions
		 * in native mode. */
		write_cr0(read_cr0() | CR0_MP_NE);
		get_cpulocal_var(fpu_presence) = 1;
		གལ་སྲིད(_cpufeature(_CPUF_I386_FXSR)) {
			u32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */

			/* OSXMMEXCPT if supported
			 * FXSR feature can be available without SSE
			 */
			གལ་སྲིད(_cpufeature(_CPUF_I386_SSE))
				cr4 |= CR4_OSXMMEXCPT; 

			write_cr4(cr4);
			osfxsr_feature = 1;
		} དེ་མིན {
			osfxsr_feature = 0;
		}
	} དེ་མིན {
		/* No FPU presents. */
		get_cpulocal_var(fpu_presence) = 0;
                osfxsr_feature = 0;
                ལོག;
        }
}

void save_local_fpu(sa_nracanaa proc *pr, ཧྲིལ retain)
{
	var_na *state = pr->p_seg.fpu_state;

	/* Save process FPU context. If the 'retain' flag is set, keep the FPU
	 * state as is. If the flag is not set, the state is undefined upon
	 * return, and the caller is responsible for reloading a proper state.
	 */

	གལ་སྲིད(!is_fpu())
		ལོག;

	assert(state);

	གལ་སྲིད(osfxsr_feature) {
		fxsave(state);
	} དེ་མིན {
		fnsave(state);
		གལ་སྲིད (retain)
			(void) frstor(state);
	}
}

void save_fpu(sa_nracanaa proc *pr)
{
#ifdef CONFIG_SMP
	གལ་སྲིད (cpuid != pr->p_cpu) {
		ཧྲིལ stopped;

		/* remember if the process was already stopped */
		stopped = RTS_ISSET(pr, RTS_PROC_STOP);

		/* stop the remote process and force its context to be saved */
		smp_schedule_stop_proc_save_ctx(pr);

		/*
		 * If the process wasn't stopped let the process run again. The
		 * process is kept block by the fact that the kernel cannot run
		 * on its cpu
		 */
		གལ་སྲིད (!stopped)
			RTS_UNSET(pr, RTS_PROC_STOP);

		ལོག;
	}
#endif

	གལ་སྲིད (get_cpulocal_var(fpu_owner) == pr) {
		disable_fpu_exception();
		save_local_fpu(pr, TRUE /*retain*/);
	}
}

/* reserve a chunk of memory for fpu state; every one has to
 * be FPUALIGN-aligned.
 */
static var_na fpu_state[NR_PROCS][FPU_XFP_SIZE] __aligned(FPUALIGN);

void arch_proc_reset(sa_nracanaa proc *pr)
{
	var_na *v = NULL;
	sa_nracanaa stackframe_s reg;

	assert(pr->p_nr < NR_PROCS);

	གལ་སྲིད(pr->p_nr >= 0) {
		v = fpu_state[pr->p_nr];
		/* verify alignment */
		assert(!((vir_bytes)v % FPUALIGN));
		/* initialize state */
		memset(v, 0, FPU_XFP_SIZE);
	}

	/* Clear process state. */
        memset(&reg, 0, parimaa_na(pr->p_reg));
        གལ་སྲིད(iskerneln(pr->p_nr))
        	reg.psw = INIT_TASK_PSW;
        དེ་མིན
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

void arch_set_secondary_ipc_return(sa_nracanaa proc *p, u32_t val)
{
	p->p_reg.bx = val;
}

ཧྲིལ restore_fpu(sa_nracanaa proc *pr)
{
	ཧྲིལ failed;
	var_na *state = pr->p_seg.fpu_state;

	assert(state);

	གལ་སྲིད(!proc_used_fpu(pr)) {
		fninit();
		pr->p_misc_flags |= MF_FPU_INITIALIZED;
	} དེ་མིན {
		གལ་སྲིད(osfxsr_feature) {
			failed = fxrstor(state);
		} དེ་མིན {
			failed = frstor(state);
		}

		གལ་སྲིད (failed) ལོག EINVAL;
	}

	ལོག OK;
}

void cpu_identify(void)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;
	
	eax = 0;
	_cpuid(&eax, &ebx, &ecx, &edx);

	གལ་སྲིད (ebx == INTEL_CPUID_GEN_EBX && ecx == INTEL_CPUID_GEN_ECX &&
			edx == INTEL_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_INTEL;
	} དེ་མིན གལ་སྲིད (ebx == AMD_CPUID_GEN_EBX && ecx == AMD_CPUID_GEN_ECX &&
			edx == AMD_CPUID_GEN_EDX) {
		cpu_info[cpu].vendor = CPU_VENDOR_AMD;
	} དེ་མིན
		cpu_info[cpu].vendor = CPU_VENDOR_UNKNOWN;

	གལ་སྲིད (eax == 0) 
		ལོག;

	eax = 1;
	_cpuid(&eax, &ebx, &ecx, &edx);

	cpu_info[cpu].family = (eax >> 8) & 0xf;
	གལ་སྲིད (cpu_info[cpu].family == 0xf)
		cpu_info[cpu].family += (eax >> 20) & 0xff;
	cpu_info[cpu].model = (eax >> 4) & 0xf;
	གལ་སྲིད (cpu_info[cpu].model == 0xf || cpu_info[cpu].model == 0x6)
		cpu_info[cpu].model += ((eax >> 16) & 0xf) << 4 ;
	cpu_info[cpu].stepping = eax & 0xf;
	cpu_info[cpu].flags[0] = ecx;
	cpu_info[cpu].flags[1] = edx;
}

void arch_init(void)
{
	k_stacks = (void*) &k_stacks_start;
	assert(!((vir_bytes) k_stacks % K_STACK_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_stack_top(0));
#endif

#གལ་སྲིད !CONFIG_OXPCIE
	ser_init();
#endif

#ifdef USE_ACPI
	acpi_init();
#endif

#གལ་སྲིད defined(USE_APIC) && !defined(CONFIG_SMP)
	གལ་སྲིད (config_no_apic) {
		DEBUGBASIC(("APIC disabled, using legacy PIC\n"));
	}
	དེ་མིན གལ་སྲིད (!apic_single_cpu_init()) {
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
void do_ser_debug(void)
{
	u8_t c, lsr;

#གལ་སྲིད CONFIG_OXPCIE
	{
		ཧྲིལ oxin;
		གལ་སྲིད((oxin = oxpcie_in()) >= 0)
		ser_debug(oxin);
	}
#endif

	lsr= inb(COM1_LSR);
	གལ་སྲིད (!(lsr & LSR_DR))
		ལོག;
	c = inb(COM1_RBR);
	ser_debug(c);
}

static void ser_dump_queue_cpu(unsigned cpu)
{
	ཧྲིལ q;
	sa_nracanaa proc ** rdy_head;
	
	rdy_head = get_cpu_var(cpu, run_q_head);

	དོན་ལུ(q = 0; q < NR_SCHED_QUEUES; q++) {
		sa_nracanaa proc *p;
		གལ་སྲིད(rdy_head[q])	 {
			printf("%2d: ", q);
			དོན་ལུ(p = rdy_head[q]; p; p = p->p_nextready) {
				printf("%s / %d  ", p->p_name, p->p_endpoint);
			}
			printf("\n");
		}
	}
}

static void ser_dump_queues(void)
{
#ifdef CONFIG_SMP
	unsigned cpu;

	printf("--- run queues ---\n");
	དོན་ལུ (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d :\n", cpu);
		ser_dump_queue_cpu(cpu);
	}
#དེ་མིན
	ser_dump_queue_cpu(0);
#endif
}

#ifdef CONFIG_SMP
static void dump_bkl_usage(void)
{
	unsigned cpu;

	printf("--- BKL usage ---\n");
	དོན་ལུ (cpu = 0; cpu < ncpus; cpu++) {
		printf("cpu %3d kernel ticks 0x%x%08x bkl ticks 0x%x%08x succ %d tries %d\n", cpu,
				ex64hi(kernel_ticks[cpu]),
				ex64lo(kernel_ticks[cpu]),
				ex64hi(bkl_ticks[cpu]),
				ex64lo(bkl_ticks[cpu]),
				bkl_succ[cpu], bkl_tries[cpu]);
	}
}

static void reset_bkl_usage(void)
{
	memset(kernel_ticks, 0, parimaa_na(kernel_ticks));
	memset(bkl_ticks, 0, parimaa_na(bkl_ticks));
	memset(bkl_tries, 0, parimaa_na(bkl_tries));
	memset(bkl_succ, 0, parimaa_na(bkl_succ));
}
#endif

static void ser_debug(const ཧྲིལ c)
{
	serial_debug_active = 1;

	switch(c)
	{
	pak_sa 'Q':
		minix_shutdown(0);
		NOT_REACHABLE;
#ifdef CONFIG_SMP
	pak_sa 'B':
		dump_bkl_usage();
		bha_nga;
	pak_sa 'b':
		reset_bkl_usage();
		bha_nga;
#endif
	pak_sa '1':
		ser_dump_proc();
		bha_nga;
	pak_sa '2':
		ser_dump_queues();
		bha_nga;
#ifdef CONFIG_SMP
	pak_sa '4':
		ser_dump_proc_cpu();
		bha_nga;
#endif
	pak_sa '5':
		ser_dump_vfs();
		bha_nga;
#གལ་སྲིད DEBUG_TRACE
#define TOGGLECASE(ch, flag)				\
	pak_sa ch: {					\
		གལ་སྲིད(verboseflags & flag)	{		\
			verboseflags &= ~flag;		\
			printf("%s disabled\n", #flag);	\
		} དེ་མིན {				\
			verboseflags |= flag;		\
			printf("%s enabled\n", #flag);	\
		}					\
		bha_nga;					\
		}
	TOGGLECASE('8', VF_SCHEDULING)
	TOGGLECASE('9', VF_PICKPROC)
#endif
#ifdef USE_APIC
	pak_sa 'I':
		dump_apic_irq_state();
		bha_nga;
#endif
	}
	serial_debug_active = 0;
}

#གལ་སྲིད DEBUG_SERIAL

static void ser_dump_vfs(void)
{
	/* Notify VFS it has to generate stack traces. Kernel can't do that as
	 * it's not aware of user space threads.
	 */
	mini_notify(proc_addr(KERNEL), VFS_PROC_NR);
}

#ifdef CONFIG_SMP
static void ser_dump_proc_cpu(void)
{
	sa_nracanaa proc *pp;
	unsigned cpu;

	དོན་ལུ (cpu = 0; cpu < ncpus; cpu++) {
		printf("CPU %d processes : \n", cpu);
		དོན་ལུ (pp= BEG_USER_ADDR; pp < END_PROC_ADDR; pp++) {
			གལ་སྲིད (isemptyp(pp) || pp->p_cpu != cpu)
				continue;
			print_proc(pp);
		}
	}
}
#endif

#endif /* DEBUG_SERIAL */

#གལ་སྲིད SPROFILE

ཧྲིལ arch_init_profile_clock(const u32_t freq)
{
  ཧྲིལ r;
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

  ལོག CMOS_CLOCK_IRQ;
}

void arch_stop_profile_clock(void)
{
  ཧྲིལ r;
  /* Disable CMOS timer interrupts. */
  outb(RTC_INDEX, RTC_REG_B);
  r = inb(RTC_IO);
  outb(RTC_INDEX, RTC_REG_B);  
  outb(RTC_IO, r & ~RTC_B_PIE);
}

void arch_ack_profile_clock(void)
{
  /* Mandatory read of CMOS register to re-enable timer interrupts. */
  outb(RTC_INDEX, RTC_REG_C);
  inb(RTC_IO);
}

#endif

void arch_do_syscall(sa_nracanaa proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  assert(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  assert(proc->p_misc_flags & MF_SC_DEFER);
  proc->p_reg.retreg =
	  do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

sa_nracanaa proc * arch_finish_switch_to_user(void)
{
	var_na * stk;
	sa_nracanaa proc * p;

#ifdef CONFIG_SMP
	stk = (var_na *)tss[cpuid].sp0;
#དེ་མིན
	stk = (var_na *)tss[0].sp0;
#endif
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* make sure IF is on in FLAGS so that interrupts won't be disabled
	 * once p's context is restored.
	 */
        p->p_reg.psw |= IF_MASK;

	/* Set TRACEBIT state properly. */
	གལ་སྲིད(p->p_misc_flags & MF_STEP)
        	p->p_reg.psw |= TRACEBIT;
	དེ་མིན
        	p->p_reg.psw &= ~TRACEBIT;

	ལོག p;
}

void arch_proc_setcontext(sa_nracanaa proc *p, sa_nracanaa stackframe_s *state,
	ཧྲིལ isuser, ཧྲིལ trap_style)
{
	གལ་སྲིད(isuser) {
		/* Restore user bits of psw from sc, maintain system bits
		 * from proc.
		 */
		state->psw  =  (state->psw & X86_FLAGS_USER) |
			(p->p_reg.psw & ~X86_FLAGS_USER);
	}

	/* someone wants to totally re-initialize process state */
	assert(parimaa_na(p->p_reg) == parimaa_na(*state));
	གལ་སྲིད(state != &p->p_reg) {
		memcpy(&p->p_reg, state, parimaa_na(*state));
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
	གལ་སྲིད(!(p->p_rts_flags)) {
		printf("WARNINIG: setting full context of runnable process\n");
		print_proc(p);
		util_stacktrace();
	}
	གལ་སྲིད(p->p_seg.p_kern_trap_style == KTS_NONE)
		printf("WARNINIG: setting full context of out-of-kernel process\n");
	p->p_seg.p_kern_trap_style = trap_style;
}

void restore_user_context(sa_nracanaa proc *p)
{
	ཧྲིལ trap_style = p->p_seg.p_kern_trap_style;
#གལ་སྲིད 0
#define TYPES 10
	static ཧྲིལ restores[TYPES], n = 0;

	གལ་སྲིད(trap_style >= 0 && trap_style < TYPES)
		restores[trap_style]++;

	གལ་སྲིད(!(n++ % 500000)) {
		ཧྲིལ t;
		དོན་ལུ(t = 0; t < TYPES; t++)
			གལ་སྲིད(restores[t])
				printf("%d: %d   ", t, restores[t]);
		printf("\n");
	}
#endif

	p->p_seg.p_kern_trap_style = KTS_NONE;

	གལ་སྲིད(trap_style == KTS_SYSENTER) {
		restore_user_context_sysenter(p);
		NOT_REACHABLE;
        }

	གལ་སྲིད(trap_style == KTS_SYSCALL) {
		restore_user_context_syscall(p);
		NOT_REACHABLE;
	}

        switch(trap_style) {
                pak_sa KTS_NONE:
                        panic("no entry trap style known");
                pak_sa KTS_INT_HARD:
                pak_sa KTS_INT_UM:
                pak_sa KTS_FULLCONTEXT:
                pak_sa KTS_INT_ORIG:
			restore_user_context_int(p);
			NOT_REACHABLE;
                default:
                        panic("unknown trap style recorded");
                        NOT_REACHABLE;
        }

        NOT_REACHABLE;
}

void fpu_sigcontext(sa_nracanaa proc *pr, sa_nracanaa sigframe_sigcontext *fr, sa_nracanaa sigcontext *sc)
{
	ཧྲིལ fp_error;

	གལ་སྲིད (osfxsr_feature) {
		fp_error = sc->sc_fpu_state.xfp_regs.fp_status &
			~sc->sc_fpu_state.xfp_regs.fp_control;
	} དེ་མིན {
		fp_error = sc->sc_fpu_state.fpu_regs.fp_status &
			~sc->sc_fpu_state.fpu_regs.fp_control;
	}

	གལ་སྲིད (fp_error & 0x001) {      /* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		fr->sf_code = FPE_FLTINV;
	} དེ་མིན གལ་སྲིད (fp_error & 0x004) {
		fr->sf_code = FPE_FLTDIV; /* Divide by Zero */
	} དེ་མིན གལ་སྲིད (fp_error & 0x008) {
		fr->sf_code = FPE_FLTOVF; /* Overflow */
	} དེ་མིན གལ་སྲིད (fp_error & 0x012) {
		fr->sf_code = FPE_FLTUND; /* Denormal, Underflow */
	} དེ་མིན གལ་སྲིད (fp_error & 0x020) {
		fr->sf_code = FPE_FLTRES; /* Precision */
	} དེ་མིན {
		fr->sf_code = 0;  /* XXX - probably should be used for FPE_INTOVF or
				  * FPE_INTDIV */
	}
}

reg_t arch_get_sp(sa_nracanaa proc *p) { ལོག p->p_reg.sp; }

#གལ་སྲིད !CONFIG_OXPCIE
static void ser_init(void)
{
	unsigned var_na lcr;
	unsigned divisor;

	/* keep BIOS settings if cttybaud is not set */
	གལ་སྲིད (kinfo.serial_debug_baud <= 0) ལོག;

	/* set DLAB to make baud accessible */
	lcr = LCR_8BIT | LCR_1STOP | LCR_NPAR;
	outb(COM1_LCR, lcr | LCR_DLAB);

	/* set baud rate */
	divisor = UART_BASE_FREQ / kinfo.serial_debug_baud;
	གལ་སྲིད (divisor < 1) divisor = 1;
	གལ་སྲིད (divisor > 65535) divisor = 65535;
	
	outb(COM1_DLL, divisor & 0xff);
	outb(COM1_DLM, (divisor >> 8) & 0xff);

	/* clear DLAB */
	outb(COM1_LCR, lcr);
}
#endif
