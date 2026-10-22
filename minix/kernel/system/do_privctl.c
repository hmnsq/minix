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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಆದರೆ USE_PRIVCTL

#ವ್ಯಾಖ್ಯಾನಿಸು PRIV_DEBUG 0

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ update_priv(ರಚನೆ proc *rp, ರಚನೆ priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_privctl(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  ರಚನೆ proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  ಪೂರ್ಣಾಂಕ ipc_ಗಮ್ಯ_m, kcalls;
  ಪೂರ್ಣಾಂಕ i, r;
  ರಚನೆ io_range io_range;
  ರಚನೆ minix_mem_range mem_range;
  ರಚನೆ priv priv;
  ಪೂರ್ಣಾಂಕ irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  ಆದರೆ (! (priv(caller)->s_flags & SYS_PROC)) ಹಿಂತಿರುಗಿ(EPERM);
  ಆದರೆ(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	ಹಿಂತಿರುಗಿ(EINVAL);
  rp = proc_addr(proc_nr);

  ಆಯ್ಕೆ(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  ಸ್ಥಿತಿ SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	ಆದರೆ (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		ಹಿಂತಿರುಗಿ(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	ಹಿಂತಿರುಗಿ(OK);

  ಸ್ಥಿತಿ SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	ಆದರೆ (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		ಹಿಂತಿರುಗಿ(EPERM);
	}
	RTS_ಗಣ(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	ಹಿಂತಿರುಗಿ(OK);

  ಸ್ಥಿತಿ SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	ಆದರೆ (RTS_ISSET(rp, RTS_NO_PRIV)) ಹಿಂತಿರುಗಿ(EPERM);
	RTS_ಗಣ(rp, RTS_NO_PRIV);
	ಹಿಂತಿರುಗಿ(OK);

  ಸ್ಥಿತಿ SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	ಹಿಂತಿರುಗಿ(OK);

  ಸ್ಥಿತಿ SYS_PRIV_ಗಣ_SYS:
	/* Set a privilege structure of a blocked system process. */
	ಆದರೆ (! RTS_ISSET(rp, RTS_NO_PRIV)) ಹಿಂತಿರುಗಿ(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	ಆದರೆ (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		ಆದರೆ((r=ದತ್ತಾಂಶ_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_ಬೈಟುಗಳು) &priv, ಗಾತ್ರ(priv))) != OK)
			ಹಿಂತಿರುಗಿ r;

		/* See if the caller wants to assign a static privilege id. */
		ಆದರೆ(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	ಆದರೆ ((i=get_priv(rp, priv_id)) != OK)
	{
		ಬರೆ("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		ಹಿಂತಿರುಗಿ(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	ಗಾಗಿ (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	ಗಾಗಿ (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
	      priv(rp)->s_notify_pending.chunk[i] = 0;	/* - notifications */
	priv(rp)->s_int_pending = 0;			/* - interrupts */
	(ಶೂನ್ಯ) sigemptyset(&priv(rp)->s_sig_pending);	/* - signals */
	reset_kernel_timer(&priv(rp)->s_alarm_timer);	/* - alarm */
	priv(rp)->s_asyntab= -1;			/* - asynsends */
	priv(rp)->s_asynsize= 0;
	priv(rp)->s_asynendpoint = rp->p_endpoint;
	priv(rp)->s_diag_sig = FALSE;		/* no request for diag sigs */

	/* Set defaults for privilege bitmaps. */
	priv(rp)->s_flags= DSRV_F;           /* privilege flags */
	priv(rp)->s_init_flags= DSRV_I;      /* initialization flags */
	priv(rp)->s_trap_ಮುಖವಾಡ= DSRV_T;       /* allowed traps */
	memset(&map, 0, ಗಾತ್ರ(map));
	ipc_ಗಮ್ಯ_m = DSRV_M;                   /* allowed targets */
	ಆದರೆ (ipc_ಗಮ್ಯ_m == ALL_M) {
		ಗಾಗಿ (i = 0; i < NR_SYS_PROCS; i++)
			ಗಣ_sys_ಬಿಟ್(map, i);
	}
	fill_sendto_ಮುಖವಾಡ(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	ಗಾಗಿ(i = 0; i < SYS_CALL_ಮುಖವಾಡ_ಗಾತ್ರ; i++) {
		priv(rp)->s_k_call_ಮುಖವಾಡ[i] = (kcalls == NO_C ? 0 : (~0));
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
	priv(rp)->s_grant_ಕೋಷ್ಟಕ= 0;
	priv(rp)->s_grant_entries= 0;
	priv(rp)->s_grant_endpoint = rp->p_endpoint;
	priv(rp)->s_state_ಕೋಷ್ಟಕ= 0;
	priv(rp)->s_state_entries= 0;
	priv(rp)->s_ipcf= 0;

	/* Override defaults if the caller has supplied a privilege structure. */
	ಆದರೆ (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		ಆದರೆ((r = update_priv(rp, &priv)) != OK) {
			ಹಿಂತಿರುಗಿ r;
		} 
	}

	ಹಿಂತಿರುಗಿ(OK);

  ಸ್ಥಿತಿ SYS_PRIV_ಗಣ_USER:
	/* Set a privilege structure of a blocked user process. */
	ಆದರೆ (!RTS_ISSET(rp, RTS_NO_PRIV)) ಹಿಂತಿರುಗಿ(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	ಹಿಂತಿರುಗಿ(OK);

  ಸ್ಥಿತಿ SYS_PRIV_ಕೂಡಿಸು_IO:
	ಆದರೆ (RTS_ISSET(rp, RTS_NO_PRIV))
		ಹಿಂತಿರುಗಿ(EPERM);

#ಆದರೆ 0 /* XXX -- do we need a call for this? */
	ಆದರೆ (strcmp(rp->p_name, "fxp") == 0 ||
		strcmp(rp->p_name, "rtl8139") == 0)
	{
		ಬರೆ("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_ಗುರಿ= rp->p_endpoint;
	}
#ಆದರೆ_ಅಂತ್ಯ

	/* Get the I/O range */
	ದತ್ತಾಂಶ_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_ಬೈಟುಗಳು) &io_range, ಗಾತ್ರ(io_range));
	/* Add the I/O range */
	ಹಿಂತಿರುಗಿ priv_ಕೂಡಿಸು_io(rp, &io_range);

  ಸ್ಥಿತಿ SYS_PRIV_ಕೂಡಿಸು_MEM:
	ಆದರೆ (RTS_ISSET(rp, RTS_NO_PRIV))
		ಹಿಂತಿರುಗಿ(EPERM);

	/* Get the memory range */
	ಆದರೆ((r=ದತ್ತಾಂಶ_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_ಬೈಟುಗಳು) &mem_range, ಗಾತ್ರ(mem_range))) != OK)
		ಹಿಂತಿರುಗಿ r;
	/* Add the memory range */
	ಹಿಂತಿರುಗಿ priv_ಕೂಡಿಸು_mem(rp, &mem_range);

  ಸ್ಥಿತಿ SYS_PRIV_ಕೂಡಿಸು_IRQ:
	ಆದರೆ (RTS_ISSET(rp, RTS_NO_PRIV))
		ಹಿಂತಿರುಗಿ(EPERM);

#ಆದರೆ 0
	/* Only system processes get IRQs? */
	ಆದರೆ (!(priv(rp)->s_flags & SYS_PROC))
		ಹಿಂತಿರುಗಿ EPERM;
#ಆದರೆ_ಅಂತ್ಯ
	ದತ್ತಾಂಶ_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_ಬೈಟುಗಳು) &irq, ಗಾತ್ರ(irq));
	/* Add the IRQ. */
	ಹಿಂತಿರುಗಿ priv_ಕೂಡಿಸು_irq(rp, irq);

  ಸ್ಥಿತಿ SYS_PRIV_QUERY_MEM:
  {
	phys_ಬೈಟುಗಳು addr, ಎಲ್ಲೆ_2;
  	ರಚನೆ priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_ಬೈಟುಗಳು) m_ptr->m_lsys_krn_sys_privctl.phys_ಆರಂಭ;
	ಎಲ್ಲೆ_2 = addr + (phys_ಬೈಟುಗಳು) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	ಆದರೆ(ಎಲ್ಲೆ_2 < addr)
		ಹಿಂತಿರುಗಿ EPERM;
	ಆದರೆ(!(sp = priv(rp)))
		ಹಿಂತಿರುಗಿ EPERM;
	ಗಾಗಿ(i = 0; i < sp->s_nr_mem_range; i++) {
		ಆದರೆ(addr >= sp->s_mem_tab[i].mr_ತಳ &&
		   ಎಲ್ಲೆ_2 <= sp->s_mem_tab[i].mr_ಎಲ್ಲೆ)
			ಹಿಂತಿರುಗಿ OK;
	}
	ಹಿಂತಿರುಗಿ EPERM;
  }

  ಸ್ಥಿತಿ SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	ಆದರೆ(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) ಹಿಂತಿರುಗಿ EINVAL;

	/* Copy privilege structure from caller */
	ಆದರೆ((r=ದತ್ತಾಂಶ_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_ಬೈಟುಗಳು) &priv, ಗಾತ್ರ(priv))) != OK)
		ಹಿಂತಿರುಗಿ r;

	/* Override settings in existing privilege structure. */
	ಆದರೆ((r = update_priv(rp, &priv)) != OK) {
		ಹಿಂತಿರುಗಿ r;
	}

	ಹಿಂತಿರುಗಿ(OK);

  ಪೂರ್ವನಿಯೋಜಿತ:
	ಬರೆ("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	ಹಿಂತಿರುಗಿ EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ update_priv(ರಚನೆ proc *rp, ರಚನೆ priv *priv)
{
/* Update the privilege structure of a given process. */

  ಪೂರ್ಣಾಂಕ i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  ಆದರೆ(priv->s_flags & CHECK_IRQ) {
  	ಆದರೆ (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		ಹಿಂತಿರುಗಿ EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	ಗಾಗಿ (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#ಆದರೆ PRIV_DEBUG
  		ಬರೆ("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#ಆದರೆ_ಅಂತ್ಯ
  	}
  }

  /* Copy I/O ranges. */
  ಆದರೆ(priv->s_flags & CHECK_IO_ಪೋರ್ಟ್) {
  	ಆದರೆ (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		ಹಿಂತಿರುಗಿ EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	ಗಾಗಿ (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#ಆದರೆ PRIV_DEBUG
  		ಬರೆ("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_ತಳ,
  			priv(rp)->s_io_tab[i].ior_ಎಲ್ಲೆ,
  			rp->p_endpoint);
#ಆದರೆ_ಅಂತ್ಯ
  	}
  }

  /* Copy memory ranges. */
  ಆದರೆ(priv->s_flags & CHECK_MEM) {
  	ಆದರೆ (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		ಹಿಂತಿರುಗಿ EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	ಗಾಗಿ (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#ಆದರೆ PRIV_DEBUG
  		ಬರೆ("do_privctl: adding mem range [%x..%x] for %d\n",
  			priv(rp)->s_mem_tab[i].mr_ತಳ,
  			priv(rp)->s_mem_tab[i].mr_ಎಲ್ಲೆ,
  			rp->p_endpoint);
#ಆದರೆ_ಅಂತ್ಯ
  	}
  }

  /* Copy trap mask. */
  priv(rp)->s_trap_ಮುಖವಾಡ = priv->s_trap_ಮುಖವಾಡ;

  /* Copy target mask. */
#ಆದರೆ PRIV_DEBUG
  ಬರೆ("do_privctl: Setting ipc target mask for %d:");
  ಗಾಗಿ (i=0; i < NR_SYS_PROCS; i += BITCHUNK_ಬಿಟ್ಟುಗಳು) {
  	ಬರೆ(" %08x", get_sys_ಬಿಟ್ಟುಗಳು(priv->s_ipc_ಗಮ್ಯ, i));
  }
  ಬರೆ("\n");
#ಆದರೆ_ಅಂತ್ಯ

  fill_sendto_ಮುಖವಾಡ(rp, &priv->s_ipc_ಗಮ್ಯ);

#ಆದರೆ PRIV_DEBUG
  ಬರೆ("do_privctl: Set ipc target mask for %d:");
  ಗಾಗಿ (i=0; i < NR_SYS_PROCS; i += BITCHUNK_ಬಿಟ್ಟುಗಳು) {
  	ಬರೆ(" %08x", get_sys_ಬಿಟ್ಟುಗಳು(priv(rp)->s_ipc_ಗಮ್ಯ, i));
  }
  ಬರೆ("\n");
#ಆದರೆ_ಅಂತ್ಯ

  /* Copy kernel call mask. */
  ಸ್ಮರಣೆ_ನಕಲು(priv(rp)->s_k_call_ಮುಖವಾಡ, priv->s_k_call_ಮುಖವಾಡ,
  	ಗಾತ್ರ(priv(rp)->s_k_call_ಮುಖವಾಡ));

  ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_PRIVCTL */

