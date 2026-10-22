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

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <signal.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_PRIVCTL

#నిర్వచించు PRIV_DEBUG 0

స్థిర పూర్ణాంకం update_priv(నిర్మాణం proc *rp, నిర్మాణం priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
పూర్ణాంకం do_privctl(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  నిర్మాణం proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  పూర్ణాంకం ipc_గమ్యం_m, kcalls;
  పూర్ణాంకం i, r;
  నిర్మాణం io_range io_range;
  నిర్మాణం minix_mem_range mem_range;
  నిర్మాణం priv priv;
  పూర్ణాంకం irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  ఒకవేళ (! (priv(caller)->s_flags & SYS_PROC)) తిరిగి(EPERM);
  ఒకవేళ(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  లేకపోతే ఒకవేళ(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	తిరిగి(EINVAL);
  rp = proc_addr(proc_nr);

  ఎంపిక(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  స్థితి SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	ఒకవేళ (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		తిరిగి(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	తిరిగి(OK);

  స్థితి SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	ఒకవేళ (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		తిరిగి(EPERM);
	}
	RTS_సమితి(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	తిరిగి(OK);

  స్థితి SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	ఒకవేళ (RTS_ISSET(rp, RTS_NO_PRIV)) తిరిగి(EPERM);
	RTS_సమితి(rp, RTS_NO_PRIV);
	తిరిగి(OK);

  స్థితి SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	తిరిగి(OK);

  స్థితి SYS_PRIV_సమితి_SYS:
	/* Set a privilege structure of a blocked system process. */
	ఒకవేళ (! RTS_ISSET(rp, RTS_NO_PRIV)) తిరిగి(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	ఒకవేళ (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		ఒకవేళ((r=దత్తాంశం_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_బైట్లు) &priv, పరిమాణం(priv))) != OK)
			తిరిగి r;

		/* See if the caller wants to assign a static privilege id. */
		ఒకవేళ(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	ఒకవేళ ((i=get_priv(rp, priv_id)) != OK)
	{
		రాయి("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		తిరిగి(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	కొరకు (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	కొరకు (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
	      priv(rp)->s_notify_pending.chunk[i] = 0;	/* - notifications */
	priv(rp)->s_int_pending = 0;			/* - interrupts */
	(శూన్య) sigemptyset(&priv(rp)->s_sig_pending);	/* - signals */
	reset_kernel_timer(&priv(rp)->s_alarm_timer);	/* - alarm */
	priv(rp)->s_asyntab= -1;			/* - asynsends */
	priv(rp)->s_asynsize= 0;
	priv(rp)->s_asynendpoint = rp->p_endpoint;
	priv(rp)->s_diag_sig = FALSE;		/* no request for diag sigs */

	/* Set defaults for privilege bitmaps. */
	priv(rp)->s_flags= DSRV_F;           /* privilege flags */
	priv(rp)->s_init_flags= DSRV_I;      /* initialization flags */
	priv(rp)->s_trap_ముసుగు= DSRV_T;       /* allowed traps */
	memset(&map, 0, పరిమాణం(map));
	ipc_గమ్యం_m = DSRV_M;                   /* allowed targets */
	ఒకవేళ (ipc_గమ్యం_m == ALL_M) {
		కొరకు (i = 0; i < NR_SYS_PROCS; i++)
			సమితి_sys_బిట్టు(map, i);
	}
	fill_sendto_ముసుగు(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	కొరకు(i = 0; i < SYS_CALL_ముసుగు_కొలత; i++) {
		priv(rp)->s_k_call_ముసుగు[i] = (kcalls == NO_C ? 0 : (~0));
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
	priv(rp)->s_grant_పట్టిక= 0;
	priv(rp)->s_grant_entries= 0;
	priv(rp)->s_grant_endpoint = rp->p_endpoint;
	priv(rp)->s_state_పట్టిక= 0;
	priv(rp)->s_state_entries= 0;
	priv(rp)->s_ipcf= 0;

	/* Override defaults if the caller has supplied a privilege structure. */
	ఒకవేళ (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		ఒకవేళ((r = update_priv(rp, &priv)) != OK) {
			తిరిగి r;
		} 
	}

	తిరిగి(OK);

  స్థితి SYS_PRIV_సమితి_USER:
	/* Set a privilege structure of a blocked user process. */
	ఒకవేళ (!RTS_ISSET(rp, RTS_NO_PRIV)) తిరిగి(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	తిరిగి(OK);

  స్థితి SYS_PRIV_కూడు_IO:
	ఒకవేళ (RTS_ISSET(rp, RTS_NO_PRIV))
		తిరిగి(EPERM);

#ఒకవేళ 0 /* XXX -- do we need a call for this? */
	ఒకవేళ (strcmp(rp->p_name, "fxp") == 0 ||
		strcmp(rp->p_name, "rtl8139") == 0)
	{
		రాయి("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_లక్ష్యం= rp->p_endpoint;
	}
#అయితే_ముగింపు

	/* Get the I/O range */
	దత్తాంశం_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_బైట్లు) &io_range, పరిమాణం(io_range));
	/* Add the I/O range */
	తిరిగి priv_కూడు_io(rp, &io_range);

  స్థితి SYS_PRIV_కూడు_MEM:
	ఒకవేళ (RTS_ISSET(rp, RTS_NO_PRIV))
		తిరిగి(EPERM);

	/* Get the memory range */
	ఒకవేళ((r=దత్తాంశం_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_బైట్లు) &mem_range, పరిమాణం(mem_range))) != OK)
		తిరిగి r;
	/* Add the memory range */
	తిరిగి priv_కూడు_mem(rp, &mem_range);

  స్థితి SYS_PRIV_కూడు_IRQ:
	ఒకవేళ (RTS_ISSET(rp, RTS_NO_PRIV))
		తిరిగి(EPERM);

#ఒకవేళ 0
	/* Only system processes get IRQs? */
	ఒకవేళ (!(priv(rp)->s_flags & SYS_PROC))
		తిరిగి EPERM;
#అయితే_ముగింపు
	దత్తాంశం_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_బైట్లు) &irq, పరిమాణం(irq));
	/* Add the IRQ. */
	తిరిగి priv_కూడు_irq(rp, irq);

  స్థితి SYS_PRIV_QUERY_MEM:
  {
	phys_బైట్లు addr, హద్దు_2;
  	నిర్మాణం priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_బైట్లు) m_ptr->m_lsys_krn_sys_privctl.phys_ప్రారంభం;
	హద్దు_2 = addr + (phys_బైట్లు) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	ఒకవేళ(హద్దు_2 < addr)
		తిరిగి EPERM;
	ఒకవేళ(!(sp = priv(rp)))
		తిరిగి EPERM;
	కొరకు(i = 0; i < sp->s_nr_mem_range; i++) {
		ఒకవేళ(addr >= sp->s_mem_tab[i].mr_పునాది &&
		   హద్దు_2 <= sp->s_mem_tab[i].mr_హద్దు)
			తిరిగి OK;
	}
	తిరిగి EPERM;
  }

  స్థితి SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	ఒకవేళ(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) తిరిగి EINVAL;

	/* Copy privilege structure from caller */
	ఒకవేళ((r=దత్తాంశం_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_బైట్లు) &priv, పరిమాణం(priv))) != OK)
		తిరిగి r;

	/* Override settings in existing privilege structure. */
	ఒకవేళ((r = update_priv(rp, &priv)) != OK) {
		తిరిగి r;
	}

	తిరిగి(OK);

  అప్రమేయ:
	రాయి("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	తిరిగి EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
స్థిర పూర్ణాంకం update_priv(నిర్మాణం proc *rp, నిర్మాణం priv *priv)
{
/* Update the privilege structure of a given process. */

  పూర్ణాంకం i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  ఒకవేళ(priv->s_flags & CHECK_IRQ) {
  	ఒకవేళ (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		తిరిగి EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	కొరకు (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#ఒకవేళ PRIV_DEBUG
  		రాయి("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#అయితే_ముగింపు
  	}
  }

  /* Copy I/O ranges. */
  ఒకవేళ(priv->s_flags & CHECK_IO_పోర్టు) {
  	ఒకవేళ (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		తిరిగి EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	కొరకు (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#ఒకవేళ PRIV_DEBUG
  		రాయి("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_పునాది,
  			priv(rp)->s_io_tab[i].ior_హద్దు,
  			rp->p_endpoint);
#అయితే_ముగింపు
  	}
  }

  /* Copy memory ranges. */
  ఒకవేళ(priv->s_flags & CHECK_MEM) {
  	ఒకవేళ (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		తిరిగి EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	కొరకు (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#ఒకవేళ PRIV_DEBUG
  		రాయి("do_privctl: adding mem range [%x..%x] for %d\n",
  			priv(rp)->s_mem_tab[i].mr_పునాది,
  			priv(rp)->s_mem_tab[i].mr_హద్దు,
  			rp->p_endpoint);
#అయితే_ముగింపు
  	}
  }

  /* Copy trap mask. */
  priv(rp)->s_trap_ముసుగు = priv->s_trap_ముసుగు;

  /* Copy target mask. */
#ఒకవేళ PRIV_DEBUG
  రాయి("do_privctl: Setting ipc target mask for %d:");
  కొరకు (i=0; i < NR_SYS_PROCS; i += BITCHUNK_బిట్లు) {
  	రాయి(" %08x", get_sys_బిట్లు(priv->s_ipc_గమ్యం, i));
  }
  రాయి("\n");
#అయితే_ముగింపు

  fill_sendto_ముసుగు(rp, &priv->s_ipc_గమ్యం);

#ఒకవేళ PRIV_DEBUG
  రాయి("do_privctl: Set ipc target mask for %d:");
  కొరకు (i=0; i < NR_SYS_PROCS; i += BITCHUNK_బిట్లు) {
  	రాయి(" %08x", get_sys_బిట్లు(priv(rp)->s_ipc_గమ్యం, i));
  }
  రాయి("\n");
#అయితే_ముగింపు

  /* Copy kernel call mask. */
  జ్ఞాపకం_నకలు(priv(rp)->s_k_call_ముసుగు, priv->s_k_call_ముసుగు,
  	పరిమాణం(priv(rp)->s_k_call_ముసుగు));

  తిరిగి OK;
}

#అయితే_ముగింపు /* USE_PRIVCTL */

