/* system dependent functions for use inside the whole kernel. */

#ضمن "kernel/kernel.h"

#ضمن <unistd.h>
#ضمن <ctype.h>
#ضمن <string.h>
#ضمن <minix/cpufeature.h>
#ضمن <assert.h>
#ضمن <signal.h>
#ضمن <machine/vm.h>
#ضمن <machine/signal.h>
#ضمن <arm/armreg.h>

#ضمن <minix/u64.h>

#ضمن "archconst.h"
#ضمن "arch_proto.h"
#ضمن "kernel/proc.h"
#ضمن "kernel/debug.h"
#ضمن "ccnt.h"
#ضمن "bsp_init.h"
#ضمن "bsp_serial.h"

#ضمن "glo.h"

فراغ * k_stacks;


فراغ fpu_init(فراغ)
{
}

فراغ save_local_fpu(بنية proc *pr, صحيح retain)
{
}

فراغ save_fpu(بنية proc *pr)
{
}

فراغ arch_proc_reset(بنية proc *pr)
{
	أكد(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, حجم(pr->p_reg));
	إذا(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} وإلا {
		pr->p_reg.psr = INIT_PSR;
	}
}

فراغ arch_proc_setcontext(بنية proc *p, بنية stackframe_s *state,
	صحيح isuser, صحيح trapstyle)
{
        أكد(حجم(p->p_reg) == حجم(*state));
	إذا(state != &p->p_reg) {
	        يادگيري_نقل(&p->p_reg, state, حجم(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_سيٽ;

        إذا(!(p->p_rts_flags)) {
                لکو("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

فراغ arch_سيٽ_secondary_ipc_return(بنية proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

صحيح restore_fpu(بنية proc *pr)
{
	أرجع 0;
}

فراغ cpu_identify(فراغ)
{
	u32_t midr;
	بنانشان cpu = cpuid;

	asm متطاير("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

فراغ arch_init(فراغ)
{
        u32_t قدر;

	k_stacks = (فراغ*) &k_stacks_شروعات;
	أكد(!((vir_بائيٽون) k_stacks % K_ڍير_سائيز));

#جيڪڏهن_وصف_نه_ٿيل CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ڍير_چوٽي(0));
#جيڪڏهن_پورو


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm متطاير ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (قدر));
        قدر |= PMU_PMCR_C; /* Reset counter */
        قدر |= PMU_PMCR_E; /* Enable counter hardware */
        asm متطاير ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (قدر));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        قدر = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm متطاير ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (قدر));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        قدر = PMU_PMUSERENR_EN;
        asm متطاير ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (قدر));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
فراغ do_ser_debug(فراغ)
{
}

فراغ arch_do_syscall(بنية proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  أكد(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_ڍير;

بنية proc * arch_پڄاڻي_switch_منزل_user(فراغ)
{
	حرف * stk;
	بنية proc * p;

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
	stk = (حرف *)tss[cpuid].sp0;
#وإلا
	stk = (حرف *)tss[0].sp0;
#جيڪڏهن_پورو
	svc_ڍير = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	أرجع p;
}

فراغ fpu_sigcontext(بنية proc *pr, بنية sigframe_sigcontext *fr, بنية sigcontext *sc)
{
}

reg_t arch_get_sp(بنية proc *p) { أرجع p->p_reg.sp; }

فراغ get_randomness(بنية k_randomness *rand, صحيح ذريعو)
{
}

فراغ arch_ser_init(فراغ)
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
فراغ __switch_address_space(بنية proc *p, بنية proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	إذا (new_ttbr == 0)
	    أرجع;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	إذا (new_ttbr == orig_ttbr)
	    أرجع;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	أرجع;
}
