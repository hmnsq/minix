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

#समावेश "kernel/system.h"
#समावेश <signal.h>
#समावेश <string.h>
#समावेश <minix/endpoint.h>

#अगर USE_PRIVCTL

#घोषणा PRIV_DEBUG 0

अटल पूर्णांक update_priv(काष्ठा proc *rp, काष्ठा priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
पूर्णांक do_privctl(काष्ठा proc * caller, message * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  काष्ठा proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  पूर्णांक ipc_to_m, kcalls;
  पूर्णांक i, r;
  काष्ठा io_range io_range;
  काष्ठा minix_mem_range mem_range;
  काष्ठा priv priv;
  पूर्णांक irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  अगर (! (priv(caller)->s_flags & SYS_PROC)) वापस(EPERM);
  अगर(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  अन्यथा अगर(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	वापस(EINVAL);
  rp = proc_addr(proc_nr);

  चयन(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  हाल SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	अगर (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		वापस(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	वापस(OK);

  हाल SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	अगर (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		वापस(EPERM);
	}
	RTS_SET(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	वापस(OK);

  हाल SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	अगर (RTS_ISSET(rp, RTS_NO_PRIV)) वापस(EPERM);
	RTS_SET(rp, RTS_NO_PRIV);
	वापस(OK);

  हाल SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	वापस(OK);

  हाल SYS_PRIV_SET_SYS:
	/* Set a privilege structure of a blocked system process. */
	अगर (! RTS_ISSET(rp, RTS_NO_PRIV)) वापस(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	अगर (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		अगर((r=तथ्य_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_bytes) &priv, माप(priv))) != OK)
			वापस r;

		/* See if the caller wants to assign a static privilege id. */
		अगर(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	अगर ((i=get_priv(rp, priv_id)) != OK)
	{
		म_लिखो("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		वापस(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	क्रम (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	क्रम (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
	      priv(rp)->s_notify_pending.chunk[i] = 0;	/* - notifications */
	priv(rp)->s_int_pending = 0;			/* - interrupts */
	(व्योम) sigemptyset(&priv(rp)->s_sig_pending);	/* - signals */
	reset_kernel_timer(&priv(rp)->s_alarm_timer);	/* - alarm */
	priv(rp)->s_asyntab= -1;			/* - asynsends */
	priv(rp)->s_asynsize= 0;
	priv(rp)->s_asynendpoint = rp->p_endpoint;
	priv(rp)->s_diag_sig = FALSE;		/* no request for diag sigs */

	/* Set defaults for privilege bitmaps. */
	priv(rp)->s_flags= DSRV_F;           /* privilege flags */
	priv(rp)->s_init_flags= DSRV_I;      /* initialization flags */
	priv(rp)->s_trap_mask= DSRV_T;       /* allowed traps */
	स_रखो(&map, 0, माप(map));
	ipc_to_m = DSRV_M;                   /* allowed targets */
	अगर (ipc_to_m == ALL_M) {
		क्रम (i = 0; i < NR_SYS_PROCS; i++)
			set_sys_bit(map, i);
	}
	fill_sendto_mask(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	क्रम(i = 0; i < SYS_CALL_MASK_आकार; i++) {
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
	अगर (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		अगर((r = update_priv(rp, &priv)) != OK) {
			वापस r;
		} 
	}

	वापस(OK);

  हाल SYS_PRIV_SET_USER:
	/* Set a privilege structure of a blocked user process. */
	अगर (!RTS_ISSET(rp, RTS_NO_PRIV)) वापस(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	वापस(OK);

  हाल SYS_PRIV_ADD_IO:
	अगर (RTS_ISSET(rp, RTS_NO_PRIV))
		वापस(EPERM);

#अगर 0 /* XXX -- do we need a call for this? */
	अगर (म_भेद(rp->p_name, "fxp") == 0 ||
		म_भेद(rp->p_name, "rtl8139") == 0)
	{
		म_लिखो("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_target= rp->p_endpoint;
	}
#पूर्ण_अगर

	/* Get the I/O range */
	तथ्य_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &io_range, माप(io_range));
	/* Add the I/O range */
	वापस priv_add_io(rp, &io_range);

  हाल SYS_PRIV_ADD_MEM:
	अगर (RTS_ISSET(rp, RTS_NO_PRIV))
		वापस(EPERM);

	/* Get the memory range */
	अगर((r=तथ्य_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &mem_range, माप(mem_range))) != OK)
		वापस r;
	/* Add the memory range */
	वापस priv_add_mem(rp, &mem_range);

  हाल SYS_PRIV_ADD_IRQ:
	अगर (RTS_ISSET(rp, RTS_NO_PRIV))
		वापस(EPERM);

#अगर 0
	/* Only system processes get IRQs? */
	अगर (!(priv(rp)->s_flags & SYS_PROC))
		वापस EPERM;
#पूर्ण_अगर
	तथ्य_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &irq, माप(irq));
	/* Add the IRQ. */
	वापस priv_add_irq(rp, irq);

  हाल SYS_PRIV_QUERY_MEM:
  {
	phys_bytes addr, limit;
  	काष्ठा priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_start;
	limit = addr + (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	अगर(limit < addr)
		वापस EPERM;
	अगर(!(sp = priv(rp)))
		वापस EPERM;
	क्रम(i = 0; i < sp->s_nr_mem_range; i++) {
		अगर(addr >= sp->s_mem_tab[i].mr_base &&
		   limit <= sp->s_mem_tab[i].mr_limit)
			वापस OK;
	}
	वापस EPERM;
  }

  हाल SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	अगर(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) वापस EINVAL;

	/* Copy privilege structure from caller */
	अगर((r=तथ्य_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &priv, माप(priv))) != OK)
		वापस r;

	/* Override settings in existing privilege structure. */
	अगर((r = update_priv(rp, &priv)) != OK) {
		वापस r;
	}

	वापस(OK);

  शेष:
	म_लिखो("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	वापस EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
अटल पूर्णांक update_priv(काष्ठा proc *rp, काष्ठा priv *priv)
{
/* Update the privilege structure of a given process. */

  पूर्णांक i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  अगर(priv->s_flags & CHECK_IRQ) {
  	अगर (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		वापस EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	क्रम (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#अगर PRIV_DEBUG
  		म_लिखो("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#पूर्ण_अगर
  	}
  }

  /* Copy I/O ranges. */
  अगर(priv->s_flags & CHECK_IO_PORT) {
  	अगर (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		वापस EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	क्रम (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#अगर PRIV_DEBUG
  		म_लिखो("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_base,
  			priv(rp)->s_io_tab[i].ior_limit,
  			rp->p_endpoint);
#पूर्ण_अगर
  	}
  }

  /* Copy memory ranges. */
  अगर(priv->s_flags & CHECK_MEM) {
  	अगर (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		वापस EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	क्रम (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#अगर PRIV_DEBUG
  		म_लिखो("do_privctl: adding mem range [%x..%x] for %d\n",
  			priv(rp)->s_mem_tab[i].mr_base,
  			priv(rp)->s_mem_tab[i].mr_limit,
  			rp->p_endpoint);
#पूर्ण_अगर
  	}
  }

  /* Copy trap mask. */
  priv(rp)->s_trap_mask = priv->s_trap_mask;

  /* Copy target mask. */
#अगर PRIV_DEBUG
  म_लिखो("do_privctl: Setting ipc target mask for %d:");
  क्रम (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	म_लिखो(" %08x", get_sys_bits(priv->s_ipc_to, i));
  }
  म_लिखो("\n");
#पूर्ण_अगर

  fill_sendto_mask(rp, &priv->s_ipc_to);

#अगर PRIV_DEBUG
  म_लिखो("do_privctl: Set ipc target mask for %d:");
  क्रम (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	म_लिखो(" %08x", get_sys_bits(priv(rp)->s_ipc_to, i));
  }
  म_लिखो("\n");
#पूर्ण_अगर

  /* Copy kernel call mask. */
  स_नकल(priv(rp)->s_k_call_mask, priv->s_k_call_mask,
  	माप(priv(rp)->s_k_call_mask));

  वापस OK;
}

#पूर्ण_अगर /* USE_PRIVCTL */

