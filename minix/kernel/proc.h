#నిర్వచితం_కాకపోతే PROC_H
#నిర్వచించు PROC_H

#అన్తర్భావ <minix/const.h>
#అన్తర్భావ <sys/cdefs.h>

#నిర్వచితం_కాకపోతే __ASSEMBLY__

/* Here is the declaration of the process table.  It contains all process
 * data, including registers, flags, scheduling priority, memory map, 
 * accounting, message passing (IPC) information, and so on. 
 *
 * Many assembly code routines reference fields in it.  The offsets to these
 * fields are defined in the assembler include file sconst.h.  When changing
 * struct proc, be sure to change sconst.h to match.
 */
#అన్తర్భావ <minix/com.h>
#అన్తర్భావ <minix/portio.h>
#అన్తర్భావ "const.h"
#అన్తర్భావ "priv.h"

నిర్మాణం proc {
  నిర్మాణం stackframe_s p_reg;	/* process' registers saved in stack frame */
  నిర్మాణం segframe p_seg;	/* segment descriptors */
  proc_nr_t p_nr;		/* number of this process (for fast access) */
  నిర్మాణం priv *p_priv;		/* system privileges structure */
  అస్థిర u32_t p_rts_flags;	/* process is runnable only if zero */
  అస్థిర u32_t p_misc_flags;	/* flags that do not suspend the process */

  అక్షరం p_ప్రాధాన్యత;		/* current process priority */
  u64_t p_cpu_సమయం_ఎడమ;	/* time left to use the cpu */
  గుర్తులేని p_కాలాంశం_కొలత_ms;	/* assigned time quantum in ms
				   FIXME remove this */
  నిర్మాణం proc *p_scheduler;	/* who should get out of quantum msg */
  గుర్తులేని p_cpu;		/* what CPU is the process running on */
#నిర్వచితమైతే CONFIG_SMP
  bitchunk_t p_cpu_ముసుగు[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* what CPUs is the
							    process allowed to
							    run on */
  bitchunk_t p_stale_tlb[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* On which cpu are
				possibly stale entries from this process and has
				to be fresed the next kernel touches this
				processes memory
				 */
#అయితే_ముగింపు

  /* Accounting statistics that get passed to the process' scheduler */
  నిర్మాణం {
	u64_t enter_వరుస;	/* time when enqueued (cycles) */
	u64_t సమయం_లోపల_వరుస;	/* time spent in queue */
	గుర్తులేని దీర్ఘ dequeues;
	గుర్తులేని దీర్ఘ ipc_sync;
	గుర్తులేని దీర్ఘ ipc_async;
	గుర్తులేని దీర్ఘ preempted;
  } p_accounting;

  clock_t p_dequeued;		/* uptime at which process was last dequeued */

  clock_t p_user_సమయం;		/* user time in ticks */
  clock_t p_sys_సమయం;		/* sys time in ticks */

  clock_t p_virt_ఎడమ;		/* number of ticks left on virtual timer */
  clock_t p_prof_ఎడమ;		/* number of ticks left on profile timer */

  u64_t p_చక్రాలు;		/* how many cycles did the process use */
  u64_t p_kcall_చక్రాలు;		/* kernel cycles caused by this proc (kcall) */
  u64_t p_kipc_చక్రాలు;		/* cycles caused by this proc (ipc) */

  u64_t p_tick_చక్రాలు;		/* cycles accumulated for up to a clock tick */
  నిర్మాణం cpuavg p_cpuavg;	/* running CPU average, for ps(1) */

  నిర్మాణం proc *p_nextready;	/* pointer to next ready process */
  నిర్మాణం proc *p_caller_q;	/* head of list of procs wishing to send */
  నిర్మాణం proc *p_q_link;	/* link to next proc wishing to send */
  endpoint_t p_getfrom_e;	/* from whom does process want to receive? */
  endpoint_t p_sendto_e;	/* to whom does process want to send? */

  sigset_t p_pending;		/* bit map for pending kernel signals */

  అక్షరం p_name[PROC_NAME_LEN];	/* name of the process, including \0 */

  endpoint_t p_endpoint;	/* endpoint number, generation-aware */

  సందేశం p_sendmsg;		/* Message from this process if SENDING */
  సందేశం p_delivermsg;		/* Message for this process if MF_DELIVERMSG */
  vir_బైట్లు p_delivermsg_vir;	/* Virtual addr this proc wants message at */

  /* If handler functions detect a process wants to do something with
   * memory that isn't present, VM has to fix it. Until it has asked
   * what needs to be done and fixed it, save necessary state here.
   *
   * The requester gets a copy of its request message in reqmsg and gets
   * VMREQUEST set.
   */
  నిర్మాణం {
	నిర్మాణం proc	*nextrestart;	/* next in vmrestart chain */
	నిర్మాణం proc	*nextrequestor;	/* next in vmrequest chain */
#నిర్వచించు VMSTYPE_SYS_NONE	0
#నిర్వచించు VMSTYPE_KERNELCALL	1
#నిర్వచించు VMSTYPE_DELIVERMSG	2
#నిర్వచించు VMSTYPE_MAP		3

	పూర్ణాంకం		type;		/* suspended operation */
	సంఘం ixfer_saved{
		/* VMSTYPE_SYS_MESSAGE */
		సందేశం		reqmsg;	/* suspended request message */
	} saved;

	/* Parameters of request to VM */
	పూర్ణాంకం		req_type;
	endpoint_t	లక్ష్యం;
	సంఘం ixfer_params{
		నిర్మాణం {
			vir_బైట్లు 	ప్రారంభం, నిడివి;	/* memory range */
			u8_t		writeflag;	/* nonzero for write access */
		} check;
	} params;
	/* VM result when available */
	పూర్ణాంకం		vmresult;

	/* If the suspended operation is a sys_call, its details are
	 * stored here.
	 */
  } p_vmrequest;

  పూర్ణాంకం p_దొరికింది;	/* consistency checking variables */
  పూర్ణాంకం p_magic;		/* check validity of proc pointers */

  /* if MF_SC_DEFER is set, this struct is valid and contains the
   * do_ipc() arguments that are still to be executed
   */
  నిర్మాణం { reg_t r1, r2, r3; } p_defer;

#ఒకవేళ DEBUG_TRACE
  పూర్ణాంకం p_schedules;
#అయితే_ముగింపు
};

#అయితే_ముగింపు /* __ASSEMBLY__ */

/* Bits for the runtime flags. A process is runnable iff p_rts_flags == 0. */
#నిర్వచించు RTS_స్థానం_FREE	0x01	/* process slot is free */
#నిర్వచించు RTS_PROC_STOP	0x02	/* process has been stopped */
#నిర్వచించు RTS_SENDING	0x04	/* process blocked trying to send */
#నిర్వచించు RTS_RECEIVING	0x08	/* process blocked trying to receive */
#నిర్వచించు RTS_SIGNALED	0x10	/* set when new kernel signal arrives */
#నిర్వచించు RTS_SIG_PENDING	0x20	/* unready while signal being processed */
#నిర్వచించు RTS_P_STOP	0x40	/* set when process is being traced */
#నిర్వచించు RTS_NO_PRIV	0x80	/* keep forked system process from running */
#నిర్వచించు RTS_NO_ENDPOINT	0x100	/* process cannot send or receive messages */
#నిర్వచించు RTS_VMINHIBIT	0x200	/* not scheduled until pagetable set by VM */
#నిర్వచించు RTS_PAGEFAULT	0x400	/* process has unhandled pagefault */
#నిర్వచించు RTS_VMREQUEST	0x800	/* originator of vm memory request */
#నిర్వచించు RTS_VMREQTARGET	0x1000	/* target of vm memory request */
#నిర్వచించు RTS_PREEMPTED	0x4000	/* this process was preempted by a higher
				   priority process and we should pick a new one
				   to run. Processes with this flag should be
				   returned to the front of their current
				   priority queue if they are still runnable
				   before we pick a new one
				 */
#నిర్వచించు RTS_NO_కాలాంశం	0x8000	/* process ran out of its quantum and we should
				   pick a new one. Process was dequeued and
				   should be enqueued at the end of some run
				   queue again */
#నిర్వచించు RTS_BOOTINHIBIT	0x10000	/* not ready until VM has made it */

/* A process is runnable iff p_rts_flags == 0. */
#నిర్వచించు rts_f_is_runnable(flg)	((flg) == 0)
#నిర్వచించు proc_is_runnable(p)	(rts_f_is_runnable((p)->p_rts_flags))

#నిర్వచించు proc_is_preempted(p)	((p)->p_rts_flags & RTS_PREEMPTED)
#నిర్వచించు proc_no_కాలాంశం(p)	((p)->p_rts_flags & RTS_NO_కాలాంశం)
#నిర్వచించు proc_ptr_ok(p)		((p)->p_magic == PMAGIC)
#నిర్వచించు proc_వాడిన_fpu(p)	((p)->p_misc_flags & (MF_FPU_INITIALIZED))

/* test whether the process is scheduled by the kernel's default policy  */
#నిర్వచించు proc_kernel_scheduler(p)	((p)->p_scheduler == ఖాళీ || \
					(p)->p_scheduler == (p))

/* Macro to return: on which process is a certain process blocked?
 * return endpoint number (can be ANY) or NONE. It's important to
 * check RTS_SENDING first, and then RTS_RECEIVING, as they could
 * both be on (if a ipc_sendrec() blocks on sending), and p_getfrom_e
 * could be nonsense even though RTS_RECEIVING is on.
 */
#నిర్వచించు P_BLOCKEDON(p)							\
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

#నిర్వచించు RTS_ISSET(rp, f) (((rp)->p_rts_flags & (f)) == (f))


/* Set flag and dequeue if the process was runnable. */
#నిర్వచించు RTS_సమితి(rp, f)							\
	చేయి {								\
		స్థిరరాశి పూర్ణాంకం rts = (rp)->p_rts_flags;			\
		(rp)->p_rts_flags |= (f);				\
		ఒకవేళ(rts_f_is_runnable(rts) && !proc_is_runnable(rp)) {	\
			వరుస_నుండి_తీయి(rp);					\
		}							\
	} అయితే(0)

/* Clear flag and enqueue if the process was not runnable but is now. */
#నిర్వచించు RTS_UNSET(rp, f) 						\
	చేయి {								\
		పూర్ణాంకం rts;						\
		rts = (rp)->p_rts_flags;				\
		(rp)->p_rts_flags &= ~(f);				\
		ఒకవేళ(!rts_f_is_runnable(rts) && proc_is_runnable(rp)) {	\
			వరుసలో_చేర్చు(rp);					\
		}							\
	} అయితే(0)

/* Set flags to this value. */
#నిర్వచించు RTS_SETFLAGS(rp, f)					\
	చేయి {								\
		ఒకవేళ(proc_is_runnable(rp) && (f)) { వరుస_నుండి_తీయి(rp); }		\
		(rp)->p_rts_flags = (f);				\
	} అయితే(0)

/* Misc flags */
#నిర్వచించు MF_జవాబు_PEND	0x001	/* reply to IPC_REQUEST is pending */
#నిర్వచించు MF_VIRT_TIMER	0x002	/* process-virtual timer is running */
#నిర్వచించు MF_PROF_TIMER	0x004	/* process-virtual profile timer is running */
#నిర్వచించు MF_KCALL_RESUME 0x008	/* processing a kernel call was interrupted,
				   most likely because we need VM to resolve a
				   problem or a long running copy was preempted.
				   We need to resume the kernel call execution
				   now
				 */
#నిర్వచించు MF_DELIVERMSG	0x040	/* Copy message for him before running */
#నిర్వచించు MF_SIG_DELAY	0x080	/* Send signal when no longer sending */
#నిర్వచించు MF_SC_చురుకైన	0x100	/* Syscall tracing: in a system call now */
#నిర్వచించు MF_SC_DEFER	0x200	/* Syscall tracing: deferred system call */
#నిర్వచించు MF_SC_TRACE	0x400	/* Syscall tracing: trigger syscall events */
#నిర్వచించు MF_FPU_INITIALIZED	0x1000  /* process already used math, so fpu
					 * regs are significant (initialized)*/
#నిర్వచించు MF_SENDING_FROM_KERNEL	0x2000 /* message of this process is from kernel */
#నిర్వచించు MF_CONTEXT_సమితి	0x4000 /* don't touch context */
#నిర్వచించు MF_SPROF_చూసిన	0x8000 /* profiling has seen this process */
#నిర్వచించు MF_FLUSH_TLB	0x10000	/* if set, TLB must be flushed before letting
				   this process run again. Currently it only
				   applies to SMP */
#నిర్వచించు MF_SENDA_VM_తప్పిపోవు 0x20000 /* set if a processes wanted to receive an asyn
				    message from this sender but could not
				    because of VM modifying the sender's address
				    space*/
#నిర్వచించు MF_అడుగు		 0x40000 /* Single-step process */
#నిర్వచించు MF_MSGFAILED	 0x80000
#నిర్వచించు MF_NICED	0x100000 /* user has lowered max process priority */

/* Magic process table addresses. */
#నిర్వచించు BEG_PROC_ADDR (&proc[0])
#నిర్వచించు BEG_USER_ADDR (&proc[NR_TASKS])
#నిర్వచించు చివర_PROC_ADDR (&proc[NR_TASKS + NR_PROCS])

#నిర్వచించు proc_addr(n)      (&(proc[NR_TASKS + (n)]))
#నిర్వచించు proc_nr(p) 	  ((p)->p_nr)

#నిర్వచించు isokprocn(n)      ((గుర్తులేని) ((n) + NR_TASKS) < NR_PROCS + NR_TASKS)
#నిర్వచించు isemptyn(n)       isemptyp(proc_addr(n)) 
#నిర్వచించు isemptyp(p)       ((p)->p_rts_flags == RTS_స్థానం_FREE)
#నిర్వచించు iskernelp(p)	  ((p) < BEG_USER_ADDR)
#నిర్వచించు iskerneln(n)	  ((n) < 0)
#నిర్వచించు isuserp(p)        isusern((p) >= BEG_USER_ADDR)
#నిర్వచించు isusern(n)        ((n) >= 0)
#నిర్వచించు isrootsysn(n)	  ((n) == మూలం_SYS_PROC_NR)

#నిర్వచితం_కాకపోతే __ASSEMBLY__

EXTERN నిర్మాణం proc proc[NR_TASKS + NR_PROCS];	/* process table */

పూర్ణాంకం mini_send(నిర్మాణం proc *caller_ptr, endpoint_t dst_e, సందేశం *m_ptr,
	పూర్ణాంకం flags);

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు /* PROC_H */
