#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PRIV_H
#ವ್ಯಾಖ್ಯಾನಿಸು PRIV_H

/* Declaration of the system privileges structure. It defines flags, system 
 * call masks, an synchronous alarm timer, I/O privileges, pending hardware 
 * interrupts and notifications, and so on.
 * System processes each get their own structure with properties, whereas all 
 * user processes share one structure. This setup provides a clear separation
 * between common and privileged process fields and is very space efficient. 
 *
 * Changes:
 *   Nov 22, 2009  rewrite of privilege management (Cristiano Giuffrida)
 *   Jul 01, 2005  Created.  (Jorrit N. Herder)	
 */
#ಅನ್ತರ್ಭಾವ <minix/const.h>
#ಅನ್ತರ್ಭಾವ <minix/priv.h>
#ಅನ್ತರ್ಭಾವ "kernel/const.h"
#ಅನ್ತರ್ಭಾವ "kernel/type.h"
#ಅನ್ತರ್ಭಾವ "kernel/ipc_filter.h"

ರಚನೆ priv {
  proc_nr_t s_proc_nr;		/* number of associated process */
  sys_id_t s_id;		/* index of this system structure */
  ಗಿಡ್ಡ s_flags;		/* PREEMTIBLE, BILLABLE, etc. */
  ಪೂರ್ಣಾಂಕ s_init_flags;             /* initialization flags given to the process. */

  /* Asynchronous sends */
  vir_ಬೈಟುಗಳು s_asyntab;		/* addr. of table in process' address space */
  size_t s_asynsize;		/* number of elements in table. 0 when not in
				 * use
				 */
  endpoint_t s_asynendpoint;    /* the endpoint the asyn table belongs to. */

  ಗಿಡ್ಡ s_trap_ಮುಖವಾಡ;		/* allowed system call traps */
  sys_map_t s_ipc_ಗಮ್ಯ;		/* allowed destination processes */

  /* allowed kernel calls */
  bitchunk_t s_k_call_ಮುಖವಾಡ[SYS_CALL_ಮುಖವಾಡ_ಗಾತ್ರ];

  endpoint_t s_sig_mgr;		/* signal manager for system signals */
  endpoint_t s_bak_sig_mgr;	/* backup signal manager for system signals */
  sys_map_t s_notify_pending;  	/* bit map with pending notifications */
  sys_map_t s_asyn_pending;	/* bit map with pending asyn messages */
  irq_id_t s_int_pending;	/* pending hardware interrupts */
  sigset_t s_sig_pending;	/* pending signals */
  ipc_filter_t *s_ipcf;         /* ipc filter (NULL when no filter is set) */

  minix_timer_t s_alarm_timer;	/* synchronous alarm timer */
  reg_t *s_ರಾಶಿ_guard;		/* stack guard word for kernel tasks */

  ಅಕ್ಷರ s_diag_sig;		/* send a SIGKMESS when diagnostics arrive? */

  ಪೂರ್ಣಾಂಕ s_nr_io_range;		/* allowed I/O ports */
  ರಚನೆ io_range s_io_tab[NR_IO_RANGE];

  ಪೂರ್ಣಾಂಕ s_nr_mem_range;		/* allowed memory ranges */
  ರಚನೆ minix_mem_range s_mem_tab[NR_MEM_RANGE];

  ಪೂರ್ಣಾಂಕ s_nr_irq;			/* allowed IRQ lines */
  ಪೂರ್ಣಾಂಕ s_irq_tab[NR_IRQ];
  vir_ಬೈಟುಗಳು s_grant_ಕೋಷ್ಟಕ;	/* grant table address of process, or 0 */
  ಪೂರ್ಣಾಂಕ s_grant_entries;		/* no. of entries, or 0 */
  endpoint_t s_grant_endpoint;  /* the endpoint the grant table belongs to */
  vir_ಬೈಟುಗಳು s_state_ಕೋಷ್ಟಕ;	/* state table address of process, or 0 */
  ಪೂರ್ಣಾಂಕ s_state_entries;		/* no. of entries, or 0 */
};

/* Guard word for task stacks. */
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_GUARD	((reg_t) (ಗಾತ್ರ(reg_t) == 2 ? 0xBEEF : 0xDEADBEEF))

/* Magic system structure table addresses. */
#ವ್ಯಾಖ್ಯಾನಿಸು BEG_PRIV_ADDR              (&priv[0])
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆ_PRIV_ADDR              (&priv[NR_SYS_PROCS])
#ವ್ಯಾಖ್ಯಾನಿಸು BEG_STATIC_PRIV_ADDR       BEG_PRIV_ADDR
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆ_STATIC_PRIV_ADDR       (BEG_STATIC_PRIV_ADDR + NR_STATIC_PRIV_IDS)
#ವ್ಯಾಖ್ಯಾನಿಸು BEG_DYN_PRIV_ADDR          ಕೊನೆ_STATIC_PRIV_ADDR
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆ_DYN_PRIV_ADDR          ಕೊನೆ_PRIV_ADDR

#ವ್ಯಾಖ್ಯಾನಿಸು priv_addr(i)      (ppriv_addr)[(i)]
#ವ್ಯಾಖ್ಯಾನಿಸು priv_id(rp)	  ((rp)->p_priv->s_id)
#ವ್ಯಾಖ್ಯಾನಿಸು priv(rp)	  ((rp)->p_priv)

#ವ್ಯಾಖ್ಯಾನಿಸು id_ಗಮ್ಯ_nr(id)	priv_addr(id)->s_proc_nr
#ವ್ಯಾಖ್ಯಾನಿಸು nr_ಗಮ್ಯ_id(nr)    priv(proc_addr(nr))->s_id

#ವ್ಯಾಖ್ಯಾನಿಸು may_send_ಗಮ್ಯ(rp, nr) (get_sys_ಬಿಟ್(priv(rp)->s_ipc_ಗಮ್ಯ, nr_ಗಮ್ಯ_id(nr)))
#ವ್ಯಾಖ್ಯಾನಿಸು may_asynsend_ಗಮ್ಯ(rp, nr) (may_send_ಗಮ್ಯ(rp, nr) || (rp)->p_nr == nr)

/* The system structures table and pointers to individual table slots. The 
 * pointers allow faster access because now a process entry can be found by 
 * indexing the psys_addr array, while accessing an element i requires a 
 * multiplication with sizeof(struct sys) to determine the address. 
 */
EXTERN ರಚನೆ priv priv[NR_SYS_PROCS];		/* system properties table */
EXTERN ರಚನೆ priv *ppriv_addr[NR_SYS_PROCS];	/* direct slot pointers */

/* Make sure the system can boot. The following sanity check verifies that
 * the system privileges table is large enough for the number of processes
 * in the boot image. 
 */
#ಆದರೆ (NR_BOOT_PROCS > NR_SYS_PROCS)
#ದೋಷ NR_SYS_PROCS must be larger than NR_BOOT_PROCS
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* PRIV_H */
