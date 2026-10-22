#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ਪੂਰਨ do_ਸਮਾਂ_ਸਾਰਣੀ(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
	ਬਣਤਰ proc *p;
	ਪੂਰਨ proc_nr;
	ਪੂਰਨ ਤਰਜੀਹ, ਸਮਾਂ_ਹਿੱਸਾ, cpu, niced;

	ਜੇ (!isokendpt(m_ptr->m_lsys_krn_ਸਮਾਂ_ਸਾਰਣੀ.endpoint, &proc_nr))
		ਵਾਪਸ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ਜੇ (caller != p->p_scheduler)
		ਵਾਪਸ(EPERM);

	/* Try to schedule the process. */
	ਤਰਜੀਹ = m_ptr->m_lsys_krn_ਸਮਾਂ_ਸਾਰਣੀ.ਤਰਜੀਹ;
	ਸਮਾਂ_ਹਿੱਸਾ = m_ptr->m_lsys_krn_ਸਮਾਂ_ਸਾਰਣੀ.ਸਮਾਂ_ਹਿੱਸਾ;
	cpu = m_ptr->m_lsys_krn_ਸਮਾਂ_ਸਾਰਣੀ.cpu;
	niced = !!(m_ptr->m_lsys_krn_ਸਮਾਂ_ਸਾਰਣੀ.niced);

	ਵਾਪਸ sched_proc(p, ਤਰਜੀਹ, ਸਮਾਂ_ਹਿੱਸਾ, cpu, niced);
}
