/* system dependent functions for use inside the whole kernel. */

#અન્તર્ભાવ "kernel/kernel.h"

#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <minix/cpufeature.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <machine/vm.h>
#અન્તર્ભાવ <machine/signal.h>
#અન્તર્ભાવ <arm/armreg.h>

#અન્તર્ભાવ <minix/u64.h>

#અન્તર્ભાવ "archconst.h"
#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ "kernel/proc.h"
#અન્તર્ભાવ "kernel/debug.h"
#અન્તર્ભાવ "ccnt.h"
#અન્તર્ભાવ "bsp_init.h"
#અન્તર્ભાવ "bsp_serial.h"

#અન્તર્ભાવ "glo.h"

શૂન્ય * k_stacks;


શૂન્ય fpu_init(શૂન્ય)
{
}

શૂન્ય save_local_fpu(સંરચના proc *pr, પૂર્ણાંક retain)
{
}

શૂન્ય save_fpu(સંરચના proc *pr)
{
}

શૂન્ય arch_proc_reset(સંરચના proc *pr)
{
	પ્રતિજ્ઞા(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, કદ(pr->p_reg));
	જો(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} નહીંતર {
		pr->p_reg.psr = INIT_PSR;
	}
}

શૂન્ય arch_proc_setcontext(સંરચના proc *p, સંરચના stackframe_s *state,
	પૂર્ણાંક isuser, પૂર્ણાંક trapstyle)
{
        પ્રતિજ્ઞા(કદ(p->p_reg) == કદ(*state));
	જો(state != &p->p_reg) {
	        memcpy(&p->p_reg, state, કદ(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_SET;

        જો(!(p->p_rts_flags)) {
                printf("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

શૂન્ય arch_set_secondary_ipc_return(સંરચના proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

પૂર્ણાંક restore_fpu(સંરચના proc *pr)
{
	પાછા 0;
}

શૂન્ય cpu_identify(શૂન્ય)
{
	u32_t midr;
	unsigned cpu = cpuid;

	asm અસ્થિર("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

શૂન્ય arch_init(શૂન્ય)
{
        u32_t કિંમત;

	k_stacks = (શૂન્ય*) &k_stacks_start;
	પ્રતિજ્ઞા(!((vir_bytes) k_stacks % K_ઢગલો_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ઢગલો_ટોચ(0));
#endif


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm અસ્થિર ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (કિંમત));
        કિંમત |= PMU_PMCR_C; /* Reset counter */
        કિંમત |= PMU_PMCR_E; /* Enable counter hardware */
        asm અસ્થિર ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (કિંમત));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        કિંમત = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm અસ્થિર ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (કિંમત));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        કિંમત = PMU_PMUSERENR_EN;
        asm અસ્થિર ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (કિંમત));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
શૂન્ય do_ser_debug(શૂન્ય)
{
}

શૂન્ય arch_do_syscall(સંરચના proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  પ્રતિજ્ઞા(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_ઢગલો;

સંરચના proc * arch_finish_switch_to_user(શૂન્ય)
{
	અક્ષર * stk;
	સંરચના proc * p;

#ifdef CONFIG_SMP
	stk = (અક્ષર *)tss[cpuid].sp0;
#નહીંતર
	stk = (અક્ષર *)tss[0].sp0;
#endif
	svc_ઢગલો = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	પાછા p;
}

શૂન્ય fpu_sigcontext(સંરચના proc *pr, સંરચના sigframe_sigcontext *fr, સંરચના sigcontext *sc)
{
}

reg_t arch_get_sp(સંરચના proc *p) { પાછા p->p_reg.sp; }

શૂન્ય get_randomness(સંરચના k_randomness *rand, પૂર્ણાંક source)
{
}

શૂન્ય arch_ser_init(શૂન્ય)
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
શૂન્ય __switch_address_space(સંરચના proc *p, સંરચના proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	જો (new_ttbr == 0)
	    પાછા;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	જો (new_ttbr == orig_ttbr)
	    પાછા;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	પાછા;
}
