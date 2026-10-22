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

خالی * k_stacks;


خالی fpu_init(خالی)
{
}

خالی save_local_fpu(ساخت proc *pr, صحیح retain)
{
}

خالی save_fpu(ساخت proc *pr)
{
}

خالی arch_proc_reset(ساخت proc *pr)
{
	أكد(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	memset(&pr->p_reg, 0, حجم(pr->p_reg));
	اگر(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} ورنہ {
		pr->p_reg.psr = INIT_PSR;
	}
}

خالی arch_proc_setcontext(ساخت proc *p, ساخت stackframe_s *state,
	صحیح isuser, صحیح trapstyle)
{
        أكد(حجم(p->p_reg) == حجم(*state));
	اگر(state != &p->p_reg) {
	        حافظہ_نقل(&p->p_reg, state, حجم(*state));
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_سیٹ;

        اگر(!(p->p_rts_flags)) {
                لکھو("WARNINIG: setting full context of runnable process\n");
                print_proc(p);
                util_stacktrace();
        }
}

خالی arch_سیٹ_secondary_ipc_return(ساخت proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

صحیح restore_fpu(ساخت proc *pr)
{
	واپس 0;
}

خالی cpu_identify(خالی)
{
	u32_t midr;
	غیرعلامتی cpu = cpuid;

	asm متلون("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

خالی arch_init(خالی)
{
        u32_t قدر;

	k_stacks = (خالی*) &k_stacks_آغاز;
	أكد(!((vir_بائٹیں) k_stacks % K_ڈھیر_سائز));

#اگر_غیر_تعریف_شدہ CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_ڈھیر_چوٹی(0));
#اگر_ختم


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm متلون ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (قدر));
        قدر |= PMU_PMCR_C; /* Reset counter */
        قدر |= PMU_PMCR_E; /* Enable counter hardware */
        asm متلون ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (قدر));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        قدر = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm متلون ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (قدر));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        قدر = PMU_PMUSERENR_EN;
        asm متلون ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (قدر));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
خالی do_ser_debug(خالی)
{
}

خالی arch_do_syscall(ساخت proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  أكد(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_ڈھیر;

ساخت proc * arch_اختتام_switch_مقصد_user(خالی)
{
	حرف * stk;
	ساخت proc * p;

#اگر_تعریف_شدہ CONFIG_SMP
	stk = (حرف *)tss[cpuid].sp0;
#ورنہ
	stk = (حرف *)tss[0].sp0;
#اگر_ختم
	svc_ڈھیر = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	واپس p;
}

خالی fpu_sigcontext(ساخت proc *pr, ساخت sigframe_sigcontext *fr, ساخت sigcontext *sc)
{
}

reg_t arch_get_sp(ساخت proc *p) { واپس p->p_reg.sp; }

خالی get_randomness(ساخت k_randomness *rand, صحیح ماخذ)
{
}

خالی arch_ser_init(خالی)
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
خالی __switch_address_space(ساخت proc *p, ساخت proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	اگر (new_ttbr == 0)
	    واپس;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	اگر (new_ttbr == orig_ttbr)
	    واپس;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	واپس;
}
