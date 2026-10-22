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

#якщо USE_PRIVCTL

#define PRIV_DEBUG 0

static ціле update_priv(struct proc *rp, struct priv *priv);

/*===========================================================================*
 *				do_privctl				     *
 *===========================================================================*/
ціле do_privctl(struct proc * caller, message * m_ptr)
{
/* Handle sys_privctl(). Update a process' privileges. If the process is not
 * yet a system process, make sure it gets its own privilege structure.
 */
  struct proc *rp;
  proc_nr_t proc_nr;
  sys_id_t priv_id;
  sys_map_t map;
  ціле ipc_to_m, kcalls;
  ціле i, r;
  struct io_range io_range;
  struct minix_mem_range mem_range;
  struct priv priv;
  ціле irq;

  /* Check whether caller is allowed to make this call. Privileged processes 
   * can only update the privileges of processes that are inhibited from 
   * running by the RTS_NO_PRIV flag. This flag is set when a privileged process
   * forks. 
   */
  якщо (! (priv(caller)->s_flags & SYS_PROC)) повернути(EPERM);
  якщо(m_ptr->m_lsys_krn_sys_privctl.endpt == SELF) okendpt(caller->p_endpoint,
	&proc_nr);
  інакше якщо(!isokendpt(m_ptr->m_lsys_krn_sys_privctl.endpt, &proc_nr))
	повернути(EINVAL);
  rp = proc_addr(proc_nr);

  switch(m_ptr->m_lsys_krn_sys_privctl.request)
  {
  case SYS_PRIV_ALLOW:
	/* Allow process to run. Make sure its privilege structure has already
	 * been set.
	 */
	якщо (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		повернути(EPERM);
	}
	RTS_UNSET(rp, RTS_NO_PRIV);
	повернути(OK);

  case SYS_PRIV_YIELD:
	/* Allow process to run and suspend the caller. */
	якщо (!RTS_ISSET(rp, RTS_NO_PRIV) || priv(rp)->s_proc_nr == NONE) {
		повернути(EPERM);
	}
	RTS_SET(caller, RTS_NO_PRIV);
	RTS_UNSET(rp, RTS_NO_PRIV);
	повернути(OK);

  case SYS_PRIV_DISALLOW:
	/* Disallow process from running. */
	якщо (RTS_ISSET(rp, RTS_NO_PRIV)) повернути(EPERM);
	RTS_SET(rp, RTS_NO_PRIV);
	повернути(OK);

  case SYS_PRIV_CLEAR_IPC_REFS:
	/* Clear pending IPC for the process. */
	clear_ipc_refs(rp, EDEADSRCDST);
	повернути(OK);

  case SYS_PRIV_SET_SYS:
	/* Set a privilege structure of a blocked system process. */
	якщо (! RTS_ISSET(rp, RTS_NO_PRIV)) повернути(EPERM);

	/* Check whether a static or dynamic privilege id must be allocated. */
	priv_id = NULL_PRIV_ID;
	якщо (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		/* Copy privilege structure from caller */
		якщо((r=data_copy(caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
			(vir_bytes) &priv, sizeof(priv))) != OK)
			повернути r;

		/* See if the caller wants to assign a static privilege id. */
		якщо(!(priv.s_flags & DYN_PRIV_ID)) {
			priv_id = priv.s_id;
		}
	}

	/* Make sure this process has its own privileges structure. This may
	 * fail, since there are only a limited number of system processes.
	 * Then copy privileges from the caller and restore some defaults.
	 */
	якщо ((i=get_priv(rp, priv_id)) != OK)
	{
		printf("do_privctl: unable to allocate priv_id %d: %d\n",
			priv_id, i);
		повернути(i);
	}
	priv_id = priv(rp)->s_id;		/* backup privilege id */
	*priv(rp) = *priv(caller);		/* copy from caller */
	priv(rp)->s_id = priv_id;		/* restore privilege id */
	priv(rp)->s_proc_nr = proc_nr;		/* reassociate process nr */

	для (i=0; i< NR_SYS_CHUNKS; i++)		/* remove pending: */
	      priv(rp)->s_asyn_pending.chunk[i] = 0;	/* - incoming asyn */
	для (i=0; i< NR_SYS_CHUNKS; i++)		/*   messages */
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
	memset(&map, 0, sizeof(map));
	ipc_to_m = DSRV_M;                   /* allowed targets */
	якщо (ipc_to_m == ALL_M) {
		для (i = 0; i < NR_SYS_PROCS; i++)
			set_sys_bit(map, i);
	}
	fill_sendto_mask(rp, &map);
	kcalls = DSRV_KC;                    /* allowed kernel calls */
	для(i = 0; i < SYS_CALL_MASK_SIZE; i++) {
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
	якщо (m_ptr->m_lsys_krn_sys_privctl.arg_ptr)
	{
		якщо((r = update_priv(rp, &priv)) != OK) {
			повернути r;
		} 
	}

	повернути(OK);

  case SYS_PRIV_SET_USER:
	/* Set a privilege structure of a blocked user process. */
	якщо (!RTS_ISSET(rp, RTS_NO_PRIV)) повернути(EPERM);

	/* Link the process to the privilege structure of the root user
	 * process all the user processes share.
	 */
	priv(rp) = priv_addr(USER_PRIV_ID);

	повернути(OK);

  case SYS_PRIV_ADD_IO:
	якщо (RTS_ISSET(rp, RTS_NO_PRIV))
		повернути(EPERM);

#якщо 0 /* XXX -- do we need a call for this? */
	якщо (strcmp(rp->p_name, "fxp") == 0 ||
		strcmp(rp->p_name, "rtl8139") == 0)
	{
		printf("setting ipc_stats_target to %d\n", rp->p_endpoint);
		ipc_stats_target= rp->p_endpoint;
	}
#endif

	/* Get the I/O range */
	data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &io_range, sizeof(io_range));
	/* Add the I/O range */
	повернути priv_add_io(rp, &io_range);

  case SYS_PRIV_ADD_MEM:
	якщо (RTS_ISSET(rp, RTS_NO_PRIV))
		повернути(EPERM);

	/* Get the memory range */
	якщо((r=data_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &mem_range, sizeof(mem_range))) != OK)
		повернути r;
	/* Add the memory range */
	повернути priv_add_mem(rp, &mem_range);

  case SYS_PRIV_ADD_IRQ:
	якщо (RTS_ISSET(rp, RTS_NO_PRIV))
		повернути(EPERM);

#якщо 0
	/* Only system processes get IRQs? */
	якщо (!(priv(rp)->s_flags & SYS_PROC))
		повернути EPERM;
#endif
	data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_privctl.arg_ptr,
		KERNEL, (vir_bytes) &irq, sizeof(irq));
	/* Add the IRQ. */
	повернути priv_add_irq(rp, irq);

  case SYS_PRIV_QUERY_MEM:
  {
	phys_bytes addr, limit;
  	struct priv *sp;
	/* See if a certain process is allowed to map in certain physical
	 * memory.
	 */
	addr = (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_start;
	limit = addr + (phys_bytes) m_ptr->m_lsys_krn_sys_privctl.phys_len - 1;
	якщо(limit < addr)
		повернути EPERM;
	якщо(!(sp = priv(rp)))
		повернути EPERM;
	для(i = 0; i < sp->s_nr_mem_range; i++) {
		якщо(addr >= sp->s_mem_tab[i].mr_base &&
		   limit <= sp->s_mem_tab[i].mr_limit)
			повернути OK;
	}
	повернути EPERM;
  }

  case SYS_PRIV_UPDATE_SYS:
	/* Update the privilege structure of a system process. */
	якщо(!m_ptr->m_lsys_krn_sys_privctl.arg_ptr) повернути EINVAL;

	/* Copy privilege structure from caller */
	якщо((r=data_copy(caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_privctl.arg_ptr, KERNEL,
		(vir_bytes) &priv, sizeof(priv))) != OK)
		повернути r;

	/* Override settings in existing privilege structure. */
	якщо((r = update_priv(rp, &priv)) != OK) {
		повернути r;
	}

	повернути(OK);

  default:
	printf("do_privctl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_privctl.request);
	повернути EINVAL;
  }
}

/*===========================================================================*
 *				update_priv				     *
 *===========================================================================*/
static ціле update_priv(struct proc *rp, struct priv *priv)
{
/* Update the privilege structure of a given process. */

  ціле i;

  /* Copy flags and signal managers. */
  priv(rp)->s_flags = priv->s_flags;
  priv(rp)->s_init_flags = priv->s_init_flags;
  priv(rp)->s_sig_mgr = priv->s_sig_mgr;
  priv(rp)->s_bak_sig_mgr = priv->s_bak_sig_mgr;

  /* Copy IRQs. */
  якщо(priv->s_flags & CHECK_IRQ) {
  	якщо (priv->s_nr_irq < 0 || priv->s_nr_irq > NR_IRQ)
  		повернути EINVAL;
  	priv(rp)->s_nr_irq= priv->s_nr_irq;
  	для (i= 0; i<priv->s_nr_irq; i++)
  	{
  		priv(rp)->s_irq_tab[i]= priv->s_irq_tab[i];
#якщо PRIV_DEBUG
  		printf("do_privctl: adding IRQ %d for %d\n",
  			priv(rp)->s_irq_tab[i], rp->p_endpoint);
#endif
  	}
  }

  /* Copy I/O ranges. */
  якщо(priv->s_flags & CHECK_IO_PORT) {
  	якщо (priv->s_nr_io_range < 0 || priv->s_nr_io_range > NR_IO_RANGE)
  		повернути EINVAL;
  	priv(rp)->s_nr_io_range= priv->s_nr_io_range;
  	для (i= 0; i<priv->s_nr_io_range; i++)
  	{
  		priv(rp)->s_io_tab[i]= priv->s_io_tab[i];
#якщо PRIV_DEBUG
  		printf("do_privctl: adding I/O range [%x..%x] for %d\n",
  			priv(rp)->s_io_tab[i].ior_base,
  			priv(rp)->s_io_tab[i].ior_limit,
  			rp->p_endpoint);
#endif
  	}
  }

  /* Copy memory ranges. */
  якщо(priv->s_flags & CHECK_MEM) {
  	якщо (priv->s_nr_mem_range < 0 || priv->s_nr_mem_range > NR_MEM_RANGE)
  		повернути EINVAL;
  	priv(rp)->s_nr_mem_range= priv->s_nr_mem_range;
  	для (i= 0; i<priv->s_nr_mem_range; i++)
  	{
  		priv(rp)->s_mem_tab[i]= priv->s_mem_tab[i];
#якщо PRIV_DEBUG
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
#якщо PRIV_DEBUG
  printf("do_privctl: Setting ipc target mask for %d:");
  для (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	printf(" %08x", get_sys_bits(priv->s_ipc_to, i));
  }
  printf("\n");
#endif

  fill_sendto_mask(rp, &priv->s_ipc_to);

#якщо PRIV_DEBUG
  printf("do_privctl: Set ipc target mask for %d:");
  для (i=0; i < NR_SYS_PROCS; i += BITCHUNK_BITS) {
  	printf(" %08x", get_sys_bits(priv(rp)->s_ipc_to, i));
  }
  printf("\n");
#endif

  /* Copy kernel call mask. */
  memcpy(priv(rp)->s_k_call_mask, priv->s_k_call_mask,
  	sizeof(priv(rp)->s_k_call_mask));

  повернути OK;
}

#endif /* USE_PRIVCTL */

