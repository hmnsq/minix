#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PRIV_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRIV_H

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
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/const.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/priv.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/const.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/type.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/ipc_filter.h"

ਬਣਤਰ priv {
  proc_nr_t s_proc_nr;		/* number of associated process */
  sys_id_t s_id;		/* index of this system structure */
  ਛੋਟਾ s_flags;		/* PREEMTIBLE, BILLABLE, etc. */
  ਪੂਰਨ s_init_flags;             /* initialization flags given to the process. */

  /* Asynchronous sends */
  vir_ਬਾਈਟਾਂ s_asyntab;		/* addr. of table in process' address space */
  size_t s_asynsize;		/* number of elements in table. 0 when not in
				 * use
				 */
  endpoint_t s_asynendpoint;    /* the endpoint the asyn table belongs to. */

  ਛੋਟਾ s_trap_ਨਕਾਬ;		/* allowed system call traps */
  sys_map_t s_ipc_ਮੰਜ਼ਿਲ;		/* allowed destination processes */

  /* allowed kernel calls */
  bitchunk_t s_k_call_ਨਕਾਬ[SYS_CALL_ਨਕਾਬ_ਮਾਪ];

  endpoint_t s_sig_mgr;		/* signal manager for system signals */
  endpoint_t s_bak_sig_mgr;	/* backup signal manager for system signals */
  sys_map_t s_notify_pending;  	/* bit map with pending notifications */
  sys_map_t s_asyn_pending;	/* bit map with pending asyn messages */
  irq_id_t s_int_pending;	/* pending hardware interrupts */
  sigset_t s_sig_pending;	/* pending signals */
  ipc_filter_t *s_ipcf;         /* ipc filter (NULL when no filter is set) */

  minix_timer_t s_alarm_timer;	/* synchronous alarm timer */
  reg_t *s_ਢੇਰ_guard;		/* stack guard word for kernel tasks */

  ਅੱਖਰ s_diag_sig;		/* send a SIGKMESS when diagnostics arrive? */

  ਪੂਰਨ s_nr_io_range;		/* allowed I/O ports */
  ਬਣਤਰ io_range s_io_tab[NR_IO_RANGE];

  ਪੂਰਨ s_nr_mem_range;		/* allowed memory ranges */
  ਬਣਤਰ minix_mem_range s_mem_tab[NR_MEM_RANGE];

  ਪੂਰਨ s_nr_irq;			/* allowed IRQ lines */
  ਪੂਰਨ s_irq_tab[NR_IRQ];
  vir_ਬਾਈਟਾਂ s_grant_ਸਾਰਣੀ;	/* grant table address of process, or 0 */
  ਪੂਰਨ s_grant_entries;		/* no. of entries, or 0 */
  endpoint_t s_grant_endpoint;  /* the endpoint the grant table belongs to */
  vir_ਬਾਈਟਾਂ s_state_ਸਾਰਣੀ;	/* state table address of process, or 0 */
  ਪੂਰਨ s_state_entries;		/* no. of entries, or 0 */
};

/* Guard word for task stacks. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_GUARD	((reg_t) (ਆਕਾਰ(reg_t) == 2 ? 0xBEEF : 0xDEADBEEF))

/* Magic system structure table addresses. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEG_PRIV_ADDR              (&priv[0])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਾ_PRIV_ADDR              (&priv[NR_SYS_PROCS])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEG_STATIC_PRIV_ADDR       BEG_PRIV_ADDR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਾ_STATIC_PRIV_ADDR       (BEG_STATIC_PRIV_ADDR + NR_STATIC_PRIV_IDS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEG_DYN_PRIV_ADDR          ਸਿਰਾ_STATIC_PRIV_ADDR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਾ_DYN_PRIV_ADDR          ਸਿਰਾ_PRIV_ADDR

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ priv_addr(i)      (ppriv_addr)[(i)]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ priv_id(rp)	  ((rp)->p_priv->s_id)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ priv(rp)	  ((rp)->p_priv)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ id_ਮੰਜ਼ਿਲ_nr(id)	priv_addr(id)->s_proc_nr
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ nr_ਮੰਜ਼ਿਲ_id(nr)    priv(proc_addr(nr))->s_id

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ may_send_ਮੰਜ਼ਿਲ(rp, nr) (get_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(rp)->s_ipc_ਮੰਜ਼ਿਲ, nr_ਮੰਜ਼ਿਲ_id(nr)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ may_asynsend_ਮੰਜ਼ਿਲ(rp, nr) (may_send_ਮੰਜ਼ਿਲ(rp, nr) || (rp)->p_nr == nr)

/* The system structures table and pointers to individual table slots. The 
 * pointers allow faster access because now a process entry can be found by 
 * indexing the psys_addr array, while accessing an element i requires a 
 * multiplication with sizeof(struct sys) to determine the address. 
 */
EXTERN ਬਣਤਰ priv priv[NR_SYS_PROCS];		/* system properties table */
EXTERN ਬਣਤਰ priv *ppriv_addr[NR_SYS_PROCS];	/* direct slot pointers */

/* Make sure the system can boot. The following sanity check verifies that
 * the system privileges table is large enough for the number of processes
 * in the boot image. 
 */
#ਜੇ (NR_BOOT_PROCS > NR_SYS_PROCS)
#ਗਲਤੀ NR_SYS_PROCS must be larger than NR_BOOT_PROCS
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* PRIV_H */
