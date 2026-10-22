/* This file contains essentially all of the process and message handling.
 * Together with "mpx.s" it forms the lowest layer of the MINIX kernel.
 * There is one entry point from the outside:
 *
 *   sys_call: 	      a system call, i.e., the kernel is trapped with an INT
 *
 * Changes:
 *   Aug 19, 2005     rewrote scheduling code  (Jorrit N. Herder)
 *   Jul 25, 2005     rewrote system call handling  (Jorrit N. Herder)
 *   May 26, 2005     rewrote message passing functions  (Jorrit N. Herder)
 *   May 24, 2005     new notification system call  (Jorrit N. Herder)
 *   Oct 28, 2004     nonblocking send and receive calls  (Jorrit N. Herder)
 *
 * The code here is critical to make everything work and is important for the
 * overall performance of the system. A large fraction of the code deals with
 * list manipulation. To make this both easy to understand and fast to execute 
 * pointer pointers are used throughout the code. Pointer pointers prevent
 * exceptions for the head or tail of a linked list. 
 *
 *  node_t *queue, *new_node;	// assume these as global variables
 *  node_t **xpp = &queue; 	// get pointer pointer to head of queue 
 *  while (*xpp != NULL) 	// find last pointer of the linked list
 *      xpp = &(*xpp)->next;	// get pointer to next pointer 
 *  *xpp = new_node;		// now replace the end (the NULL pointer) 
 *  new_node->next = NULL;	// and mark the new end of the list
 * 
 * For example, when adding a new node to the end of the list, one normally 
 * makes an exception for an empty list and looks up the end of the list for 
 * nonempty lists. As shown above, this is not required with pointer pointers.
 */

#ضمن <stddef.h>
#ضمن <signal.h>
#ضمن <assert.h>
#ضمن <string.h>

#ضمن "vm.h"
#ضمن "clock.h"
#ضمن "spinlock.h"
#ضمن "arch_proto.h"

#ضمن <minix/syslib.h>

/* Scheduling and message passing functions */
ساكن فراغ idle(فراغ);
/**
 * Made public for use in clock.c (for user-space scheduling)
static int mini_send(struct proc *caller_ptr, endpoint_t dst_e, message
	*m_ptr, int flags);
*/
ساكن صحيح mini_receive(بنية proc *caller_ptr, endpoint_t src,
	message *m_buff_usr, صحيح flags);
ساكن صحيح mini_senda(بنية proc *caller_ptr, asynmsg_t *table, size_t
	size);
ساكن صحيح deadlock(صحيح function, سجل بنية proc *caller,
	endpoint_t src_dst_e);
ساكن صحيح try_async(بنية proc *caller_ptr);
ساكن صحيح try_one(endpoint_t receive_e, بنية proc *src_ptr,
	بنية proc *dst_ptr);
ساكن بنية proc * pick_proc(فراغ);
ساكن فراغ enqueue_head(بنية proc *rp);

/* all idles share the same idle_priv structure */
ساكن بنية priv idle_priv;

ساكن فراغ set_idle_name(حرف * name, صحيح n)
{
        صحيح i, c;
        صحيح p_z = 0;

        אם (n > 999) 
                n = 999; 

        name[0] = 'i'; 
        name[1] = 'd'; 
        name[2] = 'l'; 
        name[3] = 'e'; 

        עבור (i = 4, c = 100; c > 0; c /= 10) {
                صحيح digit;

                digit = n / c;  
                n -= digit * c;  

                אם (p_z || digit != 0 || c == 1) {
                        p_z = 1;
                        name[i++] = '0' + digit;
                }   
        }    

        name[i] = '\0';

}


#define PICK_ANY	1
#define PICK_HIGHERONLY	2

#define BuildNotifyMessage(m_ptr, src, dst_ptr) \
	memset((m_ptr), 0, حجم(*(m_ptr)));				\
	(m_ptr)->m_type = NOTIFY_MESSAGE;				\
	(m_ptr)->m_notify.timestamp = get_monotonic();		\
	بدل (src) {							\
	حالة HARDWARE:							\
		(m_ptr)->m_notify.interrupts =			\
			priv(dst_ptr)->s_int_pending;			\
		priv(dst_ptr)->s_int_pending = 0;			\
		اكسر;							\
	حالة SYSTEM:							\
		memcpy(&(m_ptr)->m_notify.sigset,			\
			&priv(dst_ptr)->s_sig_pending,			\
			حجم(sigset_t));				\
		sigemptyset(&priv(dst_ptr)->s_sig_pending);		\
		اكسر;							\
	}

ساكن message m_notify_buff = { 0, NOTIFY_MESSAGE };

فراغ proc_init(فراغ)
{
	بنية proc * rp;
	بنية priv *sp;
	صحيح i;

	/* Clear the process table. Announce each slot as empty and set up
	 * mappings for proc_addr() and proc_nr() macros. Do the same for the
	 * table with privilege structures for the system processes. 
	 */
	עבור (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < END_PROC_ADDR; ++rp, ++i) {
		rp->p_rts_flags = RTS_SLOT_FREE;/* initialize free slot */
		rp->p_magic = PMAGIC;
		rp->p_nr = i;			/* proc number from ptr */
		rp->p_endpoint = _ENDPOINT(0, rp->p_nr); /* generation no. 0 */
		rp->p_scheduler = عدم;		/* no user space scheduler */
		rp->p_priority = 0;		/* no priority */
		rp->p_quantum_size_ms = 0;	/* no quantum size */

		/* arch-specific initialization */
		arch_proc_reset(rp);
	}
	עבור (sp = BEG_PRIV_ADDR, i = 0; sp < END_PRIV_ADDR; ++sp, ++i) {
		sp->s_proc_nr = NONE;		/* initialize as free */
		sp->s_id = (sys_id_t) i;	/* priv structure index */
		ppriv_addr[i] = sp;		/* priv ptr from number */
		sp->s_sig_mgr = NONE;		/* clear signal managers */
		sp->s_bak_sig_mgr = NONE;
	}

	idle_priv.s_flags = IDL_F;
	/* initialize IDLE structures for every CPU */
	עבור (i = 0; i < CONFIG_MAX_CPUS; i++) {
		بنية proc * ip = get_cpu_var_ptr(i, idle_proc);
		ip->p_endpoint = IDLE;
		ip->p_priv = &idle_priv;
		/* must not let idle ever get scheduled */
		ip->p_rts_flags |= RTS_PROC_STOP;
		set_idle_name(ip->p_name, i);
	}
}

ساكن فراغ switch_address_space_idle(فراغ)
{
#ifdef CONFIG_SMP
	/*
	 * currently we bet that VM is always alive and its pages available so
	 * when the CPU wakes up the kernel is mapped and no surprises happen.
	 * This is only a problem if more than 1 cpus are available
	 */
	switch_address_space(proc_addr(VM_PROC_NR));
#endif
}

/*===========================================================================*
 *				idle					     * 
 *===========================================================================*/
ساكن فراغ idle(فراغ)
{
	بنية proc * p;

	/* This function is called whenever there is no work to do.
	 * Halt the CPU, and measure how many timestamp counter ticks are
	 * spent not doing anything. This allows test setups to measure
	 * the CPU utilization of certain workloads with high precision.
	 */

	p = get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	אם (priv(p)->s_flags & BILLABLE)
		get_cpulocal_var(bill_ptr) = p;

	switch_address_space_idle();

#ifdef CONFIG_SMP
	get_cpulocal_var(cpu_is_idle) = 1;
	/* we don't need to keep time on APs as it is handled on the BSP */
	אם (cpuid != bsp_cpu_id)
		stop_local_timer();
	אחרת
#endif
	{
		/*
		 * If the timer has expired while in kernel we must
		 * rearm it before we go to sleep
		 */
		restart_local_timer();
	}

	/* start accounting for the idle time */
	context_stop(proc_addr(KERNEL));
#אם !SPROFILE
	halt_cpu();
#אחרת
	אם (!sprofiling)
		halt_cpu();
	אחרת {
		متطاير صحيح * v;

		v = get_cpulocal_var_ptr(idle_interrupted);
		interrupts_enable();
		כלעוד (!*v)
			arch_pause();
		interrupts_disable();
		*v = 0;
	}
#endif
	/*
	 * end of accounting for the idle task does not happen here, the kernel
	 * is handling stuff for quite a while before it gets back here!
	 */
}

/*===========================================================================*
 *                              vm_suspend                                *
 *===========================================================================*/
فراغ vm_suspend(بنية proc *caller, ثابت بنية proc *target,
        ثابت vir_bytes linaddr, ثابت vir_bytes len, ثابت صحيح type,
        ثابت صحيح writeflag)
{
        /* This range is not OK for this process. Set parameters
         * of the request and notify VM about the pending request.
         */
        أكد(!RTS_ISSET(caller, RTS_VMREQUEST));
        أكد(!RTS_ISSET(target, RTS_VMREQUEST));

        RTS_SET(caller, RTS_VMREQUEST);

        caller->p_vmrequest.req_type = VMPTYPE_CHECK;
        caller->p_vmrequest.target = target->p_endpoint;
        caller->p_vmrequest.params.check.start = linaddr;
        caller->p_vmrequest.params.check.length = len;
        caller->p_vmrequest.params.check.writeflag = writeflag;
        caller->p_vmrequest.type = type;

        /* Connect caller on vmrequest wait queue. */
        אם(!(caller->p_vmrequest.nextrequestor = vmrequest))
                אם(OK != send_sig(VM_PROC_NR, SIGKMEM))
                        panic("send_sig failed");
        vmrequest = caller;
}

/*===========================================================================*
 *                              delivermsg                                *
 *===========================================================================*/
ساكن فراغ delivermsg(بنية proc *rp)
{
        أكد(!RTS_ISSET(rp, RTS_VMREQUEST));
        أكد(rp->p_misc_flags & MF_DELIVERMSG);
        أكد(rp->p_delivermsg.m_source != NONE);

        אם (copy_msg_to_user(&rp->p_delivermsg,
                                (message *) rp->p_delivermsg_vir)) {
                אם(rp->p_misc_flags & MF_MSGFAILED) {
                        /* 2nd consecutive failure means this won't succeed */
                        printf("WARNING wrong user pointer 0x%08lx from "
                                "process %s / %d\n",
                                rp->p_delivermsg_vir,
                                rp->p_name,
                                rp->p_endpoint);
                        cause_sig(rp->p_nr, SIGSEGV);
                } אחרת {
                        /* 1st failure means we have to ask VM to handle it */
                        vm_suspend(rp, rp, rp->p_delivermsg_vir,
                                حجم(message), VMSTYPE_DELIVERMSG, 1);
                        rp->p_misc_flags |= MF_MSGFAILED;
                }
        } אחרת {
                /* Indicate message has been delivered; address is 'used'. */
                rp->p_delivermsg.m_source = NONE;
                rp->p_misc_flags &= ~(MF_DELIVERMSG|MF_MSGFAILED);

                אם(!(rp->p_misc_flags & MF_CONTEXT_SET)) {
                        rp->p_reg.retreg = OK;
                }
        }
}

/*===========================================================================*
 *				switch_to_user				     * 
 *===========================================================================*/
فراغ switch_to_user(فراغ)
{
	/* This function is called an instant before proc_ptr is
	 * to be scheduled again.
	 */
	بنية proc * p;
#ifdef CONFIG_SMP
	صحيح tlb_must_refresh = 0;
#endif

	p = get_cpulocal_var(proc_ptr);
	/*
	 * if the current process is still runnable check the misc flags and let
	 * it run unless it becomes not runnable in the meantime
	 */
	אם (proc_is_runnable(p))
		اذهب check_misc_flags;
	/*
	 * if a process becomes not runnable while handling the misc flags, we
	 * need to pick a new one here and start from scratch. Also if the
	 * current process wasn't runnable, we pick a new one here
	 */
not_runnable_pick_new:
	אם (proc_is_preempted(p)) {
		p->p_rts_flags &= ~RTS_PREEMPTED;
		אם (proc_is_runnable(p)) {
			אם (p->p_cpu_time_left)
				enqueue_head(p);
			אחרת
				enqueue(p);
		}
	}

	/*
	 * if we have no process to run, set IDLE as the current process for
	 * time accounting and put the cpu in an idle state. After the next
	 * timer interrupt the execution resumes here and we can pick another
	 * process. If there is still nothing runnable we "schedule" IDLE again
	 */
	כלעוד (!(p = pick_proc())) {
		idle();
	}

	/* update the global variable */
	get_cpulocal_var(proc_ptr) = p;

#ifdef CONFIG_SMP
	אם (p->p_misc_flags & MF_FLUSH_TLB && get_cpulocal_var(ptproc) == p)
		tlb_must_refresh = 1;
#endif
	switch_address_space(p);

check_misc_flags:

	أكد(p);
	أكد(proc_is_runnable(p));
	כלעוד (p->p_misc_flags &
		(MF_KCALL_RESUME | MF_DELIVERMSG |
		 MF_SC_DEFER | MF_SC_TRACE | MF_SC_ACTIVE)) {

		أكد(proc_is_runnable(p));
		אם (p->p_misc_flags & MF_KCALL_RESUME) {
			kernel_call_resume(p);
		}
		אחרת אם (p->p_misc_flags & MF_DELIVERMSG) {
			TRACE(VF_SCHEDULING, printf("delivering to %s / %d\n",
				p->p_name, p->p_endpoint););
			delivermsg(p);
		}
		אחרת אם (p->p_misc_flags & MF_SC_DEFER) {
			/* Perform the system call that we deferred earlier. */

			أكد (!(p->p_misc_flags & MF_SC_ACTIVE));

			arch_do_syscall(p);

			/* If the process is stopped for signal delivery, and
			 * not blocked sending a message after the system call,
			 * inform PM.
			 */
			אם ((p->p_misc_flags & MF_SIG_DELAY) &&
					!RTS_ISSET(p, RTS_SENDING))
				sig_delay_done(p);
		}
		אחרת אם (p->p_misc_flags & MF_SC_TRACE) {
			/* Trigger a system call leave event if this was a
			 * system call. We must do this after processing the
			 * other flags above, both for tracing correctness and
			 * to be able to use 'break'.
			 */
			אם (!(p->p_misc_flags & MF_SC_ACTIVE))
				اكسر;

			p->p_misc_flags &=
				~(MF_SC_TRACE | MF_SC_ACTIVE);

			/* Signal the "leave system call" event.
			 * Block the process.
			 */
			cause_sig(proc_nr(p), SIGTRAP);
		}
		אחרת אם (p->p_misc_flags & MF_SC_ACTIVE) {
			/* If MF_SC_ACTIVE was set, remove it now:
			 * we're leaving the system call.
			 */
			p->p_misc_flags &= ~MF_SC_ACTIVE;

			اكسر;
		}

		/*
		 * the selected process might not be runnable anymore. We have
		 * to checkit and schedule another one
		 */
		אם (!proc_is_runnable(p))
			اذهب not_runnable_pick_new;
	}
	/*
	 * check the quantum left before it runs again. We must do it only here
	 * as we are sure that a possible out-of-quantum message to the
	 * scheduler will not collide with the regular ipc
	 */
	אם (!p->p_cpu_time_left)
		proc_no_time(p);
	/*
	 * After handling the misc flags the selected process might not be
	 * runnable anymore. We have to checkit and schedule another one
	 */
	אם (!proc_is_runnable(p))
		اذهب not_runnable_pick_new;

	TRACE(VF_SCHEDULING, printf("cpu %d starting %s / %d "
				"pc 0x%08x\n",
		cpuid, p->p_name, p->p_endpoint, p->p_reg.pc););
#אם DEBUG_TRACE
	p->p_schedules++;
#endif

	p = arch_finish_switch_to_user();
	أكد(p->p_cpu_time_left);

	context_stop(proc_addr(KERNEL));

	/* If the process isn't the owner of FPU, enable the FPU exception */
	אם (get_cpulocal_var(fpu_owner) != p)
		enable_fpu_exception();
	אחרת
		disable_fpu_exception();

	/* If MF_CONTEXT_SET is set, don't clobber process state within
	 * the kernel. The next kernel entry is OK again though.
	 */
	p->p_misc_flags &= ~MF_CONTEXT_SET;

#אם defined(__i386__)
  	أكد(p->p_seg.p_cr3 != 0);
#وإلاإذا defined(__arm__)
	أكد(p->p_seg.p_ttbr != 0);
#endif
#ifdef CONFIG_SMP
	אם (p->p_misc_flags & MF_FLUSH_TLB) {
		אם (tlb_must_refresh)
			refresh_tlb();
		p->p_misc_flags &= ~MF_FLUSH_TLB;
	}
#endif
	
	restart_local_timer();
	
	/*
	 * restore_user_context() carries out the actual mode switch from kernel
	 * to userspace. This function does not return
	 */
	restore_user_context(p);
	NOT_REACHABLE;
}

/*
 * handler for all synchronous IPC calls
 */
ساكن صحيح do_sync_ipc(بنية proc * caller_ptr, /* who made the call */
			صحيح call_nr,	/* system call number and flags */
			endpoint_t src_dst_e,	/* src or dst of the call */
			message *m_ptr)	/* users pointer to a message */
{
  صحيح result;					/* the system call's result */
  صحيح src_dst_p;				/* Process slot number */
  حرف *callname;

  /* Check destination. RECEIVE is the only call that accepts ANY (in addition
   * to a real endpoint). The other calls (SEND, SENDREC, and NOTIFY) require an
   * endpoint to corresponds to a process. In addition, it is necessary to check
   * whether a process is allowed to send to a given destination.
   */
  أكد(call_nr != SENDA);

  /* Only allow non-negative call_nr values less than 32 */
  אם (call_nr < 0 || call_nr > IPCNO_HIGHEST || call_nr >= 32
      || !(callname = ipc_call_names[call_nr])) {
#אם DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          call_nr, proc_nr(caller_ptr), src_dst_e);
#endif
	החזר(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  אם (src_dst_e == ANY)
  {
	אם (call_nr != RECEIVE)
	{
#אם 0
		printf("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#endif
		החזר EINVAL;
	}
	src_dst_p = (صحيح) src_dst_e;
  }
  אחרת
  {
	/* Require a valid source and/or destination process. */
	אם(!isokendpt(src_dst_e, &src_dst_p)) {
#אם 0
		printf("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#endif
		החזר EDEADSRCDST;
	}

	/* If the call is to send to a process, i.e., for SEND, SENDNB,
	 * SENDREC or NOTIFY, verify that the caller is allowed to send to
	 * the given destination. 
	 */
	אם (call_nr != RECEIVE)
	{
		אם (!may_send_to(caller_ptr, src_dst_p)) {
#אם DEBUG_ENABLE_IPC_WARNINGS
			printf(
			"sys_call: ipc mask denied %s from %d to %d\n",
				callname,
				caller_ptr->p_endpoint, src_dst_e);
#endif
			החזר(ECALLDENIED);	/* call denied by ipc mask */
		}
	}
  }

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  אם (!(priv(caller_ptr)->s_trap_mask & (1 << call_nr))) {
#אם DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: %s not allowed, caller %d, src_dst %d\n", 
          callname, proc_nr(caller_ptr), src_dst_p);
#endif
	החזר(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  אם (call_nr != SENDREC && call_nr != RECEIVE && iskerneln(src_dst_p)) {
#אם DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: trap %s not allowed, caller %d, src_dst %d\n",
           callname, proc_nr(caller_ptr), src_dst_e);
#endif
	החזר(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  بدل(call_nr) {
  حالة SENDREC:
	/* A flag is set so that notifications cannot interrupt SENDREC. */
	caller_ptr->p_misc_flags |= MF_REPLY_PEND;
	/* fall through */
  حالة SEND:			
	result = mini_send(caller_ptr, src_dst_e, m_ptr, 0);
	אם (call_nr == SEND || result != OK)
		اكسر;				/* done, or SEND failed */
	/* fall through for SENDREC */
  حالة RECEIVE:			
	אם (call_nr == RECEIVE) {
		caller_ptr->p_misc_flags &= ~MF_REPLY_PEND;
		IPC_STATUS_CLEAR(caller_ptr);  /* clear IPC status code */
	}
	result = mini_receive(caller_ptr, src_dst_e, m_ptr, 0);
	اكسر;
  حالة NOTIFY:
	result = mini_notify(caller_ptr, src_dst_e);
	اكسر;
  حالة SENDNB:
        result = mini_send(caller_ptr, src_dst_e, m_ptr, NON_BLOCKING);
        اكسر;
  افتراضي:
	result = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  החזר(result);
}

صحيح do_ipc(reg_t r1, reg_t r2, reg_t r3)
{
  بنية proc *ثابت caller_ptr = get_cpulocal_var(proc_ptr);	/* get pointer to caller */
  صحيح call_nr = (صحيح) r1;

  أكد(!RTS_ISSET(caller_ptr, RTS_SLOT_FREE));

  /* bill kernel time to this process. */
  kbill_ipc = caller_ptr;

  /* If this process is subject to system call tracing, handle that first. */
  אם (caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
	/* Are we tracing this process, and is it the first sys_call entry? */
	אם ((caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) ==
							MF_SC_TRACE) {
		/* We must notify the tracer before processing the actual
		 * system call. If we don't, the tracer could not obtain the
		 * input message. Postpone the entire system call.
		 */
		caller_ptr->p_misc_flags &= ~MF_SC_TRACE;
		أكد(!(caller_ptr->p_misc_flags & MF_SC_DEFER));
		caller_ptr->p_misc_flags |= MF_SC_DEFER;
		caller_ptr->p_defer.r1 = r1;
		caller_ptr->p_defer.r2 = r2;
		caller_ptr->p_defer.r3 = r3;

		/* Signal the "enter system call" event. Block the process. */
		cause_sig(proc_nr(caller_ptr), SIGTRAP);

		/* Preserve the return register's value. */
		החזר caller_ptr->p_reg.retreg;
	}

	/* If the MF_SC_DEFER flag is set, the syscall is now being resumed. */
	caller_ptr->p_misc_flags &= ~MF_SC_DEFER;

	أكد (!(caller_ptr->p_misc_flags & MF_SC_ACTIVE));

	/* Set a flag to allow reliable tracing of leaving the system call. */
	caller_ptr->p_misc_flags |= MF_SC_ACTIVE;
  }

  אם(caller_ptr->p_misc_flags & MF_DELIVERMSG) {
	panic("sys_call: MF_DELIVERMSG on for %s / %d\n",
		caller_ptr->p_name, caller_ptr->p_endpoint);
  }

  /* Now check if the call is known and try to perform the request. The only
   * system calls that exist in MINIX are sending and receiving messages.
   *   - SENDREC: combines SEND and RECEIVE in a single system call
   *   - SEND:    sender blocks until its message has been delivered
   *   - RECEIVE: receiver blocks until an acceptable message has arrived
   *   - NOTIFY:  asynchronous call; deliver notification or mark pending
   *   - SENDA:   list of asynchronous send requests
   */
  بدل(call_nr) {
  	حالة SENDREC:
  	حالة SEND:			
  	حالة RECEIVE:			
  	حالة NOTIFY:
  	حالة SENDNB:
  	{
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_sync++;

  	    החזר do_sync_ipc(caller_ptr, call_nr, (endpoint_t) r2,
			    (message *) r3);
  	}
  	حالة SENDA:
  	{
 	    /*
  	     * Get and check the size of the argument in bytes as it is a
  	     * table
  	     */
  	    size_t msg_size = (size_t) r2;
  
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_async++;
 
  	    /* Limit size to something reasonable. An arbitrary choice is 16
  	     * times the number of process table entries.
  	     */
  	    אם (msg_size > 16*(NR_TASKS + NR_PROCS))
	        החזר EDOM;
  	    החזר mini_senda(caller_ptr, (asynmsg_t *) r3, msg_size);
  	}
  	حالة MINIX_KERNINFO:
	{
		/* It might not be initialized yet. */
	  	אם(!minix_kerninfo_user) {
			החזר EBADCALL;
		}

  		arch_set_secondary_ipc_return(caller_ptr, minix_kerninfo_user);
  		החזר OK;
	}
  	افتراضي:
	החזר EBADCALL;		/* illegal system call */
  }
}

/*===========================================================================*
 *				deadlock				     * 
 *===========================================================================*/
ساكن صحيح deadlock(
  صحيح function,				/* trap number */
  سجل بنية proc *cp,		/* pointer to caller */
  endpoint_t src_dst_e			/* src or dst process */
)
{
/* Check for deadlock. This can happen if 'caller_ptr' and 'src_dst' have
 * a cyclic dependency of blocking send and receive calls. The only cyclic 
 * dependency that is not fatal is if the caller and target directly SEND(REC)
 * and RECEIVE to each other. If a deadlock is found, the group size is 
 * returned. Otherwise zero is returned. 
 */
  سجل بنية proc *xp;			/* process pointer */
  صحيح group_size = 1;				/* start with only caller */
#אם DEBUG_ENABLE_IPC_WARNINGS
  ساكن بنية proc *processes[NR_PROCS + NR_TASKS];
  processes[0] = cp;
#endif

  כלעוד (src_dst_e != ANY) { 			/* check while process nr */
      صحيح src_dst_slot;
      okendpt(src_dst_e, &src_dst_slot);
      xp = proc_addr(src_dst_slot);		/* follow chain of processes */
      أكد(proc_ptr_ok(xp));
      أكد(!RTS_ISSET(xp, RTS_SLOT_FREE));
#אם DEBUG_ENABLE_IPC_WARNINGS
      processes[group_size] = xp;
#endif
      group_size ++;				/* extra process in group */

      /* Check whether the last process in the chain has a dependency. If it 
       * has not, the cycle cannot be closed and we are done.
       */
      אם((src_dst_e = P_BLOCKEDON(xp)) == NONE)
	החזר 0;

      /* Now check if there is a cyclic dependency. For group sizes of two,  
       * a combination of SEND(REC) and RECEIVE is not fatal. Larger groups
       * or other combinations indicate a deadlock.  
       */
      אם (src_dst_e == cp->p_endpoint) {	/* possible deadlock */
	  אם (group_size == 2) {		/* caller and src_dst */
	      /* The function number is magically converted to flags. */
	      אם ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING) { 
	          החזר(0);			/* not a deadlock */
	      }
	  }
#אם DEBUG_ENABLE_IPC_WARNINGS
	  {
		صحيح i;
		printf("deadlock between these processes:\n");
		עבור(i = 0; i < group_size; i++) {
			printf(" %10s ", processes[i]->p_name);
		}
		printf("\n\n");
		עבור(i = 0; i < group_size; i++) {
			print_proc(processes[i]);
			proc_stacktrace(processes[i]);
		}
	  }
#endif
          החזר(group_size);			/* deadlock found */
      }
  }
  החזר(0);					/* not a deadlock */
}

/*===========================================================================*
 *				has_pending				     * 
 *===========================================================================*/
ساكن صحيح has_pending(sys_map_t *map, صحيح src_p, صحيح asynm)
{
/* Check to see if there is a pending message from the desired source
 * available.
 */

  صحيح src_id;
  sys_id_t id = NULL_PRIV_ID;
#ifdef CONFIG_SMP
  بنية proc * p;
#endif

  /* Either check a specific bit in the mask map, or find the first bit set in
   * it (if any), depending on whether the receive was called on a specific
   * source endpoint.
   */
  אם (src_p != ANY) {
	src_id = nr_to_id(src_p);
	אם (get_sys_bit(*map, src_id)) {
#ifdef CONFIG_SMP
		p = proc_addr(id_to_nr(src_id));
		אם (asynm && RTS_ISSET(p, RTS_VMINHIBIT))
			p->p_misc_flags |= MF_SENDA_VM_MISS;
		אחרת
#endif
			id = src_id;
	}
  } אחרת {
	/* Find a source with a pending message */
	עבור (src_id = 0; src_id < NR_SYS_PROCS; src_id += BITCHUNK_BITS) {
		אם (get_sys_bits(*map, src_id) != 0) {
#ifdef CONFIG_SMP
			כלעוד (src_id < NR_SYS_PROCS) {
				כלעוד (!get_sys_bit(*map, src_id)) {
					אם (src_id == NR_SYS_PROCS)
						اذهب quit_search;
					src_id++;
				}
				p = proc_addr(id_to_nr(src_id));
				/*
				 * We must not let kernel fiddle with pages of a
				 * process which are currently being changed by
				 * VM.  It is dangerous! So do not report such a
				 * process as having pending async messages.
				 * Skip it.
				 */
				אם (asynm && RTS_ISSET(p, RTS_VMINHIBIT)) {
					p->p_misc_flags |= MF_SENDA_VM_MISS;
					src_id++;
				} אחרת
					اذهب quit_search;
			}
#אחרת
			כלעוד (!get_sys_bit(*map, src_id)) src_id++;
			اذهب quit_search;
#endif
		}
	}

quit_search:
	אם (src_id < NR_SYS_PROCS)	/* Found one */
		id = src_id;
  }

  החזר(id);
}

/*===========================================================================*
 *				has_pending_notify			     *
 *===========================================================================*/
صحيح has_pending_notify(بنية proc * caller, صحيح src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	החזר has_pending(map, src_p, 0);
}

/*===========================================================================*
 *				has_pending_asend			     *
 *===========================================================================*/
صحيح has_pending_asend(بنية proc * caller, صحيح src_p)
{
	sys_map_t * map = &priv(caller)->s_asyn_pending;
	החזר has_pending(map, src_p, 1);
}

/*===========================================================================*
 *				unset_notify_pending			     *
 *===========================================================================*/
فراغ unset_notify_pending(بنية proc * caller, صحيح src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	unset_sys_bit(*map, src_p);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
صحيح mini_send(
  سجل بنية proc *caller_ptr,	/* who is trying to send a message? */
  endpoint_t dst_e,			/* to whom is message being sent? */
  message *m_ptr,			/* pointer to message buffer */
  ثابت صحيح flags
)
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  سجل بنية proc *dst_ptr;
  سجل بنية proc **xpp;
  صحيح dst_p;
  dst_p = _ENDPOINT_P(dst_e);
  dst_ptr = proc_addr(dst_p);

  אם (RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT))
  {
	החזר EDEADSRCDST;
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * RTS_SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  אם (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, (vir_bytes)m_ptr, عدم)) {
	صحيح call;
	/* Destination is indeed waiting for this message. */
	أكد(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));	

	אם (!(flags & FROM_KERNEL)) {
		אם(copy_msg_from_user(m_ptr, &dst_ptr->p_delivermsg))
			החזר EFAULT;
	} אחרת {
		dst_ptr->p_delivermsg = *m_ptr;
		IPC_STATUS_ADD_FLAGS(dst_ptr, IPC_FLG_MSG_FROM_KERNEL);
	}

	dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;

	call = (caller_ptr->p_misc_flags & MF_REPLY_PEND ? SENDREC
		: (flags & NON_BLOCKING ? SENDNB : SEND));
	IPC_STATUS_ADD_CALL(dst_ptr, call);

	אם (dst_ptr->p_misc_flags & MF_REPLY_PEND)
		dst_ptr->p_misc_flags &= ~MF_REPLY_PEND;

	RTS_UNSET(dst_ptr, RTS_RECEIVING);

#אם DEBUG_IPC_HOOK
	hook_ipc_msgsend(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#endif
  } אחרת {
	אם(flags & NON_BLOCKING) {
		החזר(ENOTREADY);
	}

	/* Check for a possible deadlock before actually blocking. */
	אם (deadlock(SEND, caller_ptr, dst_e)) {
		החזר(ELOCKED);
	}

	/* Destination is not waiting.  Block and dequeue caller. */
	אם (!(flags & FROM_KERNEL)) {
		אם(copy_msg_from_user(m_ptr, &caller_ptr->p_sendmsg))
			החזר EFAULT;
	} אחרת {
		caller_ptr->p_sendmsg = *m_ptr;
		/*
		 * we need to remember that this message is from kernel so we
		 * can set the delivery status flags when the message is
		 * actually delivered
		 */
		caller_ptr->p_misc_flags |= MF_SENDING_FROM_KERNEL;
	}

	RTS_SET(caller_ptr, RTS_SENDING);
	caller_ptr->p_sendto_e = dst_e;

	/* Process is now blocked.  Put in on the destination's queue. */
	أكد(caller_ptr->p_q_link == عدم);
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	כלעוד (*xpp) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */

#אם DEBUG_IPC_HOOK
	hook_ipc_msgsend(&caller_ptr->p_sendmsg, caller_ptr, dst_ptr);
#endif
  }
  החזר(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
ساكن صحيح mini_receive(بنية proc * caller_ptr,
			endpoint_t src_e, /* which message source is wanted */
			message * m_buff_usr, /* pointer to message buffer */
			ثابت صحيح flags)
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller.
 */
  سجل بنية proc **xpp;
  صحيح r, src_id, found, src_proc_nr, src_p;
  endpoint_t sender_e;

  أكد(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));

  /* This is where we want our message. */
  caller_ptr->p_delivermsg_vir = (vir_bytes) m_buff_usr;

  אם(src_e == ANY) src_p = ANY;
  אחרת
  {
	okendpt(src_e, &src_p);
	אם (RTS_ISSET(proc_addr(src_p), RTS_NO_ENDPOINT))
	{
		החזר EDEADSRCDST;
	}
  }


  /* Check to see if a message from desired source is already available.  The
   * caller's RTS_SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  אם (!RTS_ISSET(caller_ptr, RTS_SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    אם (! (caller_ptr->p_misc_flags & MF_REPLY_PEND)) {

	/* Check for pending notifications */
        src_id = has_pending_notify(caller_ptr, src_p);
        found = src_id != NULL_PRIV_ID;
        אם(found) {
            src_proc_nr = id_to_nr(src_id);		/* get source proc */
            sender_e = proc_addr(src_proc_nr)->p_endpoint;
        }

        אם (found && CANRECEIVE(src_e, sender_e, caller_ptr, 0,
          &m_notify_buff)) {

#אם DEBUG_ENABLE_IPC_WARNINGS
	    אם(src_proc_nr == NONE) {
		printf("mini_receive: sending notify from NONE\n");
	    }
#endif
	    أكد(src_proc_nr != NONE);
            unset_notify_pending(caller_ptr, src_id);	/* no longer pending */

            /* Found a suitable source, deliver the notification message. */
	    أكد(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));	
	    أكد(src_e == ANY || sender_e == src_e);

	    /* assemble message */
	    BuildNotifyMessage(&caller_ptr->p_delivermsg, src_proc_nr, caller_ptr);
	    caller_ptr->p_delivermsg.m_source = sender_e;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;

	    IPC_STATUS_ADD_CALL(caller_ptr, NOTIFY);

	    اذهب receive_done;
        }
    }

    /* Check for pending asynchronous messages */
    אם (has_pending_asend(caller_ptr, src_p) != NULL_PRIV_ID) {
        אם (src_p != ANY)
		r = try_one(src_e, proc_addr(src_p), caller_ptr);
        אחרת
        	r = try_async(caller_ptr);

	אם (r == OK) {
            IPC_STATUS_ADD_CALL(caller_ptr, SENDA);
            اذهب receive_done;
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    כלעוד (*xpp) {
	بنية proc * sender = *xpp;
	endpoint_t sender_e = sender->p_endpoint;

        אם (CANRECEIVE(src_e, sender_e, caller_ptr, 0, &sender->p_sendmsg)) {
            صحيح call;
	    أكد(!RTS_ISSET(sender, RTS_SLOT_FREE));
	    أكد(!RTS_ISSET(sender, RTS_NO_ENDPOINT));

	    /* Found acceptable message. Copy it and update status. */
  	    أكد(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	    caller_ptr->p_delivermsg = sender->p_sendmsg;
	    caller_ptr->p_delivermsg.m_source = sender->p_endpoint;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;
	    RTS_UNSET(sender, RTS_SENDING);

	    call = (sender->p_misc_flags & MF_REPLY_PEND ? SENDREC : SEND);
	    IPC_STATUS_ADD_CALL(caller_ptr, call);

	    /*
	     * if the message is originally from the kernel on behalf of this
	     * process, we must send the status flags accordingly
	     */
	    אם (sender->p_misc_flags & MF_SENDING_FROM_KERNEL) {
		IPC_STATUS_ADD_FLAGS(caller_ptr, IPC_FLG_MSG_FROM_KERNEL);
		/* we can clean the flag now, not need anymore */
		sender->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
	    }
	    אם (sender->p_misc_flags & MF_SIG_DELAY)
		sig_delay_done(sender);

#אם DEBUG_IPC_HOOK
            hook_ipc_msgrecv(&caller_ptr->p_delivermsg, *xpp, caller_ptr);
#endif
		
            *xpp = sender->p_q_link;		/* remove from queue */
	    sender->p_q_link = عدم;
	    اذهب receive_done;
	}
	xpp = &sender->p_q_link;		/* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC. 
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  אם ( ! (flags & NON_BLOCKING)) {
      /* Check for a possible deadlock before actually blocking. */
      אם (deadlock(RECEIVE, caller_ptr, src_e)) {
          החזר(ELOCKED);
      }

      caller_ptr->p_getfrom_e = src_e;		
      RTS_SET(caller_ptr, RTS_RECEIVING);
      החזר(OK);
  } אחרת {
	החזר(ENOTREADY);
  }

receive_done:
  אם (caller_ptr->p_misc_flags & MF_REPLY_PEND)
	  caller_ptr->p_misc_flags &= ~MF_REPLY_PEND;
  החזר OK;
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
صحيح mini_notify(
  ثابت بنية proc *caller_ptr,	/* sender of the notification */
  endpoint_t dst_e			/* which process to notify */
)
{
  سجل بنية proc *dst_ptr;
  صحيح src_id;				/* source id for late delivery */
  صحيح dst_p;

  אם (!isokendpt(dst_e, &dst_p)) {
	util_stacktrace();
	printf("mini_notify: bogus endpoint %d\n", dst_e);
	החזר EDEADSRCDST;
  }

  dst_ptr = proc_addr(dst_p);

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  אם (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, 0, &m_notify_buff) &&
    !(dst_ptr->p_misc_flags & MF_REPLY_PEND)) {
      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      أكد(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));

      BuildNotifyMessage(&dst_ptr->p_delivermsg, proc_nr(caller_ptr), dst_ptr);
      dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;

      IPC_STATUS_ADD_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING);

      החזר(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the privilege id
   * instead of the process number is used in the pending bit map.
   */ 
  src_id = priv(caller_ptr)->s_id;
  set_sys_bit(priv(dst_ptr)->s_notify_pending, src_id); 
  החזר(OK);
}

#define ASCOMPLAIN(caller, entry, field)	\
	printf("kernel:%s:%d: asyn failed for %s in %s "	\
	"(%d/%zu, tab 0x%lx)\n",__FILE__,__LINE__,	\
field, caller->p_name, entry, priv(caller)->s_asynsize, priv(caller)->s_asyntab)

#define A_RETR(entry) עשה {			\
  אם (data_copy(				\
  		caller_ptr->p_endpoint, table_v + (entry)*حجم(asynmsg_t),\
  		KERNEL, (vir_bytes) &tabent,	\
  		حجم(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, entry, "message entry");	\
  			r = EFAULT;		\
	                اذهب asyn_error; \
  }						\
  אחרת אם(tabent.dst == SELF) { \
      tabent.dst = caller_ptr->p_endpoint; \
  } \
  			 } כלעוד(0)

#define A_INSRT(entry) עשה {			\
  אם (data_copy(KERNEL, (vir_bytes) &tabent,	\
  		caller_ptr->p_endpoint, table_v + (entry)*حجم(asynmsg_t),\
  		حجم(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, entry, "message entry");	\
			/* Do NOT set r or goto asyn_error here! */ \
  }						\
  			  } כלעוד(0)	

/*===========================================================================*
 *				try_deliver_senda			     *
 *===========================================================================*/
صحيح try_deliver_senda(بنية proc *caller_ptr,
				asynmsg_t *table,
				size_t size)
{
  صحيح r, dst_p, done, do_notify;
  unsigned صحيح i;
  unsigned flags;
  endpoint_t dst;
  بنية proc *dst_ptr;
  بنية priv *privp;
  asynmsg_t tabent;
  ثابت vir_bytes table_v = (vir_bytes) table;
  message *m_ptr = عدم;

  privp = priv(caller_ptr);

  /* Clear table */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  privp->s_asynendpoint = caller_ptr->p_endpoint;

  אם (size == 0) החזר(OK);  /* Nothing to do, just return */

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;

  /* Limit size to something reasonable. An arbitrary choice is 16
   * times the number of process table entries.
   *
   * (this check has been duplicated in sys_call but is left here
   * as a sanity check)
   */
  אם (size > 16*(NR_TASKS + NR_PROCS)) {
    r = EDOM;
    החזר r;
  }

  עבור (i = 0; i < size; i++) {
	/* Process each entry in the table and store the result in the table.
	 * If we're done handling a message, copy the result to the sender. */

	dst = NONE;
	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	אם (flags == 0) استمر; /* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	אם(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR)) {
		r = EINVAL;
		اذهب asyn_error;
	}
	אם (!(flags & AMF_VALID)) { /* Must contain message */
		r = EINVAL;
		اذهب asyn_error;
	}
	אם (flags & AMF_DONE) استمر;	/* Already done processing */

	r = OK;
	אם (!isokendpt(tabent.dst, &dst_p)) 
		r = EDEADSRCDST; /* Bad destination, report the error */
	אחרת אם (iskerneln(dst_p)) 
		r = ECALLDENIED; /* Asyn sends to the kernel are not allowed */
	אחרת אם (!may_asynsend_to(caller_ptr, dst_p))
		r = ECALLDENIED; /* Send denied by IPC mask */
	אחרת 	/* r == OK */
		dst_ptr = proc_addr(dst_p);

	/* XXX: RTS_NO_ENDPOINT should be removed */
	אם (r == OK && RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT)) {
		r = EDEADSRCDST;
	}

	/* Check if 'dst' is blocked waiting for this message.
	 * If AMF_NOREPLY is set, do not satisfy the receiving part of
	 * a SENDREC.
	 */
	אם (r == OK && WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr,
	    (vir_bytes)&table[i].msg, عدم) &&
	    (!(flags&AMF_NOREPLY) || !(dst_ptr->p_misc_flags&MF_REPLY_PEND))) {
		/* Destination is indeed waiting for this message. */
		dst_ptr->p_delivermsg = tabent.msg;
		dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
		dst_ptr->p_misc_flags |= MF_DELIVERMSG;
		IPC_STATUS_ADD_CALL(dst_ptr, SENDA);
		RTS_UNSET(dst_ptr, RTS_RECEIVING);
#אם DEBUG_IPC_HOOK
		hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#endif
	} אחרת אם (r == OK) {
		/* Inform receiver that something is pending */
		set_sys_bit(priv(dst_ptr)->s_asyn_pending, 
			    priv(caller_ptr)->s_id); 
		done = FALSE;
		استمر;
	} 

	/* Store results */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	אם (flags & AMF_NOTIFY)
		do_notify = TRUE;
	אחרת אם (r != OK && (flags & AMF_NOTIFY_ERR))
		do_notify = TRUE;
	A_INSRT(i);	/* Copy results to caller; ignore errors */
	استمر;

asyn_error:
	אם (dst != NONE)
		printf("KERNEL senda error %d to %d\n", r, dst);
	אחרת
		printf("KERNEL senda error %d\n", r);
  }

  אם (do_notify) 
	mini_notify(proc_addr(ASYNCM), caller_ptr->p_endpoint);

  אם (!done) {
	privp->s_asyntab = (vir_bytes) table;
	privp->s_asynsize = size;
  }

  החזר(OK);
}

/*===========================================================================*
 *				mini_senda				     *
 *===========================================================================*/
ساكن صحيح mini_senda(بنية proc *caller_ptr, asynmsg_t *table, size_t size)
{
  بنية priv *privp;

  privp = priv(caller_ptr);
  אם (!(privp->s_flags & SYS_PROC)) {
	printf( "mini_senda: warning caller has no privilege structure\n");
	החזר(EPERM);
  }

  החזר try_deliver_senda(caller_ptr, table, size);
}


/*===========================================================================*
 *				try_async				     * 
 *===========================================================================*/
ساكن صحيح try_async(بنية proc * caller_ptr)
{
  صحيح r;
  بنية priv *privp;
  بنية proc *src_ptr;
  sys_map_t *map;

  map = &priv(caller_ptr)->s_asyn_pending;

  /* Try all privilege structures */
  עבור (privp = BEG_PRIV_ADDR; privp < END_PRIV_ADDR; ++privp)  {
	אם (privp->s_proc_nr == NONE)
		استمر;

	אם (!get_sys_bit(*map, privp->s_id)) 
		استمر;

	src_ptr = proc_addr(privp->s_proc_nr);

#ifdef CONFIG_SMP
	/*
	 * Do not copy from a process which does not have a stable address space
	 * due to VM fiddling with it
	 */
	אם (RTS_ISSET(src_ptr, RTS_VMINHIBIT)) {
		src_ptr->p_misc_flags |= MF_SENDA_VM_MISS;
		استمر;
	}
#endif

	أكد(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	אם ((r = try_one(ANY, src_ptr, caller_ptr)) == OK)
		החזר(r);
  }

  החזר(ESRCH);
}


/*===========================================================================*
 *				try_one					     *
 *===========================================================================*/
ساكن صحيح try_one(endpoint_t receive_e, بنية proc *src_ptr,
    بنية proc *dst_ptr)
{
/* Try to receive an asynchronous message from 'src_ptr' */
  صحيح r = EAGAIN, done, do_notify;
  unsigned صحيح flags, i;
  size_t size;
  endpoint_t dst, src_e;
  بنية proc *caller_ptr;
  بنية priv *privp;
  asynmsg_t tabent;
  vir_bytes table_v;

  privp = priv(src_ptr);
  אם (!(privp->s_flags & SYS_PROC)) החזר(EPERM);
  size = privp->s_asynsize;
  table_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  unset_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  אם (size == 0) החזר(EAGAIN);
  אם (privp->s_asynendpoint != src_ptr->p_endpoint) החזר EAGAIN;
  אם (!may_asynsend_to(src_ptr, proc_nr(dst_ptr))) החזר (ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */
  src_e = src_ptr->p_endpoint;

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;

  עבור (i = 0; i < size; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the meantime.
  	 */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	אם (flags == 0) استمر;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	אם(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	אחרת אם (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	אחרת אם (flags & AMF_DONE) استمر; /* Already done processing */

	/* Clear done flag. The sender is done sending when all messages in the
	 * table are marked done or empty. However, we will know that only
	 * the next time we enter this function or when the sender decides to
	 * send additional asynchronous messages and manages to deliver them
	 * all.
	 */
	done = FALSE;

	אם (r == EINVAL)
		اذهب store_result;

	/* Message must be directed at receiving end */
	אם (dst != dst_ptr->p_endpoint) استمر;

	אם (!CANRECEIVE(receive_e, src_e, dst_ptr,
		table_v + i*حجم(asynmsg_t) + offsetof(بنية asynmsg,msg),
		عدم)) {
		استمر;
	}

	/* If AMF_NOREPLY is set, then this message is not a reply to a
	 * SENDREC and thus should not satisfy the receiving part of the
	 * SENDREC. This message is to be delivered later.
	 */
	אם ((flags & AMF_NOREPLY) && (dst_ptr->p_misc_flags & MF_REPLY_PEND)) 
		استمر;

	/* Destination is ready to receive the message; deliver it */
	r = OK;
	dst_ptr->p_delivermsg = tabent.msg;
	dst_ptr->p_delivermsg.m_source = src_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;
#אם DEBUG_IPC_HOOK
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, src_ptr, dst_ptr);
#endif

store_result:
	/* Store results for sender. We may just have started delivering a
	 * message, so we must not return an error to the caller in the case
	 * that storing the results triggers an error!
	 */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	אם (flags & AMF_NOTIFY) do_notify = TRUE;
	אחרת אם (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */

	اكسر;
  }

  אם (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  אם (done) {
	privp->s_asyntab = -1;
	privp->s_asynsize = 0;
  } אחרת {
	set_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);
  }

asyn_error:
  החזר(r);
}

/*===========================================================================*
 *				cancel_async				     *
 *===========================================================================*/
صحيح cancel_async(بنية proc *src_ptr, بنية proc *dst_ptr)
{
/* Cancel asynchronous messages from src to dst, because dst is not interested
 * in them (e.g., dst has been restarted) */
  صحيح done, do_notify;
  unsigned صحيح flags, i;
  size_t size;
  endpoint_t dst;
  بنية proc *caller_ptr;
  بنية priv *privp;
  asynmsg_t tabent;
  vir_bytes table_v;

  privp = priv(src_ptr);
  אם (!(privp->s_flags & SYS_PROC)) החזר(EPERM);
  size = privp->s_asynsize;
  table_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  unset_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  אם (size == 0) החזר(EAGAIN);
  אם (!may_send_to(src_ptr, proc_nr(dst_ptr))) החזר(ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;


  עבור (i = 0; i < size; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the mean time.
  	 */

  	صحيح r = EDEADSRCDST;	/* Cancel delivery due to dead dst */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	אם (flags == 0) استمر;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	אם(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	אחרת אם (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	אחרת אם (flags & AMF_DONE) استمر; /* Already done processing */

	/* Message must be directed at receiving end */
	אם (dst != dst_ptr->p_endpoint) {
		done = FALSE;
		استمر;
	}

	/* Store results for sender */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	אם (flags & AMF_NOTIFY) do_notify = TRUE;
	אחרת אם (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */
  }

  אם (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  אם (!done) {
	privp->s_asyntab = table_v;
	privp->s_asynsize = size;
  }

asyn_error:
  החזר(OK);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
فراغ enqueue(
  سجل بنية proc *rp	/* this process is now runnable */
)
{
/* Add 'rp' to one of the queues of runnable processes.  This function is 
 * responsible for inserting a process into one of the scheduling queues. 
 * The mechanism is implemented here.   The actual scheduling policy is
 * defined in sched() and pick_proc().
 *
 * This function can be used x-cpu as it always uses the queues of the cpu the
 * process is assigned to.
 */
  صحيح q = rp->p_priority;	 		/* scheduling queue to use */
  بنية proc **rdy_head, **rdy_tail;
  
  أكد(proc_is_runnable(rp));

  أكد(q >= 0);

  rdy_head = get_cpu_var(rp->p_cpu, run_q_head);
  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now add the process to the queue. */
  אם (!rdy_head[q]) {		/* add to empty queue */
      rdy_head[q] = rdy_tail[q] = rp; 		/* create a new queue */
      rp->p_nextready = عدم;		/* mark new end */
  } 
  אחרת {					/* add to tail of queue */
      rdy_tail[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_tail[q] = rp;				/* set new queue tail */
      rp->p_nextready = عدم;		/* mark new end */
  }

  אם (cpuid == rp->p_cpu) {
	  /*
	   * enqueueing a process with a higher priority than the current one,
	   * it gets preempted. The current process must be preemptible. Testing
	   * the priority also makes sure that a process does not preempt itself
	   */
	  بنية proc * p;
	  p = get_cpulocal_var(proc_ptr);
	  أكد(p);
	  אם((p->p_priority > rp->p_priority) &&
			  (priv(p)->s_flags & PREEMPTIBLE))
		  RTS_SET(p, RTS_PREEMPTED); /* calls dequeue() */
  }
#ifdef CONFIG_SMP
  /*
   * if the process was enqueued on a different cpu and the cpu is idle, i.e.
   * the time is off, we need to wake up that cpu and let it schedule this new
   * process
   */
  אחרת אם (get_cpu_var(rp->p_cpu, cpu_is_idle)) {
	  smp_schedule(rp->p_cpu);
  }
#endif

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr)->p_accounting.enter_queue));


#אם DEBUG_SANITYCHECKS
  أكد(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				enqueue_head				     *
 *===========================================================================*/
/*
 * put a process at the front of its run queue. It comes handy when a process is
 * preempted and removed from run queue to not to have a currently not-runnable
 * process on a run queue. We have to put this process back at the fron to be
 * fair
 */
ساكن فراغ enqueue_head(بنية proc *rp)
{
  ثابت صحيح q = rp->p_priority;	 		/* scheduling queue to use */

  بنية proc **rdy_head, **rdy_tail;

  أكد(proc_ptr_ok(rp));
  أكد(proc_is_runnable(rp));

  /*
   * the process was runnable without its quantum expired when dequeued. A
   * process with no time left should have been handled else and differently
   */
  أكد(rp->p_cpu_time_left);

  أكد(q >= 0);


  rdy_head = get_cpu_var(rp->p_cpu, run_q_head);
  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now add the process to the queue. */
  אם (!rdy_head[q]) {		/* add to empty queue */
	rdy_head[q] = rdy_tail[q] = rp; 	/* create a new queue */
	rp->p_nextready = عدم;			/* mark new end */
  } אחרת {					/* add to head of queue */
	rp->p_nextready = rdy_head[q];		/* chain head of queue */
	rdy_head[q] = rp;			/* set new queue head */
  }

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr->p_accounting.enter_queue)));


  /* Process accounting for scheduling */
  rp->p_accounting.dequeues--;
  rp->p_accounting.preempted++;

#אם DEBUG_SANITYCHECKS
  أكد(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
فراغ dequeue(بنية proc *rp)
/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 *
 * This function can operate x-cpu as it always removes the process from the
 * queue of the cpu the process is currently assigned to.
 */
  صحيح q = rp->p_priority;		/* queue to use */
  بنية proc **xpp;			/* iterate over queue */
  بنية proc *prev_xp;
  u64_t tsc, tsc_delta;

  بنية proc **rdy_tail;

  أكد(proc_ptr_ok(rp));
  أكد(!proc_is_runnable(rp));

  /* Side-effect for kernel: check if the task's stack still is ok? */
  أكد (!iskernelp(rp) || *priv(rp)->s_stack_guard == STACK_GUARD);

  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  prev_xp = عدم;				
  עבור (xpp = get_cpu_var_ptr(rp->p_cpu, run_q_head[q]); *xpp;
		  xpp = &(*xpp)->p_nextready) {
      אם (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          אם (rp == rdy_tail[q]) {		/* queue tail removed */
              rdy_tail[q] = prev_xp;		/* set new tail */
	  }

          اكسر;
      }
      prev_xp = *xpp;				/* save previous in chain */
  }

	
  /* Process accounting for scheduling */
  rp->p_accounting.dequeues++;

  /* this is not all that accurate on virtual machines, especially with
     IO bound processes that only spend a short amount of time in the queue
     at a time. */
  אם (rp->p_accounting.enter_queue) {
	read_tsc_64(&tsc);
	tsc_delta = tsc - rp->p_accounting.enter_queue;
	rp->p_accounting.time_in_queue = rp->p_accounting.time_in_queue +
		tsc_delta;
	rp->p_accounting.enter_queue = 0;
  }

  /* For ps(1), remember when the process was last dequeued. */
  rp->p_dequeued = get_monotonic();

#אם DEBUG_SANITYCHECKS
  أكد(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
ساكن بنية proc * pick_proc(فراغ)
{
/* Decide who to run now.  A new process is selected and returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
  سجل بنية proc *rp;			/* process to run */
  بنية proc **rdy_head;
  صحيح q;				/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the task table.
   * If there are no processes ready to run, return NULL.
   */
  rdy_head = get_cpulocal_var(run_q_head);
  עבור (q=0; q < NR_SCHED_QUEUES; q++) {	
	אם(!(rp = rdy_head[q])) {
		TRACE(VF_PICKPROC, printf("cpu %d queue %d empty\n", cpuid, q););
		استمر;
	}
	أكد(proc_is_runnable(rp));
	אם (priv(rp)->s_flags & BILLABLE)	 	
		get_cpulocal_var(bill_ptr) = rp; /* bill for system time */
	החזר rp;
  }
  החזר عدم;
}

/*===========================================================================*
 *				endpoint_lookup				     *
 *===========================================================================*/
بنية proc *endpoint_lookup(endpoint_t e)
{
	صحيح n;

	אם(!isokendpt(e, &n)) החזר عدم;

	החזר proc_addr(n);
}

/*===========================================================================*
 *				isokendpt_f				     *
 *===========================================================================*/
#אם DEBUG_ENABLE_IPC_WARNINGS
صحيح isokendpt_f(ثابت حرف * file, صحيح line, endpoint_t e, صحيح * p,
	ثابت صحيح fatalflag)
#אחרת
صحيح isokendpt_f(endpoint_t e, صحيح * p, ثابت صحيح fatalflag)
#endif
{
	صحيح ok = 0;
	/* Convert an endpoint number into a process number.
	 * Return nonzero if the process is alive with the corresponding
	 * generation number, zero otherwise.
	 *
	 * This function is called with file and line number by the
	 * isokendpt_d macro if DEBUG_ENABLE_IPC_WARNINGS is defined,
	 * otherwise without. This allows us to print the where the
	 * conversion was attempted, making the errors verbose without
	 * adding code for that at every call.
	 * 
	 * If fatalflag is nonzero, we must panic if the conversion doesn't
	 * succeed.
	 */
	*p = _ENDPOINT_P(e);
	ok = 0;
	אם(isokprocn(*p) && !isemptyn(*p) && proc_addr(*p)->p_endpoint == e)
		ok = 1;
	אם(!ok && fatalflag)
		panic("invalid endpoint: %d",  e);
	החזר ok;
}

ساكن فراغ notify_scheduler(بنية proc *p)
{
	message m_no_quantum;
	صحيح err;

	أكد(!proc_kernel_scheduler(p));

	/* dequeue the process */
	RTS_SET(p, RTS_NO_QUANTUM);
	/*
	 * Notify the process's scheduler that it has run out of
	 * quantum. This is done by sending a message to the scheduler
	 * on the process's behalf
	 */
	m_no_quantum.m_source = p->p_endpoint;
	m_no_quantum.m_type   = SCHEDULING_NO_QUANTUM;
	m_no_quantum.m_krn_lsys_schedule.acnt_queue = cpu_time_2_ms(p->p_accounting.time_in_queue);
	m_no_quantum.m_krn_lsys_schedule.acnt_deqs      = p->p_accounting.dequeues;
	m_no_quantum.m_krn_lsys_schedule.acnt_ipc_sync  = p->p_accounting.ipc_sync;
	m_no_quantum.m_krn_lsys_schedule.acnt_ipc_async = p->p_accounting.ipc_async;
	m_no_quantum.m_krn_lsys_schedule.acnt_preempt   = p->p_accounting.preempted;
	m_no_quantum.m_krn_lsys_schedule.acnt_cpu       = cpuid;
	m_no_quantum.m_krn_lsys_schedule.acnt_cpu_load  = cpu_load();

	/* Reset accounting */
	reset_proc_accounting(p);

	אם ((err = mini_send(p, p->p_scheduler->p_endpoint,
					&m_no_quantum, FROM_KERNEL))) {
		panic("WARNING: Scheduling: mini_send returned %d\n", err);
	}
}

فراغ proc_no_time(بنية proc * p)
{
	אם (!proc_kernel_scheduler(p) && priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		notify_scheduler(p);
	}
	אחרת {
		/*
		 * non-preemptible processes only need their quantum to
		 * be renewed. In fact, they by pass scheduling
		 */
		p->p_cpu_time_left = ms_2_cpu_time(p->p_quantum_size_ms);
#אם DEBUG_RACE
		RTS_SET(p, RTS_PREEMPTED);
		RTS_UNSET(p, RTS_PREEMPTED);
#endif
	}
}

فراغ reset_proc_accounting(بنية proc *p)
{
  p->p_accounting.preempted = 0;
  p->p_accounting.ipc_sync  = 0;
  p->p_accounting.ipc_async = 0;
  p->p_accounting.dequeues  = 0;
  p->p_accounting.time_in_queue = 0;
  p->p_accounting.enter_queue = 0;
}
	
فراغ copr_not_available_handler(فراغ)
{
	بنية proc * p;
	بنية proc ** local_fpu_owner;
	/*
	 * Disable the FPU exception (both for the kernel and for the process
	 * once it's scheduled), and initialize or restore the FPU state.
	 */

	disable_fpu_exception();

	p = get_cpulocal_var(proc_ptr);

	/* if FPU is not owned by anyone, do not store anything */
	local_fpu_owner = get_cpulocal_var_ptr(fpu_owner);
	אם (*local_fpu_owner != عدم) {
		أكد(*local_fpu_owner != p);
		save_local_fpu(*local_fpu_owner, FALSE /*retain*/);
	}

	/*
	 * restore the current process' state and let it run again, do not
	 * schedule!
	 */
	אם (restore_fpu(p) != OK) {
		/* Restoring FPU state failed. This is always the process's own
		 * fault. Send a signal, and schedule another process instead.
		 */
		*local_fpu_owner = عدم;		/* release FPU */
		cause_sig(proc_nr(p), SIGFPE);
		החזר;
	}

	*local_fpu_owner = p;
	context_stop(proc_addr(KERNEL));
	restore_user_context(p);
	NOT_REACHABLE;
}

فراغ release_fpu(بنية proc * p) {
	بنية proc ** fpu_owner_ptr;

	fpu_owner_ptr = get_cpu_var_ptr(p->p_cpu, fpu_owner);

	אם (*fpu_owner_ptr == p)
		*fpu_owner_ptr = عدم;
}

فراغ ser_dump_proc(فراغ)
{
        بنية proc *pp;

        עבור (pp= BEG_PROC_ADDR; pp < END_PROC_ADDR; pp++)
        {
                אם (isemptyp(pp))
                        استمر;
                print_proc_recursive(pp);
        }
}
