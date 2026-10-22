/* system dependent functions for use inside the whole kernel. */

#অন্তর্ভাৱ "kernel/kernel.h"

#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <ctype.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <minix/cpufeature.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <machine/vm.h>
#অন্তর্ভাৱ <machine/signal.h>
#অন্তর্ভাৱ <arm/armreg.h>

#অন্তর্ভাৱ <minix/u64.h>

#অন্তর্ভাৱ "archconst.h"
#অন্তর্ভাৱ "arch_proto.h"
#অন্তর্ভাৱ "kernel/proc.h"
#অন্তর্ভাৱ "kernel/debug.h"
#অন্তর্ভাৱ "ccnt.h"
#অন্তর্ভাৱ "bsp_init.h"
#অন্তর্ভাৱ "bsp_serial.h"

#অন্তর্ভাৱ "glo.h"

শূন্য * k_stacks;


শূন্য fpu_init(শূন্য)
{
}

শূন্য save_local_fpu(সংরচনা proc *pr, পূর্ণাঙ্ক retain)
{
}

শূন্য save_fpu(সংরচনা proc *pr)
{
}

শূন্য arch_proc_reset(সংরচনা proc *pr)
{
	প্রতিজ্ঞা(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, পরিমাণ(pr->p_reg));
	যদি(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} অন্যথা {
		pr->p_reg.psr = INIT_PSR;
	}
}

শূন্য arch_proc_setcontext(সংরচনা proc *p, সংরচনা stackframe_s *state,
	পূর্ণাঙ্ক isuser, পূর্ণাঙ্ক trapstyle)
{
        প্রতিজ্ঞা(পরিমাণ(p->p_reg) == পরিমাণ(*state));
	যদি(state != &p->p_reg) {
	        memcpy(&p->p_reg, state, পরিমাণ(*state));
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

শূন্য arch_set_secondary_ipc_return(সংরচনা proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

পূর্ণাঙ্ক restore_fpu(সংরচনা proc *pr)
{
	ঘূৰাই 0;
}

শূন্য cpu_identify(শূন্য)
{
	u32_t midr;
	unsigned cpu = cpuid;

	asm চঞ্চল("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
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
	প্রতিজ্ঞা(!((vir_bytes) k_stacks % K_স্তূপ_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_স্তূপ_শীৰ্ষ(0));
#endif


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm চঞ্চল ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (মান));
        মান |= PMU_PMCR_C; /* Reset counter */
        মান |= PMU_PMCR_E; /* Enable counter hardware */
        asm চঞ্চল ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (মান));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        মান = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm চঞ্চল ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (মান));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        মান = PMU_PMUSERENR_EN;
        asm চঞ্চল ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (মান));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
শূন্য do_ser_debug(শূন্য)
{
}

শূন্য arch_do_syscall(সংরচনা proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  প্রতিজ্ঞা(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_স্তূপ;

সংরচনা proc * arch_finish_switch_to_user(শূন্য)
{
	ৱর্ণ * stk;
	সংরচনা proc * p;

#ifdef CONFIG_SMP
	stk = (ৱর্ণ *)tss[cpuid].sp0;
#অন্যথা
	stk = (ৱর্ণ *)tss[0].sp0;
#endif
	svc_স্তূপ = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	ঘূৰাই p;
}

শূন্য fpu_sigcontext(সংরচনা proc *pr, সংরচনা sigframe_sigcontext *fr, সংরচনা sigcontext *sc)
{
}

reg_t arch_get_sp(সংরচনা proc *p) { ঘূৰাই p->p_reg.sp; }

শূন্য get_randomness(সংরচনা k_randomness *rand, পূর্ণাঙ্ক source)
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
শূন্য __switch_address_space(সংরচনা proc *p, সংরচনা proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	যদি (new_ttbr == 0)
	    ঘূৰাই;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	যদি (new_ttbr == orig_ttbr)
	    ঘূৰাই;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	ঘূৰাই;
}
