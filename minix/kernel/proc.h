#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROC_H
#ವ್ಯಾಖ್ಯಾನಿಸು PROC_H

#ಅನ್ತರ್ಭಾವ <minix/const.h>
#ಅನ್ತರ್ಭಾವ <sys/cdefs.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

/* Here is the declaration of the process table.  It contains all process
 * data, including registers, flags, scheduling priority, memory map, 
 * accounting, message passing (IPC) information, and so on. 
 *
 * Many assembly code routines reference fields in it.  The offsets to these
 * fields are defined in the assembler include file sconst.h.  When changing
 * struct proc, be sure to change sconst.h to match.
 */
#ಅನ್ತರ್ಭಾವ <minix/com.h>
#ಅನ್ತರ್ಭಾವ <minix/portio.h>
#ಅನ್ತರ್ಭಾವ "const.h"
#ಅನ್ತರ್ಭಾವ "priv.h"

ರಚನೆ proc {
  ರಚನೆ stackframe_s p_reg;	/* process' registers saved in stack frame */
  ರಚನೆ segframe p_seg;	/* segment descriptors */
  proc_nr_t p_nr;		/* number of this process (for fast access) */
  ರಚನೆ priv *p_priv;		/* system privileges structure */
  ಅಸ್ಥಿರ u32_t p_rts_flags;	/* process is runnable only if zero */
  ಅಸ್ಥಿರ u32_t p_misc_flags;	/* flags that do not suspend the process */

  ಅಕ್ಷರ p_ಆದ್ಯತೆ;		/* current process priority */
  u64_t p_cpu_ಸಮಯ_ಎಡ;	/* time left to use the cpu */
  ಚಿಹ್ನೆರಹಿತ p_ಕಾಲಾಂಶ_ಗಾತ್ರ_ms;	/* assigned time quantum in ms
				   FIXME remove this */
  ರಚನೆ proc *p_scheduler;	/* who should get out of quantum msg */
  ಚಿಹ್ನೆರಹಿತ p_cpu;		/* what CPU is the process running on */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
  bitchunk_t p_cpu_ಮುಖವಾಡ[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* what CPUs is the
							    process allowed to
							    run on */
  bitchunk_t p_stale_tlb[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* On which cpu are
				possibly stale entries from this process and has
				to be fresed the next kernel touches this
				processes memory
				 */
#ಆದರೆ_ಅಂತ್ಯ

  /* Accounting statistics that get passed to the process' scheduler */
  ರಚನೆ {
	u64_t enter_ಸರತಿ;	/* time when enqueued (cycles) */
	u64_t ಸಮಯ_ಒಳ_ಸರತಿ;	/* time spent in queue */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ dequeues;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ipc_sync;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ipc_async;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ preempted;
  } p_accounting;

  clock_t p_dequeued;		/* uptime at which process was last dequeued */

  clock_t p_user_ಸಮಯ;		/* user time in ticks */
  clock_t p_sys_ಸಮಯ;		/* sys time in ticks */

  clock_t p_virt_ಎಡ;		/* number of ticks left on virtual timer */
  clock_t p_prof_ಎಡ;		/* number of ticks left on profile timer */

  u64_t p_ಚಕ್ರಗಳು;		/* how many cycles did the process use */
  u64_t p_kcall_ಚಕ್ರಗಳು;		/* kernel cycles caused by this proc (kcall) */
  u64_t p_kipc_ಚಕ್ರಗಳು;		/* cycles caused by this proc (ipc) */

  u64_t p_tick_ಚಕ್ರಗಳು;		/* cycles accumulated for up to a clock tick */
  ರಚನೆ cpuavg p_cpuavg;	/* running CPU average, for ps(1) */

  ರಚನೆ proc *p_nextready;	/* pointer to next ready process */
  ರಚನೆ proc *p_caller_q;	/* head of list of procs wishing to send */
  ರಚನೆ proc *p_q_link;	/* link to next proc wishing to send */
  endpoint_t p_getfrom_e;	/* from whom does process want to receive? */
  endpoint_t p_sendto_e;	/* to whom does process want to send? */

  sigset_t p_pending;		/* bit map for pending kernel signals */

  ಅಕ್ಷರ p_name[PROC_NAME_LEN];	/* name of the process, including \0 */

  endpoint_t p_endpoint;	/* endpoint number, generation-aware */

  ಸಂದೇಶ p_sendmsg;		/* Message from this process if SENDING */
  ಸಂದೇಶ p_delivermsg;		/* Message for this process if MF_DELIVERMSG */
  vir_ಬೈಟುಗಳು p_delivermsg_vir;	/* Virtual addr this proc wants message at */

  /* If handler functions detect a process wants to do something with
   * memory that isn't present, VM has to fix it. Until it has asked
   * what needs to be done and fixed it, save necessary state here.
   *
   * The requester gets a copy of its request message in reqmsg and gets
   * VMREQUEST set.
   */
  ರಚನೆ {
	ರಚನೆ proc	*nextrestart;	/* next in vmrestart chain */
	ರಚನೆ proc	*nextrequestor;	/* next in vmrequest chain */
#ವ್ಯಾಖ್ಯಾನಿಸು VMSTYPE_SYS_NONE	0
#ವ್ಯಾಖ್ಯಾನಿಸು VMSTYPE_KERNELCALL	1
#ವ್ಯಾಖ್ಯಾನಿಸು VMSTYPE_DELIVERMSG	2
#ವ್ಯಾಖ್ಯಾನಿಸು VMSTYPE_MAP		3

	ಪೂರ್ಣಾಂಕ		type;		/* suspended operation */
	ಒಕ್ಕೂಟ ixfer_saved{
		/* VMSTYPE_SYS_MESSAGE */
		ಸಂದೇಶ		reqmsg;	/* suspended request message */
	} saved;

	/* Parameters of request to VM */
	ಪೂರ್ಣಾಂಕ		req_type;
	endpoint_t	ಗುರಿ;
	ಒಕ್ಕೂಟ ixfer_params{
		ರಚನೆ {
			vir_ಬೈಟುಗಳು 	ಆರಂಭ, ಉದ್ದಳತೆ;	/* memory range */
			u8_t		writeflag;	/* nonzero for write access */
		} check;
	} params;
	/* VM result when available */
	ಪೂರ್ಣಾಂಕ		vmresult;

	/* If the suspended operation is a sys_call, its details are
	 * stored here.
	 */
  } p_vmrequest;

  ಪೂರ್ಣಾಂಕ p_ಸಿಕ್ಕಿತು;	/* consistency checking variables */
  ಪೂರ್ಣಾಂಕ p_magic;		/* check validity of proc pointers */

  /* if MF_SC_DEFER is set, this struct is valid and contains the
   * do_ipc() arguments that are still to be executed
   */
  ರಚನೆ { reg_t r1, r2, r3; } p_defer;

#ಆದರೆ DEBUG_TRACE
  ಪೂರ್ಣಾಂಕ p_schedules;
#ಆದರೆ_ಅಂತ್ಯ
};

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

/* Bits for the runtime flags. A process is runnable iff p_rts_flags == 0. */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_ಸ್ಥಾನ_FREE	0x01	/* process slot is free */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_PROC_STOP	0x02	/* process has been stopped */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_SENDING	0x04	/* process blocked trying to send */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_RECEIVING	0x08	/* process blocked trying to receive */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_SIGNALED	0x10	/* set when new kernel signal arrives */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_SIG_PENDING	0x20	/* unready while signal being processed */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_P_STOP	0x40	/* set when process is being traced */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_NO_PRIV	0x80	/* keep forked system process from running */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_NO_ENDPOINT	0x100	/* process cannot send or receive messages */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_VMINHIBIT	0x200	/* not scheduled until pagetable set by VM */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_PAGEFAULT	0x400	/* process has unhandled pagefault */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_VMREQUEST	0x800	/* originator of vm memory request */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_VMREQTARGET	0x1000	/* target of vm memory request */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_PREEMPTED	0x4000	/* this process was preempted by a higher
				   priority process and we should pick a new one
				   to run. Processes with this flag should be
				   returned to the front of their current
				   priority queue if they are still runnable
				   before we pick a new one
				 */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_NO_ಕಾಲಾಂಶ	0x8000	/* process ran out of its quantum and we should
				   pick a new one. Process was dequeued and
				   should be enqueued at the end of some run
				   queue again */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_BOOTINHIBIT	0x10000	/* not ready until VM has made it */

/* A process is runnable iff p_rts_flags == 0. */
#ವ್ಯಾಖ್ಯಾನಿಸು rts_f_is_runnable(flg)	((flg) == 0)
#ವ್ಯಾಖ್ಯಾನಿಸು proc_is_runnable(p)	(rts_f_is_runnable((p)->p_rts_flags))

#ವ್ಯಾಖ್ಯಾನಿಸು proc_is_preempted(p)	((p)->p_rts_flags & RTS_PREEMPTED)
#ವ್ಯಾಖ್ಯಾನಿಸು proc_no_ಕಾಲಾಂಶ(p)	((p)->p_rts_flags & RTS_NO_ಕಾಲಾಂಶ)
#ವ್ಯಾಖ್ಯಾನಿಸು proc_ptr_ok(p)		((p)->p_magic == PMAGIC)
#ವ್ಯಾಖ್ಯಾನಿಸು proc_ಬಳಸಿದ_fpu(p)	((p)->p_misc_flags & (MF_FPU_INITIALIZED))

/* test whether the process is scheduled by the kernel's default policy  */
#ವ್ಯಾಖ್ಯಾನಿಸು proc_kernel_scheduler(p)	((p)->p_scheduler == ಖಾಲಿ || \
					(p)->p_scheduler == (p))

/* Macro to return: on which process is a certain process blocked?
 * return endpoint number (can be ANY) or NONE. It's important to
 * check RTS_SENDING first, and then RTS_RECEIVING, as they could
 * both be on (if a ipc_sendrec() blocks on sending), and p_getfrom_e
 * could be nonsense even though RTS_RECEIVING is on.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು P_BLOCKEDON(p)							\
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

#ವ್ಯಾಖ್ಯಾನಿಸು RTS_ISSET(rp, f) (((rp)->p_rts_flags & (f)) == (f))


/* Set flag and dequeue if the process was runnable. */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_ಗಣ(rp, f)							\
	ಮಾಡು {								\
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ rts = (rp)->p_rts_flags;			\
		(rp)->p_rts_flags |= (f);				\
		ಆದರೆ(rts_f_is_runnable(rts) && !proc_is_runnable(rp)) {	\
			ಸರತಿಯಿಂದ_ತೆಗೆ(rp);					\
		}							\
	} ಆಗ(0)

/* Clear flag and enqueue if the process was not runnable but is now. */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_UNSET(rp, f) 						\
	ಮಾಡು {								\
		ಪೂರ್ಣಾಂಕ rts;						\
		rts = (rp)->p_rts_flags;				\
		(rp)->p_rts_flags &= ~(f);				\
		ಆದರೆ(!rts_f_is_runnable(rts) && proc_is_runnable(rp)) {	\
			ಸರತಿಗೆ_ಸೇರಿಸು(rp);					\
		}							\
	} ಆಗ(0)

/* Set flags to this value. */
#ವ್ಯಾಖ್ಯಾನಿಸು RTS_SETFLAGS(rp, f)					\
	ಮಾಡು {								\
		ಆದರೆ(proc_is_runnable(rp) && (f)) { ಸರತಿಯಿಂದ_ತೆಗೆ(rp); }		\
		(rp)->p_rts_flags = (f);				\
	} ಆಗ(0)

/* Misc flags */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_ಉತ್ತರ_PEND	0x001	/* reply to IPC_REQUEST is pending */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_VIRT_TIMER	0x002	/* process-virtual timer is running */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_PROF_TIMER	0x004	/* process-virtual profile timer is running */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_KCALL_RESUME 0x008	/* processing a kernel call was interrupted,
				   most likely because we need VM to resolve a
				   problem or a long running copy was preempted.
				   We need to resume the kernel call execution
				   now
				 */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_DELIVERMSG	0x040	/* Copy message for him before running */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_SIG_DELAY	0x080	/* Send signal when no longer sending */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_SC_ಸಕ್ರಿಯ	0x100	/* Syscall tracing: in a system call now */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_SC_DEFER	0x200	/* Syscall tracing: deferred system call */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_SC_TRACE	0x400	/* Syscall tracing: trigger syscall events */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_FPU_INITIALIZED	0x1000  /* process already used math, so fpu
					 * regs are significant (initialized)*/
#ವ್ಯಾಖ್ಯಾನಿಸು MF_SENDING_FROM_KERNEL	0x2000 /* message of this process is from kernel */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_CONTEXT_ಗಣ	0x4000 /* don't touch context */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_SPROF_ನೋಡಿದ	0x8000 /* profiling has seen this process */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_FLUSH_TLB	0x10000	/* if set, TLB must be flushed before letting
				   this process run again. Currently it only
				   applies to SMP */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_SENDA_VM_ತಪ್ಪಿದ 0x20000 /* set if a processes wanted to receive an asyn
				    message from this sender but could not
				    because of VM modifying the sender's address
				    space*/
#ವ್ಯಾಖ್ಯಾನಿಸು MF_ಹೆಜ್ಜೆ		 0x40000 /* Single-step process */
#ವ್ಯಾಖ್ಯಾನಿಸು MF_MSGFAILED	 0x80000
#ವ್ಯಾಖ್ಯಾನಿಸು MF_NICED	0x100000 /* user has lowered max process priority */

/* Magic process table addresses. */
#ವ್ಯಾಖ್ಯಾನಿಸು BEG_PROC_ADDR (&proc[0])
#ವ್ಯಾಖ್ಯಾನಿಸು BEG_USER_ADDR (&proc[NR_TASKS])
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆ_PROC_ADDR (&proc[NR_TASKS + NR_PROCS])

#ವ್ಯಾಖ್ಯಾನಿಸು proc_addr(n)      (&(proc[NR_TASKS + (n)]))
#ವ್ಯಾಖ್ಯಾನಿಸು proc_nr(p) 	  ((p)->p_nr)

#ವ್ಯಾಖ್ಯಾನಿಸು isokprocn(n)      ((ಚಿಹ್ನೆರಹಿತ) ((n) + NR_TASKS) < NR_PROCS + NR_TASKS)
#ವ್ಯಾಖ್ಯಾನಿಸು isemptyn(n)       isemptyp(proc_addr(n)) 
#ವ್ಯಾಖ್ಯಾನಿಸು isemptyp(p)       ((p)->p_rts_flags == RTS_ಸ್ಥಾನ_FREE)
#ವ್ಯಾಖ್ಯಾನಿಸು iskernelp(p)	  ((p) < BEG_USER_ADDR)
#ವ್ಯಾಖ್ಯಾನಿಸು iskerneln(n)	  ((n) < 0)
#ವ್ಯಾಖ್ಯಾನಿಸು isuserp(p)        isusern((p) >= BEG_USER_ADDR)
#ವ್ಯಾಖ್ಯಾನಿಸು isusern(n)        ((n) >= 0)
#ವ್ಯಾಖ್ಯಾನಿಸು isrootsysn(n)	  ((n) == ಬೇರು_SYS_PROC_NR)

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

EXTERN ರಚನೆ proc proc[NR_TASKS + NR_PROCS];	/* process table */

ಪೂರ್ಣಾಂಕ mini_send(ರಚನೆ proc *caller_ptr, endpoint_t dst_e, ಸಂದೇಶ *m_ptr,
	ಪೂರ್ಣಾಂಕ flags);

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ /* PROC_H */
