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

#অন্তর্ভাৱ <stddef.h>
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "vm.h"
#অন্তর্ভাৱ "clock.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "arch_proto.h"

#অন্তর্ভাৱ <minix/syslib.h>

/* Scheduling and message passing functions */
স্থির শূন্য idle(শূন্য);
/**
 * Made public for use in clock.c (for user-space scheduling)
static int mini_send(struct proc *caller_ptr, endpoint_t dst_e, message
	*m_ptr, int flags);
*/
স্থির পূর্ণাঙ্ক mini_receive(সংরচনা proc *caller_ptr, endpoint_t src,
	message *m_buff_usr, পূর্ণাঙ্ক flags);
স্থির পূর্ণাঙ্ক mini_senda(সংরচনা proc *caller_ptr, asynmsg_t *table, size_t
	size);
স্থির পূর্ণাঙ্ক deadlock(পূর্ণাঙ্ক function, পঞ্জিকা সংরচনা proc *caller,
	endpoint_t src_dst_e);
স্থির পূর্ণাঙ্ক try_async(সংরচনা proc *caller_ptr);
স্থির পূর্ণাঙ্ক try_one(endpoint_t receive_e, সংরচনা proc *src_ptr,
	সংরচনা proc *dst_ptr);
স্থির সংরচনা proc * pick_proc(শূন্য);
স্থির শূন্য enqueue_head(সংরচনা proc *rp);

/* all idles share the same idle_priv structure */
স্থির সংরচনা priv idle_priv;

স্থির শূন্য set_idle_name(ৱর্ণ * name, পূর্ণাঙ্ক n)
{
        পূর্ণাঙ্ক i, c;
        পূর্ণাঙ্ক p_z = 0;

        যদি (n > 999) 
                n = 999; 

        name[0] = 'i'; 
        name[1] = 'd'; 
        name[2] = 'l'; 
        name[3] = 'e'; 

        বাবে (i = 4, c = 100; c > 0; c /= 10) {
                পূর্ণাঙ্ক digit;

                digit = n / c;  
                n -= digit * c;  

                যদি (p_z || digit != 0 || c == 1) {
                        p_z = 1;
                        name[i++] = '0' + digit;
                }   
        }    

        name[i] = '\0';

}


#define PICK_ANY	1
#define PICK_HIGHERONLY	2

#define BuildNotifyMessage(m_ptr, src, dst_ptr) \
	memset((m_ptr), 0, পরিমাণ(*(m_ptr)));				\
	(m_ptr)->m_type = NOTIFY_MESSAGE;				\
	(m_ptr)->m_notify.timestamp = get_monotonic();		\
	ৱিকল্প (src) {							\
	পক্ষ HARDWARE:							\
		(m_ptr)->m_notify.interrupts =			\
			priv(dst_ptr)->s_int_pending;			\
		priv(dst_ptr)->s_int_pending = 0;			\
		ভঙ্গ;							\
	পক্ষ SYSTEM:							\
		memcpy(&(m_ptr)->m_notify.sigset,			\
			&priv(dst_ptr)->s_sig_pending,			\
			পরিমাণ(sigset_t));				\
		sigemptyset(&priv(dst_ptr)->s_sig_pending);		\
		ভঙ্গ;							\
	}

স্থির message m_notify_buff = { 0, NOTIFY_MESSAGE };

শূন্য proc_init(শূন্য)
{
	সংরচনা proc * rp;
	সংরচনা priv *sp;
	পূর্ণাঙ্ক i;

	/* Clear the process table. Announce each slot as empty and set up
	 * mappings for proc_addr() and proc_nr() macros. Do the same for the
	 * table with privilege structures for the system processes. 
	 */
	বাবে (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < END_PROC_ADDR; ++rp, ++i) {
		rp->p_rts_flags = RTS_SLOT_FREE;/* initialize free slot */
		rp->p_magic = PMAGIC;
		rp->p_nr = i;			/* proc number from ptr */
		rp->p_endpoint = _ENDPOINT(0, rp->p_nr); /* generation no. 0 */
		rp->p_scheduler = রিক্ত;		/* no user space scheduler */
		rp->p_priority = 0;		/* no priority */
		rp->p_quantum_size_ms = 0;	/* no quantum size */

		/* arch-specific initialization */
		arch_proc_reset(rp);
	}
	বাবে (sp = BEG_PRIV_ADDR, i = 0; sp < END_PRIV_ADDR; ++sp, ++i) {
		sp->s_proc_nr = NONE;		/* initialize as free */
		sp->s_id = (sys_id_t) i;	/* priv structure index */
		ppriv_addr[i] = sp;		/* priv ptr from number */
		sp->s_sig_mgr = NONE;		/* clear signal managers */
		sp->s_bak_sig_mgr = NONE;
	}

	idle_priv.s_flags = IDL_F;
	/* initialize IDLE structures for every CPU */
	বাবে (i = 0; i < CONFIG_MAX_CPUS; i++) {
		সংরচনা proc * ip = get_cpu_var_ptr(i, idle_proc);
		ip->p_endpoint = IDLE;
		ip->p_priv = &idle_priv;
		/* must not let idle ever get scheduled */
		ip->p_rts_flags |= RTS_PROC_STOP;
		set_idle_name(ip->p_name, i);
	}
}

স্থির শূন্য switch_address_space_idle(শূন্য)
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
স্থির শূন্য idle(শূন্য)
{
	সংরচনা proc * p;

	/* This function is called whenever there is no work to do.
	 * Halt the CPU, and measure how many timestamp counter ticks are
	 * spent not doing anything. This allows test setups to measure
	 * the CPU utilization of certain workloads with high precision.
	 */

	p = get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	যদি (priv(p)->s_flags & BILLABLE)
		get_cpulocal_var(bill_ptr) = p;

	switch_address_space_idle();

#ifdef CONFIG_SMP
	get_cpulocal_var(cpu_is_idle) = 1;
	/* we don't need to keep time on APs as it is handled on the BSP */
	যদি (cpuid != bsp_cpu_id)
		stop_local_timer();
	অন্যথা
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
#যদি !SPROFILE
	halt_cpu();
#অন্যথা
	যদি (!sprofiling)
		halt_cpu();
	অন্যথা {
		চঞ্চল পূর্ণাঙ্ক * v;

		v = get_cpulocal_var_ptr(idle_interrupted);
		interrupts_enable();
		যেতিয়ালৈ (!*v)
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
শূন্য vm_suspend(সংরচনা proc *caller, ধ্রুৱ সংরচনা proc *target,
        ধ্রুৱ vir_bytes linaddr, ধ্রুৱ vir_bytes len, ধ্রুৱ পূর্ণাঙ্ক type,
        ধ্রুৱ পূর্ণাঙ্ক writeflag)
{
        /* This range is not OK for this process. Set parameters
         * of the request and notify VM about the pending request.
         */
        প্রতিজ্ঞা(!RTS_ISSET(caller, RTS_VMREQUEST));
        প্রতিজ্ঞা(!RTS_ISSET(target, RTS_VMREQUEST));

        RTS_SET(caller, RTS_VMREQUEST);

        caller->p_vmrequest.req_type = VMPTYPE_CHECK;
        caller->p_vmrequest.target = target->p_endpoint;
        caller->p_vmrequest.params.check.start = linaddr;
        caller->p_vmrequest.params.check.length = len;
        caller->p_vmrequest.params.check.writeflag = writeflag;
        caller->p_vmrequest.type = type;

        /* Connect caller on vmrequest wait queue. */
        যদি(!(caller->p_vmrequest.nextrequestor = vmrequest))
                যদি(OK != send_sig(VM_PROC_NR, SIGKMEM))
                        panic("send_sig failed");
        vmrequest = caller;
}

/*===========================================================================*
 *                              delivermsg                                *
 *===========================================================================*/
স্থির শূন্য delivermsg(সংরচনা proc *rp)
{
        প্রতিজ্ঞা(!RTS_ISSET(rp, RTS_VMREQUEST));
        প্রতিজ্ঞা(rp->p_misc_flags & MF_DELIVERMSG);
        প্রতিজ্ঞা(rp->p_delivermsg.m_source != NONE);

        যদি (copy_বাৰ্তা_to_user(&rp->p_delivermsg,
                                (message *) rp->p_delivermsg_vir)) {
                যদি(rp->p_misc_flags & MF_MSGFAILED) {
                        /* 2nd consecutive failure means this won't succeed */
                        printf("WARNING wrong user pointer 0x%08lx from "
                                "process %s / %d\n",
                                rp->p_delivermsg_vir,
                                rp->p_name,
                                rp->p_endpoint);
                        cause_sig(rp->p_nr, SIGSEGV);
                } অন্যথা {
                        /* 1st failure means we have to ask VM to handle it */
                        vm_suspend(rp, rp, rp->p_delivermsg_vir,
                                পরিমাণ(message), VMSTYPE_DELIVERMSG, 1);
                        rp->p_misc_flags |= MF_MSGFAILED;
                }
        } অন্যথা {
                /* Indicate message has been delivered; address is 'used'. */
                rp->p_delivermsg.m_source = NONE;
                rp->p_misc_flags &= ~(MF_DELIVERMSG|MF_MSGFAILED);

                যদি(!(rp->p_misc_flags & MF_CONTEXT_SET)) {
                        rp->p_reg.retreg = OK;
                }
        }
}

/*===========================================================================*
 *				switch_to_user				     * 
 *===========================================================================*/
শূন্য switch_to_user(শূন্য)
{
	/* This function is called an instant before proc_ptr is
	 * to be scheduled again.
	 */
	সংরচনা proc * p;
#ifdef CONFIG_SMP
	পূর্ণাঙ্ক tlb_must_refresh = 0;
#endif

	p = get_cpulocal_var(proc_ptr);
	/*
	 * if the current process is still runnable check the misc flags and let
	 * it run unless it becomes not runnable in the meantime
	 */
	যদি (proc_is_runnable(p))
		গচ্ছ check_misc_flags;
	/*
	 * if a process becomes not runnable while handling the misc flags, we
	 * need to pick a new one here and start from scratch. Also if the
	 * current process wasn't runnable, we pick a new one here
	 */
not_runnable_pick_new:
	যদি (proc_is_preempted(p)) {
		p->p_rts_flags &= ~RTS_PREEMPTED;
		যদি (proc_is_runnable(p)) {
			যদি (p->p_cpu_time_বাম)
				enqueue_head(p);
			অন্যথা
				enqueue(p);
		}
	}

	/*
	 * if we have no process to run, set IDLE as the current process for
	 * time accounting and put the cpu in an idle state. After the next
	 * timer interrupt the execution resumes here and we can pick another
	 * process. If there is still nothing runnable we "schedule" IDLE again
	 */
	যেতিয়ালৈ (!(p = pick_proc())) {
		idle();
	}

	/* update the global variable */
	get_cpulocal_var(proc_ptr) = p;

#ifdef CONFIG_SMP
	যদি (p->p_misc_flags & MF_FLUSH_TLB && get_cpulocal_var(ptproc) == p)
		tlb_must_refresh = 1;
#endif
	switch_address_space(p);

check_misc_flags:

	প্রতিজ্ঞা(p);
	প্রতিজ্ঞা(proc_is_runnable(p));
	যেতিয়ালৈ (p->p_misc_flags &
		(MF_KCALL_RESUME | MF_DELIVERMSG |
		 MF_SC_DEFER | MF_SC_TRACE | MF_SC_ACTIVE)) {

		প্রতিজ্ঞা(proc_is_runnable(p));
		যদি (p->p_misc_flags & MF_KCALL_RESUME) {
			kernel_call_resume(p);
		}
		অন্যথা যদি (p->p_misc_flags & MF_DELIVERMSG) {
			TRACE(VF_SCHEDULING, printf("delivering to %s / %d\n",
				p->p_name, p->p_endpoint););
			delivermsg(p);
		}
		অন্যথা যদি (p->p_misc_flags & MF_SC_DEFER) {
			/* Perform the system call that we deferred earlier. */

			প্রতিজ্ঞা (!(p->p_misc_flags & MF_SC_ACTIVE));

			arch_do_syscall(p);

			/* If the process is stopped for signal delivery, and
			 * not blocked sending a message after the system call,
			 * inform PM.
			 */
			যদি ((p->p_misc_flags & MF_SIG_DELAY) &&
					!RTS_ISSET(p, RTS_SENDING))
				sig_delay_done(p);
		}
		অন্যথা যদি (p->p_misc_flags & MF_SC_TRACE) {
			/* Trigger a system call leave event if this was a
			 * system call. We must do this after processing the
			 * other flags above, both for tracing correctness and
			 * to be able to use 'break'.
			 */
			যদি (!(p->p_misc_flags & MF_SC_ACTIVE))
				ভঙ্গ;

			p->p_misc_flags &=
				~(MF_SC_TRACE | MF_SC_ACTIVE);

			/* Signal the "leave system call" event.
			 * Block the process.
			 */
			cause_sig(proc_nr(p), SIGTRAP);
		}
		অন্যথা যদি (p->p_misc_flags & MF_SC_ACTIVE) {
			/* If MF_SC_ACTIVE was set, remove it now:
			 * we're leaving the system call.
			 */
			p->p_misc_flags &= ~MF_SC_ACTIVE;

			ভঙ্গ;
		}

		/*
		 * the selected process might not be runnable anymore. We have
		 * to checkit and schedule another one
		 */
		যদি (!proc_is_runnable(p))
			গচ্ছ not_runnable_pick_new;
	}
	/*
	 * check the quantum left before it runs again. We must do it only here
	 * as we are sure that a possible out-of-quantum message to the
	 * scheduler will not collide with the regular ipc
	 */
	যদি (!p->p_cpu_time_বাম)
		proc_no_time(p);
	/*
	 * After handling the misc flags the selected process might not be
	 * runnable anymore. We have to checkit and schedule another one
	 */
	যদি (!proc_is_runnable(p))
		গচ্ছ not_runnable_pick_new;

	TRACE(VF_SCHEDULING, printf("cpu %d starting %s / %d "
				"pc 0x%08x\n",
		cpuid, p->p_name, p->p_endpoint, p->p_reg.pc););
#যদি DEBUG_TRACE
	p->p_schedules++;
#endif

	p = arch_finish_switch_to_user();
	প্রতিজ্ঞা(p->p_cpu_time_বাম);

	context_stop(proc_addr(KERNEL));

	/* If the process isn't the owner of FPU, enable the FPU exception */
	যদি (get_cpulocal_var(fpu_owner) != p)
		enable_fpu_exception();
	অন্যথা
		disable_fpu_exception();

	/* If MF_CONTEXT_SET is set, don't clobber process state within
	 * the kernel. The next kernel entry is OK again though.
	 */
	p->p_misc_flags &= ~MF_CONTEXT_SET;

#যদি defined(__i386__)
  	প্রতিজ্ঞা(p->p_seg.p_cr3 != 0);
#নোচেদ্যদি defined(__arm__)
	প্রতিজ্ঞা(p->p_seg.p_ttbr != 0);
#endif
#ifdef CONFIG_SMP
	যদি (p->p_misc_flags & MF_FLUSH_TLB) {
		যদি (tlb_must_refresh)
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
স্থির পূর্ণাঙ্ক do_sync_ipc(সংরচনা proc * caller_ptr, /* who made the call */
			পূর্ণাঙ্ক call_nr,	/* system call number and flags */
			endpoint_t src_dst_e,	/* src or dst of the call */
			message *m_ptr)	/* users pointer to a message */
{
  পূর্ণাঙ্ক result;					/* the system call's result */
  পূর্ণাঙ্ক src_dst_p;				/* Process slot number */
  ৱর্ণ *callname;

  /* Check destination. RECEIVE is the only call that accepts ANY (in addition
   * to a real endpoint). The other calls (SEND, SENDREC, and NOTIFY) require an
   * endpoint to corresponds to a process. In addition, it is necessary to check
   * whether a process is allowed to send to a given destination.
   */
  প্রতিজ্ঞা(call_nr != SENDA);

  /* Only allow non-negative call_nr values less than 32 */
  যদি (call_nr < 0 || call_nr > IPCNO_HIGHEST || call_nr >= 32
      || !(callname = ipc_call_names[call_nr])) {
#যদি DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          call_nr, proc_nr(caller_ptr), src_dst_e);
#endif
	ঘূৰাই(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  যদি (src_dst_e == ANY)
  {
	যদি (call_nr != RECEIVE)
	{
#যদি 0
		printf("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#endif
		ঘূৰাই EINVAL;
	}
	src_dst_p = (পূর্ণাঙ্ক) src_dst_e;
  }
  অন্যথা
  {
	/* Require a valid source and/or destination process. */
	যদি(!isokendpt(src_dst_e, &src_dst_p)) {
#যদি 0
		printf("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#endif
		ঘূৰাই EDEADSRCDST;
	}

	/* If the call is to send to a process, i.e., for SEND, SENDNB,
	 * SENDREC or NOTIFY, verify that the caller is allowed to send to
	 * the given destination. 
	 */
	যদি (call_nr != RECEIVE)
	{
		যদি (!may_send_to(caller_ptr, src_dst_p)) {
#যদি DEBUG_ENABLE_IPC_WARNINGS
			printf(
			"sys_call: ipc mask denied %s from %d to %d\n",
				callname,
				caller_ptr->p_endpoint, src_dst_e);
#endif
			ঘূৰাই(ECALLDENIED);	/* call denied by ipc mask */
		}
	}
  }

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  যদি (!(priv(caller_ptr)->s_trap_mask & (1 << call_nr))) {
#যদি DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: %s not allowed, caller %d, src_dst %d\n", 
          callname, proc_nr(caller_ptr), src_dst_p);
#endif
	ঘূৰাই(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  যদি (call_nr != SENDREC && call_nr != RECEIVE && iskerneln(src_dst_p)) {
#যদি DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: trap %s not allowed, caller %d, src_dst %d\n",
           callname, proc_nr(caller_ptr), src_dst_e);
#endif
	ঘূৰাই(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ৱিকল্প(call_nr) {
  পক্ষ SENDREC:
	/* A flag is set so that notifications cannot interrupt SENDREC. */
	caller_ptr->p_misc_flags |= MF_REPLY_PEND;
	/* fall through */
  পক্ষ SEND:			
	result = mini_send(caller_ptr, src_dst_e, m_ptr, 0);
	যদি (call_nr == SEND || result != OK)
		ভঙ্গ;				/* done, or SEND failed */
	/* fall through for SENDREC */
  পক্ষ RECEIVE:			
	যদি (call_nr == RECEIVE) {
		caller_ptr->p_misc_flags &= ~MF_REPLY_PEND;
		IPC_STATUS_CLEAR(caller_ptr);  /* clear IPC status code */
	}
	result = mini_receive(caller_ptr, src_dst_e, m_ptr, 0);
	ভঙ্গ;
  পক্ষ NOTIFY:
	result = mini_notify(caller_ptr, src_dst_e);
	ভঙ্গ;
  পক্ষ SENDNB:
        result = mini_send(caller_ptr, src_dst_e, m_ptr, NON_BLOCKING);
        ভঙ্গ;
  স্ৱভাৱ:
	result = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  ঘূৰাই(result);
}

পূর্ণাঙ্ক do_ipc(reg_t r1, reg_t r2, reg_t r3)
{
  সংরচনা proc *ধ্রুৱ caller_ptr = get_cpulocal_var(proc_ptr);	/* get pointer to caller */
  পূর্ণাঙ্ক call_nr = (পূর্ণাঙ্ক) r1;

  প্রতিজ্ঞা(!RTS_ISSET(caller_ptr, RTS_SLOT_FREE));

  /* bill kernel time to this process. */
  kbill_ipc = caller_ptr;

  /* If this process is subject to system call tracing, handle that first. */
  যদি (caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
	/* Are we tracing this process, and is it the first sys_call entry? */
	যদি ((caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) ==
							MF_SC_TRACE) {
		/* We must notify the tracer before processing the actual
		 * system call. If we don't, the tracer could not obtain the
		 * input message. Postpone the entire system call.
		 */
		caller_ptr->p_misc_flags &= ~MF_SC_TRACE;
		প্রতিজ্ঞা(!(caller_ptr->p_misc_flags & MF_SC_DEFER));
		caller_ptr->p_misc_flags |= MF_SC_DEFER;
		caller_ptr->p_defer.r1 = r1;
		caller_ptr->p_defer.r2 = r2;
		caller_ptr->p_defer.r3 = r3;

		/* Signal the "enter system call" event. Block the process. */
		cause_sig(proc_nr(caller_ptr), SIGTRAP);

		/* Preserve the return register's value. */
		ঘূৰাই caller_ptr->p_reg.retreg;
	}

	/* If the MF_SC_DEFER flag is set, the syscall is now being resumed. */
	caller_ptr->p_misc_flags &= ~MF_SC_DEFER;

	প্রতিজ্ঞা (!(caller_ptr->p_misc_flags & MF_SC_ACTIVE));

	/* Set a flag to allow reliable tracing of leaving the system call. */
	caller_ptr->p_misc_flags |= MF_SC_ACTIVE;
  }

  যদি(caller_ptr->p_misc_flags & MF_DELIVERMSG) {
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
  ৱিকল্প(call_nr) {
  	পক্ষ SENDREC:
  	পক্ষ SEND:			
  	পক্ষ RECEIVE:			
  	পক্ষ NOTIFY:
  	পক্ষ SENDNB:
  	{
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_sync++;

  	    ঘূৰাই do_sync_ipc(caller_ptr, call_nr, (endpoint_t) r2,
			    (message *) r3);
  	}
  	পক্ষ SENDA:
  	{
 	    /*
  	     * Get and check the size of the argument in bytes as it is a
  	     * table
  	     */
  	    size_t বাৰ্তা_size = (size_t) r2;
  
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_async++;
 
  	    /* Limit size to something reasonable. An arbitrary choice is 16
  	     * times the number of process table entries.
  	     */
  	    যদি (বাৰ্তা_size > 16*(NR_TASKS + NR_PROCS))
	        ঘূৰাই EDOM;
  	    ঘূৰাই mini_senda(caller_ptr, (asynmsg_t *) r3, বাৰ্তা_size);
  	}
  	পক্ষ MINIX_KERNINFO:
	{
		/* It might not be initialized yet. */
	  	যদি(!minix_kerninfo_user) {
			ঘূৰাই EBADCALL;
		}

  		arch_set_secondary_ipc_return(caller_ptr, minix_kerninfo_user);
  		ঘূৰাই OK;
	}
  	স্ৱভাৱ:
	ঘূৰাই EBADCALL;		/* illegal system call */
  }
}

/*===========================================================================*
 *				deadlock				     * 
 *===========================================================================*/
স্থির পূর্ণাঙ্ক deadlock(
  পূর্ণাঙ্ক function,				/* trap number */
  পঞ্জিকা সংরচনা proc *cp,		/* pointer to caller */
  endpoint_t src_dst_e			/* src or dst process */
)
{
/* Check for deadlock. This can happen if 'caller_ptr' and 'src_dst' have
 * a cyclic dependency of blocking send and receive calls. The only cyclic 
 * dependency that is not fatal is if the caller and target directly SEND(REC)
 * and RECEIVE to each other. If a deadlock is found, the group size is 
 * returned. Otherwise zero is returned. 
 */
  পঞ্জিকা সংরচনা proc *xp;			/* process pointer */
  পূর্ণাঙ্ক group_size = 1;				/* start with only caller */
#যদি DEBUG_ENABLE_IPC_WARNINGS
  স্থির সংরচনা proc *processes[NR_PROCS + NR_TASKS];
  processes[0] = cp;
#endif

  যেতিয়ালৈ (src_dst_e != ANY) { 			/* check while process nr */
      পূর্ণাঙ্ক src_dst_slot;
      okendpt(src_dst_e, &src_dst_slot);
      xp = proc_addr(src_dst_slot);		/* follow chain of processes */
      প্রতিজ্ঞা(proc_ptr_ok(xp));
      প্রতিজ্ঞা(!RTS_ISSET(xp, RTS_SLOT_FREE));
#যদি DEBUG_ENABLE_IPC_WARNINGS
      processes[group_size] = xp;
#endif
      group_size ++;				/* extra process in group */

      /* Check whether the last process in the chain has a dependency. If it 
       * has not, the cycle cannot be closed and we are done.
       */
      যদি((src_dst_e = P_BLOCKEDON(xp)) == NONE)
	ঘূৰাই 0;

      /* Now check if there is a cyclic dependency. For group sizes of two,  
       * a combination of SEND(REC) and RECEIVE is not fatal. Larger groups
       * or other combinations indicate a deadlock.  
       */
      যদি (src_dst_e == cp->p_endpoint) {	/* possible deadlock */
	  যদি (group_size == 2) {		/* caller and src_dst */
	      /* The function number is magically converted to flags. */
	      যদি ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING) { 
	          ঘূৰাই(0);			/* not a deadlock */
	      }
	  }
#যদি DEBUG_ENABLE_IPC_WARNINGS
	  {
		পূর্ণাঙ্ক i;
		printf("deadlock between these processes:\n");
		বাবে(i = 0; i < group_size; i++) {
			printf(" %10s ", processes[i]->p_name);
		}
		printf("\n\n");
		বাবে(i = 0; i < group_size; i++) {
			print_proc(processes[i]);
			proc_stacktrace(processes[i]);
		}
	  }
#endif
          ঘূৰাই(group_size);			/* deadlock found */
      }
  }
  ঘূৰাই(0);					/* not a deadlock */
}

/*===========================================================================*
 *				has_pending				     * 
 *===========================================================================*/
স্থির পূর্ণাঙ্ক has_pending(sys_map_t *map, পূর্ণাঙ্ক src_p, পূর্ণাঙ্ক asynm)
{
/* Check to see if there is a pending message from the desired source
 * available.
 */

  পূর্ণাঙ্ক src_id;
  sys_id_t id = NULL_PRIV_ID;
#ifdef CONFIG_SMP
  সংরচনা proc * p;
#endif

  /* Either check a specific bit in the mask map, or find the first bit set in
   * it (if any), depending on whether the receive was called on a specific
   * source endpoint.
   */
  যদি (src_p != ANY) {
	src_id = nr_to_id(src_p);
	যদি (get_sys_bit(*map, src_id)) {
#ifdef CONFIG_SMP
		p = proc_addr(id_to_nr(src_id));
		যদি (asynm && RTS_ISSET(p, RTS_VMINHIBIT))
			p->p_misc_flags |= MF_SENDA_VM_MISS;
		অন্যথা
#endif
			id = src_id;
	}
  } অন্যথা {
	/* Find a source with a pending message */
	বাবে (src_id = 0; src_id < NR_SYS_PROCS; src_id += BITCHUNK_BITS) {
		যদি (get_sys_bits(*map, src_id) != 0) {
#ifdef CONFIG_SMP
			যেতিয়ালৈ (src_id < NR_SYS_PROCS) {
				যেতিয়ালৈ (!get_sys_bit(*map, src_id)) {
					যদি (src_id == NR_SYS_PROCS)
						গচ্ছ quit_অনুসন্ধান;
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
				যদি (asynm && RTS_ISSET(p, RTS_VMINHIBIT)) {
					p->p_misc_flags |= MF_SENDA_VM_MISS;
					src_id++;
				} অন্যথা
					গচ্ছ quit_অনুসন্ধান;
			}
#অন্যথা
			যেতিয়ালৈ (!get_sys_bit(*map, src_id)) src_id++;
			গচ্ছ quit_অনুসন্ধান;
#endif
		}
	}

quit_অনুসন্ধান:
	যদি (src_id < NR_SYS_PROCS)	/* Found one */
		id = src_id;
  }

  ঘূৰাই(id);
}

/*===========================================================================*
 *				has_pending_notify			     *
 *===========================================================================*/
পূর্ণাঙ্ক has_pending_notify(সংরচনা proc * caller, পূর্ণাঙ্ক src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	ঘূৰাই has_pending(map, src_p, 0);
}

/*===========================================================================*
 *				has_pending_asend			     *
 *===========================================================================*/
পূর্ণাঙ্ক has_pending_asend(সংরচনা proc * caller, পূর্ণাঙ্ক src_p)
{
	sys_map_t * map = &priv(caller)->s_asyn_pending;
	ঘূৰাই has_pending(map, src_p, 1);
}

/*===========================================================================*
 *				unset_notify_pending			     *
 *===========================================================================*/
শূন্য unset_notify_pending(সংরচনা proc * caller, পূর্ণাঙ্ক src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	unset_sys_bit(*map, src_p);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
পূর্ণাঙ্ক mini_send(
  পঞ্জিকা সংরচনা proc *caller_ptr,	/* who is trying to send a message? */
  endpoint_t dst_e,			/* to whom is message being sent? */
  message *m_ptr,			/* pointer to message buffer */
  ধ্রুৱ পূর্ণাঙ্ক flags
)
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  পঞ্জিকা সংরচনা proc *dst_ptr;
  পঞ্জিকা সংরচনা proc **xpp;
  পূর্ণাঙ্ক dst_p;
  dst_p = _ENDPOINT_P(dst_e);
  dst_ptr = proc_addr(dst_p);

  যদি (RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT))
  {
	ঘূৰাই EDEADSRCDST;
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * RTS_SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  যদি (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, (vir_bytes)m_ptr, রিক্ত)) {
	পূর্ণাঙ্ক call;
	/* Destination is indeed waiting for this message. */
	প্রতিজ্ঞা(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));	

	যদি (!(flags & FROM_KERNEL)) {
		যদি(copy_বাৰ্তা_from_user(m_ptr, &dst_ptr->p_delivermsg))
			ঘূৰাই EFAULT;
	} অন্যথা {
		dst_ptr->p_delivermsg = *m_ptr;
		IPC_STATUS_ADD_FLAGS(dst_ptr, IPC_FLG_বাৰ্তা_FROM_KERNEL);
	}

	dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;

	call = (caller_ptr->p_misc_flags & MF_REPLY_PEND ? SENDREC
		: (flags & NON_BLOCKING ? SENDNB : SEND));
	IPC_STATUS_ADD_CALL(dst_ptr, call);

	যদি (dst_ptr->p_misc_flags & MF_REPLY_PEND)
		dst_ptr->p_misc_flags &= ~MF_REPLY_PEND;

	RTS_UNSET(dst_ptr, RTS_RECEIVING);

#যদি DEBUG_IPC_HOOK
	hook_ipc_msgsend(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#endif
  } অন্যথা {
	যদি(flags & NON_BLOCKING) {
		ঘূৰাই(ENOTREADY);
	}

	/* Check for a possible deadlock before actually blocking. */
	যদি (deadlock(SEND, caller_ptr, dst_e)) {
		ঘূৰাই(ELOCKED);
	}

	/* Destination is not waiting.  Block and dequeue caller. */
	যদি (!(flags & FROM_KERNEL)) {
		যদি(copy_বাৰ্তা_from_user(m_ptr, &caller_ptr->p_sendmsg))
			ঘূৰাই EFAULT;
	} অন্যথা {
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
	প্রতিজ্ঞা(caller_ptr->p_q_link == রিক্ত);
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	যেতিয়ালৈ (*xpp) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */

#যদি DEBUG_IPC_HOOK
	hook_ipc_msgsend(&caller_ptr->p_sendmsg, caller_ptr, dst_ptr);
#endif
  }
  ঘূৰাই(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
স্থির পূর্ণাঙ্ক mini_receive(সংরচনা proc * caller_ptr,
			endpoint_t src_e, /* which message source is wanted */
			message * m_buff_usr, /* pointer to message buffer */
			ধ্রুৱ পূর্ণাঙ্ক flags)
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller.
 */
  পঞ্জিকা সংরচনা proc **xpp;
  পূর্ণাঙ্ক r, src_id, found, src_proc_nr, src_p;
  endpoint_t sender_e;

  প্রতিজ্ঞা(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));

  /* This is where we want our message. */
  caller_ptr->p_delivermsg_vir = (vir_bytes) m_buff_usr;

  যদি(src_e == ANY) src_p = ANY;
  অন্যথা
  {
	okendpt(src_e, &src_p);
	যদি (RTS_ISSET(proc_addr(src_p), RTS_NO_ENDPOINT))
	{
		ঘূৰাই EDEADSRCDST;
	}
  }


  /* Check to see if a message from desired source is already available.  The
   * caller's RTS_SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  যদি (!RTS_ISSET(caller_ptr, RTS_SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    যদি (! (caller_ptr->p_misc_flags & MF_REPLY_PEND)) {

	/* Check for pending notifications */
        src_id = has_pending_notify(caller_ptr, src_p);
        found = src_id != NULL_PRIV_ID;
        যদি(found) {
            src_proc_nr = id_to_nr(src_id);		/* get source proc */
            sender_e = proc_addr(src_proc_nr)->p_endpoint;
        }

        যদি (found && CANRECEIVE(src_e, sender_e, caller_ptr, 0,
          &m_notify_buff)) {

#যদি DEBUG_ENABLE_IPC_WARNINGS
	    যদি(src_proc_nr == NONE) {
		printf("mini_receive: sending notify from NONE\n");
	    }
#endif
	    প্রতিজ্ঞা(src_proc_nr != NONE);
            unset_notify_pending(caller_ptr, src_id);	/* no longer pending */

            /* Found a suitable source, deliver the notification message. */
	    প্রতিজ্ঞা(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));	
	    প্রতিজ্ঞা(src_e == ANY || sender_e == src_e);

	    /* assemble message */
	    BuildNotifyMessage(&caller_ptr->p_delivermsg, src_proc_nr, caller_ptr);
	    caller_ptr->p_delivermsg.m_source = sender_e;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;

	    IPC_STATUS_ADD_CALL(caller_ptr, NOTIFY);

	    গচ্ছ receive_done;
        }
    }

    /* Check for pending asynchronous messages */
    যদি (has_pending_asend(caller_ptr, src_p) != NULL_PRIV_ID) {
        যদি (src_p != ANY)
		r = try_one(src_e, proc_addr(src_p), caller_ptr);
        অন্যথা
        	r = try_async(caller_ptr);

	যদি (r == OK) {
            IPC_STATUS_ADD_CALL(caller_ptr, SENDA);
            গচ্ছ receive_done;
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    যেতিয়ালৈ (*xpp) {
	সংরচনা proc * sender = *xpp;
	endpoint_t sender_e = sender->p_endpoint;

        যদি (CANRECEIVE(src_e, sender_e, caller_ptr, 0, &sender->p_sendmsg)) {
            পূর্ণাঙ্ক call;
	    প্রতিজ্ঞা(!RTS_ISSET(sender, RTS_SLOT_FREE));
	    প্রতিজ্ঞা(!RTS_ISSET(sender, RTS_NO_ENDPOINT));

	    /* Found acceptable message. Copy it and update status. */
  	    প্রতিজ্ঞা(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
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
	    যদি (sender->p_misc_flags & MF_SENDING_FROM_KERNEL) {
		IPC_STATUS_ADD_FLAGS(caller_ptr, IPC_FLG_বাৰ্তা_FROM_KERNEL);
		/* we can clean the flag now, not need anymore */
		sender->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
	    }
	    যদি (sender->p_misc_flags & MF_SIG_DELAY)
		sig_delay_done(sender);

#যদি DEBUG_IPC_HOOK
            hook_ipc_msgrecv(&caller_ptr->p_delivermsg, *xpp, caller_ptr);
#endif
		
            *xpp = sender->p_q_link;		/* remove from queue */
	    sender->p_q_link = রিক্ত;
	    গচ্ছ receive_done;
	}
	xpp = &sender->p_q_link;		/* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC. 
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  যদি ( ! (flags & NON_BLOCKING)) {
      /* Check for a possible deadlock before actually blocking. */
      যদি (deadlock(RECEIVE, caller_ptr, src_e)) {
          ঘূৰাই(ELOCKED);
      }

      caller_ptr->p_getfrom_e = src_e;		
      RTS_SET(caller_ptr, RTS_RECEIVING);
      ঘূৰাই(OK);
  } অন্যথা {
	ঘূৰাই(ENOTREADY);
  }

receive_done:
  যদি (caller_ptr->p_misc_flags & MF_REPLY_PEND)
	  caller_ptr->p_misc_flags &= ~MF_REPLY_PEND;
  ঘূৰাই OK;
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
পূর্ণাঙ্ক mini_notify(
  ধ্রুৱ সংরচনা proc *caller_ptr,	/* sender of the notification */
  endpoint_t dst_e			/* which process to notify */
)
{
  পঞ্জিকা সংরচনা proc *dst_ptr;
  পূর্ণাঙ্ক src_id;				/* source id for late delivery */
  পূর্ণাঙ্ক dst_p;

  যদি (!isokendpt(dst_e, &dst_p)) {
	util_stacktrace();
	printf("mini_notify: bogus endpoint %d\n", dst_e);
	ঘূৰাই EDEADSRCDST;
  }

  dst_ptr = proc_addr(dst_p);

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  যদি (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, 0, &m_notify_buff) &&
    !(dst_ptr->p_misc_flags & MF_REPLY_PEND)) {
      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      প্রতিজ্ঞা(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));

      BuildNotifyMessage(&dst_ptr->p_delivermsg, proc_nr(caller_ptr), dst_ptr);
      dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;

      IPC_STATUS_ADD_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING);

      ঘূৰাই(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the privilege id
   * instead of the process number is used in the pending bit map.
   */ 
  src_id = priv(caller_ptr)->s_id;
  set_sys_bit(priv(dst_ptr)->s_notify_pending, src_id); 
  ঘূৰাই(OK);
}

#define ASCOMPLAIN(caller, entry, field)	\
	printf("kernel:%s:%d: asyn failed for %s in %s "	\
	"(%d/%zu, tab 0x%lx)\n",__FILE__,__LINE__,	\
field, caller->p_name, entry, priv(caller)->s_asynsize, priv(caller)->s_asyntab)

#define A_RETR(entry) কৰা {			\
  যদি (data_copy(				\
  		caller_ptr->p_endpoint, table_v + (entry)*পরিমাণ(asynmsg_t),\
  		KERNEL, (vir_bytes) &tabent,	\
  		পরিমাণ(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, entry, "message entry");	\
  			r = EFAULT;		\
	                গচ্ছ asyn_error; \
  }						\
  অন্যথা যদি(tabent.dst == SELF) { \
      tabent.dst = caller_ptr->p_endpoint; \
  } \
  			 } যেতিয়ালৈ(0)

#define A_INSRT(entry) কৰা {			\
  যদি (data_copy(KERNEL, (vir_bytes) &tabent,	\
  		caller_ptr->p_endpoint, table_v + (entry)*পরিমাণ(asynmsg_t),\
  		পরিমাণ(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, entry, "message entry");	\
			/* Do NOT set r or goto asyn_error here! */ \
  }						\
  			  } যেতিয়ালৈ(0)	

/*===========================================================================*
 *				try_deliver_senda			     *
 *===========================================================================*/
পূর্ণাঙ্ক try_deliver_senda(সংরচনা proc *caller_ptr,
				asynmsg_t *table,
				size_t size)
{
  পূর্ণাঙ্ক r, dst_p, done, do_notify;
  unsigned পূর্ণাঙ্ক i;
  unsigned flags;
  endpoint_t dst;
  সংরচনা proc *dst_ptr;
  সংরচনা priv *privp;
  asynmsg_t tabent;
  ধ্রুৱ vir_bytes table_v = (vir_bytes) table;
  message *m_ptr = রিক্ত;

  privp = priv(caller_ptr);

  /* Clear table */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  privp->s_asynendpoint = caller_ptr->p_endpoint;

  যদি (size == 0) ঘূৰাই(OK);  /* Nothing to do, just return */

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;

  /* Limit size to something reasonable. An arbitrary choice is 16
   * times the number of process table entries.
   *
   * (this check has been duplicated in sys_call but is left here
   * as a sanity check)
   */
  যদি (size > 16*(NR_TASKS + NR_PROCS)) {
    r = EDOM;
    ঘূৰাই r;
  }

  বাবে (i = 0; i < size; i++) {
	/* Process each entry in the table and store the result in the table.
	 * If we're done handling a message, copy the result to the sender. */

	dst = NONE;
	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	যদি (flags == 0) অনুৱর্ত; /* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	যদি(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR)) {
		r = EINVAL;
		গচ্ছ asyn_error;
	}
	যদি (!(flags & AMF_VALID)) { /* Must contain message */
		r = EINVAL;
		গচ্ছ asyn_error;
	}
	যদি (flags & AMF_DONE) অনুৱর্ত;	/* Already done processing */

	r = OK;
	যদি (!isokendpt(tabent.dst, &dst_p)) 
		r = EDEADSRCDST; /* Bad destination, report the error */
	অন্যথা যদি (iskerneln(dst_p)) 
		r = ECALLDENIED; /* Asyn sends to the kernel are not allowed */
	অন্যথা যদি (!may_asynsend_to(caller_ptr, dst_p))
		r = ECALLDENIED; /* Send denied by IPC mask */
	অন্যথা 	/* r == OK */
		dst_ptr = proc_addr(dst_p);

	/* XXX: RTS_NO_ENDPOINT should be removed */
	যদি (r == OK && RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT)) {
		r = EDEADSRCDST;
	}

	/* Check if 'dst' is blocked waiting for this message.
	 * If AMF_NOREPLY is set, do not satisfy the receiving part of
	 * a SENDREC.
	 */
	যদি (r == OK && WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr,
	    (vir_bytes)&table[i].বাৰ্তা, রিক্ত) &&
	    (!(flags&AMF_NOREPLY) || !(dst_ptr->p_misc_flags&MF_REPLY_PEND))) {
		/* Destination is indeed waiting for this message. */
		dst_ptr->p_delivermsg = tabent.বাৰ্তা;
		dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
		dst_ptr->p_misc_flags |= MF_DELIVERMSG;
		IPC_STATUS_ADD_CALL(dst_ptr, SENDA);
		RTS_UNSET(dst_ptr, RTS_RECEIVING);
#যদি DEBUG_IPC_HOOK
		hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#endif
	} অন্যথা যদি (r == OK) {
		/* Inform receiver that something is pending */
		set_sys_bit(priv(dst_ptr)->s_asyn_pending, 
			    priv(caller_ptr)->s_id); 
		done = FALSE;
		অনুৱর্ত;
	} 

	/* Store results */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	যদি (flags & AMF_NOTIFY)
		do_notify = TRUE;
	অন্যথা যদি (r != OK && (flags & AMF_NOTIFY_ERR))
		do_notify = TRUE;
	A_INSRT(i);	/* Copy results to caller; ignore errors */
	অনুৱর্ত;

asyn_error:
	যদি (dst != NONE)
		printf("KERNEL senda error %d to %d\n", r, dst);
	অন্যথা
		printf("KERNEL senda error %d\n", r);
  }

  যদি (do_notify) 
	mini_notify(proc_addr(ASYNCM), caller_ptr->p_endpoint);

  যদি (!done) {
	privp->s_asyntab = (vir_bytes) table;
	privp->s_asynsize = size;
  }

  ঘূৰাই(OK);
}

/*===========================================================================*
 *				mini_senda				     *
 *===========================================================================*/
স্থির পূর্ণাঙ্ক mini_senda(সংরচনা proc *caller_ptr, asynmsg_t *table, size_t size)
{
  সংরচনা priv *privp;

  privp = priv(caller_ptr);
  যদি (!(privp->s_flags & SYS_PROC)) {
	printf( "mini_senda: warning caller has no privilege structure\n");
	ঘূৰাই(EPERM);
  }

  ঘূৰাই try_deliver_senda(caller_ptr, table, size);
}


/*===========================================================================*
 *				try_async				     * 
 *===========================================================================*/
স্থির পূর্ণাঙ্ক try_async(সংরচনা proc * caller_ptr)
{
  পূর্ণাঙ্ক r;
  সংরচনা priv *privp;
  সংরচনা proc *src_ptr;
  sys_map_t *map;

  map = &priv(caller_ptr)->s_asyn_pending;

  /* Try all privilege structures */
  বাবে (privp = BEG_PRIV_ADDR; privp < END_PRIV_ADDR; ++privp)  {
	যদি (privp->s_proc_nr == NONE)
		অনুৱর্ত;

	যদি (!get_sys_bit(*map, privp->s_id)) 
		অনুৱর্ত;

	src_ptr = proc_addr(privp->s_proc_nr);

#ifdef CONFIG_SMP
	/*
	 * Do not copy from a process which does not have a stable address space
	 * due to VM fiddling with it
	 */
	যদি (RTS_ISSET(src_ptr, RTS_VMINHIBIT)) {
		src_ptr->p_misc_flags |= MF_SENDA_VM_MISS;
		অনুৱর্ত;
	}
#endif

	প্রতিজ্ঞা(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	যদি ((r = try_one(ANY, src_ptr, caller_ptr)) == OK)
		ঘূৰাই(r);
  }

  ঘূৰাই(ESRCH);
}


/*===========================================================================*
 *				try_one					     *
 *===========================================================================*/
স্থির পূর্ণাঙ্ক try_one(endpoint_t receive_e, সংরচনা proc *src_ptr,
    সংরচনা proc *dst_ptr)
{
/* Try to receive an asynchronous message from 'src_ptr' */
  পূর্ণাঙ্ক r = EAGAIN, done, do_notify;
  unsigned পূর্ণাঙ্ক flags, i;
  size_t size;
  endpoint_t dst, src_e;
  সংরচনা proc *caller_ptr;
  সংরচনা priv *privp;
  asynmsg_t tabent;
  vir_bytes table_v;

  privp = priv(src_ptr);
  যদি (!(privp->s_flags & SYS_PROC)) ঘূৰাই(EPERM);
  size = privp->s_asynsize;
  table_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  unset_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  যদি (size == 0) ঘূৰাই(EAGAIN);
  যদি (privp->s_asynendpoint != src_ptr->p_endpoint) ঘূৰাই EAGAIN;
  যদি (!may_asynsend_to(src_ptr, proc_nr(dst_ptr))) ঘূৰাই (ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */
  src_e = src_ptr->p_endpoint;

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;

  বাবে (i = 0; i < size; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the meantime.
  	 */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	যদি (flags == 0) অনুৱর্ত;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	যদি(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	অন্যথা যদি (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	অন্যথা যদি (flags & AMF_DONE) অনুৱর্ত; /* Already done processing */

	/* Clear done flag. The sender is done sending when all messages in the
	 * table are marked done or empty. However, we will know that only
	 * the next time we enter this function or when the sender decides to
	 * send additional asynchronous messages and manages to deliver them
	 * all.
	 */
	done = FALSE;

	যদি (r == EINVAL)
		গচ্ছ store_result;

	/* Message must be directed at receiving end */
	যদি (dst != dst_ptr->p_endpoint) অনুৱর্ত;

	যদি (!CANRECEIVE(receive_e, src_e, dst_ptr,
		table_v + i*পরিমাণ(asynmsg_t) + offsetof(সংরচনা asynmsg,বাৰ্তা),
		রিক্ত)) {
		অনুৱর্ত;
	}

	/* If AMF_NOREPLY is set, then this message is not a reply to a
	 * SENDREC and thus should not satisfy the receiving part of the
	 * SENDREC. This message is to be delivered later.
	 */
	যদি ((flags & AMF_NOREPLY) && (dst_ptr->p_misc_flags & MF_REPLY_PEND)) 
		অনুৱর্ত;

	/* Destination is ready to receive the message; deliver it */
	r = OK;
	dst_ptr->p_delivermsg = tabent.বাৰ্তা;
	dst_ptr->p_delivermsg.m_source = src_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;
#যদি DEBUG_IPC_HOOK
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, src_ptr, dst_ptr);
#endif

store_result:
	/* Store results for sender. We may just have started delivering a
	 * message, so we must not return an error to the caller in the case
	 * that storing the results triggers an error!
	 */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	যদি (flags & AMF_NOTIFY) do_notify = TRUE;
	অন্যথা যদি (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */

	ভঙ্গ;
  }

  যদি (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  যদি (done) {
	privp->s_asyntab = -1;
	privp->s_asynsize = 0;
  } অন্যথা {
	set_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);
  }

asyn_error:
  ঘূৰাই(r);
}

/*===========================================================================*
 *				cancel_async				     *
 *===========================================================================*/
পূর্ণাঙ্ক cancel_async(সংরচনা proc *src_ptr, সংরচনা proc *dst_ptr)
{
/* Cancel asynchronous messages from src to dst, because dst is not interested
 * in them (e.g., dst has been restarted) */
  পূর্ণাঙ্ক done, do_notify;
  unsigned পূর্ণাঙ্ক flags, i;
  size_t size;
  endpoint_t dst;
  সংরচনা proc *caller_ptr;
  সংরচনা priv *privp;
  asynmsg_t tabent;
  vir_bytes table_v;

  privp = priv(src_ptr);
  যদি (!(privp->s_flags & SYS_PROC)) ঘূৰাই(EPERM);
  size = privp->s_asynsize;
  table_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  unset_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  যদি (size == 0) ঘূৰাই(EAGAIN);
  যদি (!may_send_to(src_ptr, proc_nr(dst_ptr))) ঘূৰাই(ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;


  বাবে (i = 0; i < size; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the mean time.
  	 */

  	পূর্ণাঙ্ক r = EDEADSRCDST;	/* Cancel delivery due to dead dst */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	যদি (flags == 0) অনুৱর্ত;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	যদি(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	অন্যথা যদি (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	অন্যথা যদি (flags & AMF_DONE) অনুৱর্ত; /* Already done processing */

	/* Message must be directed at receiving end */
	যদি (dst != dst_ptr->p_endpoint) {
		done = FALSE;
		অনুৱর্ত;
	}

	/* Store results for sender */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	যদি (flags & AMF_NOTIFY) do_notify = TRUE;
	অন্যথা যদি (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */
  }

  যদি (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  যদি (!done) {
	privp->s_asyntab = table_v;
	privp->s_asynsize = size;
  }

asyn_error:
  ঘূৰাই(OK);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
শূন্য enqueue(
  পঞ্জিকা সংরচনা proc *rp	/* this process is now runnable */
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
  পূর্ণাঙ্ক q = rp->p_priority;	 		/* scheduling queue to use */
  সংরচনা proc **rdy_head, **rdy_tail;
  
  প্রতিজ্ঞা(proc_is_runnable(rp));

  প্রতিজ্ঞা(q >= 0);

  rdy_head = get_cpu_var(rp->p_cpu, run_q_head);
  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now add the process to the queue. */
  যদি (!rdy_head[q]) {		/* add to empty queue */
      rdy_head[q] = rdy_tail[q] = rp; 		/* create a new queue */
      rp->p_nextready = রিক্ত;		/* mark new end */
  } 
  অন্যথা {					/* add to tail of queue */
      rdy_tail[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_tail[q] = rp;				/* set new queue tail */
      rp->p_nextready = রিক্ত;		/* mark new end */
  }

  যদি (cpuid == rp->p_cpu) {
	  /*
	   * enqueueing a process with a higher priority than the current one,
	   * it gets preempted. The current process must be preemptible. Testing
	   * the priority also makes sure that a process does not preempt itself
	   */
	  সংরচনা proc * p;
	  p = get_cpulocal_var(proc_ptr);
	  প্রতিজ্ঞা(p);
	  যদি((p->p_priority > rp->p_priority) &&
			  (priv(p)->s_flags & PREEMPTIBLE))
		  RTS_SET(p, RTS_PREEMPTED); /* calls dequeue() */
  }
#ifdef CONFIG_SMP
  /*
   * if the process was enqueued on a different cpu and the cpu is idle, i.e.
   * the time is off, we need to wake up that cpu and let it schedule this new
   * process
   */
  অন্যথা যদি (get_cpu_var(rp->p_cpu, cpu_is_idle)) {
	  smp_schedule(rp->p_cpu);
  }
#endif

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr)->p_accounting.enter_queue));


#যদি DEBUG_SANITYCHECKS
  প্রতিজ্ঞা(runqueues_ok_local());
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
স্থির শূন্য enqueue_head(সংরচনা proc *rp)
{
  ধ্রুৱ পূর্ণাঙ্ক q = rp->p_priority;	 		/* scheduling queue to use */

  সংরচনা proc **rdy_head, **rdy_tail;

  প্রতিজ্ঞা(proc_ptr_ok(rp));
  প্রতিজ্ঞা(proc_is_runnable(rp));

  /*
   * the process was runnable without its quantum expired when dequeued. A
   * process with no time left should have been handled else and differently
   */
  প্রতিজ্ঞা(rp->p_cpu_time_বাম);

  প্রতিজ্ঞা(q >= 0);


  rdy_head = get_cpu_var(rp->p_cpu, run_q_head);
  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now add the process to the queue. */
  যদি (!rdy_head[q]) {		/* add to empty queue */
	rdy_head[q] = rdy_tail[q] = rp; 	/* create a new queue */
	rp->p_nextready = রিক্ত;			/* mark new end */
  } অন্যথা {					/* add to head of queue */
	rp->p_nextready = rdy_head[q];		/* chain head of queue */
	rdy_head[q] = rp;			/* set new queue head */
  }

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr->p_accounting.enter_queue)));


  /* Process accounting for scheduling */
  rp->p_accounting.dequeues--;
  rp->p_accounting.preempted++;

#যদি DEBUG_SANITYCHECKS
  প্রতিজ্ঞা(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
শূন্য dequeue(সংরচনা proc *rp)
/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 *
 * This function can operate x-cpu as it always removes the process from the
 * queue of the cpu the process is currently assigned to.
 */
  পূর্ণাঙ্ক q = rp->p_priority;		/* queue to use */
  সংরচনা proc **xpp;			/* iterate over queue */
  সংরচনা proc *prev_xp;
  u64_t tsc, tsc_delta;

  সংরচনা proc **rdy_tail;

  প্রতিজ্ঞা(proc_ptr_ok(rp));
  প্রতিজ্ঞা(!proc_is_runnable(rp));

  /* Side-effect for kernel: check if the task's stack still is ok? */
  প্রতিজ্ঞা (!iskernelp(rp) || *priv(rp)->s_স্তূপ_guard == স্তূপ_GUARD);

  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  prev_xp = রিক্ত;				
  বাবে (xpp = get_cpu_var_ptr(rp->p_cpu, run_q_head[q]); *xpp;
		  xpp = &(*xpp)->p_nextready) {
      যদি (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          যদি (rp == rdy_tail[q]) {		/* queue tail removed */
              rdy_tail[q] = prev_xp;		/* set new tail */
	  }

          ভঙ্গ;
      }
      prev_xp = *xpp;				/* save previous in chain */
  }

	
  /* Process accounting for scheduling */
  rp->p_accounting.dequeues++;

  /* this is not all that accurate on virtual machines, especially with
     IO bound processes that only spend a short amount of time in the queue
     at a time. */
  যদি (rp->p_accounting.enter_queue) {
	read_tsc_64(&tsc);
	tsc_delta = tsc - rp->p_accounting.enter_queue;
	rp->p_accounting.time_in_queue = rp->p_accounting.time_in_queue +
		tsc_delta;
	rp->p_accounting.enter_queue = 0;
  }

  /* For ps(1), remember when the process was last dequeued. */
  rp->p_dequeued = get_monotonic();

#যদি DEBUG_SANITYCHECKS
  প্রতিজ্ঞা(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
স্থির সংরচনা proc * pick_proc(শূন্য)
{
/* Decide who to run now.  A new process is selected and returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
  পঞ্জিকা সংরচনা proc *rp;			/* process to run */
  সংরচনা proc **rdy_head;
  পূর্ণাঙ্ক q;				/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the task table.
   * If there are no processes ready to run, return NULL.
   */
  rdy_head = get_cpulocal_var(run_q_head);
  বাবে (q=0; q < NR_SCHED_QUEUES; q++) {	
	যদি(!(rp = rdy_head[q])) {
		TRACE(VF_PICKPROC, printf("cpu %d queue %d empty\n", cpuid, q););
		অনুৱর্ত;
	}
	প্রতিজ্ঞা(proc_is_runnable(rp));
	যদি (priv(rp)->s_flags & BILLABLE)	 	
		get_cpulocal_var(bill_ptr) = rp; /* bill for system time */
	ঘূৰাই rp;
  }
  ঘূৰাই রিক্ত;
}

/*===========================================================================*
 *				endpoint_lookup				     *
 *===========================================================================*/
সংরচনা proc *endpoint_lookup(endpoint_t e)
{
	পূর্ণাঙ্ক n;

	যদি(!isokendpt(e, &n)) ঘূৰাই রিক্ত;

	ঘূৰাই proc_addr(n);
}

/*===========================================================================*
 *				isokendpt_f				     *
 *===========================================================================*/
#যদি DEBUG_ENABLE_IPC_WARNINGS
পূর্ণাঙ্ক isokendpt_f(ধ্রুৱ ৱর্ণ * file, পূর্ণাঙ্ক সাৰি, endpoint_t e, পূর্ণাঙ্ক * p,
	ধ্রুৱ পূর্ণাঙ্ক fatalflag)
#অন্যথা
পূর্ণাঙ্ক isokendpt_f(endpoint_t e, পূর্ণাঙ্ক * p, ধ্রুৱ পূর্ণাঙ্ক fatalflag)
#endif
{
	পূর্ণাঙ্ক ok = 0;
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
	যদি(isokprocn(*p) && !isemptyn(*p) && proc_addr(*p)->p_endpoint == e)
		ok = 1;
	যদি(!ok && fatalflag)
		panic("invalid endpoint: %d",  e);
	ঘূৰাই ok;
}

স্থির শূন্য notify_scheduler(সংরচনা proc *p)
{
	message m_no_quantum;
	পূর্ণাঙ্ক err;

	প্রতিজ্ঞা(!proc_kernel_scheduler(p));

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

	যদি ((err = mini_send(p, p->p_scheduler->p_endpoint,
					&m_no_quantum, FROM_KERNEL))) {
		panic("WARNING: Scheduling: mini_send returned %d\n", err);
	}
}

শূন্য proc_no_time(সংরচনা proc * p)
{
	যদি (!proc_kernel_scheduler(p) && priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		notify_scheduler(p);
	}
	অন্যথা {
		/*
		 * non-preemptible processes only need their quantum to
		 * be renewed. In fact, they by pass scheduling
		 */
		p->p_cpu_time_বাম = ms_2_cpu_time(p->p_quantum_size_ms);
#যদি DEBUG_RACE
		RTS_SET(p, RTS_PREEMPTED);
		RTS_UNSET(p, RTS_PREEMPTED);
#endif
	}
}

শূন্য reset_proc_accounting(সংরচনা proc *p)
{
  p->p_accounting.preempted = 0;
  p->p_accounting.ipc_sync  = 0;
  p->p_accounting.ipc_async = 0;
  p->p_accounting.dequeues  = 0;
  p->p_accounting.time_in_queue = 0;
  p->p_accounting.enter_queue = 0;
}
	
শূন্য copr_not_available_handler(শূন্য)
{
	সংরচনা proc * p;
	সংরচনা proc ** local_fpu_owner;
	/*
	 * Disable the FPU exception (both for the kernel and for the process
	 * once it's scheduled), and initialize or restore the FPU state.
	 */

	disable_fpu_exception();

	p = get_cpulocal_var(proc_ptr);

	/* if FPU is not owned by anyone, do not store anything */
	local_fpu_owner = get_cpulocal_var_ptr(fpu_owner);
	যদি (*local_fpu_owner != রিক্ত) {
		প্রতিজ্ঞা(*local_fpu_owner != p);
		save_local_fpu(*local_fpu_owner, FALSE /*retain*/);
	}

	/*
	 * restore the current process' state and let it run again, do not
	 * schedule!
	 */
	যদি (restore_fpu(p) != OK) {
		/* Restoring FPU state failed. This is always the process's own
		 * fault. Send a signal, and schedule another process instead.
		 */
		*local_fpu_owner = রিক্ত;		/* release FPU */
		cause_sig(proc_nr(p), SIGFPE);
		ঘূৰাই;
	}

	*local_fpu_owner = p;
	context_stop(proc_addr(KERNEL));
	restore_user_context(p);
	NOT_REACHABLE;
}

শূন্য release_fpu(সংরচনা proc * p) {
	সংরচনা proc ** fpu_owner_ptr;

	fpu_owner_ptr = get_cpu_var_ptr(p->p_cpu, fpu_owner);

	যদি (*fpu_owner_ptr == p)
		*fpu_owner_ptr = রিক্ত;
}

শূন্য ser_dump_proc(শূন্য)
{
        সংরচনা proc *pp;

        বাবে (pp= BEG_PROC_ADDR; pp < END_PROC_ADDR; pp++)
        {
                যদি (isemptyp(pp))
                        অনুৱর্ত;
                print_proc_recursive(pp);
        }
}
