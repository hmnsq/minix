#اگر_غیر_تعریف_شدہ PROC_H
#تعریف_کرو PROC_H

#شامل <minix/const.h>
#شامل <sys/cdefs.h>

#اگر_غیر_تعریف_شدہ __ASSEMBLY__

/* Here is the declaration of the process table.  It contains all process
 * data, including registers, flags, scheduling priority, memory map, 
 * accounting, message passing (IPC) information, and so on. 
 *
 * Many assembly code routines reference fields in it.  The offsets to these
 * fields are defined in the assembler include file sconst.h.  When changing
 * struct proc, be sure to change sconst.h to match.
 */
#شامل <minix/com.h>
#شامل <minix/portio.h>
#شامل "const.h"
#شامل "priv.h"

ساخت proc {
  ساخت stackframe_s p_reg;	/* process' registers saved in stack frame */
  ساخت segframe p_seg;	/* segment descriptors */
  proc_nr_t p_nr;		/* number of this process (for fast access) */
  ساخت priv *p_priv;		/* system privileges structure */
  متلون u32_t p_rts_flags;	/* process is runnable only if zero */
  متلون u32_t p_misc_flags;	/* flags that do not suspend the process */

  حرف p_ترجیح;		/* current process priority */
  u64_t p_cpu_وقت_بایاں;	/* time left to use the cpu */
  غیرعلامتی p_وقتی_حصہ_سائز_ms;	/* assigned time quantum in ms
				   FIXME remove this */
  ساخت proc *p_scheduler;	/* who should get out of quantum msg */
  غیرعلامتی p_cpu;		/* what CPU is the process running on */
#اگر_تعریف_شدہ CONFIG_SMP
  bitchunk_t p_cpu_نقاب[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* what CPUs is the
							    process allowed to
							    run on */
  bitchunk_t p_stale_tlb[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* On which cpu are
				possibly stale entries from this process and has
				to be fresed the next kernel touches this
				processes memory
				 */
#اگر_ختم

  /* Accounting statistics that get passed to the process' scheduler */
  ساخت {
	u64_t enter_قطار;	/* time when enqueued (cycles) */
	u64_t وقت_اندر_قطار;	/* time spent in queue */
	غیرعلامتی طویل dequeues;
	غیرعلامتی طویل ipc_sync;
	غیرعلامتی طویل ipc_async;
	غیرعلامتی طویل preempted;
  } p_accounting;

  clock_t p_dequeued;		/* uptime at which process was last dequeued */

  clock_t p_user_وقت;		/* user time in ticks */
  clock_t p_sys_وقت;		/* sys time in ticks */

  clock_t p_virt_بایاں;		/* number of ticks left on virtual timer */
  clock_t p_prof_بایاں;		/* number of ticks left on profile timer */

  u64_t p_ادوار;		/* how many cycles did the process use */
  u64_t p_kcall_ادوار;		/* kernel cycles caused by this proc (kcall) */
  u64_t p_kipc_ادوار;		/* cycles caused by this proc (ipc) */

  u64_t p_tick_ادوار;		/* cycles accumulated for up to a clock tick */
  ساخت cpuavg p_cpuavg;	/* running CPU average, for ps(1) */

  ساخت proc *p_nextready;	/* pointer to next ready process */
  ساخت proc *p_caller_q;	/* head of list of procs wishing to send */
  ساخت proc *p_q_link;	/* link to next proc wishing to send */
  endpoint_t p_getfrom_e;	/* from whom does process want to receive? */
  endpoint_t p_sendto_e;	/* to whom does process want to send? */

  sigset_t p_pending;		/* bit map for pending kernel signals */

  حرف p_name[PROC_NAME_LEN];	/* name of the process, including \0 */

  endpoint_t p_endpoint;	/* endpoint number, generation-aware */

  پیغام p_sendmsg;		/* Message from this process if SENDING */
  پیغام p_delivermsg;		/* Message for this process if MF_DELIVERMSG */
  vir_بائٹیں p_delivermsg_vir;	/* Virtual addr this proc wants message at */

  /* If handler functions detect a process wants to do something with
   * memory that isn't present, VM has to fix it. Until it has asked
   * what needs to be done and fixed it, save necessary state here.
   *
   * The requester gets a copy of its request message in reqmsg and gets
   * VMREQUEST set.
   */
  ساخت {
	ساخت proc	*nextrestart;	/* next in vmrestart chain */
	ساخت proc	*nextrequestor;	/* next in vmrequest chain */
#تعریف_کرو VMSTYPE_SYS_NONE	0
#تعریف_کرو VMSTYPE_KERNELCALL	1
#تعریف_کرو VMSTYPE_DELIVERMSG	2
#تعریف_کرو VMSTYPE_MAP		3

	صحیح		type;		/* suspended operation */
	اتحاد ixfer_saved{
		/* VMSTYPE_SYS_MESSAGE */
		پیغام		reqmsg;	/* suspended request message */
	} saved;

	/* Parameters of request to VM */
	صحیح		req_type;
	endpoint_t	ہدف;
	اتحاد ixfer_params{
		ساخت {
			vir_بائٹیں 	آغاز, لمبائی;	/* memory range */
			u8_t		writeflag;	/* nonzero for write access */
		} check;
	} params;
	/* VM result when available */
	صحیح		vmresult;

	/* If the suspended operation is a sys_call, its details are
	 * stored here.
	 */
  } p_vmrequest;

  صحیح p_ملا;	/* consistency checking variables */
  صحیح p_magic;		/* check validity of proc pointers */

  /* if MF_SC_DEFER is set, this struct is valid and contains the
   * do_ipc() arguments that are still to be executed
   */
  ساخت { reg_t r1, r2, r3; } p_defer;

#اگر DEBUG_TRACE
  صحیح p_schedules;
#اگر_ختم
};

#اگر_ختم /* __ASSEMBLY__ */

/* Bits for the runtime flags. A process is runnable iff p_rts_flags == 0. */
#تعریف_کرو RTS_خانہ_FREE	0x01	/* process slot is free */
#تعریف_کرو RTS_PROC_STOP	0x02	/* process has been stopped */
#تعریف_کرو RTS_SENDING	0x04	/* process blocked trying to send */
#تعریف_کرو RTS_RECEIVING	0x08	/* process blocked trying to receive */
#تعریف_کرو RTS_SIGNALED	0x10	/* set when new kernel signal arrives */
#تعریف_کرو RTS_SIG_PENDING	0x20	/* unready while signal being processed */
#تعریف_کرو RTS_P_STOP	0x40	/* set when process is being traced */
#تعریف_کرو RTS_NO_PRIV	0x80	/* keep forked system process from running */
#تعریف_کرو RTS_NO_ENDPOINT	0x100	/* process cannot send or receive messages */
#تعریف_کرو RTS_VMINHIBIT	0x200	/* not scheduled until pagetable set by VM */
#تعریف_کرو RTS_PAGEFAULT	0x400	/* process has unhandled pagefault */
#تعریف_کرو RTS_VMREQUEST	0x800	/* originator of vm memory request */
#تعریف_کرو RTS_VMREQTARGET	0x1000	/* target of vm memory request */
#تعریف_کرو RTS_PREEMPTED	0x4000	/* this process was preempted by a higher
				   priority process and we should pick a new one
				   to run. Processes with this flag should be
				   returned to the front of their current
				   priority queue if they are still runnable
				   before we pick a new one
				 */
#تعریف_کرو RTS_NO_وقتی_حصہ	0x8000	/* process ran out of its quantum and we should
				   pick a new one. Process was dequeued and
				   should be enqueued at the end of some run
				   queue again */
#تعریف_کرو RTS_BOOTINHIBIT	0x10000	/* not ready until VM has made it */

/* A process is runnable iff p_rts_flags == 0. */
#تعریف_کرو rts_f_is_runnable(flg)	((flg) == 0)
#تعریف_کرو proc_is_runnable(p)	(rts_f_is_runnable((p)->p_rts_flags))

#تعریف_کرو proc_is_preempted(p)	((p)->p_rts_flags & RTS_PREEMPTED)
#تعریف_کرو proc_no_وقتی_حصہ(p)	((p)->p_rts_flags & RTS_NO_وقتی_حصہ)
#تعریف_کرو proc_ptr_ok(p)		((p)->p_magic == PMAGIC)
#تعریف_کرو proc_مستعمل_fpu(p)	((p)->p_misc_flags & (MF_FPU_INITIALIZED))

/* test whether the process is scheduled by the kernel's default policy  */
#تعریف_کرو proc_kernel_scheduler(p)	((p)->p_scheduler == تہی || \
					(p)->p_scheduler == (p))

/* Macro to return: on which process is a certain process blocked?
 * return endpoint number (can be ANY) or NONE. It's important to
 * check RTS_SENDING first, and then RTS_RECEIVING, as they could
 * both be on (if a ipc_sendrec() blocks on sending), and p_getfrom_e
 * could be nonsense even though RTS_RECEIVING is on.
 */
#تعریف_کرو P_BLOCKEDON(p)							\
	(								\
		((p)->p_rts_flags & RTS_SENDING) ? 			\
		(p)->p_sendto_e : 					\
		(							\
			(						\
				((p)->p_rts_flags & RTS_RECEIVING) ?	\
				(p)->p_getfrom_e : 			\
				NONE					\
			) 						\
		)							\
	)

/* These runtime flags can be tested and manipulated by these macros. */

#تعریف_کرو RTS_ISSET(rp, f) (((rp)->p_rts_flags & (f)) == (f))


/* Set flag and dequeue if the process was runnable. */
#تعریف_کرو RTS_سیٹ(rp, f)							\
	کرو {								\
		ثابت صحیح rts = (rp)->p_rts_flags;			\
		(rp)->p_rts_flags |= (f);				\
		اگر(rts_f_is_runnable(rts) && !proc_is_runnable(rp)) {	\
			قطار_سے_نکالو(rp);					\
		}							\
	} جب(0)

/* Clear flag and enqueue if the process was not runnable but is now. */
#تعریف_کرو RTS_UNSET(rp, f) 						\
	کرو {								\
		صحیح rts;						\
		rts = (rp)->p_rts_flags;				\
		(rp)->p_rts_flags &= ~(f);				\
		اگر(!rts_f_is_runnable(rts) && proc_is_runnable(rp)) {	\
			قطار_میں_ڈالو(rp);					\
		}							\
	} جب(0)

/* Set flags to this value. */
#تعریف_کرو RTS_SETFLAGS(rp, f)					\
	کرو {								\
		اگر(proc_is_runnable(rp) && (f)) { قطار_سے_نکالو(rp); }		\
		(rp)->p_rts_flags = (f);				\
	} جب(0)

/* Misc flags */
#تعریف_کرو MF_جواب_PEND	0x001	/* reply to IPC_REQUEST is pending */
#تعریف_کرو MF_VIRT_TIMER	0x002	/* process-virtual timer is running */
#تعریف_کرو MF_PROF_TIMER	0x004	/* process-virtual profile timer is running */
#تعریف_کرو MF_KCALL_RESUME 0x008	/* processing a kernel call was interrupted,
				   most likely because we need VM to resolve a
				   problem or a long running copy was preempted.
				   We need to resume the kernel call execution
				   now
				 */
#تعریف_کرو MF_DELIVERMSG	0x040	/* Copy message for him before running */
#تعریف_کرو MF_SIG_DELAY	0x080	/* Send signal when no longer sending */
#تعریف_کرو MF_SC_فعال	0x100	/* Syscall tracing: in a system call now */
#تعریف_کرو MF_SC_DEFER	0x200	/* Syscall tracing: deferred system call */
#تعریف_کرو MF_SC_TRACE	0x400	/* Syscall tracing: trigger syscall events */
#تعریف_کرو MF_FPU_INITIALIZED	0x1000  /* process already used math, so fpu
					 * regs are significant (initialized)*/
#تعریف_کرو MF_SENDING_FROM_KERNEL	0x2000 /* message of this process is from kernel */
#تعریف_کرو MF_CONTEXT_سیٹ	0x4000 /* don't touch context */
#تعریف_کرو MF_SPROF_نظر_آیا	0x8000 /* profiling has seen this process */
#تعریف_کرو MF_FLUSH_TLB	0x10000	/* if set, TLB must be flushed before letting
				   this process run again. Currently it only
				   applies to SMP */
#تعریف_کرو MF_SENDA_VM_ناکامی 0x20000 /* set if a processes wanted to receive an asyn
				    message from this sender but could not
				    because of VM modifying the sender's address
				    space*/
#تعریف_کرو MF_قدم		 0x40000 /* Single-step process */
#تعریف_کرو MF_MSGFAILED	 0x80000
#تعریف_کرو MF_NICED	0x100000 /* user has lowered max process priority */

/* Magic process table addresses. */
#تعریف_کرو BEG_PROC_ADDR (&proc[0])
#تعریف_کرو BEG_USER_ADDR (&proc[NR_TASKS])
#تعریف_کرو انتہا_PROC_ADDR (&proc[NR_TASKS + NR_PROCS])

#تعریف_کرو proc_addr(n)      (&(proc[NR_TASKS + (n)]))
#تعریف_کرو proc_nr(p) 	  ((p)->p_nr)

#تعریف_کرو isokprocn(n)      ((غیرعلامتی) ((n) + NR_TASKS) < NR_PROCS + NR_TASKS)
#تعریف_کرو isemptyn(n)       isemptyp(proc_addr(n)) 
#تعریف_کرو isemptyp(p)       ((p)->p_rts_flags == RTS_خانہ_FREE)
#تعریف_کرو iskernelp(p)	  ((p) < BEG_USER_ADDR)
#تعریف_کرو iskerneln(n)	  ((n) < 0)
#تعریف_کرو isuserp(p)        isusern((p) >= BEG_USER_ADDR)
#تعریف_کرو isusern(n)        ((n) >= 0)
#تعریف_کرو isrootsysn(n)	  ((n) == جڑ_SYS_PROC_NR)

#اگر_غیر_تعریف_شدہ __ASSEMBLY__

EXTERN ساخت proc proc[NR_TASKS + NR_PROCS];	/* process table */

صحیح mini_send(ساخت proc *caller_ptr, endpoint_t dst_e, پیغام *m_ptr,
	صحیح flags);

#اگر_ختم /* __ASSEMBLY__ */

#اگر_ختم /* PROC_H */
