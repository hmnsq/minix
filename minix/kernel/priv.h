#که_نه_تعریف_شوی PRIV_H
#تعریف_کړه PRIV_H

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
#ضمن <minix/const.h>
#ضمن <minix/priv.h>
#ضمن "kernel/const.h"
#ضمن "kernel/type.h"
#ضمن "kernel/ipc_filter.h"

بنية priv {
  proc_nr_t s_proc_nr;		/* number of associated process */
  sys_id_t s_id;		/* index of this system structure */
  لنډ s_flags;		/* PREEMTIBLE, BILLABLE, etc. */
  صحيح s_init_flags;             /* initialization flags given to the process. */

  /* Asynchronous sends */
  vir_بایټونه s_asyntab;		/* addr. of table in process' address space */
  size_t s_asynsize;		/* number of elements in table. 0 when not in
				 * use
				 */
  endpoint_t s_asynendpoint;    /* the endpoint the asyn table belongs to. */

  لنډ s_trap_ماسک;		/* allowed system call traps */
  sys_map_t s_ipc_منزل;		/* allowed destination processes */

  /* allowed kernel calls */
  bitchunk_t s_k_call_ماسک[SYS_CALL_ماسک_اندازه];

  endpoint_t s_sig_mgr;		/* signal manager for system signals */
  endpoint_t s_bak_sig_mgr;	/* backup signal manager for system signals */
  sys_map_t s_notify_pending;  	/* bit map with pending notifications */
  sys_map_t s_asyn_pending;	/* bit map with pending asyn messages */
  irq_id_t s_int_pending;	/* pending hardware interrupts */
  sigset_t s_sig_pending;	/* pending signals */
  ipc_filter_t *s_ipcf;         /* ipc filter (NULL when no filter is set) */

  minix_timer_t s_alarm_timer;	/* synchronous alarm timer */
  reg_t *s_ډېری_guard;		/* stack guard word for kernel tasks */

  حرف s_diag_sig;		/* send a SIGKMESS when diagnostics arrive? */

  صحيح s_nr_io_range;		/* allowed I/O ports */
  بنية io_range s_io_tab[NR_IO_RANGE];

  صحيح s_nr_mem_range;		/* allowed memory ranges */
  بنية minix_mem_range s_mem_tab[NR_MEM_RANGE];

  صحيح s_nr_irq;			/* allowed IRQ lines */
  صحيح s_irq_tab[NR_IRQ];
  vir_بایټونه s_grant_جدول;	/* grant table address of process, or 0 */
  صحيح s_grant_entries;		/* no. of entries, or 0 */
  endpoint_t s_grant_endpoint;  /* the endpoint the grant table belongs to */
  vir_بایټونه s_state_جدول;	/* state table address of process, or 0 */
  صحيح s_state_entries;		/* no. of entries, or 0 */
};

/* Guard word for task stacks. */
#تعریف_کړه ډېری_GUARD	((reg_t) (حجم(reg_t) == 2 ? 0xBEEF : 0xDEADBEEF))

/* Magic system structure table addresses. */
#تعریف_کړه BEG_PRIV_ADDR              (&priv[0])
#تعریف_کړه پای_ټکی_PRIV_ADDR              (&priv[NR_SYS_PROCS])
#تعریف_کړه BEG_STATIC_PRIV_ADDR       BEG_PRIV_ADDR
#تعریف_کړه پای_ټکی_STATIC_PRIV_ADDR       (BEG_STATIC_PRIV_ADDR + NR_STATIC_PRIV_IDS)
#تعریف_کړه BEG_DYN_PRIV_ADDR          پای_ټکی_STATIC_PRIV_ADDR
#تعریف_کړه پای_ټکی_DYN_PRIV_ADDR          پای_ټکی_PRIV_ADDR

#تعریف_کړه priv_addr(i)      (ppriv_addr)[(i)]
#تعریف_کړه priv_id(rp)	  ((rp)->p_priv->s_id)
#تعریف_کړه priv(rp)	  ((rp)->p_priv)

#تعریف_کړه id_منزل_nr(id)	priv_addr(id)->s_proc_nr
#تعریف_کړه nr_منزل_id(nr)    priv(proc_addr(nr))->s_id

#تعریف_کړه may_send_منزل(rp, nr) (get_sys_بټ(priv(rp)->s_ipc_منزل, nr_منزل_id(nr)))
#تعریف_کړه may_asynsend_منزل(rp, nr) (may_send_منزل(rp, nr) || (rp)->p_nr == nr)

/* The system structures table and pointers to individual table slots. The 
 * pointers allow faster access because now a process entry can be found by 
 * indexing the psys_addr array, while accessing an element i requires a 
 * multiplication with sizeof(struct sys) to determine the address. 
 */
EXTERN بنية priv priv[NR_SYS_PROCS];		/* system properties table */
EXTERN بنية priv *ppriv_addr[NR_SYS_PROCS];	/* direct slot pointers */

/* Make sure the system can boot. The following sanity check verifies that
 * the system privileges table is large enough for the number of processes
 * in the boot image. 
 */
#إذا (NR_BOOT_PROCS > NR_SYS_PROCS)
#تېروتنه NR_SYS_PROCS must be larger than NR_BOOT_PROCS
#د_که_پای

#د_که_پای /* PRIV_H */
