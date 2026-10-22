/* system dependent functions for use inside the whole kernel. */

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <minix/cpufeature.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <machine/vm.h>
#ଅନ୍ତର୍ଭାଵ <machine/signal.h>
#ଅନ୍ତର୍ଭାଵ <arm/armreg.h>

#ଅନ୍ତର୍ଭାଵ <minix/u64.h>

#ଅନ୍ତର୍ଭାଵ "archconst.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"
#ଅନ୍ତର୍ଭାଵ "kernel/proc.h"
#ଅନ୍ତର୍ଭାଵ "kernel/debug.h"
#ଅନ୍ତର୍ଭାଵ "ccnt.h"
#ଅନ୍ତର୍ଭାଵ "bsp_init.h"
#ଅନ୍ତର୍ଭାଵ "bsp_serial.h"

#ଅନ୍ତର୍ଭାଵ "glo.h"

ଶୂନ୍ୟ * k_stacks;


ଶୂନ୍ୟ fpu_init(ଶୂନ୍ୟ)
{
}

ଶୂନ୍ୟ save_local_fpu(ସଂରଚନା proc *pr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain)
{
}

ଶୂନ୍ୟ save_fpu(ସଂରଚନା proc *pr)
{
}

ଶୂନ୍ୟ arch_proc_reset(ସଂରଚନା proc *pr)
{
	ପ୍ରତିଜ୍ଞା(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, ଆକାର(pr->p_reg));
	ଯଦି(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} ଅନ୍ୟଥା {
		pr->p_reg.psr = INIT_PSR;
	}
}

ଶୂନ୍ୟ arch_proc_setcontext(ସଂରଚନା proc *p, ସଂରଚନା stackframe_s *state,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା isuser, ପୂର୍ଣ୍ଣସଂଖ୍ୟା trapstyle)
{
        ପ୍ରତିଜ୍ଞା(ଆକାର(p->p_reg) == ଆକାର(*state));
	ଯଦି(state != &p->p_reg) {
	        memcpy(&p->p_reg, state, ଆକାର(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_SET;

        ଯଦି(!(p->p_rts_flags)) {
                printf("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

ଶୂନ୍ୟ arch_set_secondary_ipc_return(ସଂରଚନା proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା restore_fpu(ସଂରଚନା proc *pr)
{
	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ cpu_identify(ଶୂନ୍ୟ)
{
	u32_t midr;
	unsigned cpu = cpuid;

	asm ଅସ୍ଥିର("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

ଶୂନ୍ୟ arch_init(ଶୂନ୍ୟ)
{
        u32_t ମୂଲ୍ୟ;

	k_stacks = (ଶୂନ୍ୟ*) &k_stacks_start;
	ପ୍ରତିଜ୍ଞା(!((vir_bytes) k_stacks % K_ଗଦା_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ଗଦା_ଶୀର୍ଷ(0));
#endif


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm ଅସ୍ଥିର ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (ମୂଲ୍ୟ));
        ମୂଲ୍ୟ |= PMU_PMCR_C; /* Reset counter */
        ମୂଲ୍ୟ |= PMU_PMCR_E; /* Enable counter hardware */
        asm ଅସ୍ଥିର ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (ମୂଲ୍ୟ));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        ମୂଲ୍ୟ = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm ଅସ୍ଥିର ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (ମୂଲ୍ୟ));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        ମୂଲ୍ୟ = PMU_PMUSERENR_EN;
        asm ଅସ୍ଥିର ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (ମୂଲ୍ୟ));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
ଶୂନ୍ୟ do_ser_debug(ଶୂନ୍ୟ)
{
}

ଶୂନ୍ୟ arch_do_syscall(ସଂରଚନା proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ପ୍ରତିଜ୍ଞା(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_ଗଦା;

ସଂରଚନା proc * arch_finish_switch_to_user(ଶୂନ୍ୟ)
{
	ଅକ୍ଷର * stk;
	ସଂରଚନା proc * p;

#ifdef CONFIG_SMP
	stk = (ଅକ୍ଷର *)tss[cpuid].sp0;
#ଅନ୍ୟଥା
	stk = (ଅକ୍ଷର *)tss[0].sp0;
#endif
	svc_ଗଦା = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	ଫେରନ୍ତୁ p;
}

ଶୂନ୍ୟ fpu_sigcontext(ସଂରଚନା proc *pr, ସଂରଚନା sigframe_sigcontext *fr, ସଂରଚନା sigcontext *sc)
{
}

reg_t arch_get_sp(ସଂରଚନା proc *p) { ଫେରନ୍ତୁ p->p_reg.sp; }

ଶୂନ୍ୟ get_randomness(ସଂରଚନା k_randomness *rand, ପୂର୍ଣ୍ଣସଂଖ୍ୟା source)
{
}

ଶୂନ୍ୟ arch_ser_init(ଶୂନ୍ୟ)
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
ଶୂନ୍ୟ __switch_address_space(ସଂରଚନା proc *p, ସଂରଚନା proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	ଯଦି (new_ttbr == 0)
	    ଫେରନ୍ତୁ;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	ଯଦି (new_ttbr == orig_ttbr)
	    ଫେରନ୍ତୁ;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	ଫେରନ୍ତୁ;
}
