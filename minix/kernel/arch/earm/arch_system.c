/* system dependent functions for use inside the whole kernel. */

#అన్తర్భావ "kernel/kernel.h"

#అన్తర్భావ <unistd.h>
#అన్తర్భావ <ctype.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <minix/cpufeature.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <machine/vm.h>
#అన్తర్భావ <machine/signal.h>
#అన్తర్భావ <arm/armreg.h>

#అన్తర్భావ <minix/u64.h>

#అన్తర్భావ "archconst.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "kernel/proc.h"
#అన్తర్భావ "kernel/debug.h"
#అన్తర్భావ "ccnt.h"
#అన్తర్భావ "bsp_init.h"
#అన్తర్భావ "bsp_serial.h"

#అన్తర్భావ "glo.h"

శూన్య * k_stacks;


శూన్య fpu_init(శూన్య)
{
}

శూన్య save_local_fpu(నిర్మాణం proc *pr, పూర్ణాంకం retain)
{
}

శూన్య save_fpu(నిర్మాణం proc *pr)
{
}

శూన్య arch_proc_reset(నిర్మాణం proc *pr)
{
	ప్రతిజ్ఞా(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, పరిమాణం(pr->p_reg));
	ఒకవేళ(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} లేకపోతే {
		pr->p_reg.psr = INIT_PSR;
	}
}

శూన్య arch_proc_setcontext(నిర్మాణం proc *p, నిర్మాణం stackframe_s *state,
	పూర్ణాంకం isuser, పూర్ణాంకం trapstyle)
{
        ప్రతిజ్ఞా(పరిమాణం(p->p_reg) == పరిమాణం(*state));
	ఒకవేళ(state != &p->p_reg) {
	        జ్ఞాపకం_నకలు(&p->p_reg, state, పరిమాణం(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_సమితి;

        ఒకవేళ(!(p->p_rts_flags)) {
                రాయి("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

శూన్య arch_సమితి_secondary_ipc_return(నిర్మాణం proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

పూర్ణాంకం restore_fpu(నిర్మాణం proc *pr)
{
	తిరిగి 0;
}

శూన్య cpu_identify(శూన్య)
{
	u32_t midr;
	గుర్తులేని cpu = cpuid;

	asm అస్థిర("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

శూన్య arch_init(శూన్య)
{
        u32_t విలువ;

	k_stacks = (శూన్య*) &k_stacks_ప్రారంభం;
	ప్రతిజ్ఞా(!((vir_బైట్లు) k_stacks % K_దొంతర_కొలత));

#నిర్వచితం_కాకపోతే CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_దొంతర_శిఖరం(0));
#అయితే_ముగింపు


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm అస్థిర ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (విలువ));
        విలువ |= PMU_PMCR_C; /* Reset counter */
        విలువ |= PMU_PMCR_E; /* Enable counter hardware */
        asm అస్థిర ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (విలువ));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        విలువ = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm అస్థిర ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (విలువ));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        విలువ = PMU_PMUSERENR_EN;
        asm అస్థిర ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (విలువ));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
శూన్య do_ser_debug(శూన్య)
{
}

శూన్య arch_do_syscall(నిర్మాణం proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ప్రతిజ్ఞా(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_దొంతర;

నిర్మాణం proc * arch_ముగింపు_switch_గమ్యం_user(శూన్య)
{
	అక్షరం * stk;
	నిర్మాణం proc * p;

#నిర్వచితమైతే CONFIG_SMP
	stk = (అక్షరం *)tss[cpuid].sp0;
#లేకపోతే
	stk = (అక్షరం *)tss[0].sp0;
#అయితే_ముగింపు
	svc_దొంతర = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	తిరిగి p;
}

శూన్య fpu_sigcontext(నిర్మాణం proc *pr, నిర్మాణం sigframe_sigcontext *fr, నిర్మాణం sigcontext *sc)
{
}

reg_t arch_get_sp(నిర్మాణం proc *p) { తిరిగి p->p_reg.sp; }

శూన్య get_randomness(నిర్మాణం k_randomness *rand, పూర్ణాంకం ఆధారం)
{
}

శూన్య arch_ser_init(శూన్య)
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
శూన్య __switch_address_space(నిర్మాణం proc *p, నిర్మాణం proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	ఒకవేళ (new_ttbr == 0)
	    తిరిగి;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	ఒకవేళ (new_ttbr == orig_ttbr)
	    తిరిగి;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	తిరిగి;
}
