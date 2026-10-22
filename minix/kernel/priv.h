#اگر_غیر_تعریف_شدہ PRIV_H
#تعریف_کرو PRIV_H

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
#شامل <minix/const.h>
#شامل <minix/priv.h>
#شامل "kernel/const.h"
#شامل "kernel/type.h"
#شامل "kernel/ipc_filter.h"

ساخت priv {
  proc_nr_t s_proc_nr;		/* number of associated process */
  sys_id_t s_id;		/* index of this system structure */
  مختصر s_flags;		/* PREEMTIBLE, BILLABLE, etc. */
  صحیح s_init_flags;             /* initialization flags given to the process. */

  /* Asynchronous sends */
  vir_بائٹیں s_asyntab;		/* addr. of table in process' address space */
  size_t s_asynsize;		/* number of elements in table. 0 when not in
				 * use
				 */
  endpoint_t s_asynendpoint;    /* the endpoint the asyn table belongs to. */

  مختصر s_trap_نقاب;		/* allowed system call traps */
  sys_map_t s_ipc_مقصد;		/* allowed destination processes */

  /* allowed kernel calls */
  bitchunk_t s_k_call_نقاب[SYS_CALL_نقاب_سائز];

  endpoint_t s_sig_mgr;		/* signal manager for system signals */
  endpoint_t s_bak_sig_mgr;	/* backup signal manager for system signals */
  sys_map_t s_notify_pending;  	/* bit map with pending notifications */
  sys_map_t s_asyn_pending;	/* bit map with pending asyn messages */
  irq_id_t s_int_pending;	/* pending hardware interrupts */
  sigset_t s_sig_pending;	/* pending signals */
  ipc_filter_t *s_ipcf;         /* ipc filter (NULL when no filter is set) */

  minix_timer_t s_alarm_timer;	/* synchronous alarm timer */
  reg_t *s_ڈھیر_guard;		/* stack guard word for kernel tasks */

  حرف s_diag_sig;		/* send a SIGKMESS when diagnostics arrive? */

  صحیح s_nr_io_range;		/* allowed I/O ports */
  ساخت io_range s_io_tab[NR_IO_RANGE];

  صحیح s_nr_mem_range;		/* allowed memory ranges */
  ساخت minix_mem_range s_mem_tab[NR_MEM_RANGE];

  صحیح s_nr_irq;			/* allowed IRQ lines */
  صحیح s_irq_tab[NR_IRQ];
  vir_بائٹیں s_grant_جدول;	/* grant table address of process, or 0 */
  صحیح s_grant_entries;		/* no. of entries, or 0 */
  endpoint_t s_grant_endpoint;  /* the endpoint the grant table belongs to */
  vir_بائٹیں s_state_جدول;	/* state table address of process, or 0 */
  صحیح s_state_entries;		/* no. of entries, or 0 */
};

/* Guard word for task stacks. */
#تعریف_کرو ڈھیر_GUARD	((reg_t) (حجم(reg_t) == 2 ? 0xBEEF : 0xDEADBEEF))

/* Magic system structure table addresses. */
#تعریف_کرو BEG_PRIV_ADDR              (&priv[0])
#تعریف_کرو انتہا_PRIV_ADDR              (&priv[NR_SYS_PROCS])
#تعریف_کرو BEG_STATIC_PRIV_ADDR       BEG_PRIV_ADDR
#تعریف_کرو انتہا_STATIC_PRIV_ADDR       (BEG_STATIC_PRIV_ADDR + NR_STATIC_PRIV_IDS)
#تعریف_کرو BEG_DYN_PRIV_ADDR          انتہا_STATIC_PRIV_ADDR
#تعریف_کرو انتہا_DYN_PRIV_ADDR          انتہا_PRIV_ADDR

#تعریف_کرو priv_addr(i)      (ppriv_addr)[(i)]
#تعریف_کرو priv_id(rp)	  ((rp)->p_priv->s_id)
#تعریف_کرو priv(rp)	  ((rp)->p_priv)

#تعریف_کرو id_مقصد_nr(id)	priv_addr(id)->s_proc_nr
#تعریف_کرو nr_مقصد_id(nr)    priv(proc_addr(nr))->s_id

#تعریف_کرو may_send_مقصد(rp, nr) (get_sys_ثنائی_ہندسہ(priv(rp)->s_ipc_مقصد, nr_مقصد_id(nr)))
#تعریف_کرو may_asynsend_مقصد(rp, nr) (may_send_مقصد(rp, nr) || (rp)->p_nr == nr)

/* The system structures table and pointers to individual table slots. The 
 * pointers allow faster access because now a process entry can be found by 
 * indexing the psys_addr array, while accessing an element i requires a 
 * multiplication with sizeof(struct sys) to determine the address. 
 */
EXTERN ساخت priv priv[NR_SYS_PROCS];		/* system properties table */
EXTERN ساخت priv *ppriv_addr[NR_SYS_PROCS];	/* direct slot pointers */

/* Make sure the system can boot. The following sanity check verifies that
 * the system privileges table is large enough for the number of processes
 * in the boot image. 
 */
#اگر (NR_BOOT_PROCS > NR_SYS_PROCS)
#غلطی NR_SYS_PROCS must be larger than NR_BOOT_PROCS
#اگر_ختم

#اگر_ختم /* PRIV_H */
