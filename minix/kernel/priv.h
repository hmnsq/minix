#నిర్వచితం_కాకపోతే PRIV_H
#నిర్వచించు PRIV_H

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
#అన్తర్భావ <minix/const.h>
#అన్తర్భావ <minix/priv.h>
#అన్తర్భావ "kernel/const.h"
#అన్తర్భావ "kernel/type.h"
#అన్తర్భావ "kernel/ipc_filter.h"

నిర్మాణం priv {
  proc_nr_t s_proc_nr;		/* number of associated process */
  sys_id_t s_id;		/* index of this system structure */
  పొట్టి s_flags;		/* PREEMTIBLE, BILLABLE, etc. */
  పూర్ణాంకం s_init_flags;             /* initialization flags given to the process. */

  /* Asynchronous sends */
  vir_బైట్లు s_asyntab;		/* addr. of table in process' address space */
  size_t s_asynsize;		/* number of elements in table. 0 when not in
				 * use
				 */
  endpoint_t s_asynendpoint;    /* the endpoint the asyn table belongs to. */

  పొట్టి s_trap_ముసుగు;		/* allowed system call traps */
  sys_map_t s_ipc_గమ్యం;		/* allowed destination processes */

  /* allowed kernel calls */
  bitchunk_t s_k_call_ముసుగు[SYS_CALL_ముసుగు_కొలత];

  endpoint_t s_sig_mgr;		/* signal manager for system signals */
  endpoint_t s_bak_sig_mgr;	/* backup signal manager for system signals */
  sys_map_t s_notify_pending;  	/* bit map with pending notifications */
  sys_map_t s_asyn_pending;	/* bit map with pending asyn messages */
  irq_id_t s_int_pending;	/* pending hardware interrupts */
  sigset_t s_sig_pending;	/* pending signals */
  ipc_filter_t *s_ipcf;         /* ipc filter (NULL when no filter is set) */

  minix_timer_t s_alarm_timer;	/* synchronous alarm timer */
  reg_t *s_దొంతర_guard;		/* stack guard word for kernel tasks */

  అక్షరం s_diag_sig;		/* send a SIGKMESS when diagnostics arrive? */

  పూర్ణాంకం s_nr_io_range;		/* allowed I/O ports */
  నిర్మాణం io_range s_io_tab[NR_IO_RANGE];

  పూర్ణాంకం s_nr_mem_range;		/* allowed memory ranges */
  నిర్మాణం minix_mem_range s_mem_tab[NR_MEM_RANGE];

  పూర్ణాంకం s_nr_irq;			/* allowed IRQ lines */
  పూర్ణాంకం s_irq_tab[NR_IRQ];
  vir_బైట్లు s_grant_పట్టిక;	/* grant table address of process, or 0 */
  పూర్ణాంకం s_grant_entries;		/* no. of entries, or 0 */
  endpoint_t s_grant_endpoint;  /* the endpoint the grant table belongs to */
  vir_బైట్లు s_state_పట్టిక;	/* state table address of process, or 0 */
  పూర్ణాంకం s_state_entries;		/* no. of entries, or 0 */
};

/* Guard word for task stacks. */
#నిర్వచించు దొంతర_GUARD	((reg_t) (పరిమాణం(reg_t) == 2 ? 0xBEEF : 0xDEADBEEF))

/* Magic system structure table addresses. */
#నిర్వచించు BEG_PRIV_ADDR              (&priv[0])
#నిర్వచించు చివర_PRIV_ADDR              (&priv[NR_SYS_PROCS])
#నిర్వచించు BEG_STATIC_PRIV_ADDR       BEG_PRIV_ADDR
#నిర్వచించు చివర_STATIC_PRIV_ADDR       (BEG_STATIC_PRIV_ADDR + NR_STATIC_PRIV_IDS)
#నిర్వచించు BEG_DYN_PRIV_ADDR          చివర_STATIC_PRIV_ADDR
#నిర్వచించు చివర_DYN_PRIV_ADDR          చివర_PRIV_ADDR

#నిర్వచించు priv_addr(i)      (ppriv_addr)[(i)]
#నిర్వచించు priv_id(rp)	  ((rp)->p_priv->s_id)
#నిర్వచించు priv(rp)	  ((rp)->p_priv)

#నిర్వచించు id_గమ్యం_nr(id)	priv_addr(id)->s_proc_nr
#నిర్వచించు nr_గమ్యం_id(nr)    priv(proc_addr(nr))->s_id

#నిర్వచించు may_send_గమ్యం(rp, nr) (get_sys_బిట్టు(priv(rp)->s_ipc_గమ్యం, nr_గమ్యం_id(nr)))
#నిర్వచించు may_asynsend_గమ్యం(rp, nr) (may_send_గమ్యం(rp, nr) || (rp)->p_nr == nr)

/* The system structures table and pointers to individual table slots. The 
 * pointers allow faster access because now a process entry can be found by 
 * indexing the psys_addr array, while accessing an element i requires a 
 * multiplication with sizeof(struct sys) to determine the address. 
 */
EXTERN నిర్మాణం priv priv[NR_SYS_PROCS];		/* system properties table */
EXTERN నిర్మాణం priv *ppriv_addr[NR_SYS_PROCS];	/* direct slot pointers */

/* Make sure the system can boot. The following sanity check verifies that
 * the system privileges table is large enough for the number of processes
 * in the boot image. 
 */
#ఒకవేళ (NR_BOOT_PROCS > NR_SYS_PROCS)
#దోషం NR_SYS_PROCS must be larger than NR_BOOT_PROCS
#అయితే_ముగింపు

#అయితే_ముగింపు /* PRIV_H */
