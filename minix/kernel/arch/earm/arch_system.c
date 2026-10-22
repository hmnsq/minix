/* system dependent functions for use inside the whole kernel. */

#شامل "kernel/kernel.h"

#شامل <unistd.h>
#شامل <ctype.h>
#شامل <string.h>
#شامل <minix/cpufeature.h>
#شامل <assert.h>
#شامل <signal.h>
#شامل <machine/vm.h>
#شامل <machine/signal.h>
#شامل <arm/armreg.h>

#شامل <minix/u64.h>

#شامل "archconst.h"
#شامل "arch_proto.h"
#شامل "kernel/proc.h"
#شامل "kernel/debug.h"
#شامل "ccnt.h"
#شامل "bsp_init.h"
#شامل "bsp_serial.h"

#شامل "glo.h"

تهی * k_stacks;


تهی fpu_init(تهی)
{
}

تهی save_local_fpu(ساختار proc *pr, صحیح retain)
{
}

تهی save_fpu(ساختار proc *pr)
{
}

تهی arch_proc_reset(ساختار proc *pr)
{
	أكد(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, اندازه(pr->p_reg));
	اگر(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} وگرنه {
		pr->p_reg.psr = INIT_PSR;
	}
}

تهی arch_proc_setcontext(ساختار proc *p, ساختار stackframe_s *state,
	صحیح isuser, صحیح trapstyle)
{
        أكد(اندازه(p->p_reg) == اندازه(*state));
	اگر(state != &p->p_reg) {
	        memcpy(&p->p_reg, state, اندازه(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_SET;

        اگر(!(p->p_rts_flags)) {
                printf("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

تهی arch_set_secondary_ipc_return(ساختار proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

صحیح restore_fpu(ساختار proc *pr)
{
	بازگشت 0;
}

تهی cpu_identify(تهی)
{
	u32_t midr;
	unsigned cpu = cpuid;

	asm فرار("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

تهی arch_init(تهی)
{
        u32_t مقدار;

	k_stacks = (تهی*) &k_stacks_start;
	أكد(!((vir_bytes) k_stacks % K_پشته_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_پشته_بالاترین(0));
#endif


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm فرار ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (مقدار));
        مقدار |= PMU_PMCR_C; /* Reset counter */
        مقدار |= PMU_PMCR_E; /* Enable counter hardware */
        asm فرار ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (مقدار));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        مقدار = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm فرار ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (مقدار));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        مقدار = PMU_PMUSERENR_EN;
        asm فرار ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (مقدار));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
تهی do_ser_debug(تهی)
{
}

تهی arch_do_syscall(ساختار proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  أكد(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_پشته;

ساختار proc * arch_finish_switch_to_user(تهی)
{
	نویسه * stk;
	ساختار proc * p;

#ifdef CONFIG_SMP
	stk = (نویسه *)tss[cpuid].sp0;
#وگرنه
	stk = (نویسه *)tss[0].sp0;
#endif
	svc_پشته = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	بازگشت p;
}

تهی fpu_sigcontext(ساختار proc *pr, ساختار sigframe_sigcontext *fr, ساختار sigcontext *sc)
{
}

reg_t arch_get_sp(ساختار proc *p) { بازگشت p->p_reg.sp; }

تهی get_randomness(ساختار k_randomness *rand, صحیح source)
{
}

تهی arch_ser_init(تهی)
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
تهی __switch_address_space(ساختار proc *p, ساختار proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	اگر (new_ttbr == 0)
	    بازگشت;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	اگر (new_ttbr == orig_ttbr)
	    بازگشت;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	بازگشت;
}
