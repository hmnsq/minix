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

#यदि USE_PRIVCTL

#परिभाषय PRIV_DEBUG 0

स्थिर पूर्णाङ्क update_priv(संरचना proc *rp, संरचना priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
पूर्णाङ्क do_privctl(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  संरचना proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  पूर्णाङ्क ipc_गन्तव्यम्_m, kcalls;
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
  यदि (! (priv(caller)->s_flags & SYS_PROC)) प्रत्यागम(EPERM);
  यदि(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  अन्यथा यदि(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	प्रत्यागम(EINVAL);
  rp = proc_addr(proc_nr);

  विकल्प(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  पक्ष SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	यदि (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		प्रत्यागम(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	प्रत्यागम(OK);

  पक्ष SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	यदि (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		प्रत्यागम(EPERM);
	}
	RTS_समुच्चयः(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	प्रत्यागम(OK);

  पक्ष SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	यदि (RTS_ISSET(rp, RTS_NO_PRIV)) प्रत्यागम(EPERM);
	RTS_समुच्चयः(rp, RTS_NO_PRIV);
	प्रत्यागम(OK);

  पक्ष SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	प्रत्यागम(OK);

  पक्ष SYS_PRIV_समुच्चयः_SYS:
	/* Set a privilege structure of a blocked system process. */
	यदि (! RTS_ISSET(rp, RTS_NO_PRIV)) प्रत्यागम(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	यदि (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		यदि((r=दत्त_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_अष्टकानि) &priv, परिमाण(priv))) != OK)
			प्रत्यागम r;

		/* See if the caller wants to assign a static privilege id. */
		यदि(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	यदि ((i=get_priv(rp, priv_id)) != OK)
	{
		प्रारूप_लिख("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		प्रत्यागम(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	प्रति (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	प्रति (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
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
	priv(rp)->s_trap_आवरणम्= DSRV_T;       /* allowed traps */
	स्मृतिं_पूरय(&map, 0, परिमाण(map));
	ipc_गन्तव्यम्_m = DSRV_M;                   /* allowed targets */
	यदि (ipc_गन्तव्यम्_m == ALL_M) {
		प्रति (i = 0; i < NR_SYS_PROCS; i++)
			समुच्चयः_sys_bit(map, i);
	}
	fill_sendto_आवरणम्(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	प्रति(i = 0; i < SYS_CALL_आवरणम्_आकार; i++) {
		priv(rp)->s_k_call_आवरणम्[i] = (kcalls == NO_C ? 0 : (~0));
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
	priv(rp)->s_grant_सारणी= 0;
	priv(rp)->s_grant_entries= 0;
	priv(rp)->s_grant_endpoint = rp->p_endpoint;
	priv(rp)->s_state_सारणी= 0;
	priv(rp)->s_state_entries= 0;
	priv(rp)->s_ipcf= 0;

	/* Override defaults if the caller has supplied a privilege structure. */
	यदि (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		यदि((r = update_priv(rp, &priv)) != OK) {
			प्रत्यागम r;
		} 
	}

	प्रत्यागम(OK);

  पक्ष SYS_PRIV_समुच्चयः_USER:
	/* Set a privilege structure of a blocked user process. */
	यदि (!RTS_ISSET(rp, RTS_NO_PRIV)) प्रत्यागम(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	प्रत्यागम(OK);

  पक्ष SYS_PRIV_सङ्कलय_IO:
	यदि (RTS_ISSET(rp, RTS_NO_PRIV))
		प्रत्यागम(EPERM);

#यदि 0 /* XXX -- do we need a call for this? */
	यदि (वाक्यं_तुलय(rp->p_name, "fxp") == 0 ||
		वाक्यं_तुलय(rp->p_name, "rtl8139") == 0)
	{
		प्रारूप_लिख("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_target= rp->p_endpoint;
	}
#यदि_अन्तः

	/* Get the I/O range */
	दत्त_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_अष्टकानि) &io_range, परिमाण(io_range));
	/* Add the I/O range */
	प्रत्यागम priv_सङ्कलय_io(rp, &io_range);

  पक्ष SYS_PRIV_सङ्कलय_MEM:
	यदि (RTS_ISSET(rp, RTS_NO_PRIV))
		प्रत्यागम(EPERM);

	/* Get the memory range */
	यदि((r=दत्त_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_अष्टकानि) &mem_range, परिमाण(mem_range))) != OK)
		प्रत्यागम r;
	/* Add the memory range */
	प्रत्यागम priv_सङ्कलय_mem(rp, &mem_range);

  पक्ष SYS_PRIV_सङ्कलय_IRQ:
	यदि (RTS_ISSET(rp, RTS_NO_PRIV))
		प्रत्यागम(EPERM);

#यदि 0
	/* Only system processes get IRQs? */
	यदि (!(priv(rp)->s_flags & SYS_PROC))
		प्रत्यागम EPERM;
#यदि_अन्तः
	दत्त_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_अष्टकानि) &irq, परिमाण(irq));
	/* Add the IRQ. */
	प्रत्यागम priv_सङ्कलय_irq(rp, irq);

  पक्ष SYS_PRIV_QUERY_MEM:
  {
	phys_अष्टकानि addr, सीमा_2;
  	संरचना priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_अष्टकानि) m_ptr->m_lsys_krn_sys_privctl.phys_start;
	सीमा_2 = addr + (phys_अष्टकानि) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	यदि(सीमा_2 < addr)
		प्रत्यागम EPERM;
	यदि(!(sp = priv(rp)))
		प्रत्यागम EPERM;
	प्रति(i = 0; i < sp->s_nr_mem_range; i++) {
		यदि(addr >= sp->s_mem_tab[i].mr_आधारः &&
		   सीमा_2 <= sp->s_mem_tab[i].mr_सीमा)
			प्रत्यागम OK;
	}
	प्रत्यागम EPERM;
  }

  पक्ष SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	यदि(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) प्रत्यागम EINVAL;

	/* Copy privilege structure from caller */
	यदि((r=दत्त_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_अष्टकानि) &priv, परिमाण(priv))) != OK)
		प्रत्यागम r;

	/* Override settings in existing privilege structure. */
	यदि((r = update_priv(rp, &priv)) != OK) {
		प्रत्यागम r;
	}

	प्रत्यागम(OK);

  स्वभाव:
	प्रारूप_लिख("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	प्रत्यागम EINVAL;
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
  यदि(priv->s_flags & CHECK_IRQ) {
  	यदि (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		प्रत्यागम EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	प्रति (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#यदि PRIV_DEBUG
  		प्रारूप_लिख("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#यदि_अन्तः
  	}
  }

  /* Copy I/O ranges. */
  यदि(priv->s_flags & CHECK_IO_पत्तनम्) {
  	यदि (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		प्रत्यागम EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	प्रति (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#यदि PRIV_DEBUG
  		प्रारूप_लिख("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_आधारः,
  			priv(rp)->s_io_tab[i].ior_सीमा,
  			rp->p_endpoint);
#यदि_अन्तः
  	}
  }

  /* Copy memory ranges. */
  यदि(priv->s_flags & CHECK_MEM) {
  	यदि (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		प्रत्यागम EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	प्रति (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#यदि PRIV_DEBUG
  		प्रारूप_लिख("do_privctl: adding mem range [%x..%x] for %d\n",
  			priv(rp)->s_mem_tab[i].mr_आधारः,
  			priv(rp)->s_mem_tab[i].mr_सीमा,
  			rp->p_endpoint);
#यदि_अन्तः
  	}
  }

  /* Copy trap mask. */
  priv(rp)->s_trap_आवरणम् = priv->s_trap_आवरणम्;

  /* Copy target mask. */
#यदि PRIV_DEBUG
  प्रारूप_लिख("do_privctl: Setting ipc target mask for %d:");
  प्रति (i=0; i < NR_SYS_PROCS; i += BITCHUNK_द्वयङ्काः) {
  	प्रारूप_लिख(" %08x", get_sys_द्वयङ्काः(priv->s_ipc_गन्तव्यम्, i));
  }
  प्रारूप_लिख("\n");
#यदि_अन्तः

  fill_sendto_आवरणम्(rp, &priv->s_ipc_गन्तव्यम्);

#यदि PRIV_DEBUG
  प्रारूप_लिख("do_privctl: Set ipc target mask for %d:");
  प्रति (i=0; i < NR_SYS_PROCS; i += BITCHUNK_द्वयङ्काः) {
  	प्रारूप_लिख(" %08x", get_sys_द्वयङ्काः(priv(rp)->s_ipc_गन्तव्यम्, i));
  }
  प्रारूप_लिख("\n");
#यदि_अन्तः

  /* Copy kernel call mask. */
  स्मृतिं_प्रतिलिख(priv(rp)->s_k_call_आवरणम्, priv->s_k_call_आवरणम्,
  	परिमाण(priv(rp)->s_k_call_आवरणम्));

  प्रत्यागम OK;
}

#यदि_अन्तः /* USE_PRIVCTL */

