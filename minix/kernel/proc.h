#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROC_H

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/const.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/cdefs.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

/* Here is the declaration of the process table.  It contains all process
 * data, including registers, flags, scheduling priority, memory map, 
 * accounting, message passing (IPC) information, and so on. 
 *
 * Many assembly code routines reference fields in it.  The offsets to these
 * fields are defined in the assembler include file sconst.h.  When changing
 * struct proc, be sure to change sconst.h to match.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/com.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/portio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "const.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "priv.h"

ਬਣਤਰ proc {
  ਬਣਤਰ stackframe_s p_reg;	/* process' registers saved in stack frame */
  ਬਣਤਰ segframe p_seg;	/* segment descriptors */
  proc_nr_t p_nr;		/* number of this process (for fast access) */
  ਬਣਤਰ priv *p_priv;		/* system privileges structure */
  ਅਸਥਿਰ u32_t p_rts_flags;	/* process is runnable only if zero */
  ਅਸਥਿਰ u32_t p_misc_flags;	/* flags that do not suspend the process */

  ਅੱਖਰ p_ਤਰਜੀਹ;		/* current process priority */
  u64_t p_cpu_ਸਮਾਂ_ਖੱਬਾ;	/* time left to use the cpu */
  ਬਿਨਾਨਿਸ਼ਾਨੀ p_ਸਮਾਂ_ਹਿੱਸਾ_ਮਾਪ_ms;	/* assigned time quantum in ms
				   FIXME remove this */
  ਬਣਤਰ proc *p_scheduler;	/* who should get out of quantum msg */
  ਬਿਨਾਨਿਸ਼ਾਨੀ p_cpu;		/* what CPU is the process running on */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
  bitchunk_t p_cpu_ਨਕਾਬ[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* what CPUs is the
							    process allowed to
							    run on */
  bitchunk_t p_stale_tlb[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* On which cpu are
				possibly stale entries from this process and has
				to be fresed the next kernel touches this
				processes memory
				 */
#ਜੇ_ਅੰਤ

  /* Accounting statistics that get passed to the process' scheduler */
  ਬਣਤਰ {
	u64_t enter_ਕਤਾਰ;	/* time when enqueued (cycles) */
	u64_t ਸਮਾਂ_ਵਿੱਚ_ਕਤਾਰ;	/* time spent in queue */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ dequeues;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ipc_sync;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ipc_async;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ preempted;
  } p_accounting;

  clock_t p_dequeued;		/* uptime at which process was last dequeued */

  clock_t p_user_ਸਮਾਂ;		/* user time in ticks */
  clock_t p_sys_ਸਮਾਂ;		/* sys time in ticks */

  clock_t p_virt_ਖੱਬਾ;		/* number of ticks left on virtual timer */
  clock_t p_prof_ਖੱਬਾ;		/* number of ticks left on profile timer */

  u64_t p_ਚੱਕਰਾਂ;		/* how many cycles did the process use */
  u64_t p_kcall_ਚੱਕਰਾਂ;		/* kernel cycles caused by this proc (kcall) */
  u64_t p_kipc_ਚੱਕਰਾਂ;		/* cycles caused by this proc (ipc) */

  u64_t p_tick_ਚੱਕਰਾਂ;		/* cycles accumulated for up to a clock tick */
  ਬਣਤਰ cpuavg p_cpuavg;	/* running CPU average, for ps(1) */

  ਬਣਤਰ proc *p_nextready;	/* pointer to next ready process */
  ਬਣਤਰ proc *p_caller_q;	/* head of list of procs wishing to send */
  ਬਣਤਰ proc *p_q_link;	/* link to next proc wishing to send */
  endpoint_t p_getfrom_e;	/* from whom does process want to receive? */
  endpoint_t p_sendto_e;	/* to whom does process want to send? */

  sigset_t p_pending;		/* bit map for pending kernel signals */

  ਅੱਖਰ p_name[PROC_NAME_LEN];	/* name of the process, including \0 */

  endpoint_t p_endpoint;	/* endpoint number, generation-aware */

  ਸੁਨੇਹਾ p_sendmsg;		/* Message from this process if SENDING */
  ਸੁਨੇਹਾ p_delivermsg;		/* Message for this process if MF_DELIVERMSG */
  vir_ਬਾਈਟਾਂ p_delivermsg_vir;	/* Virtual addr this proc wants message at */

  /* If handler functions detect a process wants to do something with
   * memory that isn't present, VM has to fix it. Until it has asked
   * what needs to be done and fixed it, save necessary state here.
   *
   * The requester gets a copy of its request message in reqmsg and gets
   * VMREQUEST set.
   */
  ਬਣਤਰ {
	ਬਣਤਰ proc	*nextrestart;	/* next in vmrestart chain */
	ਬਣਤਰ proc	*nextrequestor;	/* next in vmrequest chain */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VMSTYPE_SYS_NONE	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VMSTYPE_KERNELCALL	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VMSTYPE_DELIVERMSG	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VMSTYPE_MAP		3

	ਪੂਰਨ		type;		/* suspended operation */
	ਸੰਘ ixfer_saved{
		/* VMSTYPE_SYS_MESSAGE */
		ਸੁਨੇਹਾ		reqmsg;	/* suspended request message */
	} saved;

	/* Parameters of request to VM */
	ਪੂਰਨ		req_type;
	endpoint_t	ਨਿਸ਼ਾਨਾ;
	ਸੰਘ ixfer_params{
		ਬਣਤਰ {
			vir_ਬਾਈਟਾਂ 	ਸ਼ੁਰੂ, ਲੰਮਾਈ;	/* memory range */
			u8_t		writeflag;	/* nonzero for write access */
		} check;
	} params;
	/* VM result when available */
	ਪੂਰਨ		vmresult;

	/* If the suspended operation is a sys_call, its details are
	 * stored here.
	 */
  } p_vmrequest;

  ਪੂਰਨ p_ਮਿਲਿਆ;	/* consistency checking variables */
  ਪੂਰਨ p_magic;		/* check validity of proc pointers */

  /* if MF_SC_DEFER is set, this struct is valid and contains the
   * do_ipc() arguments that are still to be executed
   */
  ਬਣਤਰ { reg_t r1, r2, r3; } p_defer;

#ਜੇ DEBUG_TRACE
  ਪੂਰਨ p_schedules;
#ਜੇ_ਅੰਤ
};

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

/* Bits for the runtime flags. A process is runnable iff p_rts_flags == 0. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_ਥਾਂ_FREE	0x01	/* process slot is free */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_PROC_STOP	0x02	/* process has been stopped */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_SENDING	0x04	/* process blocked trying to send */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_RECEIVING	0x08	/* process blocked trying to receive */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_SIGNALED	0x10	/* set when new kernel signal arrives */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_SIG_PENDING	0x20	/* unready while signal being processed */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_P_STOP	0x40	/* set when process is being traced */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_NO_PRIV	0x80	/* keep forked system process from running */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_NO_ENDPOINT	0x100	/* process cannot send or receive messages */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_VMINHIBIT	0x200	/* not scheduled until pagetable set by VM */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_PAGEFAULT	0x400	/* process has unhandled pagefault */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_VMREQUEST	0x800	/* originator of vm memory request */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_VMREQTARGET	0x1000	/* target of vm memory request */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_PREEMPTED	0x4000	/* this process was preempted by a higher
				   priority process and we should pick a new one
				   to run. Processes with this flag should be
				   returned to the front of their current
				   priority queue if they are still runnable
				   before we pick a new one
				 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ	0x8000	/* process ran out of its quantum and we should
				   pick a new one. Process was dequeued and
				   should be enqueued at the end of some run
				   queue again */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_BOOTINHIBIT	0x10000	/* not ready until VM has made it */

/* A process is runnable iff p_rts_flags == 0. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ rts_f_is_runnable(flg)	((flg) == 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_is_runnable(p)	(rts_f_is_runnable((p)->p_rts_flags))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_is_preempted(p)	((p)->p_rts_flags & RTS_PREEMPTED)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_no_ਸਮਾਂ_ਹਿੱਸਾ(p)	((p)->p_rts_flags & RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_ptr_ok(p)		((p)->p_magic == PMAGIC)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_ਵਰਤਿਆ_fpu(p)	((p)->p_misc_flags & (MF_FPU_INITIALIZED))

/* test whether the process is scheduled by the kernel's default policy  */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_kernel_scheduler(p)	((p)->p_scheduler == ਨਿਰਾ || \
					(p)->p_scheduler == (p))

/* Macro to return: on which process is a certain process blocked?
 * return endpoint number (can be ANY) or NONE. It's important to
 * check RTS_SENDING first, and then RTS_RECEIVING, as they could
 * both be on (if a ipc_sendrec() blocks on sending), and p_getfrom_e
 * could be nonsense even though RTS_RECEIVING is on.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_BLOCKEDON(p)							\
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_ISSET(rp, f) (((rp)->p_rts_flags & (f)) == (f))


/* Set flag and dequeue if the process was runnable. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_ਸੈੱਟ(rp, f)							\
	ਕਰੋ {								\
		ਅਚਲ ਪੂਰਨ rts = (rp)->p_rts_flags;			\
		(rp)->p_rts_flags |= (f);				\
		ਜੇ(rts_f_is_runnable(rts) && !proc_is_runnable(rp)) {	\
			ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(rp);					\
		}							\
	} ਜਦੋਂ(0)

/* Clear flag and enqueue if the process was not runnable but is now. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_UNSET(rp, f) 						\
	ਕਰੋ {								\
		ਪੂਰਨ rts;						\
		rts = (rp)->p_rts_flags;				\
		(rp)->p_rts_flags &= ~(f);				\
		ਜੇ(!rts_f_is_runnable(rts) && proc_is_runnable(rp)) {	\
			ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(rp);					\
		}							\
	} ਜਦੋਂ(0)

/* Set flags to this value. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTS_SETFLAGS(rp, f)					\
	ਕਰੋ {								\
		ਜੇ(proc_is_runnable(rp) && (f)) { ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(rp); }		\
		(rp)->p_rts_flags = (f);				\
	} ਜਦੋਂ(0)

/* Misc flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_ਜਵਾਬ_PEND	0x001	/* reply to IPC_REQUEST is pending */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_VIRT_TIMER	0x002	/* process-virtual timer is running */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_PROF_TIMER	0x004	/* process-virtual profile timer is running */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_KCALL_RESUME 0x008	/* processing a kernel call was interrupted,
				   most likely because we need VM to resolve a
				   problem or a long running copy was preempted.
				   We need to resume the kernel call execution
				   now
				 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_DELIVERMSG	0x040	/* Copy message for him before running */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_SIG_DELAY	0x080	/* Send signal when no longer sending */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_SC_ਸਰਗਰਮ	0x100	/* Syscall tracing: in a system call now */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_SC_DEFER	0x200	/* Syscall tracing: deferred system call */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_SC_TRACE	0x400	/* Syscall tracing: trigger syscall events */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_FPU_INITIALIZED	0x1000  /* process already used math, so fpu
					 * regs are significant (initialized)*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_SENDING_FROM_KERNEL	0x2000 /* message of this process is from kernel */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_CONTEXT_ਸੈੱਟ	0x4000 /* don't touch context */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_SPROF_ਵੇਖਿਆ	0x8000 /* profiling has seen this process */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_FLUSH_TLB	0x10000	/* if set, TLB must be flushed before letting
				   this process run again. Currently it only
				   applies to SMP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_SENDA_VM_ਖੁੰਝ 0x20000 /* set if a processes wanted to receive an asyn
				    message from this sender but could not
				    because of VM modifying the sender's address
				    space*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_ਕਦਮ		 0x40000 /* Single-step process */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_MSGFAILED	 0x80000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_NICED	0x100000 /* user has lowered max process priority */

/* Magic process table addresses. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEG_PROC_ADDR (&proc[0])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEG_USER_ADDR (&proc[NR_TASKS])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਾ_PROC_ADDR (&proc[NR_TASKS + NR_PROCS])

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_addr(n)      (&(proc[NR_TASKS + (n)]))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_nr(p) 	  ((p)->p_nr)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isokprocn(n)      ((ਬਿਨਾਨਿਸ਼ਾਨੀ) ((n) + NR_TASKS) < NR_PROCS + NR_TASKS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isemptyn(n)       isemptyp(proc_addr(n)) 
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isemptyp(p)       ((p)->p_rts_flags == RTS_ਥਾਂ_FREE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ iskernelp(p)	  ((p) < BEG_USER_ADDR)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ iskerneln(n)	  ((n) < 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isuserp(p)        isusern((p) >= BEG_USER_ADDR)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isusern(n)        ((n) >= 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isrootsysn(n)	  ((n) == ਜੜ੍ਹ_SYS_PROC_NR)

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

EXTERN ਬਣਤਰ proc proc[NR_TASKS + NR_PROCS];	/* process table */

ਪੂਰਨ mini_send(ਬਣਤਰ proc *caller_ptr, endpoint_t dst_e, ਸੁਨੇਹਾ *m_ptr,
	ਪੂਰਨ flags);

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ /* PROC_H */
