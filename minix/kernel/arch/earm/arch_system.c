/* system dependent functions for use inside the whole kernel. */

#अन्तर्भाव "kernel/kernel.h"

#अन्तर्भाव <unistd.h>
#अन्तर्भाव <ctype.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <machine/vm.h>
#अन्तर्भाव <machine/signal.h>
#अन्तर्भाव <arm/armreg.h>

#अन्तर्भाव <minix/u64.h>

#अन्तर्भाव "archconst.h"
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव "kernel/proc.h"
#अन्तर्भाव "kernel/debug.h"
#अन्तर्भाव "ccnt.h"
#अन्तर्भाव "bsp_init.h"
#अन्तर्भाव "bsp_serial.h"

#अन्तर्भाव "glo.h"

शून्य * k_stacks;


शून्य fpu_init(शून्य)
{
}

शून्य save_local_fpu(संरचना proc *pr, पूर्णाङ्क retain)
{
}

शून्य save_fpu(संरचना proc *pr)
{
}

शून्य arch_proc_reset(संरचना proc *pr)
{
	प्रतिज्ञा(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, परिमाण(pr->p_reg));
	यदि(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} अन्यथा {
		pr->p_reg.psr = INIT_PSR;
	}
}

शून्य arch_proc_setcontext(संरचना proc *p, संरचना stackframe_s *state,
	पूर्णाङ्क isuser, पूर्णाङ्क trapstyle)
{
        प्रतिज्ञा(परिमाण(p->p_reg) == परिमाण(*state));
	यदि(state != &p->p_reg) {
	        memcpy(&p->p_reg, state, परिमाण(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_SET;

        यदि(!(p->p_rts_flags)) {
                printf("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

शून्य arch_set_secondary_ipc_return(संरचना proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

पूर्णाङ्क restore_fpu(संरचना proc *pr)
{
	प्रत्यागम 0;
}

शून्य cpu_identify(शून्य)
{
	u32_t midr;
	unsigned cpu = cpuid;

	asm चञ्चल("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

शून्य arch_init(शून्य)
{
        u32_t मूल्य;

	k_stacks = (शून्य*) &k_stacks_start;
	प्रतिज्ञा(!((vir_bytes) k_stacks % K_ढेर_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ढेर_शीर्ष(0));
#endif


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm चञ्चल ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (मूल्य));
        मूल्य |= PMU_PMCR_C; /* Reset counter */
        मूल्य |= PMU_PMCR_E; /* Enable counter hardware */
        asm चञ्चल ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (मूल्य));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        मूल्य = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm चञ्चल ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (मूल्य));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        मूल्य = PMU_PMUSERENR_EN;
        asm चञ्चल ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (मूल्य));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
शून्य do_ser_debug(शून्य)
{
}

शून्य arch_do_syscall(संरचना proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  प्रतिज्ञा(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_ढेर;

संरचना proc * arch_finish_switch_to_user(शून्य)
{
	वर्ण * stk;
	संरचना proc * p;

#ifdef CONFIG_SMP
	stk = (वर्ण *)tss[cpuid].sp0;
#अन्यथा
	stk = (वर्ण *)tss[0].sp0;
#endif
	svc_ढेर = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	प्रत्यागम p;
}

शून्य fpu_sigcontext(संरचना proc *pr, संरचना sigframe_sigcontext *fr, संरचना sigcontext *sc)
{
}

reg_t arch_get_sp(संरचना proc *p) { प्रत्यागम p->p_reg.sp; }

शून्य get_randomness(संरचना k_randomness *rand, पूर्णाङ्क source)
{
}

शून्य arch_ser_init(शून्य)
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
शून्य __switch_address_space(संरचना proc *p, संरचना proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	यदि (new_ttbr == 0)
	    प्रत्यागम;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	यदि (new_ttbr == orig_ttbr)
	    प्रत्यागम;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	प्रत्यागम;
}
