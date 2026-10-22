#ifndef PRIV_H
#define PRIV_H

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
#अन्तर्भाव <minix/const.h>
#अन्तर्भाव <minix/priv.h>
#अन्तर्भाव "kernel/const.h"
#अन्तर्भाव "kernel/type.h"
#अन्तर्भाव "kernel/ipc_filter.h"

संरचना priv {
  proc_nr_t s_proc_nr;		/* number of associated process */
  sys_id_t s_id;		/* index of this system structure */
  short s_flags;		/* PREEMTIBLE, BILLABLE, etc. */
  पूर्णाङ्क s_init_flags;             /* initialization flags given to the process. */

  /* Asynchronous sends */
  vir_bytes s_asyntab;		/* addr. of table in process' address space */
  size_t s_asynsize;		/* number of elements in table. 0 when not in
				 * use
				 */
  endpoint_t s_asynendpoint;    /* the endpoint the asyn table belongs to. */

  short s_trap_mask;		/* allowed system call traps */
  sys_map_t s_ipc_to;		/* allowed destination processes */

  /* allowed kernel calls */
  bitchunk_t s_k_call_mask[SYS_CALL_MASK_SIZE];

  endpoint_t s_sig_mgr;		/* signal manager for system signals */
  endpoint_t s_bak_sig_mgr;	/* backup signal manager for system signals */
  sys_map_t s_notify_pending;  	/* bit map with pending notifications */
  sys_map_t s_asyn_pending;	/* bit map with pending asyn messages */
  irq_id_t s_int_pending;	/* pending hardware interrupts */
  sigset_t s_sig_pending;	/* pending signals */
  ipc_filter_t *s_ipcf;         /* ipc filter (NULL when no filter is set) */

  minix_timer_t s_alarm_timer;	/* synchronous alarm timer */
  reg_t *s_राशिः_guard;		/* stack guard word for kernel tasks */

  वर्ण s_diag_sig;		/* send a SIGKMESS when diagnostics arrive? */

  पूर्णाङ्क s_nr_io_range;		/* allowed I/O ports */
  संरचना io_range s_io_tab[NR_IO_RANGE];

  पूर्णाङ्क s_nr_mem_range;		/* allowed memory ranges */
  संरचना minix_mem_range s_mem_tab[NR_MEM_RANGE];

  पूर्णाङ्क s_nr_irq;			/* allowed IRQ lines */
  पूर्णाङ्क s_irq_tab[NR_IRQ];
  vir_bytes s_grant_table;	/* grant table address of process, or 0 */
  पूर्णाङ्क s_grant_entries;		/* no. of entries, or 0 */
  endpoint_t s_grant_endpoint;  /* the endpoint the grant table belongs to */
  vir_bytes s_state_table;	/* state table address of process, or 0 */
  पूर्णाङ्क s_state_entries;		/* no. of entries, or 0 */
};

/* Guard word for task stacks. */
#define राशिः_GUARD	((reg_t) (परिमाण(reg_t) == 2 ? 0xBEEF : 0xDEADBEEF))

/* Magic system structure table addresses. */
#define BEG_PRIV_ADDR              (&priv[0])
#define END_PRIV_ADDR              (&priv[NR_SYS_PROCS])
#define BEG_STATIC_PRIV_ADDR       BEG_PRIV_ADDR
#define END_STATIC_PRIV_ADDR       (BEG_STATIC_PRIV_ADDR + NR_STATIC_PRIV_IDS)
#define BEG_DYN_PRIV_ADDR          END_STATIC_PRIV_ADDR
#define END_DYN_PRIV_ADDR          END_PRIV_ADDR

#define priv_addr(i)      (ppriv_addr)[(i)]
#define priv_id(rp)	  ((rp)->p_priv->s_id)
#define priv(rp)	  ((rp)->p_priv)

#define id_to_nr(id)	priv_addr(id)->s_proc_nr
#define nr_to_id(nr)    priv(proc_addr(nr))->s_id

#define may_send_to(rp, nr) (get_sys_bit(priv(rp)->s_ipc_to, nr_to_id(nr)))
#define may_asynsend_to(rp, nr) (may_send_to(rp, nr) || (rp)->p_nr == nr)

/* The system structures table and pointers to individual table slots. The 
 * pointers allow faster access because now a process entry can be found by 
 * indexing the psys_addr array, while accessing an element i requires a 
 * multiplication with sizeof(struct sys) to determine the address. 
 */
EXTERN संरचना priv priv[NR_SYS_PROCS];		/* system properties table */
EXTERN संरचना priv *ppriv_addr[NR_SYS_PROCS];	/* direct slot pointers */

/* Make sure the system can boot. The following sanity check verifies that
 * the system privileges table is large enough for the number of processes
 * in the boot image. 
 */
#सचे (NR_BOOT_PROCS > NR_SYS_PROCS)
#error NR_SYS_PROCS must be larger than NR_BOOT_PROCS
#endif

#endif /* PRIV_H */
