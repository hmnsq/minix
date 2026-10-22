/* system dependent functions for use inside the whole kernel. */

#অন্তর্ভাব "kernel/kernel.h"

#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <minix/cpufeature.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <machine/vm.h>
#অন্তর্ভাব <machine/signal.h>
#অন্তর্ভাব <arm/armreg.h>

#অন্তর্ভাব <minix/u64.h>

#অন্তর্ভাব "archconst.h"
#অন্তর্ভাব "arch_proto.h"
#অন্তর্ভাব "kernel/proc.h"
#অন্তর্ভাব "kernel/debug.h"
#অন্তর্ভাব "ccnt.h"
#অন্তর্ভাব "bsp_init.h"
#অন্তর্ভাব "bsp_serial.h"

#অন্তর্ভাব "glo.h"

শূন্য * k_stacks;


শূন্য fpu_init(শূন্য)
{
}

শূন্য save_local_fpu(কাঠামো proc *pr, পূর্ণসংখ্যা retain)
{
}

শূন্য save_fpu(কাঠামো proc *pr)
{
}

শূন্য arch_proc_reset(কাঠামো proc *pr)
{
	প্রতিজ্ঞা(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, আকার(pr->p_reg));
	যদি(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} অন্যথা {
		pr->p_reg.psr = INIT_PSR;
	}
}

শূন্য arch_proc_setcontext(কাঠামো proc *p, কাঠামো stackframe_s *state,
	পূর্ণসংখ্যা isuser, পূর্ণসংখ্যা trapstyle)
{
        প্রতিজ্ঞা(আকার(p->p_reg) == আকার(*state));
	যদি(state != &p->p_reg) {
	        memcpy(&p->p_reg, state, আকার(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_SET;

        যদি(!(p->p_rts_flags)) {
                printf("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

শূন্য arch_set_secondary_ipc_return(কাঠামো proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

পূর্ণসংখ্যা restore_fpu(কাঠামো proc *pr)
{
	ফেরত 0;
}

শূন্য cpu_identify(শূন্য)
{
	u32_t midr;
	unsigned cpu = cpuid;

	asm অস্থির("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

শূন্য arch_init(শূন্য)
{
        u32_t মান;

	k_stacks = (শূন্য*) &k_stacks_start;
	প্রতিজ্ঞা(!((vir_bytes) k_stacks % K_স্তূপ_আকার));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_স্তূপ_শীর্ষ(0));
#endif


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm অস্থির ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (মান));
        মান |= PMU_PMCR_C; /* Reset counter */
        মান |= PMU_PMCR_E; /* Enable counter hardware */
        asm অস্থির ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (মান));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        মান = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm অস্থির ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (মান));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        মান = PMU_PMUSERENR_EN;
        asm অস্থির ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (মান));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
শূন্য do_ser_debug(শূন্য)
{
}

শূন্য arch_do_syscall(কাঠামো proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  প্রতিজ্ঞা(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_স্তূপ;

কাঠামো proc * arch_finish_switch_to_user(শূন্য)
{
	অক্ষর * stk;
	কাঠামো proc * p;

#ifdef CONFIG_SMP
	stk = (অক্ষর *)tss[cpuid].sp0;
#অন্যথা
	stk = (অক্ষর *)tss[0].sp0;
#endif
	svc_স্তূপ = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	ফেরত p;
}

শূন্য fpu_sigcontext(কাঠামো proc *pr, কাঠামো sigframe_sigcontext *fr, কাঠামো sigcontext *sc)
{
}

reg_t arch_get_sp(কাঠামো proc *p) { ফেরত p->p_reg.sp; }

শূন্য get_randomness(কাঠামো k_randomness *rand, পূর্ণসংখ্যা source)
{
}

শূন্য arch_ser_init(শূন্য)
{
	bsp_ser_init();
}

/*===========================================================================*/
/*			      __switch_address_space			     */
/*===========================================================================*/
/*
 * sets the ttbr register to the supplied value if it is not already set to the
 * same value in which case it would only result in an extra TLB flush which is
 * not desirable
 */
শূন্য __switch_address_space(কাঠামো proc *p, কাঠামো proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	যদি (new_ttbr == 0)
	    ফেরত;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	যদি (new_ttbr == orig_ttbr)
	    ফেরত;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	ফেরত;
}
