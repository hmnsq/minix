/* system dependent functions for use inside the whole kernel. */

#समावेश "kernel/kernel.h"

#समावेश <unistd.h>
#समावेश <ctype.h>
#समावेश <string.h>
#समावेश <minix/cpufeature.h>
#समावेश <assert.h>
#समावेश <signal.h>
#समावेश <machine/vm.h>
#समावेश <machine/signal.h>
#समावेश <arm/armreg.h>

#समावेश <minix/u64.h>

#समावेश "archconst.h"
#समावेश "arch_proto.h"
#समावेश "kernel/proc.h"
#समावेश "kernel/debug.h"
#समावेश "ccnt.h"
#समावेश "bsp_init.h"
#समावेश "bsp_serial.h"

#समावेश "glo.h"

व्योम * k_stacks;


व्योम fpu_init(व्योम)
{
}

व्योम save_local_fpu(काष्ठा proc *pr, पूर्णांक retain)
{
}

व्योम save_fpu(काष्ठा proc *pr)
{
}

व्योम arch_proc_reset(काष्ठा proc *pr)
{
	निश्चित(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	स_रखो(&pr->p_reg, 0, माप(pr->p_reg));
	अगर(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} अन्यथा {
		pr->p_reg.psr = INIT_PSR;
	}
}

व्योम arch_proc_setcontext(काष्ठा proc *p, काष्ठा stackframe_s *state,
	पूर्णांक isuser, पूर्णांक trapstyle)
{
        निश्चित(माप(p->p_reg) == माप(*state));
	अगर(state != &p->p_reg) {
	        स_नकल(&p->p_reg, state, माप(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_SET;

        अगर(!(p->p_rts_flags)) {
                म_लिखो("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

व्योम arch_set_secondary_ipc_return(काष्ठा proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

पूर्णांक restore_fpu(काष्ठा proc *pr)
{
	वापस 0;
}

व्योम cpu_identify(व्योम)
{
	u32_t midr;
	अचिन्हित cpu = cpuid;

	यंत्र अस्थिर("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

व्योम arch_init(व्योम)
{
        u32_t मूल्य;

	k_stacks = (व्योम*) &k_stacks_start;
	निश्चित(!((vir_bytes) k_stacks % K_ढेर_आकार));

#अगर_अघोषित CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ढेर_शीर्ष(0));
#पूर्ण_अगर


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        यंत्र अस्थिर ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (मूल्य));
        मूल्य |= PMU_PMCR_C; /* Reset counter */
        मूल्य |= PMU_PMCR_E; /* Enable counter hardware */
        यंत्र अस्थिर ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (मूल्य));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        मूल्य = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        यंत्र अस्थिर ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (मूल्य));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        मूल्य = PMU_PMUSERENR_EN;
        यंत्र अस्थिर ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (मूल्य));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
व्योम do_ser_debug(व्योम)
{
}

व्योम arch_do_syscall(काष्ठा proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  निश्चित(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_ढेर;

काष्ठा proc * arch_finish_switch_to_user(व्योम)
{
	अक्षर * stk;
	काष्ठा proc * p;

#अगर_घोषित CONFIG_SMP
	stk = (अक्षर *)tss[cpuid].sp0;
#अन्यथा
	stk = (अक्षर *)tss[0].sp0;
#पूर्ण_अगर
	svc_ढेर = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	वापस p;
}

व्योम fpu_sigcontext(काष्ठा proc *pr, काष्ठा sigframe_sigcontext *fr, काष्ठा sigcontext *sc)
{
}

reg_t arch_get_sp(काष्ठा proc *p) { वापस p->p_reg.sp; }

व्योम get_randomness(काष्ठा k_randomness *अक्रम, पूर्णांक source)
{
}

व्योम arch_ser_init(व्योम)
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
व्योम __switch_address_space(काष्ठा proc *p, काष्ठा proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	अगर (new_ttbr == 0)
	    वापस;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	अगर (new_ttbr == orig_ttbr)
	    वापस;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	वापस;
}
