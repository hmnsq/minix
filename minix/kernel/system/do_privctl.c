/* The kernel call implemented in this file:
 *   m_type:	SYS_PRIVCTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_privctl.endpt		(process endpoint of target)
 *   m_lsys_krn_sys_privctl.request		(privilege control request)
 *   m_lsys_krn_sys_privctl.arg_ptr		(pointer to request data)
 *   m.m_lsys_krn_sys_privctl.phys_start
 *   m.m_lsys_krn_sys_privctl.phys_len
 */

#ضمن "kernel/system.h"
#ضمن <signal.h>
#ضمن <string.h>
#ضمن <minix/endpoint.h>

#إذا USE_PRIVCTL

#تعریف_کړه PRIV_DEBUG 0

ساكن صحيح update_priv(بنية proc *rp, بنية priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
صحيح do_privctl(بنية proc * caller, پیغام * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  بنية proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  صحيح ipc_منزل_m, kcalls;
  صحيح i, r;
  بنية io_range io_range;
  بنية minix_mem_range mem_range;
  بنية priv priv;
  صحيح irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  إذا (! (priv(caller)->s_flags & SYS_PROC)) أرجع(EPERM);
  إذا(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  وإلا إذا(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	أرجع(EINVAL);
  rp = proc_addr(proc_nr);

  بدل(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  حالة SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	إذا (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		أرجع(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	أرجع(OK);

  حالة SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	إذا (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		أرجع(EPERM);
	}
	RTS_سېټ(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	أرجع(OK);

  حالة SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	إذا (RTS_ISSET(rp, RTS_NO_PRIV)) أرجع(EPERM);
	RTS_سېټ(rp, RTS_NO_PRIV);
	أرجع(OK);

  حالة SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	أرجع(OK);

  حالة SYS_PRIV_سېټ_SYS:
	/* Set a privilege structure of a blocked system process. */
	إذا (! RTS_ISSET(rp, RTS_NO_PRIV)) أرجع(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	إذا (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		إذا((r=معلومات_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_بایټونه) &priv, حجم(priv))) != OK)
			أرجع r;

		/* See if the caller wants to assign a static privilege id. */
		إذا(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	إذا ((i=get_priv(rp, priv_id)) != OK)
	{
		چاپ_کړه("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		أرجع(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	لكل (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	لكل (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
	      priv(rp)->s_notify_pending.chunk[i] = 0;	/* - notifications */
	priv(rp)->s_int_pending = 0;			/* - interrupts */
	(فراغ) sigemptyset(&priv(rp)->s_sig_pending);	/* - signals */
	reset_kernel_timer(&priv(rp)->s_alarm_timer);	/* - alarm */
	priv(rp)->s_asyntab= -1;			/* - asynsends */
	priv(rp)->s_asynsize= 0;
	priv(rp)->s_asynendpoint = rp->p_endpoint;
	priv(rp)->s_diag_sig = FALSE;		/* no request for diag sigs */

	/* Set defaults for privilege bitmaps. */
	priv(rp)->s_flags= DSRV_F;           /* privilege flags */
	priv(rp)->s_init_flags= DSRV_I;      /* initialization flags */
	priv(rp)->s_trap_ماسک= DSRV_T;       /* allowed traps */
	memset(&map, 0, حجم(map));
	ipc_منزل_m = DSRV_M;                   /* allowed targets */
	إذا (ipc_منزل_m == ALL_M) {
		لكل (i = 0; i < NR_SYS_PROCS; i++)
			سېټ_sys_بټ(map, i);
	}
	fill_sendto_ماسک(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	لكل(i = 0; i < SYS_CALL_ماسک_اندازه; i++) {
		priv(rp)->s_k_call_ماسک[i] = (kcalls == NO_C ? 0 : (~0));
	}

	/* Set the default signal managers. */
	priv(rp)->s_sig_mgr = DSRV_SM;
	priv(rp)->s_bak_sig_mgr = NONE;

	/* Set defaults for resources: no I/O resources, no memory resources,
	 * no IRQs, no grant table, no ipc filter
	 */
	priv(rp)->s_nr_io_range= 0;
	priv(rp)->s_nr_mem_range= 0;
	priv(rp)->s_nr_irq= 0;
	priv(rp)->s_grant_جدول= 0;
	priv(rp)->s_grant_entries= 0;
	priv(rp)->s_grant_endpoint = rp->p_endpoint;
	priv(rp)->s_state_جدول= 0;
	priv(rp)->s_state_entries= 0;
	priv(rp)->s_ipcf= 0;

	/* Override defaults if the caller has supplied a privilege structure. */
	إذا (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		إذا((r = update_priv(rp, &priv)) != OK) {
			أرجع r;
		} 
	}

	أرجع(OK);

  حالة SYS_PRIV_سېټ_USER:
	/* Set a privilege structure of a blocked user process. */
	إذا (!RTS_ISSET(rp, RTS_NO_PRIV)) أرجع(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	أرجع(OK);

  حالة SYS_PRIV_جمع_کړه_IO:
	إذا (RTS_ISSET(rp, RTS_NO_PRIV))
		أرجع(EPERM);

#إذا 0 /* XXX -- do we need a call for this? */
	إذا (strcmp(rp->p_name, "fxp") == 0 ||
		strcmp(rp->p_name, "rtl8139") == 0)
	{
		چاپ_کړه("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_موخه= rp->p_endpoint;
	}
#د_که_پای

	/* Get the I/O range */
	معلومات_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_بایټونه) &io_range, حجم(io_range));
	/* Add the I/O range */
	أرجع priv_جمع_کړه_io(rp, &io_range);

  حالة SYS_PRIV_جمع_کړه_MEM:
	إذا (RTS_ISSET(rp, RTS_NO_PRIV))
		أرجع(EPERM);

	/* Get the memory range */
	إذا((r=معلومات_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_بایټونه) &mem_range, حجم(mem_range))) != OK)
		أرجع r;
	/* Add the memory range */
	أرجع priv_جمع_کړه_mem(rp, &mem_range);

  حالة SYS_PRIV_جمع_کړه_IRQ:
	إذا (RTS_ISSET(rp, RTS_NO_PRIV))
		أرجع(EPERM);

#إذا 0
	/* Only system processes get IRQs? */
	إذا (!(priv(rp)->s_flags & SYS_PROC))
		أرجع EPERM;
#د_که_پای
	معلومات_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_بایټونه) &irq, حجم(irq));
	/* Add the IRQ. */
	أرجع priv_جمع_کړه_irq(rp, irq);

  حالة SYS_PRIV_QUERY_MEM:
  {
	phys_بایټونه addr, حد_2;
  	بنية priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_بایټونه) m_ptr->m_lsys_krn_sys_privctl.phys_پیل;
	حد_2 = addr + (phys_بایټونه) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	إذا(حد_2 < addr)
		أرجع EPERM;
	إذا(!(sp = priv(rp)))
		أرجع EPERM;
	لكل(i = 0; i < sp->s_nr_mem_range; i++) {
		إذا(addr >= sp->s_mem_tab[i].mr_بنسټ &&
		   حد_2 <= sp->s_mem_tab[i].mr_حد)
			أرجع OK;
	}
	أرجع EPERM;
  }

  حالة SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	إذا(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) أرجع EINVAL;

	/* Copy privilege structure from caller */
	إذا((r=معلومات_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_بایټونه) &priv, حجم(priv))) != OK)
		أرجع r;

	/* Override settings in existing privilege structure. */
	إذا((r = update_priv(rp, &priv)) != OK) {
		أرجع r;
	}

	أرجع(OK);

  افتراضي:
	چاپ_کړه("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	أرجع EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
ساكن صحيح update_priv(بنية proc *rp, بنية priv *priv)
{
/* Update the privilege structure of a given process. */

  صحيح i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  إذا(priv->s_flags & CHECK_IRQ) {
  	إذا (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		أرجع EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	لكل (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#إذا PRIV_DEBUG
  		چاپ_کړه("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#د_که_پای
  	}
  }

  /* Copy I/O ranges. */
  إذا(priv->s_flags & CHECK_IO_پورټ) {
  	إذا (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		أرجع EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	لكل (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#إذا PRIV_DEBUG
  		چاپ_کړه("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_بنسټ,
  			priv(rp)->s_io_tab[i].ior_حد,
  			rp->p_endpoint);
#د_که_پای
  	}
  }

  /* Copy memory ranges. */
  إذا(priv->s_flags & CHECK_MEM) {
  	إذا (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		أرجع EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	لكل (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#إذا PRIV_DEBUG
  		چاپ_کړه("do_privctl: adding mem range [%x..%x] for %d\n",
  			priv(rp)->s_mem_tab[i].mr_بنسټ,
  			priv(rp)->s_mem_tab[i].mr_حد,
  			rp->p_endpoint);
#د_که_پای
  	}
  }

  /* Copy trap mask. */
  priv(rp)->s_trap_ماسک = priv->s_trap_ماسک;

  /* Copy target mask. */
#إذا PRIV_DEBUG
  چاپ_کړه("do_privctl: Setting ipc target mask for %d:");
  لكل (i=0; i < NR_SYS_PROCS; i += BITCHUNK_بټونه) {
  	چاپ_کړه(" %08x", get_sys_بټونه(priv->s_ipc_منزل, i));
  }
  چاپ_کړه("\n");
#د_که_پای

  fill_sendto_ماسک(rp, &priv->s_ipc_منزل);

#إذا PRIV_DEBUG
  چاپ_کړه("do_privctl: Set ipc target mask for %d:");
  لكل (i=0; i < NR_SYS_PROCS; i += BITCHUNK_بټونه) {
  	چاپ_کړه(" %08x", get_sys_بټونه(priv(rp)->s_ipc_منزل, i));
  }
  چاپ_کړه("\n");
#د_که_پای

  /* Copy kernel call mask. */
  حافظه_نقل(priv(rp)->s_k_call_ماسک, priv->s_k_call_ماسک,
  	حجم(priv(rp)->s_k_call_ماسک));

  أرجع OK;
}

#د_که_پای /* USE_PRIVCTL */

