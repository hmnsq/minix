#यदि_अपरिभाषितम् PROC_H
#परिभाषय PROC_H

#अन्तर्भाव <minix/const.h>
#अन्तर्भाव <sys/cdefs.h>

#यदि_अपरिभाषितम् __ASSEMBLY__

/* Here is the declaration of the process table.  It contains all process
 * data, including registers, flags, scheduling priority, memory map, 
 * accounting, message passing (IPC) information, and so on. 
 *
 * Many assembly code routines reference fields in it.  The offsets to these
 * fields are defined in the assembler include file sconst.h.  When changing
 * struct proc, be sure to change sconst.h to match.
 */
#अन्तर्भाव <minix/com.h>
#अन्तर्भाव <minix/portio.h>
#अन्तर्भाव "const.h"
#अन्तर्भाव "priv.h"

संरचना proc {
  संरचना stackframe_s p_reg;	/* process' registers saved in stack frame */
  संरचना segframe p_seg;	/* segment descriptors */
  proc_nr_t p_nr;		/* number of this process (for fast access) */
  संरचना priv *p_priv;		/* system privileges structure */
  चञ्चल u32_t p_rts_flags;	/* process is runnable only if zero */
  चञ्चल u32_t p_misc_flags;	/* flags that do not suspend the process */

  वर्ण p_प्राथमिकता;		/* current process priority */
  u64_t p_cpu_time_वामः;	/* time left to use the cpu */
  अचिह्नित p_कालांशः_आकार_ms;	/* assigned time quantum in ms
				   FIXME remove this */
  संरचना proc *p_scheduler;	/* who should get out of quantum msg */
  अचिह्नित p_cpu;		/* what CPU is the process running on */
#यदि_परिभाषितम् CONFIG_SMP
  bitchunk_t p_cpu_आवरणम्[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* what CPUs is the
							    process allowed to
							    run on */
  bitchunk_t p_stale_tlb[BITMAP_CHUNKS(CONFIG_MAX_CPUS)]; /* On which cpu are
				possibly stale entries from this process and has
				to be fresed the next kernel touches this
				processes memory
				 */
#यदि_अन्तः

  /* Accounting statistics that get passed to the process' scheduler */
  संरचना {
	u64_t enter_queue;	/* time when enqueued (cycles) */
	u64_t time_अन्तर्_queue;	/* time spent in queue */
	अचिह्नित दीर्घ dequeues;
	अचिह्नित दीर्घ ipc_sync;
	अचिह्नित दीर्घ ipc_async;
	अचिह्नित दीर्घ preempted;
  } p_accounting;

  clock_t p_dequeued;		/* uptime at which process was last dequeued */

  clock_t p_user_time;		/* user time in ticks */
  clock_t p_sys_time;		/* sys time in ticks */

  clock_t p_virt_वामः;		/* number of ticks left on virtual timer */
  clock_t p_prof_वामः;		/* number of ticks left on profile timer */

  u64_t p_आवृत्तयः;		/* how many cycles did the process use */
  u64_t p_kcall_आवृत्तयः;		/* kernel cycles caused by this proc (kcall) */
  u64_t p_kipc_आवृत्तयः;		/* cycles caused by this proc (ipc) */

  u64_t p_tick_आवृत्तयः;		/* cycles accumulated for up to a clock tick */
  संरचना cpuavg p_cpuavg;	/* running CPU average, for ps(1) */

  संरचना proc *p_nextready;	/* pointer to next ready process */
  संरचना proc *p_caller_q;	/* head of list of procs wishing to send */
  संरचना proc *p_q_link;	/* link to next proc wishing to send */
  endpoint_t p_getfrom_e;	/* from whom does process want to receive? */
  endpoint_t p_sendto_e;	/* to whom does process want to send? */

  sigset_t p_pending;		/* bit map for pending kernel signals */

  वर्ण p_name[PROC_NAME_LEN];	/* name of the process, including \0 */

  endpoint_t p_endpoint;	/* endpoint number, generation-aware */

  सन्देशः p_sendmsg;		/* Message from this process if SENDING */
  सन्देशः p_delivermsg;		/* Message for this process if MF_DELIVERMSG */
  vir_अष्टकानि p_delivermsg_vir;	/* Virtual addr this proc wants message at */

  /* If handler functions detect a process wants to do something with
   * memory that isn't present, VM has to fix it. Until it has asked
   * what needs to be done and fixed it, save necessary state here.
   *
   * The requester gets a copy of its request message in reqmsg and gets
   * VMREQUEST set.
   */
  संरचना {
	संरचना proc	*nextrestart;	/* next in vmrestart chain */
	संरचना proc	*nextrequestor;	/* next in vmrequest chain */
#परिभाषय VMSTYPE_SYS_NONE	0
#परिभाषय VMSTYPE_KERNELCALL	1
#परिभाषय VMSTYPE_DELIVERMSG	2
#परिभाषय VMSTYPE_MAP		3

	पूर्णाङ्क		type;		/* suspended operation */
	सङ्घ ixfer_saved{
		/* VMSTYPE_SYS_MESSAGE */
		सन्देशः		reqmsg;	/* suspended request message */
	} saved;

	/* Parameters of request to VM */
	पूर्णाङ्क		req_type;
	endpoint_t	target;
	सङ्घ ixfer_params{
		संरचना {
			vir_अष्टकानि 	start, दीर्घता;	/* memory range */
			u8_t		writeflag;	/* nonzero for write access */
		} check;
	} params;
	/* VM result when available */
	पूर्णाङ्क		vmresult;

	/* If the suspended operation is a sys_call, its details are
	 * stored here.
	 */
  } p_vmrequest;

  पूर्णाङ्क p_प्राप्तम्;	/* consistency checking variables */
  पूर्णाङ्क p_magic;		/* check validity of proc pointers */

  /* if MF_SC_DEFER is set, this struct is valid and contains the
   * do_ipc() arguments that are still to be executed
   */
  संरचना { reg_t r1, r2, r3; } p_defer;

#यदि DEBUG_TRACE
  पूर्णाङ्क p_schedules;
#यदि_अन्तः
};

#यदि_अन्तः /* __ASSEMBLY__ */

/* Bits for the runtime flags. A process is runnable iff p_rts_flags == 0. */
#परिभाषय RTS_SLOT_FREE	0x01	/* process slot is free */
#परिभाषय RTS_PROC_STOP	0x02	/* process has been stopped */
#परिभाषय RTS_SENDING	0x04	/* process blocked trying to send */
#परिभाषय RTS_RECEIVING	0x08	/* process blocked trying to receive */
#परिभाषय RTS_SIGNALED	0x10	/* set when new kernel signal arrives */
#परिभाषय RTS_SIG_PENDING	0x20	/* unready while signal being processed */
#परिभाषय RTS_P_STOP	0x40	/* set when process is being traced */
#परिभाषय RTS_NO_PRIV	0x80	/* keep forked system process from running */
#परिभाषय RTS_NO_ENDPOINT	0x100	/* process cannot send or receive messages */
#परिभाषय RTS_VMINHIBIT	0x200	/* not scheduled until pagetable set by VM */
#परिभाषय RTS_PAGEFAULT	0x400	/* process has unhandled pagefault */
#परिभाषय RTS_VMREQUEST	0x800	/* originator of vm memory request */
#परिभाषय RTS_VMREQTARGET	0x1000	/* target of vm memory request */
#परिभाषय RTS_PREEMPTED	0x4000	/* this process was preempted by a higher
				   priority process and we should pick a new one
				   to run. Processes with this flag should be
				   returned to the front of their current
				   priority queue if they are still runnable
				   before we pick a new one
				 */
#परिभाषय RTS_NO_कालांशः	0x8000	/* process ran out of its quantum and we should
				   pick a new one. Process was dequeued and
				   should be enqueued at the end of some run
				   queue again */
#परिभाषय RTS_BOOTINHIBIT	0x10000	/* not ready until VM has made it */

/* A process is runnable iff p_rts_flags == 0. */
#परिभाषय rts_f_is_runnable(flg)	((flg) == 0)
#परिभाषय proc_is_runnable(p)	(rts_f_is_runnable((p)->p_rts_flags))

#परिभाषय proc_is_preempted(p)	((p)->p_rts_flags & RTS_PREEMPTED)
#परिभाषय proc_no_कालांशः(p)	((p)->p_rts_flags & RTS_NO_कालांशः)
#परिभाषय proc_ptr_ok(p)		((p)->p_magic == PMAGIC)
#परिभाषय proc_प्रयुक्तम्_fpu(p)	((p)->p_misc_flags & (MF_FPU_INITIALIZED))

/* test whether the process is scheduled by the kernel's default policy  */
#परिभाषय proc_kernel_scheduler(p)	((p)->p_scheduler == रिक्त || \
					(p)->p_scheduler == (p))

/* Macro to return: on which process is a certain process blocked?
 * return endpoint number (can be ANY) or NONE. It's important to
 * check RTS_SENDING first, and then RTS_RECEIVING, as they could
 * both be on (if a ipc_sendrec() blocks on sending), and p_getfrom_e
 * could be nonsense even though RTS_RECEIVING is on.
 */
#परिभाषय P_BLOCKEDON(p)							\
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

#परिभाषय RTS_ISSET(rp, f) (((rp)->p_rts_flags & (f)) == (f))


/* Set flag and dequeue if the process was runnable. */
#परिभाषय RTS_समुच्चयः(rp, f)							\
	कुरु {								\
		ध्रुव पूर्णाङ्क rts = (rp)->p_rts_flags;			\
		(rp)->p_rts_flags |= (f);				\
		यदि(rts_f_is_runnable(rts) && !proc_is_runnable(rp)) {	\
			पङ्क्तेः_निष्कासय(rp);					\
		}							\
	} यावत्(0)

/* Clear flag and enqueue if the process was not runnable but is now. */
#परिभाषय RTS_UNSET(rp, f) 						\
	कुरु {								\
		पूर्णाङ्क rts;						\
		rts = (rp)->p_rts_flags;				\
		(rp)->p_rts_flags &= ~(f);				\
		यदि(!rts_f_is_runnable(rts) && proc_is_runnable(rp)) {	\
			पङ्क्तौ_निक्षिप(rp);					\
		}							\
	} यावत्(0)

/* Set flags to this value. */
#परिभाषय RTS_SETFLAGS(rp, f)					\
	कुरु {								\
		यदि(proc_is_runnable(rp) && (f)) { पङ्क्तेः_निष्कासय(rp); }		\
		(rp)->p_rts_flags = (f);				\
	} यावत्(0)

/* Misc flags */
#परिभाषय MF_प्रत्युत्तरम्_PEND	0x001	/* reply to IPC_REQUEST is pending */
#परिभाषय MF_VIRT_TIMER	0x002	/* process-virtual timer is running */
#परिभाषय MF_PROF_TIMER	0x004	/* process-virtual profile timer is running */
#परिभाषय MF_KCALL_RESUME 0x008	/* processing a kernel call was interrupted,
				   most likely because we need VM to resolve a
				   problem or a long running copy was preempted.
				   We need to resume the kernel call execution
				   now
				 */
#परिभाषय MF_DELIVERMSG	0x040	/* Copy message for him before running */
#परिभाषय MF_SIG_DELAY	0x080	/* Send signal when no longer sending */
#परिभाषय MF_SC_सक्रिय	0x100	/* Syscall tracing: in a system call now */
#परिभाषय MF_SC_DEFER	0x200	/* Syscall tracing: deferred system call */
#परिभाषय MF_SC_TRACE	0x400	/* Syscall tracing: trigger syscall events */
#परिभाषय MF_FPU_INITIALIZED	0x1000  /* process already used math, so fpu
					 * regs are significant (initialized)*/
#परिभाषय MF_SENDING_FROM_KERNEL	0x2000 /* message of this process is from kernel */
#परिभाषय MF_CONTEXT_समुच्चयः	0x4000 /* don't touch context */
#परिभाषय MF_SPROF_आलोकितम्	0x8000 /* profiling has seen this process */
#परिभाषय MF_FLUSH_TLB	0x10000	/* if set, TLB must be flushed before letting
				   this process run again. Currently it only
				   applies to SMP */
#परिभाषय MF_SENDA_VM_अलब्धिः 0x20000 /* set if a processes wanted to receive an asyn
				    message from this sender but could not
				    because of VM modifying the sender's address
				    space*/
#परिभाषय MF_चरणः		 0x40000 /* Single-step process */
#परिभाषय MF_MSGFAILED	 0x80000
#परिभाषय MF_NICED	0x100000 /* user has lowered max process priority */

/* Magic process table addresses. */
#परिभाषय BEG_PROC_ADDR (&proc[0])
#परिभाषय BEG_USER_ADDR (&proc[NR_TASKS])
#परिभाषय अन्तः_PROC_ADDR (&proc[NR_TASKS + NR_PROCS])

#परिभाषय proc_addr(n)      (&(proc[NR_TASKS + (n)]))
#परिभाषय proc_nr(p) 	  ((p)->p_nr)

#परिभाषय isokprocn(n)      ((अचिह्नित) ((n) + NR_TASKS) < NR_PROCS + NR_TASKS)
#परिभाषय isemptyn(n)       isemptyp(proc_addr(n)) 
#परिभाषय isemptyp(p)       ((p)->p_rts_flags == RTS_SLOT_FREE)
#परिभाषय iskernelp(p)	  ((p) < BEG_USER_ADDR)
#परिभाषय iskerneln(n)	  ((n) < 0)
#परिभाषय isuserp(p)        isusern((p) >= BEG_USER_ADDR)
#परिभाषय isusern(n)        ((n) >= 0)
#परिभाषय isrootsysn(n)	  ((n) == मूलम्_SYS_PROC_NR)

#यदि_अपरिभाषितम् __ASSEMBLY__

EXTERN संरचना proc proc[NR_TASKS + NR_PROCS];	/* process table */

पूर्णाङ्क mini_send(संरचना proc *caller_ptr, endpoint_t dst_e, सन्देशः *m_ptr,
	पूर्णाङ्क flags);

#यदि_अन्तः /* __ASSEMBLY__ */

#यदि_अन्तः /* PROC_H */
