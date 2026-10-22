/* system dependent functions for use inside the whole kernel. */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arm/armreg.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "archconst.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/debug.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ccnt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_init.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_serial.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"

ਖਾਲੀ * k_stacks;


ਖਾਲੀ fpu_init(ਖਾਲੀ)
{
}

ਖਾਲੀ save_local_fpu(ਬਣਤਰ proc *pr, ਪੂਰਨ retain)
{
}

ਖਾਲੀ save_fpu(ਬਣਤਰ proc *pr)
{
}

ਖਾਲੀ arch_proc_reset(ਬਣਤਰ proc *pr)
{
	ਪ੍ਰਤਿਜ੍ਞਾ(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, ਆਕਾਰ(pr->p_reg));
	ਜੇ(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} ਨਹੀਂਤਾਂ {
		pr->p_reg.psr = INIT_PSR;
	}
}

ਖਾਲੀ arch_proc_setcontext(ਬਣਤਰ proc *p, ਬਣਤਰ stackframe_s *state,
	ਪੂਰਨ isuser, ਪੂਰਨ trapstyle)
{
        ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(p->p_reg) == ਆਕਾਰ(*state));
	ਜੇ(state != &p->p_reg) {
	        ਸਮ੍ਰਿਤੀ_ਨਕਲ(&p->p_reg, state, ਆਕਾਰ(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_ਸੈੱਟ;

        ਜੇ(!(p->p_rts_flags)) {
                ਲਿਖੋ("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

ਖਾਲੀ arch_ਸੈੱਟ_secondary_ipc_return(ਬਣਤਰ proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

ਪੂਰਨ restore_fpu(ਬਣਤਰ proc *pr)
{
	ਵਾਪਸ 0;
}

ਖਾਲੀ cpu_identify(ਖਾਲੀ)
{
	u32_t midr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;

	asm ਅਸਥਿਰ("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

ਖਾਲੀ arch_init(ਖਾਲੀ)
{
        u32_t ਮੁੱਲ;

	k_stacks = (ਖਾਲੀ*) &k_stacks_ਸ਼ੁਰੂ;
	ਪ੍ਰਤਿਜ੍ਞਾ(!((vir_ਬਾਈਟਾਂ) k_stacks % K_ਢੇਰ_ਮਾਪ));

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ਢੇਰ_ਸਿਖਰ(0));
#ਜੇ_ਅੰਤ


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm ਅਸਥਿਰ ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (ਮੁੱਲ));
        ਮੁੱਲ |= PMU_PMCR_C; /* Reset counter */
        ਮੁੱਲ |= PMU_PMCR_E; /* Enable counter hardware */
        asm ਅਸਥਿਰ ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (ਮੁੱਲ));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        ਮੁੱਲ = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm ਅਸਥਿਰ ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (ਮੁੱਲ));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        ਮੁੱਲ = PMU_PMUSERENR_EN;
        asm ਅਸਥਿਰ ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (ਮੁੱਲ));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
ਖਾਲੀ do_ser_debug(ਖਾਲੀ)
{
}

ਖਾਲੀ arch_do_syscall(ਬਣਤਰ proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  ਪ੍ਰਤਿਜ੍ਞਾ(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_ਢੇਰ;

ਬਣਤਰ proc * arch_ਅੰਤ_switch_ਮੰਜ਼ਿਲ_user(ਖਾਲੀ)
{
	ਅੱਖਰ * stk;
	ਬਣਤਰ proc * p;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	stk = (ਅੱਖਰ *)tss[cpuid].sp0;
#ਨਹੀਂਤਾਂ
	stk = (ਅੱਖਰ *)tss[0].sp0;
#ਜੇ_ਅੰਤ
	svc_ਢੇਰ = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	ਵਾਪਸ p;
}

ਖਾਲੀ fpu_sigcontext(ਬਣਤਰ proc *pr, ਬਣਤਰ sigframe_sigcontext *fr, ਬਣਤਰ sigcontext *sc)
{
}

reg_t arch_get_sp(ਬਣਤਰ proc *p) { ਵਾਪਸ p->p_reg.sp; }

ਖਾਲੀ get_randomness(ਬਣਤਰ k_randomness *rand, ਪੂਰਨ ਸਰੋਤ)
{
}

ਖਾਲੀ arch_ser_init(ਖਾਲੀ)
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
ਖਾਲੀ __switch_address_space(ਬਣਤਰ proc *p, ਬਣਤਰ proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	ਜੇ (new_ttbr == 0)
	    ਵਾਪਸ;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	ਜੇ (new_ttbr == orig_ttbr)
	    ਵਾਪਸ;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	ਵਾਪਸ;
}
