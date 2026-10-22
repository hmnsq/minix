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

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <signal.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <minix/endpoint.h>

#सचे USE_PRIVCTL

#define PRIV_DEBUG 0

स्थिर पूर्णाङ्क update_priv(संरचना proc *rp, संरचना priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
पूर्णाङ्क do_privctl(संरचना proc * caller, message * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  संरचना proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  पूर्णाङ्क ipc_to_m, kcalls;
  पूर्णाङ्क i, r;
  संरचना io_range io_range;
  संरचना minix_mem_range mem_range;
  संरचना priv priv;
  पूर्णाङ्क irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  सचे (! (priv(caller)->s_flags & SYS_PROC)) पटिनिवत्त(EPERM);
  सचे(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  अञ्ञथा सचे(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	पटिनिवत्त(EINVAL);
  rp = proc_addr(proc_nr);

  विकल्प(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  पक्ष SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	सचे (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		पटिनिवत्त(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	पटिनिवत्त(OK);

  पक्ष SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	सचे (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		पटिनिवत्त(EPERM);
	}
	RTS_SET(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	पटिनिवत्त(OK);

  पक्ष SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	सचे (RTS_ISSET(rp, RTS_NO_PRIV)) पटिनिवत्त(EPERM);
	RTS_SET(rp, RTS_NO_PRIV);
	पटिनिवत्त(OK);

  पक्ष SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	पटिनिवत्त(OK);

  पक्ष SYS_PRIV_SET_SYS:
	/* Set a privilege structure of a blocked system process. */
	सचे (! RTS_ISSET(rp, RTS_NO_PRIV)) पटिनिवत्त(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	सचे (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		सचे((r=data_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_bytes) &priv, परिमाण(priv))) != OK)
			पटिनिवत्त r;

		/* See if the caller wants to assign a static privilege id. */
		सचे(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	सचे ((i=get_priv(rp, priv_id)) != OK)
	{
		printf("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		पटिनिवत्त(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	अत्थाय (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	अत्थाय (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
	      priv(rp)->s_notify_pending.chunk[i] = 0;	/* - notifications */
	priv(rp)->s_int_pending = 0;			/* - interrupts */
	(शून्य) sigemptyset(&priv(rp)->s_sig_pending);	/* - signals */
	reset_kernel_timer(&priv(rp)->s_alarm_timer);	/* - alarm */
	priv(rp)->s_asyntab= -1;			/* - asynsends */
	priv(rp)->s_asynsize= 0;
	priv(rp)->s_asynendpoint = rp->p_endpoint;
	priv(rp)->s_diag_sig = FALSE;		/* no request for diag sigs */

	/* Set defaults for privilege bitmaps. */
	priv(rp)->s_flags= DSRV_F;           /* privilege flags */
	priv(rp)->s_init_flags= DSRV_I;      /* initialization flags */
	priv(rp)->s_trap_mask= DSRV_T;       /* allowed traps */
	memset(&map, 0, परिमाण(map));
	ipc_to_m = DSRV_M;                   /* allowed targets */
	सचे (ipc_to_m == ALL_M) {
		अत्थाय (i = 0; i < NR_SYS_PROCS; i++)
			set_sys_bit(map, i);
	}
	fill_sendto_mask(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	अत्थाय(i = 0; i < SYS_CALL_MASK_SIZE; i++) {
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
	सचे (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		सचे((r = update_priv(rp, &priv)) != OK) {
			पटिनिवत्त r;
		} 
	}

	पटिनिवत्त(OK);

  पक्ष SYS_PRIV_SET_USER:
	/* Set a privilege structure of a blocked user process. */
	सचे (!RTS_ISSET(rp, RTS_NO_PRIV)) पटिनिवत्त(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	पटिनिवत्त(OK);

  पक्ष SYS_PRIV_ADD_IO:
	सचे (RTS_ISSET(rp, RTS_NO_PRIV))
		पटिनिवत्त(EPERM);

#सचे 0 /* XXX -- do we need a call for this? */
	सचे (strcmp(rp->p_name, "fxp") == 0 ||
		strcmp(rp->p_name, "rtl8139") == 0)
	{
		printf("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_target= rp->p_endpoint;
	}
#endif

	/* Get the I/O range */
	data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &io_range, परिमाण(io_range));
	/* Add the I/O range */
	पटिनिवत्त priv_add_io(rp, &io_range);

  पक्ष SYS_PRIV_ADD_MEM:
	सचे (RTS_ISSET(rp, RTS_NO_PRIV))
		पटिनिवत्त(EPERM);

	/* Get the memory range */
	सचे((r=data_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &mem_range, परिमाण(mem_range))) != OK)
		पटिनिवत्त r;
	/* Add the memory range */
	पटिनिवत्त priv_add_mem(rp, &mem_range);

  पक्ष SYS_PRIV_ADD_IRQ:
	सचे (RTS_ISSET(rp, RTS_NO_PRIV))
		पटिनिवत्त(EPERM);

#सचे 0
	/* Only system processes get IRQs? */
	सचे (!(priv(rp)->s_flags & SYS_PROC))
		पटिनिवत्त EPERM;
#endif
	data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &irq, परिमाण(irq));
	/* Add the IRQ. */
	पटिनिवत्त priv_add_irq(rp, irq);

  पक्ष SYS_PRIV_QUERY_MEM:
  {
	phys_bytes addr, limit;
  	संरचना priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_start;
	limit = addr + (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	सचे(limit < addr)
		पटिनिवत्त EPERM;
	सचे(!(sp = priv(rp)))
		पटिनिवत्त EPERM;
	अत्थाय(i = 0; i < sp->s_nr_mem_range; i++) {
		सचे(addr >= sp->s_mem_tab[i].mr_base &&
		   limit <= sp->s_mem_tab[i].mr_limit)
			पटिनिवत्त OK;
	}
	पटिनिवत्त EPERM;
  }

  पक्ष SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	सचे(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) पटिनिवत्त EINVAL;

	/* Copy privilege structure from caller */
	सचे((r=data_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &priv, परिमाण(priv))) != OK)
		पटिनिवत्त r;

	/* Override settings in existing privilege structure. */
	सचे((r = update_priv(rp, &priv)) != OK) {
		पटिनिवत्त r;
	}

	पटिनिवत्त(OK);

  स्वभाव:
	printf("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	पटिनिवत्त EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
स्थिर पूर्णाङ्क update_priv(संरचना proc *rp, संरचना priv *priv)
{
/* Update the privilege structure of a given process. */

  पूर्णाङ्क i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  सचे(priv->s_flags & CHECK_IRQ) {
  	सचे (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		पटिनिवत्त EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	अत्थाय (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#सचे PRIV_DEBUG
  		printf("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#endif
  	}
  }

  /* Copy I/O ranges. */
  सचे(priv->s_flags & CHECK_IO_PORT) {
  	सचे (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		पटिनिवत्त EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	अत्थाय (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#सचे PRIV_DEBUG
  		printf("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_base,
  			priv(rp)->s_io_tab[i].ior_limit,
  			rp->p_endpoint);
#endif
  	}
  }

  /* Copy memory ranges. */
  सचे(priv->s_flags & CHECK_MEM) {
  	सचे (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		पटिनिवत्त EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	अत्थाय (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#सचे PRIV_DEBUG
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
#सचे PRIV_DEBUG
  printf("do_privctl: Setting ipc target mask for %d:");
  अत्थाय (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	printf(" %08x", get_sys_bits(priv->s_ipc_to, i));
  }
  printf("\n");
#endif

  fill_sendto_mask(rp, &priv->s_ipc_to);

#सचे PRIV_DEBUG
  printf("do_privctl: Set ipc target mask for %d:");
  अत्थाय (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	printf(" %08x", get_sys_bits(priv(rp)->s_ipc_to, i));
  }
  printf("\n");
#endif

  /* Copy kernel call mask. */
  memcpy(priv(rp)->s_k_call_mask, priv->s_k_call_mask,
  	परिमाण(priv(rp)->s_k_call_mask));

  पटिनिवत्त OK;
}

#endif /* USE_PRIVCTL */

