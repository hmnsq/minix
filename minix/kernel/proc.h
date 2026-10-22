#अगर_अघोषित PROC_H
#घोषणा PROC_H

#समावेश <minix/const.h>
#समावेश <sys/cdefs.h>

#अगर_अघोषित __ASSEMBLY__

/* Here is the declaration of the process table.  It contains all process
 * data, including registers, flags, scheduling priority, memory map, 
 * accounting, message passing (IPC) information, and so on. 
 *
 * Many assembly code routines reference fields in it.  The offsets to these
 * fields are defined in the assembler include file sconst.h.  When changing
 * struct proc, be sure to change sconst.h to match.
 */
#समावेश <minix/com.h>
#समावेश <minix/portio.h>
#समावेश "const.h"
#समावेश "priv.h"

काष्ठा proc {
  काष्ठा stackframe_s p_reg;	/* process' registers saved in stack frame */
  काष्ठा segframe p_seg;	/* segment descriptors */
  proc_nr_t p_nr;		/* number of this process (for fast access) */
  काष्ठा priv *p_priv;		/* system privileges structure */
  अस्थिर u32_t p_rts_flags;	/* process is runnable only if zero */
  अस्थिर u32_t p_misc_flags;	/* flags that do not suspend the process */

  अक्षर p_priority;		/* current process priority */
  u64_t p_cpu_time_बायाँ;	/* time left to use the cpu */
  अचिन्हित p_quantum_आकार_ms;	/* assigned time quantum in ms
				   FIXME remove this */
  काष्ठा proc *p_scheduler;	/* who should get out of quantum msg */
  अचिन्हित p_cpu;		/* what CPU is the process running on */
#अगर_घोषित CONFIG_SMP
  bitchunk_t p_cpu_mask[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* what CPUs is the
							    process allowed to
							    run on */
  bitchunk_t p_stale_tlb[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* On which cpu are
				possibly stale entries from this process and has
				to be fresed the next kernel touches this
				processes memory
				 */
#पूर्ण_अगर

  /* Accounting statistics that get passed to the process' scheduler */
  काष्ठा {
	u64_t enter_queue;	/* time when enqueued (cycles) */
	u64_t time_in_queue;	/* time spent in queue */
	अचिन्हित दीर्घ dequeues;
	अचिन्हित दीर्घ ipc_sync;
	अचिन्हित दीर्घ ipc_async;
	अचिन्हित दीर्घ preempted;
  } p_accounting;

  घड़ी_प्रकार p_dequeued;		/* uptime at which process was last dequeued */

  घड़ी_प्रकार p_user_time;		/* user time in ticks */
  घड़ी_प्रकार p_sys_time;		/* sys time in ticks */

  घड़ी_प्रकार p_virt_बायाँ;		/* number of ticks left on virtual timer */
  घड़ी_प्रकार p_prof_बायाँ;		/* number of ticks left on profile timer */

  u64_t p_cycles;		/* how many cycles did the process use */
  u64_t p_kcall_cycles;		/* kernel cycles caused by this proc (kcall) */
  u64_t p_kipc_cycles;		/* cycles caused by this proc (ipc) */

  u64_t p_tick_cycles;		/* cycles accumulated for up to a clock tick */
  काष्ठा cpuavg p_cpuavg;	/* running CPU average, for ps(1) */

  काष्ठा proc *p_nextready;	/* pointer to next ready process */
  काष्ठा proc *p_caller_q;	/* head of list of procs wishing to send */
  काष्ठा proc *p_q_link;	/* link to next proc wishing to send */
  endpoint_t p_getfrom_e;	/* from whom does process want to receive? */
  endpoint_t p_sendto_e;	/* to whom does process want to send? */

  sigset_t p_pending;		/* bit map for pending kernel signals */

  अक्षर p_name[PROC_NAME_LEN];	/* name of the process, including \0 */

  endpoint_t p_endpoint;	/* endpoint number, generation-aware */

  message p_sendmsg;		/* Message from this process if SENDING */
  message p_delivermsg;		/* Message for this process if MF_DELIVERMSG */
  vir_bytes p_delivermsg_vir;	/* Virtual addr this proc wants message at */

  /* If handler functions detect a process wants to do something with
   * memory that isn't present, VM has to fix it. Until it has asked
   * what needs to be done and fixed it, save necessary state here.
   *
   * The requester gets a copy of its request message in reqmsg and gets
   * VMREQUEST set.
   */
  काष्ठा {
	काष्ठा proc	*nextrestart;	/* next in vmrestart chain */
	काष्ठा proc	*nextrequestor;	/* next in vmrequest chain */
#घोषणा VMSTYPE_SYS_NONE	0
#घोषणा VMSTYPE_KERNELCALL	1
#घोषणा VMSTYPE_DELIVERMSG	2
#घोषणा VMSTYPE_MAP		3

	पूर्णांक		type;		/* suspended operation */
	जोड़ ixfer_saved{
		/* VMSTYPE_SYS_MESSAGE */
		message		reqmsg;	/* suspended request message */
	} saved;

	/* Parameters of request to VM */
	पूर्णांक		req_type;
	endpoint_t	target;
	जोड़ ixfer_params{
		काष्ठा {
			vir_bytes 	start, लंबाई;	/* memory range */
			u8_t		writeflag;	/* nonzero for write access */
		} check;
	} params;
	/* VM result when available */
	पूर्णांक		vmresult;

	/* If the suspended operation is a sys_call, its details are
	 * stored here.
	 */
  } p_vmrequest;

  पूर्णांक p_found;	/* consistency checking variables */
  पूर्णांक p_magic;		/* check validity of proc pointers */

  /* if MF_SC_DEFER is set, this struct is valid and contains the
   * do_ipc() arguments that are still to be executed
   */
  काष्ठा { reg_t r1, r2, r3; } p_defer;

#अगर DEBUG_TRACE
  पूर्णांक p_schedules;
#पूर्ण_अगर
};

#पूर्ण_अगर /* __ASSEMBLY__ */

/* Bits for the runtime flags. A process is runnable iff p_rts_flags == 0. */
#घोषणा RTS_SLOT_FREE	0x01	/* process slot is free */
#घोषणा RTS_PROC_STOP	0x02	/* process has been stopped */
#घोषणा RTS_SENDING	0x04	/* process blocked trying to send */
#घोषणा RTS_RECEIVING	0x08	/* process blocked trying to receive */
#घोषणा RTS_SIGNALED	0x10	/* set when new kernel signal arrives */
#घोषणा RTS_SIG_PENDING	0x20	/* unready while signal being processed */
#घोषणा RTS_P_STOP	0x40	/* set when process is being traced */
#घोषणा RTS_NO_PRIV	0x80	/* keep forked system process from running */
#घोषणा RTS_NO_ENDPOINT	0x100	/* process cannot send or receive messages */
#घोषणा RTS_VMINHIBIT	0x200	/* not scheduled until pagetable set by VM */
#घोषणा RTS_PAGEFAULT	0x400	/* process has unhandled pagefault */
#घोषणा RTS_VMREQUEST	0x800	/* originator of vm memory request */
#घोषणा RTS_VMREQTARGET	0x1000	/* target of vm memory request */
#घोषणा RTS_PREEMPTED	0x4000	/* this process was preempted by a higher
				   priority process and we should pick a new one
				   to run. Processes with this flag should be
				   returned to the front of their current
				   priority queue if they are still runnable
				   before we pick a new one
				 */
#घोषणा RTS_NO_QUANTUM	0x8000	/* process ran out of its quantum and we should
				   pick a new one. Process was dequeued and
				   should be enqueued at the end of some run
				   queue again */
#घोषणा RTS_BOOTINHIBIT	0x10000	/* not ready until VM has made it */

/* A process is runnable iff p_rts_flags == 0. */
#घोषणा rts_f_is_runnable(flg)	((flg) == 0)
#घोषणा proc_is_runnable(p)	(rts_f_is_runnable((p)->p_rts_flags))

#घोषणा proc_is_preempted(p)	((p)->p_rts_flags & RTS_PREEMPTED)
#घोषणा proc_no_quantum(p)	((p)->p_rts_flags & RTS_NO_QUANTUM)
#घोषणा proc_ptr_ok(p)		((p)->p_magic == PMAGIC)
#घोषणा proc_used_fpu(p)	((p)->p_misc_flags & (MF_FPU_INITIALIZED))

/* test whether the process is scheduled by the kernel's default policy  */
#घोषणा proc_kernel_scheduler(p)	((p)->p_scheduler == शून्य || \
					(p)->p_scheduler == (p))

/* Macro to return: on which process is a certain process blocked?
 * return endpoint number (can be ANY) or NONE. It's important to
 * check RTS_SENDING first, and then RTS_RECEIVING, as they could
 * both be on (if a ipc_sendrec() blocks on sending), and p_getfrom_e
 * could be nonsense even though RTS_RECEIVING is on.
 */
#घोषणा P_BLOCKEDON(p)							\
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

#घोषणा RTS_ISSET(rp, f) (((rp)->p_rts_flags & (f)) == (f))


/* Set flag and dequeue if the process was runnable. */
#घोषणा RTS_SET(rp, f)							\
	करो {								\
		स्थिर पूर्णांक rts = (rp)->p_rts_flags;			\
		(rp)->p_rts_flags |= (f);				\
		अगर(rts_f_is_runnable(rts) && !proc_is_runnable(rp)) {	\
			dequeue(rp);					\
		}							\
	} जबतक(0)

/* Clear flag and enqueue if the process was not runnable but is now. */
#घोषणा RTS_UNSET(rp, f) 						\
	करो {								\
		पूर्णांक rts;						\
		rts = (rp)->p_rts_flags;				\
		(rp)->p_rts_flags &= ~(f);				\
		अगर(!rts_f_is_runnable(rts) && proc_is_runnable(rp)) {	\
			enqueue(rp);					\
		}							\
	} जबतक(0)

/* Set flags to this value. */
#घोषणा RTS_SETFLAGS(rp, f)					\
	करो {								\
		अगर(proc_is_runnable(rp) && (f)) { dequeue(rp); }		\
		(rp)->p_rts_flags = (f);				\
	} जबतक(0)

/* Misc flags */
#घोषणा MF_REPLY_PEND	0x001	/* reply to IPC_REQUEST is pending */
#घोषणा MF_VIRT_TIMER	0x002	/* process-virtual timer is running */
#घोषणा MF_PROF_TIMER	0x004	/* process-virtual profile timer is running */
#घोषणा MF_KCALL_RESUME 0x008	/* processing a kernel call was interrupted,
				   most likely because we need VM to resolve a
				   problem or a long running copy was preempted.
				   We need to resume the kernel call execution
				   now
				 */
#घोषणा MF_DELIVERMSG	0x040	/* Copy message for him before running */
#घोषणा MF_SIG_DELAY	0x080	/* Send signal when no longer sending */
#घोषणा MF_SC_ACTIVE	0x100	/* Syscall tracing: in a system call now */
#घोषणा MF_SC_DEFER	0x200	/* Syscall tracing: deferred system call */
#घोषणा MF_SC_TRACE	0x400	/* Syscall tracing: trigger syscall events */
#घोषणा MF_FPU_INITIALIZED	0x1000  /* process already used math, so fpu
					 * regs are significant (initialized)*/
#घोषणा MF_SENDING_FROM_KERNEL	0x2000 /* message of this process is from kernel */
#घोषणा MF_CONTEXT_SET	0x4000 /* don't touch context */
#घोषणा MF_SPROF_SEEN	0x8000 /* profiling has seen this process */
#घोषणा MF_FLUSH_TLB	0x10000	/* if set, TLB must be flushed before letting
				   this process run again. Currently it only
				   applies to SMP */
#घोषणा MF_SENDA_VM_MISS 0x20000 /* set if a processes wanted to receive an asyn
				    message from this sender but could not
				    because of VM modifying the sender's address
				    space*/
#घोषणा MF_STEP		 0x40000 /* Single-step process */
#घोषणा MF_MSGFAILED	 0x80000
#घोषणा MF_NICED	0x100000 /* user has lowered max process priority */

/* Magic process table addresses. */
#घोषणा BEG_PROC_ADDR (&proc[0])
#घोषणा BEG_USER_ADDR (&proc[NR_TASKS])
#घोषणा END_PROC_ADDR (&proc[NR_TASKS + NR_PROCS])

#घोषणा proc_addr(n)      (&(proc[NR_TASKS + (n)]))
#घोषणा proc_nr(p) 	  ((p)->p_nr)

#घोषणा isokprocn(n)      ((अचिन्हित) ((n) + NR_TASKS) < NR_PROCS + NR_TASKS)
#घोषणा isemptyn(n)       isemptyp(proc_addr(n)) 
#घोषणा isemptyp(p)       ((p)->p_rts_flags == RTS_SLOT_FREE)
#घोषणा iskernelp(p)	  ((p) < BEG_USER_ADDR)
#घोषणा iskerneln(n)	  ((n) < 0)
#घोषणा isuserp(p)        isusern((p) >= BEG_USER_ADDR)
#घोषणा isusern(n)        ((n) >= 0)
#घोषणा isrootsysn(n)	  ((n) == मूल_SYS_PROC_NR)

#अगर_अघोषित __ASSEMBLY__

EXTERN काष्ठा proc proc[NR_TASKS + NR_PROCS];	/* process table */

पूर्णांक mini_send(काष्ठा proc *caller_ptr, endpoint_t dst_e, message *m_ptr,
	पूर्णांक flags);

#पूर्ण_अगर /* __ASSEMBLY__ */

#पूर्ण_अगर /* PROC_H */
