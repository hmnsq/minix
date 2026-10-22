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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਜੇ USE_PRIVCTL

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRIV_DEBUG 0

ਸਥਿਰ ਪੂਰਨ update_priv(ਬਣਤਰ proc *rp, ਬਣਤਰ priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
ਪੂਰਨ do_privctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  ਬਣਤਰ proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  ਪੂਰਨ ipc_ਮੰਜ਼ਿਲ_m, kcalls;
  ਪੂਰਨ i, r;
  ਬਣਤਰ io_range io_range;
  ਬਣਤਰ minix_mem_range mem_range;
  ਬਣਤਰ priv priv;
  ਪੂਰਨ irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  ਜੇ (! (priv(caller)->s_flags & SYS_PROC)) ਵਾਪਸ(EPERM);
  ਜੇ(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  ਨਹੀਂਤਾਂ ਜੇ(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	ਵਾਪਸ(EINVAL);
  rp = proc_addr(proc_nr);

  ਚੋਣ(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  ਹਾਲਤ SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	ਜੇ (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		ਵਾਪਸ(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	ਵਾਪਸ(OK);

  ਹਾਲਤ SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	ਜੇ (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		ਵਾਪਸ(EPERM);
	}
	RTS_ਸੈੱਟ(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	ਵਾਪਸ(OK);

  ਹਾਲਤ SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	ਜੇ (RTS_ISSET(rp, RTS_NO_PRIV)) ਵਾਪਸ(EPERM);
	RTS_ਸੈੱਟ(rp, RTS_NO_PRIV);
	ਵਾਪਸ(OK);

  ਹਾਲਤ SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	ਵਾਪਸ(OK);

  ਹਾਲਤ SYS_PRIV_ਸੈੱਟ_SYS:
	/* Set a privilege structure of a blocked system process. */
	ਜੇ (! RTS_ISSET(rp, RTS_NO_PRIV)) ਵਾਪਸ(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	ਜੇ (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		ਜੇ((r=ਅੰਕੜੇ_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_ਬਾਈਟਾਂ) &priv, ਆਕਾਰ(priv))) != OK)
			ਵਾਪਸ r;

		/* See if the caller wants to assign a static privilege id. */
		ਜੇ(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	ਜੇ ((i=get_priv(rp, priv_id)) != OK)
	{
		ਲਿਖੋ("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		ਵਾਪਸ(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	ਲਈ (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	ਲਈ (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
	      priv(rp)->s_notify_pending.chunk[i] = 0;	/* - notifications */
	priv(rp)->s_int_pending = 0;			/* - interrupts */
	(ਖਾਲੀ) sigemptyset(&priv(rp)->s_sig_pending);	/* - signals */
	reset_kernel_timer(&priv(rp)->s_alarm_timer);	/* - alarm */
	priv(rp)->s_asyntab= -1;			/* - asynsends */
	priv(rp)->s_asynsize= 0;
	priv(rp)->s_asynendpoint = rp->p_endpoint;
	priv(rp)->s_diag_sig = FALSE;		/* no request for diag sigs */

	/* Set defaults for privilege bitmaps. */
	priv(rp)->s_flags= DSRV_F;           /* privilege flags */
	priv(rp)->s_init_flags= DSRV_I;      /* initialization flags */
	priv(rp)->s_trap_ਨਕਾਬ= DSRV_T;       /* allowed traps */
	memset(&map, 0, ਆਕਾਰ(map));
	ipc_ਮੰਜ਼ਿਲ_m = DSRV_M;                   /* allowed targets */
	ਜੇ (ipc_ਮੰਜ਼ਿਲ_m == ALL_M) {
		ਲਈ (i = 0; i < NR_SYS_PROCS; i++)
			ਸੈੱਟ_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(map, i);
	}
	fill_sendto_ਨਕਾਬ(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	ਲਈ(i = 0; i < SYS_CALL_ਨਕਾਬ_ਮਾਪ; i++) {
		priv(rp)->s_k_call_ਨਕਾਬ[i] = (kcalls == NO_C ? 0 : (~0));
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
	priv(rp)->s_grant_ਸਾਰਣੀ= 0;
	priv(rp)->s_grant_entries= 0;
	priv(rp)->s_grant_endpoint = rp->p_endpoint;
	priv(rp)->s_state_ਸਾਰਣੀ= 0;
	priv(rp)->s_state_entries= 0;
	priv(rp)->s_ipcf= 0;

	/* Override defaults if the caller has supplied a privilege structure. */
	ਜੇ (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		ਜੇ((r = update_priv(rp, &priv)) != OK) {
			ਵਾਪਸ r;
		} 
	}

	ਵਾਪਸ(OK);

  ਹਾਲਤ SYS_PRIV_ਸੈੱਟ_USER:
	/* Set a privilege structure of a blocked user process. */
	ਜੇ (!RTS_ISSET(rp, RTS_NO_PRIV)) ਵਾਪਸ(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	ਵਾਪਸ(OK);

  ਹਾਲਤ SYS_PRIV_ਜੋੜੋ_IO:
	ਜੇ (RTS_ISSET(rp, RTS_NO_PRIV))
		ਵਾਪਸ(EPERM);

#ਜੇ 0 /* XXX -- do we need a call for this? */
	ਜੇ (strcmp(rp->p_name, "fxp") == 0 ||
		strcmp(rp->p_name, "rtl8139") == 0)
	{
		ਲਿਖੋ("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_ਨਿਸ਼ਾਨਾ= rp->p_endpoint;
	}
#ਜੇ_ਅੰਤ

	/* Get the I/O range */
	ਅੰਕੜੇ_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_ਬਾਈਟਾਂ) &io_range, ਆਕਾਰ(io_range));
	/* Add the I/O range */
	ਵਾਪਸ priv_ਜੋੜੋ_io(rp, &io_range);

  ਹਾਲਤ SYS_PRIV_ਜੋੜੋ_MEM:
	ਜੇ (RTS_ISSET(rp, RTS_NO_PRIV))
		ਵਾਪਸ(EPERM);

	/* Get the memory range */
	ਜੇ((r=ਅੰਕੜੇ_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_ਬਾਈਟਾਂ) &mem_range, ਆਕਾਰ(mem_range))) != OK)
		ਵਾਪਸ r;
	/* Add the memory range */
	ਵਾਪਸ priv_ਜੋੜੋ_mem(rp, &mem_range);

  ਹਾਲਤ SYS_PRIV_ਜੋੜੋ_IRQ:
	ਜੇ (RTS_ISSET(rp, RTS_NO_PRIV))
		ਵਾਪਸ(EPERM);

#ਜੇ 0
	/* Only system processes get IRQs? */
	ਜੇ (!(priv(rp)->s_flags & SYS_PROC))
		ਵਾਪਸ EPERM;
#ਜੇ_ਅੰਤ
	ਅੰਕੜੇ_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_ਬਾਈਟਾਂ) &irq, ਆਕਾਰ(irq));
	/* Add the IRQ. */
	ਵਾਪਸ priv_ਜੋੜੋ_irq(rp, irq);

  ਹਾਲਤ SYS_PRIV_QUERY_MEM:
  {
	phys_ਬਾਈਟਾਂ addr, ਹੱਦਬੰਦੀ_2;
  	ਬਣਤਰ priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_ਬਾਈਟਾਂ) m_ptr->m_lsys_krn_sys_privctl.phys_ਸ਼ੁਰੂ;
	ਹੱਦਬੰਦੀ_2 = addr + (phys_ਬਾਈਟਾਂ) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	ਜੇ(ਹੱਦਬੰਦੀ_2 < addr)
		ਵਾਪਸ EPERM;
	ਜੇ(!(sp = priv(rp)))
		ਵਾਪਸ EPERM;
	ਲਈ(i = 0; i < sp->s_nr_mem_range; i++) {
		ਜੇ(addr >= sp->s_mem_tab[i].mr_ਅਧਾਰ &&
		   ਹੱਦਬੰਦੀ_2 <= sp->s_mem_tab[i].mr_ਹੱਦਬੰਦੀ)
			ਵਾਪਸ OK;
	}
	ਵਾਪਸ EPERM;
  }

  ਹਾਲਤ SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	ਜੇ(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) ਵਾਪਸ EINVAL;

	/* Copy privilege structure from caller */
	ਜੇ((r=ਅੰਕੜੇ_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_ਬਾਈਟਾਂ) &priv, ਆਕਾਰ(priv))) != OK)
		ਵਾਪਸ r;

	/* Override settings in existing privilege structure. */
	ਜੇ((r = update_priv(rp, &priv)) != OK) {
		ਵਾਪਸ r;
	}

	ਵਾਪਸ(OK);

  ਮੂਲ:
	ਲਿਖੋ("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	ਵਾਪਸ EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ update_priv(ਬਣਤਰ proc *rp, ਬਣਤਰ priv *priv)
{
/* Update the privilege structure of a given process. */

  ਪੂਰਨ i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  ਜੇ(priv->s_flags & CHECK_IRQ) {
  	ਜੇ (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		ਵਾਪਸ EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	ਲਈ (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#ਜੇ PRIV_DEBUG
  		ਲਿਖੋ("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#ਜੇ_ਅੰਤ
  	}
  }

  /* Copy I/O ranges. */
  ਜੇ(priv->s_flags & CHECK_IO_ਪੋਰਟ) {
  	ਜੇ (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		ਵਾਪਸ EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	ਲਈ (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#ਜੇ PRIV_DEBUG
  		ਲਿਖੋ("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_ਅਧਾਰ,
  			priv(rp)->s_io_tab[i].ior_ਹੱਦਬੰਦੀ,
  			rp->p_endpoint);
#ਜੇ_ਅੰਤ
  	}
  }

  /* Copy memory ranges. */
  ਜੇ(priv->s_flags & CHECK_MEM) {
  	ਜੇ (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		ਵਾਪਸ EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	ਲਈ (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#ਜੇ PRIV_DEBUG
  		ਲਿਖੋ("do_privctl: adding mem range [%x..%x] for %d\n",
  			priv(rp)->s_mem_tab[i].mr_ਅਧਾਰ,
  			priv(rp)->s_mem_tab[i].mr_ਹੱਦਬੰਦੀ,
  			rp->p_endpoint);
#ਜੇ_ਅੰਤ
  	}
  }

  /* Copy trap mask. */
  priv(rp)->s_trap_ਨਕਾਬ = priv->s_trap_ਨਕਾਬ;

  /* Copy target mask. */
#ਜੇ PRIV_DEBUG
  ਲਿਖੋ("do_privctl: Setting ipc target mask for %d:");
  ਲਈ (i=0; i < NR_SYS_PROCS; i += BITCHUNK_ਬਿੱਟ) {
  	ਲਿਖੋ(" %08x", get_sys_ਬਿੱਟ(priv->s_ipc_ਮੰਜ਼ਿਲ, i));
  }
  ਲਿਖੋ("\n");
#ਜੇ_ਅੰਤ

  fill_sendto_ਨਕਾਬ(rp, &priv->s_ipc_ਮੰਜ਼ਿਲ);

#ਜੇ PRIV_DEBUG
  ਲਿਖੋ("do_privctl: Set ipc target mask for %d:");
  ਲਈ (i=0; i < NR_SYS_PROCS; i += BITCHUNK_ਬਿੱਟ) {
  	ਲਿਖੋ(" %08x", get_sys_ਬਿੱਟ(priv(rp)->s_ipc_ਮੰਜ਼ਿਲ, i));
  }
  ਲਿਖੋ("\n");
#ਜੇ_ਅੰਤ

  /* Copy kernel call mask. */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(priv(rp)->s_k_call_ਨਕਾਬ, priv->s_k_call_ਨਕਾਬ,
  	ਆਕਾਰ(priv(rp)->s_k_call_ਨਕਾਬ));

  ਵਾਪਸ OK;
}

#ਜੇ_ਅੰਤ /* USE_PRIVCTL */

