#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ਪੂਰਨ do_schedctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
	ਬਣਤਰ proc *p;
	uint32_t flags;
	ਪੂਰਨ ਤਰਜੀਹ, ਸਮਾਂ_ਹਿੱਸਾ, cpu;
	ਪੂਰਨ proc_nr;
	ਪੂਰਨ r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ਜੇ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		ਲਿਖੋ("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ਵਾਪਸ EINVAL;
	}

	ਜੇ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ਵਾਪਸ EINVAL;

	p = proc_addr(proc_nr);

	ਜੇ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		ਤਰਜੀਹ = m_ptr->m_lsys_krn_schedctl.ਤਰਜੀਹ;
		ਸਮਾਂ_ਹਿੱਸਾ = m_ptr->m_lsys_krn_schedctl.ਸਮਾਂ_ਹਿੱਸਾ;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ਜੇ((r = sched_proc(p, ਤਰਜੀਹ, ਸਮਾਂ_ਹਿੱਸਾ, cpu, FALSE)) != OK)
			ਵਾਪਸ r;
		p->p_scheduler = ਨਿਰਾ;
	} ਨਹੀਂਤਾਂ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ਵਾਪਸ(OK);
}
