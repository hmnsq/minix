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

#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <minix/endpoint.h>

#যদি USE_PRIVCTL

#define PRIV_DEBUG 0

স্থির পূর্ণাঙ্ক update_priv(সংরচনা proc *rp, সংরচনা priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_privctl(সংরচনা proc * caller, message * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  সংরচনা proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  পূর্ণাঙ্ক ipc_to_m, kcalls;
  পূর্ণাঙ্ক i, r;
  সংরচনা io_range io_range;
  সংরচনা minix_mem_range mem_range;
  সংরচনা priv priv;
  পূর্ণাঙ্ক irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  যদি (! (priv(caller)->s_flags & SYS_PROC)) ঘূৰাই(EPERM);
  যদি(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  অন্যথা যদি(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	ঘূৰাই(EINVAL);
  rp = proc_addr(proc_nr);

  ৱিকল্প(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  পক্ষ SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	যদি (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		ঘূৰাই(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	ঘূৰাই(OK);

  পক্ষ SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	যদি (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		ঘূৰাই(EPERM);
	}
	RTS_SET(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	ঘূৰাই(OK);

  পক্ষ SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	যদি (RTS_ISSET(rp, RTS_NO_PRIV)) ঘূৰাই(EPERM);
	RTS_SET(rp, RTS_NO_PRIV);
	ঘূৰাই(OK);

  পক্ষ SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	ঘূৰাই(OK);

  পক্ষ SYS_PRIV_SET_SYS:
	/* Set a privilege structure of a blocked system process. */
	যদি (! RTS_ISSET(rp, RTS_NO_PRIV)) ঘূৰাই(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	যদি (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		যদি((r=data_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_bytes) &priv, পরিমাণ(priv))) != OK)
			ঘূৰাই r;

		/* See if the caller wants to assign a static privilege id. */
		যদি(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	যদি ((i=get_priv(rp, priv_id)) != OK)
	{
		printf("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		ঘূৰাই(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	বাবে (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	বাবে (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
	      priv(rp)->s_notify_pending.chunk[i] = 0;	/* - notifications */
	priv(rp)->s_int_pending = 0;			/* - interrupts */
	(শূন্য) sigemptyset(&priv(rp)->s_sig_pending);	/* - signals */
	reset_kernel_timer(&priv(rp)->s_alarm_timer);	/* - alarm */
	priv(rp)->s_asyntab= -1;			/* - asynsends */
	priv(rp)->s_asynsize= 0;
	priv(rp)->s_asynendpoint = rp->p_endpoint;
	priv(rp)->s_diag_sig = FALSE;		/* no request for diag sigs */

	/* Set defaults for privilege bitmaps. */
	priv(rp)->s_flags= DSRV_F;           /* privilege flags */
	priv(rp)->s_init_flags= DSRV_I;      /* initialization flags */
	priv(rp)->s_trap_mask= DSRV_T;       /* allowed traps */
	memset(&map, 0, পরিমাণ(map));
	ipc_to_m = DSRV_M;                   /* allowed targets */
	যদি (ipc_to_m == ALL_M) {
		বাবে (i = 0; i < NR_SYS_PROCS; i++)
			set_sys_bit(map, i);
	}
	fill_sendto_mask(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	বাবে(i = 0; i < SYS_CALL_MASK_SIZE; i++) {
		priv(rp)->s_k_call_mask[i] = (kcalls == NO_C ? 0 : (~0));
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
	priv(rp)->s_grant_table= 0;
	priv(rp)->s_grant_entries= 0;
	priv(rp)->s_grant_endpoint = rp->p_endpoint;
	priv(rp)->s_state_table= 0;
	priv(rp)->s_state_entries= 0;
	priv(rp)->s_ipcf= 0;

	/* Override defaults if the caller has supplied a privilege structure. */
	যদি (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		যদি((r = update_priv(rp, &priv)) != OK) {
			ঘূৰাই r;
		} 
	}

	ঘূৰাই(OK);

  পক্ষ SYS_PRIV_SET_USER:
	/* Set a privilege structure of a blocked user process. */
	যদি (!RTS_ISSET(rp, RTS_NO_PRIV)) ঘূৰাই(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	ঘূৰাই(OK);

  পক্ষ SYS_PRIV_ADD_IO:
	যদি (RTS_ISSET(rp, RTS_NO_PRIV))
		ঘূৰাই(EPERM);

#যদি 0 /* XXX -- do we need a call for this? */
	যদি (strcmp(rp->p_name, "fxp") == 0 ||
		strcmp(rp->p_name, "rtl8139") == 0)
	{
		printf("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_target= rp->p_endpoint;
	}
#endif

	/* Get the I/O range */
	data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &io_range, পরিমাণ(io_range));
	/* Add the I/O range */
	ঘূৰাই priv_add_io(rp, &io_range);

  পক্ষ SYS_PRIV_ADD_MEM:
	যদি (RTS_ISSET(rp, RTS_NO_PRIV))
		ঘূৰাই(EPERM);

	/* Get the memory range */
	যদি((r=data_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &mem_range, পরিমাণ(mem_range))) != OK)
		ঘূৰাই r;
	/* Add the memory range */
	ঘূৰাই priv_add_mem(rp, &mem_range);

  পক্ষ SYS_PRIV_ADD_IRQ:
	যদি (RTS_ISSET(rp, RTS_NO_PRIV))
		ঘূৰাই(EPERM);

#যদি 0
	/* Only system processes get IRQs? */
	যদি (!(priv(rp)->s_flags & SYS_PROC))
		ঘূৰাই EPERM;
#endif
	data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &irq, পরিমাণ(irq));
	/* Add the IRQ. */
	ঘূৰাই priv_add_irq(rp, irq);

  পক্ষ SYS_PRIV_QUERY_MEM:
  {
	phys_bytes addr, limit;
  	সংরচনা priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_start;
	limit = addr + (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	যদি(limit < addr)
		ঘূৰাই EPERM;
	যদি(!(sp = priv(rp)))
		ঘূৰাই EPERM;
	বাবে(i = 0; i < sp->s_nr_mem_range; i++) {
		যদি(addr >= sp->s_mem_tab[i].mr_base &&
		   limit <= sp->s_mem_tab[i].mr_limit)
			ঘূৰাই OK;
	}
	ঘূৰাই EPERM;
  }

  পক্ষ SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	যদি(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) ঘূৰাই EINVAL;

	/* Copy privilege structure from caller */
	যদি((r=data_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &priv, পরিমাণ(priv))) != OK)
		ঘূৰাই r;

	/* Override settings in existing privilege structure. */
	যদি((r = update_priv(rp, &priv)) != OK) {
		ঘূৰাই r;
	}

	ঘূৰাই(OK);

  স্ৱভাৱ:
	printf("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	ঘূৰাই EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
স্থির পূর্ণাঙ্ক update_priv(সংরচনা proc *rp, সংরচনা priv *priv)
{
/* Update the privilege structure of a given process. */

  পূর্ণাঙ্ক i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  যদি(priv->s_flags & CHECK_IRQ) {
  	যদি (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		ঘূৰাই EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	বাবে (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#যদি PRIV_DEBUG
  		printf("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#endif
  	}
  }

  /* Copy I/O ranges. */
  যদি(priv->s_flags & CHECK_IO_PORT) {
  	যদি (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		ঘূৰাই EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	বাবে (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#যদি PRIV_DEBUG
  		printf("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_base,
  			priv(rp)->s_io_tab[i].ior_limit,
  			rp->p_endpoint);
#endif
  	}
  }

  /* Copy memory ranges. */
  যদি(priv->s_flags & CHECK_MEM) {
  	যদি (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		ঘূৰাই EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	বাবে (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#যদি PRIV_DEBUG
  		printf("do_privctl: adding mem range [%x..%x] for %d\n",
  			priv(rp)->s_mem_tab[i].mr_base,
  			priv(rp)->s_mem_tab[i].mr_limit,
  			rp->p_endpoint);
#endif
  	}
  }

  /* Copy trap mask. */
  priv(rp)->s_trap_mask = priv->s_trap_mask;

  /* Copy target mask. */
#যদি PRIV_DEBUG
  printf("do_privctl: Setting ipc target mask for %d:");
  বাবে (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	printf(" %08x", get_sys_bits(priv->s_ipc_to, i));
  }
  printf("\n");
#endif

  fill_sendto_mask(rp, &priv->s_ipc_to);

#যদি PRIV_DEBUG
  printf("do_privctl: Set ipc target mask for %d:");
  বাবে (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	printf(" %08x", get_sys_bits(priv(rp)->s_ipc_to, i));
  }
  printf("\n");
#endif

  /* Copy kernel call mask. */
  memcpy(priv(rp)->s_k_call_mask, priv->s_k_call_mask,
  	পরিমাণ(priv(rp)->s_k_call_mask));

  ঘূৰাই OK;
}

#endif /* USE_PRIVCTL */

