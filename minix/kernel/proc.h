#جيڪڏهن_وصف_نه_ٿيل PROC_H
#وصف_ڪريو PROC_H

#ضمن <minix/const.h>
#ضمن <sys/cdefs.h>

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

/* Here is the declaration of the process table.  It contains all process
 * data, including registers, flags, scheduling priority, memory map, 
 * accounting, message passing (IPC) information, and so on. 
 *
 * Many assembly code routines reference fields in it.  The offsets to these
 * fields are defined in the assembler include file sconst.h.  When changing
 * struct proc, be sure to change sconst.h to match.
 */
#ضمن <minix/com.h>
#ضمن <minix/portio.h>
#ضمن "const.h"
#ضمن "priv.h"

بنية proc {
  بنية stackframe_s p_reg;	/* process' registers saved in stack frame */
  بنية segframe p_seg;	/* segment descriptors */
  proc_nr_t p_nr;		/* number of this process (for fast access) */
  بنية priv *p_priv;		/* system privileges structure */
  متطاير u32_t p_rts_flags;	/* process is runnable only if zero */
  متطاير u32_t p_misc_flags;	/* flags that do not suspend the process */

  حرف p_ترجيح;		/* current process priority */
  u64_t p_cpu_وقت_کاٻو;	/* time left to use the cpu */
  بنانشان p_وقتي_حصو_سائيز_ms;	/* assigned time quantum in ms
				   FIXME remove this */
  بنية proc *p_scheduler;	/* who should get out of quantum msg */
  بنانشان p_cpu;		/* what CPU is the process running on */
#جيڪڏهن_وصف_ٿيل CONFIG_SMP
  bitchunk_t p_cpu_نقاب[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* what CPUs is the
							    process allowed to
							    run on */
  bitchunk_t p_stale_tlb[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* On which cpu are
				possibly stale entries from this process and has
				to be fresed the next kernel touches this
				processes memory
				 */
#جيڪڏهن_پورو

  /* Accounting statistics that get passed to the process' scheduler */
  بنية {
	u64_t enter_قطار;	/* time when enqueued (cycles) */
	u64_t وقت_اندريون_قطار;	/* time spent in queue */
	بنانشان ڊگهو dequeues;
	بنانشان ڊگهو ipc_sync;
	بنانشان ڊگهو ipc_async;
	بنانشان ڊگهو preempted;
  } p_accounting;

  clock_t p_dequeued;		/* uptime at which process was last dequeued */

  clock_t p_user_وقت;		/* user time in ticks */
  clock_t p_sys_وقت;		/* sys time in ticks */

  clock_t p_virt_کاٻو;		/* number of ticks left on virtual timer */
  clock_t p_prof_کاٻو;		/* number of ticks left on profile timer */

  u64_t p_چڪرن;		/* how many cycles did the process use */
  u64_t p_kcall_چڪرن;		/* kernel cycles caused by this proc (kcall) */
  u64_t p_kipc_چڪرن;		/* cycles caused by this proc (ipc) */

  u64_t p_tick_چڪرن;		/* cycles accumulated for up to a clock tick */
  بنية cpuavg p_cpuavg;	/* running CPU average, for ps(1) */

  بنية proc *p_nextready;	/* pointer to next ready process */
  بنية proc *p_caller_q;	/* head of list of procs wishing to send */
  بنية proc *p_q_link;	/* link to next proc wishing to send */
  endpoint_t p_getfrom_e;	/* from whom does process want to receive? */
  endpoint_t p_sendto_e;	/* to whom does process want to send? */

  sigset_t p_pending;		/* bit map for pending kernel signals */

  حرف p_name[PROC_NAME_LEN];	/* name of the process, including \0 */

  endpoint_t p_endpoint;	/* endpoint number, generation-aware */

  پيغام p_sendmsg;		/* Message from this process if SENDING */
  پيغام p_delivermsg;		/* Message for this process if MF_DELIVERMSG */
  vir_بائيٽون p_delivermsg_vir;	/* Virtual addr this proc wants message at */

  /* If handler functions detect a process wants to do something with
   * memory that isn't present, VM has to fix it. Until it has asked
   * what needs to be done and fixed it, save necessary state here.
   *
   * The requester gets a copy of its request message in reqmsg and gets
   * VMREQUEST set.
   */
  بنية {
	بنية proc	*nextrestart;	/* next in vmrestart chain */
	بنية proc	*nextrequestor;	/* next in vmrequest chain */
#وصف_ڪريو VMSTYPE_SYS_NONE	0
#وصف_ڪريو VMSTYPE_KERNELCALL	1
#وصف_ڪريو VMSTYPE_DELIVERMSG	2
#وصف_ڪريو VMSTYPE_MAP		3

	صحيح		type;		/* suspended operation */
	اتحاد ixfer_saved{
		/* VMSTYPE_SYS_MESSAGE */
		پيغام		reqmsg;	/* suspended request message */
	} saved;

	/* Parameters of request to VM */
	صحيح		req_type;
	endpoint_t	هدف;
	اتحاد ixfer_params{
		بنية {
			vir_بائيٽون 	شروعات, ڊيگهائي;	/* memory range */
			u8_t		writeflag;	/* nonzero for write access */
		} check;
	} params;
	/* VM result when available */
	صحيح		vmresult;

	/* If the suspended operation is a sys_call, its details are
	 * stored here.
	 */
  } p_vmrequest;

  صحيح p_مليو;	/* consistency checking variables */
  صحيح p_magic;		/* check validity of proc pointers */

  /* if MF_SC_DEFER is set, this struct is valid and contains the
   * do_ipc() arguments that are still to be executed
   */
  بنية { reg_t r1, r2, r3; } p_defer;

#إذا DEBUG_TRACE
  صحيح p_schedules;
#جيڪڏهن_پورو
};

#جيڪڏهن_پورو /* __ASSEMBLY__ */

/* Bits for the runtime flags. A process is runnable iff p_rts_flags == 0. */
#وصف_ڪريو RTS_جڳهه_FREE	0x01	/* process slot is free */
#وصف_ڪريو RTS_PROC_STOP	0x02	/* process has been stopped */
#وصف_ڪريو RTS_SENDING	0x04	/* process blocked trying to send */
#وصف_ڪريو RTS_RECEIVING	0x08	/* process blocked trying to receive */
#وصف_ڪريو RTS_SIGNALED	0x10	/* set when new kernel signal arrives */
#وصف_ڪريو RTS_SIG_PENDING	0x20	/* unready while signal being processed */
#وصف_ڪريو RTS_P_STOP	0x40	/* set when process is being traced */
#وصف_ڪريو RTS_NO_PRIV	0x80	/* keep forked system process from running */
#وصف_ڪريو RTS_NO_ENDPOINT	0x100	/* process cannot send or receive messages */
#وصف_ڪريو RTS_VMINHIBIT	0x200	/* not scheduled until pagetable set by VM */
#وصف_ڪريو RTS_PAGEFAULT	0x400	/* process has unhandled pagefault */
#وصف_ڪريو RTS_VMREQUEST	0x800	/* originator of vm memory request */
#وصف_ڪريو RTS_VMREQTARGET	0x1000	/* target of vm memory request */
#وصف_ڪريو RTS_PREEMPTED	0x4000	/* this process was preempted by a higher
				   priority process and we should pick a new one
				   to run. Processes with this flag should be
				   returned to the front of their current
				   priority queue if they are still runnable
				   before we pick a new one
				 */
#وصف_ڪريو RTS_NO_وقتي_حصو	0x8000	/* process ran out of its quantum and we should
				   pick a new one. Process was dequeued and
				   should be enqueued at the end of some run
				   queue again */
#وصف_ڪريو RTS_BOOTINHIBIT	0x10000	/* not ready until VM has made it */

/* A process is runnable iff p_rts_flags == 0. */
#وصف_ڪريو rts_f_is_runnable(flg)	((flg) == 0)
#وصف_ڪريو proc_is_runnable(p)	(rts_f_is_runnable((p)->p_rts_flags))

#وصف_ڪريو proc_is_preempted(p)	((p)->p_rts_flags & RTS_PREEMPTED)
#وصف_ڪريو proc_no_وقتي_حصو(p)	((p)->p_rts_flags & RTS_NO_وقتي_حصو)
#وصف_ڪريو proc_ptr_ok(p)		((p)->p_magic == PMAGIC)
#وصف_ڪريو proc_استعمال_ٿيل_fpu(p)	((p)->p_misc_flags & (MF_FPU_INITIALIZED))

/* test whether the process is scheduled by the kernel's default policy  */
#وصف_ڪريو proc_kernel_scheduler(p)	((p)->p_scheduler == عدم || \
					(p)->p_scheduler == (p))

/* Macro to return: on which process is a certain process blocked?
 * return endpoint number (can be ANY) or NONE. It's important to
 * check RTS_SENDING first, and then RTS_RECEIVING, as they could
 * both be on (if a ipc_sendrec() blocks on sending), and p_getfrom_e
 * could be nonsense even though RTS_RECEIVING is on.
 */
#وصف_ڪريو P_BLOCKEDON(p)							\
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

#وصف_ڪريو RTS_ISSET(rp, f) (((rp)->p_rts_flags & (f)) == (f))


/* Set flag and dequeue if the process was runnable. */
#وصف_ڪريو RTS_سيٽ(rp, f)							\
	افعل {								\
		ثابت صحيح rts = (rp)->p_rts_flags;			\
		(rp)->p_rts_flags |= (f);				\
		إذا(rts_f_is_runnable(rts) && !proc_is_runnable(rp)) {	\
			قطار_مان_ڪڍو(rp);					\
		}							\
	} طالما(0)

/* Clear flag and enqueue if the process was not runnable but is now. */
#وصف_ڪريو RTS_UNSET(rp, f) 						\
	افعل {								\
		صحيح rts;						\
		rts = (rp)->p_rts_flags;				\
		(rp)->p_rts_flags &= ~(f);				\
		إذا(!rts_f_is_runnable(rts) && proc_is_runnable(rp)) {	\
			قطار_اندر_وجهو(rp);					\
		}							\
	} طالما(0)

/* Set flags to this value. */
#وصف_ڪريو RTS_SETFLAGS(rp, f)					\
	افعل {								\
		إذا(proc_is_runnable(rp) && (f)) { قطار_مان_ڪڍو(rp); }		\
		(rp)->p_rts_flags = (f);				\
	} طالما(0)

/* Misc flags */
#وصف_ڪريو MF_جواب_PEND	0x001	/* reply to IPC_REQUEST is pending */
#وصف_ڪريو MF_VIRT_TIMER	0x002	/* process-virtual timer is running */
#وصف_ڪريو MF_PROF_TIMER	0x004	/* process-virtual profile timer is running */
#وصف_ڪريو MF_KCALL_RESUME 0x008	/* processing a kernel call was interrupted,
				   most likely because we need VM to resolve a
				   problem or a long running copy was preempted.
				   We need to resume the kernel call execution
				   now
				 */
#وصف_ڪريو MF_DELIVERMSG	0x040	/* Copy message for him before running */
#وصف_ڪريو MF_SIG_DELAY	0x080	/* Send signal when no longer sending */
#وصف_ڪريو MF_SC_سرگرم	0x100	/* Syscall tracing: in a system call now */
#وصف_ڪريو MF_SC_DEFER	0x200	/* Syscall tracing: deferred system call */
#وصف_ڪريو MF_SC_TRACE	0x400	/* Syscall tracing: trigger syscall events */
#وصف_ڪريو MF_FPU_INITIALIZED	0x1000  /* process already used math, so fpu
					 * regs are significant (initialized)*/
#وصف_ڪريو MF_SENDING_FROM_KERNEL	0x2000 /* message of this process is from kernel */
#وصف_ڪريو MF_CONTEXT_سيٽ	0x4000 /* don't touch context */
#وصف_ڪريو MF_SPROF_ڏٺل	0x8000 /* profiling has seen this process */
#وصف_ڪريو MF_FLUSH_TLB	0x10000	/* if set, TLB must be flushed before letting
				   this process run again. Currently it only
				   applies to SMP */
#وصف_ڪريو MF_SENDA_VM_ناڪامي 0x20000 /* set if a processes wanted to receive an asyn
				    message from this sender but could not
				    because of VM modifying the sender's address
				    space*/
#وصف_ڪريو MF_قدم		 0x40000 /* Single-step process */
#وصف_ڪريو MF_MSGFAILED	 0x80000
#وصف_ڪريو MF_NICED	0x100000 /* user has lowered max process priority */

/* Magic process table addresses. */
#وصف_ڪريو BEG_PROC_ADDR (&proc[0])
#وصف_ڪريو BEG_USER_ADDR (&proc[NR_TASKS])
#وصف_ڪريو پڇاڙي_PROC_ADDR (&proc[NR_TASKS + NR_PROCS])

#وصف_ڪريو proc_addr(n)      (&(proc[NR_TASKS + (n)]))
#وصف_ڪريو proc_nr(p) 	  ((p)->p_nr)

#وصف_ڪريو isokprocn(n)      ((بنانشان) ((n) + NR_TASKS) < NR_PROCS + NR_TASKS)
#وصف_ڪريو isemptyn(n)       isemptyp(proc_addr(n)) 
#وصف_ڪريو isemptyp(p)       ((p)->p_rts_flags == RTS_جڳهه_FREE)
#وصف_ڪريو iskernelp(p)	  ((p) < BEG_USER_ADDR)
#وصف_ڪريو iskerneln(n)	  ((n) < 0)
#وصف_ڪريو isuserp(p)        isusern((p) >= BEG_USER_ADDR)
#وصف_ڪريو isusern(n)        ((n) >= 0)
#وصف_ڪريو isrootsysn(n)	  ((n) == پاڙ_SYS_PROC_NR)

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

EXTERN بنية proc proc[NR_TASKS + NR_PROCS];	/* process table */

صحيح mini_send(بنية proc *caller_ptr, endpoint_t dst_e, پيغام *m_ptr,
	صحيح flags);

#جيڪڏهن_پورو /* __ASSEMBLY__ */

#جيڪڏهن_پورو /* PROC_H */
