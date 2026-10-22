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

#include "kernel/system.h"
#include <signal.h>
#include <string.h>
#include <minix/endpoint.h>

#ᬬᬾᬦ᭄ USE_PRIVCTL

#define PRIV_DEBUG 0

static puur_naa_nka update_priv(sa_nracanaa proc *rp, sa_nracanaa priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
puur_naa_nka do_privctl(sa_nracanaa proc * caller, message * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  sa_nracanaa proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  puur_naa_nka ipc_to_m, kcalls;
  puur_naa_nka i, r;
  sa_nracanaa io_range io_range;
  sa_nracanaa minix_mem_range mem_range;
  sa_nracanaa priv priv;
  puur_naa_nka irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  ᬬᬾᬦ᭄ (! (priv(caller)->s_flags & SYS_PROC)) ᬯᬮᬶ(EPERM);
  ᬬᬾᬦ᭄(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  ᬮᬶᬬ ᬬᬾᬦ᭄(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	ᬯᬮᬶ(EINVAL);
  rp = proc_addr(proc_nr);

  switch(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  pak_sa SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	ᬬᬾᬦ᭄ (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		ᬯᬮᬶ(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	ᬯᬮᬶ(OK);

  pak_sa SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	ᬬᬾᬦ᭄ (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		ᬯᬮᬶ(EPERM);
	}
	RTS_SET(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	ᬯᬮᬶ(OK);

  pak_sa SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	ᬬᬾᬦ᭄ (RTS_ISSET(rp, RTS_NO_PRIV)) ᬯᬮᬶ(EPERM);
	RTS_SET(rp, RTS_NO_PRIV);
	ᬯᬮᬶ(OK);

  pak_sa SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	ᬯᬮᬶ(OK);

  pak_sa SYS_PRIV_SET_SYS:
	/* Set a privilege structure of a blocked system process. */
	ᬬᬾᬦ᭄ (! RTS_ISSET(rp, RTS_NO_PRIV)) ᬯᬮᬶ(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	ᬬᬾᬦ᭄ (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		ᬬᬾᬦ᭄((r=data_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_bytes) &priv, parimaa_na(priv))) != OK)
			ᬯᬮᬶ r;

		/* See if the caller wants to assign a static privilege id. */
		ᬬᬾᬦ᭄(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	ᬬᬾᬦ᭄ ((i=get_priv(rp, priv_id)) != OK)
	{
		printf("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		ᬯᬮᬶ(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	for (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	for (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
	      priv(rp)->s_notify_pending.chunk[i] = 0;	/* - notifications */
	priv(rp)->s_int_pending = 0;			/* - interrupts */
	(void) sigemptyset(&priv(rp)->s_sig_pending);	/* - signals */
	reset_kernel_timer(&priv(rp)->s_alarm_timer);	/* - alarm */
	priv(rp)->s_asyntab= -1;			/* - asynsends */
	priv(rp)->s_asynsize= 0;
	priv(rp)->s_asynendpoint = rp->p_endpoint;
	priv(rp)->s_diag_sig = FALSE;		/* no request for diag sigs */

	/* Set defaults for privilege bitmaps. */
	priv(rp)->s_flags= DSRV_F;           /* privilege flags */
	priv(rp)->s_init_flags= DSRV_I;      /* initialization flags */
	priv(rp)->s_trap_mask= DSRV_T;       /* allowed traps */
	memset(&map, 0, parimaa_na(map));
	ipc_to_m = DSRV_M;                   /* allowed targets */
	ᬬᬾᬦ᭄ (ipc_to_m == ALL_M) {
		for (i = 0; i < NR_SYS_PROCS; i++)
			set_sys_bit(map, i);
	}
	fill_sendto_mask(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	for(i = 0; i < SYS_CALL_MASK_SIZE; i++) {
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
	ᬬᬾᬦ᭄ (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		ᬬᬾᬦ᭄((r = update_priv(rp, &priv)) != OK) {
			ᬯᬮᬶ r;
		} 
	}

	ᬯᬮᬶ(OK);

  pak_sa SYS_PRIV_SET_USER:
	/* Set a privilege structure of a blocked user process. */
	ᬬᬾᬦ᭄ (!RTS_ISSET(rp, RTS_NO_PRIV)) ᬯᬮᬶ(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	ᬯᬮᬶ(OK);

  pak_sa SYS_PRIV_ADD_IO:
	ᬬᬾᬦ᭄ (RTS_ISSET(rp, RTS_NO_PRIV))
		ᬯᬮᬶ(EPERM);

#ᬬᬾᬦ᭄ 0 /* XXX -- do we need a call for this? */
	ᬬᬾᬦ᭄ (strcmp(rp->p_name, "fxp") == 0 ||
		strcmp(rp->p_name, "rtl8139") == 0)
	{
		printf("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_target= rp->p_endpoint;
	}
#endif

	/* Get the I/O range */
	data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &io_range, parimaa_na(io_range));
	/* Add the I/O range */
	ᬯᬮᬶ priv_add_io(rp, &io_range);

  pak_sa SYS_PRIV_ADD_MEM:
	ᬬᬾᬦ᭄ (RTS_ISSET(rp, RTS_NO_PRIV))
		ᬯᬮᬶ(EPERM);

	/* Get the memory range */
	ᬬᬾᬦ᭄((r=data_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &mem_range, parimaa_na(mem_range))) != OK)
		ᬯᬮᬶ r;
	/* Add the memory range */
	ᬯᬮᬶ priv_add_mem(rp, &mem_range);

  pak_sa SYS_PRIV_ADD_IRQ:
	ᬬᬾᬦ᭄ (RTS_ISSET(rp, RTS_NO_PRIV))
		ᬯᬮᬶ(EPERM);

#ᬬᬾᬦ᭄ 0
	/* Only system processes get IRQs? */
	ᬬᬾᬦ᭄ (!(priv(rp)->s_flags & SYS_PROC))
		ᬯᬮᬶ EPERM;
#endif
	data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &irq, parimaa_na(irq));
	/* Add the IRQ. */
	ᬯᬮᬶ priv_add_irq(rp, irq);

  pak_sa SYS_PRIV_QUERY_MEM:
  {
	phys_bytes addr, limit;
  	sa_nracanaa priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_start;
	limit = addr + (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	ᬬᬾᬦ᭄(limit < addr)
		ᬯᬮᬶ EPERM;
	ᬬᬾᬦ᭄(!(sp = priv(rp)))
		ᬯᬮᬶ EPERM;
	for(i = 0; i < sp->s_nr_mem_range; i++) {
		ᬬᬾᬦ᭄(addr >= sp->s_mem_tab[i].mr_base &&
		   limit <= sp->s_mem_tab[i].mr_limit)
			ᬯᬮᬶ OK;
	}
	ᬯᬮᬶ EPERM;
  }

  pak_sa SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	ᬬᬾᬦ᭄(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) ᬯᬮᬶ EINVAL;

	/* Copy privilege structure from caller */
	ᬬᬾᬦ᭄((r=data_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &priv, parimaa_na(priv))) != OK)
		ᬯᬮᬶ r;

	/* Override settings in existing privilege structure. */
	ᬬᬾᬦ᭄((r = update_priv(rp, &priv)) != OK) {
		ᬯᬮᬶ r;
	}

	ᬯᬮᬶ(OK);

  default:
	printf("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	ᬯᬮᬶ EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
static puur_naa_nka update_priv(sa_nracanaa proc *rp, sa_nracanaa priv *priv)
{
/* Update the privilege structure of a given process. */

  puur_naa_nka i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  ᬬᬾᬦ᭄(priv->s_flags & CHECK_IRQ) {
  	ᬬᬾᬦ᭄ (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		ᬯᬮᬶ EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	for (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#ᬬᬾᬦ᭄ PRIV_DEBUG
  		printf("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#endif
  	}
  }

  /* Copy I/O ranges. */
  ᬬᬾᬦ᭄(priv->s_flags & CHECK_IO_PORT) {
  	ᬬᬾᬦ᭄ (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		ᬯᬮᬶ EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	for (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#ᬬᬾᬦ᭄ PRIV_DEBUG
  		printf("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_base,
  			priv(rp)->s_io_tab[i].ior_limit,
  			rp->p_endpoint);
#endif
  	}
  }

  /* Copy memory ranges. */
  ᬬᬾᬦ᭄(priv->s_flags & CHECK_MEM) {
  	ᬬᬾᬦ᭄ (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		ᬯᬮᬶ EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	for (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#ᬬᬾᬦ᭄ PRIV_DEBUG
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
#ᬬᬾᬦ᭄ PRIV_DEBUG
  printf("do_privctl: Setting ipc target mask for %d:");
  for (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	printf(" %08x", get_sys_bits(priv->s_ipc_to, i));
  }
  printf("\n");
#endif

  fill_sendto_mask(rp, &priv->s_ipc_to);

#ᬬᬾᬦ᭄ PRIV_DEBUG
  printf("do_privctl: Set ipc target mask for %d:");
  for (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	printf(" %08x", get_sys_bits(priv(rp)->s_ipc_to, i));
  }
  printf("\n");
#endif

  /* Copy kernel call mask. */
  memcpy(priv(rp)->s_k_call_mask, priv->s_k_call_mask,
  	parimaa_na(priv(rp)->s_k_call_mask));

  ᬯᬮᬶ OK;
}

#endif /* USE_PRIVCTL */

