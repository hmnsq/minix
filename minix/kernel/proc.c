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

#ಅನ್ತರ್ಭಾವ <stddef.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "vm.h"
#ಅನ್ತರ್ಭಾವ "clock.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ <minix/syslib.h>

/* Scheduling and message passing functions */
ಸ್ಥಿರ ಶೂನ್ಯ idle(ಶೂನ್ಯ);
/**
 * Made public for use in clock.c (for user-space scheduling)
static int mini_send(struct proc *caller_ptr, endpoint_t dst_e, message
	*m_ptr, int flags);
*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mini_receive(ಸಂರಚನಾ proc *caller_ptr, endpoint_t src,
	message *m_buff_usr, ಪೂರ್ಣಾಙ್ಕ flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mini_senda(ಸಂರಚನಾ proc *caller_ptr, asynmsg_t *table, size_t
	size);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ deadlock(ಪೂರ್ಣಾಙ್ಕ function, ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *caller,
	endpoint_t src_dst_e);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ try_async(ಸಂರಚನಾ proc *caller_ptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ try_one(endpoint_t receive_e, ಸಂರಚನಾ proc *src_ptr,
	ಸಂರಚನಾ proc *dst_ptr);
ಸ್ಥಿರ ಸಂರಚನಾ proc * pick_proc(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ enqueue_head(ಸಂರಚನಾ proc *rp);

/* all idles share the same idle_priv structure */
ಸ್ಥಿರ ಸಂರಚನಾ priv idle_priv;

ಸ್ಥಿರ ಶೂನ್ಯ set_idle_name(ವರ್ಣ * name, ಪೂರ್ಣಾಙ್ಕ n)
{
        ಪೂರ್ಣಾಙ್ಕ i, c;
        ಪೂರ್ಣಾಙ್ಕ p_z = 0;

        ಯದಿ (n > 999) 
                n = 999; 

        name[0] = 'i'; 
        name[1] = 'd'; 
        name[2] = 'l'; 
        name[3] = 'e'; 

        ಪ್ರತಿ (i = 4, c = 100; c > 0; c /= 10) {
                ಪೂರ್ಣಾಙ್ಕ digit;

                digit = n / c;  
                n -= digit * c;  

                ಯದಿ (p_z || digit != 0 || c == 1) {
                        p_z = 1;
                        name[i++] = '0' + digit;
                }   
        }    

        name[i] = '\0';

}


#define PICK_ANY	1
#define PICK_HIGHERONLY	2

#define BuildNotifyMessage(m_ptr, src, dst_ptr) \
	memset((m_ptr), 0, ಪರಿಮಾಣ(*(m_ptr)));				\
	(m_ptr)->m_type = NOTIFY_MESSAGE;				\
	(m_ptr)->m_notify.timestamp = get_monotonic();		\
	ವಿಕಲ್ಪ (src) {							\
	ಪಕ್ಷ HARDWARE:							\
		(m_ptr)->m_notify.interrupts =			\
			priv(dst_ptr)->s_int_pending;			\
		priv(dst_ptr)->s_int_pending = 0;			\
		ಭಙ್ಗ;							\
	ಪಕ್ಷ SYSTEM:							\
		memcpy(&(m_ptr)->m_notify.sigset,			\
			&priv(dst_ptr)->s_sig_pending,			\
			ಪರಿಮಾಣ(sigset_t));				\
		sigemptyset(&priv(dst_ptr)->s_sig_pending);		\
		ಭಙ್ಗ;							\
	}

ಸ್ಥಿರ message m_notify_buff = { 0, NOTIFY_MESSAGE };

ಶೂನ್ಯ proc_init(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ proc * rp;
	ಸಂರಚನಾ priv *sp;
	ಪೂರ್ಣಾಙ್ಕ i;

	/* Clear the process table. Announce each slot as empty and set up
	 * mappings for proc_addr() and proc_nr() macros. Do the same for the
	 * table with privilege structures for the system processes. 
	 */
	ಪ್ರತಿ (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < END_PROC_ADDR; ++rp, ++i) {
		rp->p_rts_flags = RTS_SLOT_FREE;/* initialize free slot */
		rp->p_magic = PMAGIC;
		rp->p_nr = i;			/* proc number from ptr */
		rp->p_endpoint = _ENDPOINT(0, rp->p_nr); /* generation no. 0 */
		rp->p_scheduler = ರಿಕ್ತ;		/* no user space scheduler */
		rp->p_priority = 0;		/* no priority */
		rp->p_quantum_size_ms = 0;	/* no quantum size */

		/* arch-specific initialization */
		arch_proc_reset(rp);
	}
	ಪ್ರತಿ (sp = BEG_PRIV_ADDR, i = 0; sp < END_PRIV_ADDR; ++sp, ++i) {
		sp->s_proc_nr = NONE;		/* initialize as free */
		sp->s_id = (sys_id_t) i;	/* priv structure index */
		ppriv_addr[i] = sp;		/* priv ptr from number */
		sp->s_sig_mgr = NONE;		/* clear signal managers */
		sp->s_bak_sig_mgr = NONE;
	}

	idle_priv.s_flags = IDL_F;
	/* initialize IDLE structures for every CPU */
	ಪ್ರತಿ (i = 0; i < CONFIG_MAX_CPUS; i++) {
		ಸಂರಚನಾ proc * ip = get_cpu_var_ptr(i, idle_proc);
		ip->p_endpoint = IDLE;
		ip->p_priv = &idle_priv;
		/* must not let idle ever get scheduled */
		ip->p_rts_flags |= RTS_PROC_STOP;
		set_idle_name(ip->p_name, i);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ switch_address_space_idle(ಶೂನ್ಯ)
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
ಸ್ಥಿರ ಶೂನ್ಯ idle(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ proc * p;

	/* This function is called whenever there is no work to do.
	 * Halt the CPU, and measure how many timestamp counter ticks are
	 * spent not doing anything. This allows test setups to measure
	 * the CPU utilization of certain workloads with high precision.
	 */

	p = get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	ಯದಿ (priv(p)->s_flags & BILLABLE)
		get_cpulocal_var(bill_ptr) = p;

	switch_address_space_idle();

#ifdef CONFIG_SMP
	get_cpulocal_var(cpu_is_idle) = 1;
	/* we don't need to keep time on APs as it is handled on the BSP */
	ಯದಿ (cpuid != bsp_cpu_id)
		stop_local_timer();
	ಅನ್ಯಥಾ
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
#ಯದಿ !SPROFILE
	halt_cpu();
#ಅನ್ಯಥಾ
	ಯದಿ (!sprofiling)
		halt_cpu();
	ಅನ್ಯಥಾ {
		ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ * v;

		v = get_cpulocal_var_ptr(idle_interrupted);
		interrupts_enable();
		ಯಾವತ್ (!*v)
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
ಶೂನ್ಯ vm_suspend(ಸಂರಚನಾ proc *caller, ಧ್ರುವ ಸಂರಚನಾ proc *target,
        ಧ್ರುವ vir_bytes linaddr, ಧ್ರುವ vir_bytes len, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ type,
        ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ writeflag)
{
        /* This range is not OK for this process. Set parameters
         * of the request and notify VM about the pending request.
         */
        ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(caller, RTS_VMREQUEST));
        ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(target, RTS_VMREQUEST));

        RTS_SET(caller, RTS_VMREQUEST);

        caller->p_vmrequest.req_type = VMPTYPE_CHECK;
        caller->p_vmrequest.target = target->p_endpoint;
        caller->p_vmrequest.params.check.start = linaddr;
        caller->p_vmrequest.params.check.length = len;
        caller->p_vmrequest.params.check.writeflag = writeflag;
        caller->p_vmrequest.type = type;

        /* Connect caller on vmrequest wait queue. */
        ಯದಿ(!(caller->p_vmrequest.nextrequestor = vmrequest))
                ಯದಿ(OK != send_sig(VM_PROC_NR, SIGKMEM))
                        panic("send_sig failed");
        vmrequest = caller;
}

/*===========================================================================*
 *                              delivermsg                                *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ delivermsg(ಸಂರಚನಾ proc *rp)
{
        ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(rp, RTS_VMREQUEST));
        ಪ್ರತಿಜ್ಞಾ(rp->p_misc_flags & MF_DELIVERMSG);
        ಪ್ರತಿಜ್ಞಾ(rp->p_delivermsg.m_source != NONE);

        ಯದಿ (copy_ಸಂದೇಶ_to_user(&rp->p_delivermsg,
                                (message *) rp->p_delivermsg_vir)) {
                ಯದಿ(rp->p_misc_flags & MF_MSGFAILED) {
                        /* 2nd consecutive failure means this won't succeed */
                        printf("WARNING wrong user pointer 0x%08lx from "
                                "process %s / %d\n",
                                rp->p_delivermsg_vir,
                                rp->p_name,
                                rp->p_endpoint);
                        cause_sig(rp->p_nr, SIGSEGV);
                } ಅನ್ಯಥಾ {
                        /* 1st failure means we have to ask VM to handle it */
                        vm_suspend(rp, rp, rp->p_delivermsg_vir,
                                ಪರಿಮಾಣ(message), VMSTYPE_DELIVERMSG, 1);
                        rp->p_misc_flags |= MF_MSGFAILED;
                }
        } ಅನ್ಯಥಾ {
                /* Indicate message has been delivered; address is 'used'. */
                rp->p_delivermsg.m_source = NONE;
                rp->p_misc_flags &= ~(MF_DELIVERMSG|MF_MSGFAILED);

                ಯದಿ(!(rp->p_misc_flags & MF_CONTEXT_SET)) {
                        rp->p_reg.retreg = OK;
                }
        }
}

/*===========================================================================*
 *				switch_to_user				     * 
 *===========================================================================*/
ಶೂನ್ಯ switch_to_user(ಶೂನ್ಯ)
{
	/* This function is called an instant before proc_ptr is
	 * to be scheduled again.
	 */
	ಸಂರಚನಾ proc * p;
#ifdef CONFIG_SMP
	ಪೂರ್ಣಾಙ್ಕ tlb_must_refresh = 0;
#endif

	p = get_cpulocal_var(proc_ptr);
	/*
	 * if the current process is still runnable check the misc flags and let
	 * it run unless it becomes not runnable in the meantime
	 */
	ಯದಿ (proc_is_runnable(p))
		ಗಚ್ಛ check_misc_flags;
	/*
	 * if a process becomes not runnable while handling the misc flags, we
	 * need to pick a new one here and start from scratch. Also if the
	 * current process wasn't runnable, we pick a new one here
	 */
not_runnable_pick_new:
	ಯದಿ (proc_is_preempted(p)) {
		p->p_rts_flags &= ~RTS_PREEMPTED;
		ಯದಿ (proc_is_runnable(p)) {
			ಯದಿ (p->p_cpu_time_ಎಡ)
				enqueue_head(p);
			ಅನ್ಯಥಾ
				enqueue(p);
		}
	}

	/*
	 * if we have no process to run, set IDLE as the current process for
	 * time accounting and put the cpu in an idle state. After the next
	 * timer interrupt the execution resumes here and we can pick another
	 * process. If there is still nothing runnable we "schedule" IDLE again
	 */
	ಯಾವತ್ (!(p = pick_proc())) {
		idle();
	}

	/* update the global variable */
	get_cpulocal_var(proc_ptr) = p;

#ifdef CONFIG_SMP
	ಯದಿ (p->p_misc_flags & MF_FLUSH_TLB && get_cpulocal_var(ptproc) == p)
		tlb_must_refresh = 1;
#endif
	switch_address_space(p);

check_misc_flags:

	ಪ್ರತಿಜ್ಞಾ(p);
	ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(p));
	ಯಾವತ್ (p->p_misc_flags &
		(MF_KCALL_RESUME | MF_DELIVERMSG |
		 MF_SC_DEFER | MF_SC_TRACE | MF_SC_ACTIVE)) {

		ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(p));
		ಯದಿ (p->p_misc_flags & MF_KCALL_RESUME) {
			kernel_call_resume(p);
		}
		ಅನ್ಯಥಾ ಯದಿ (p->p_misc_flags & MF_DELIVERMSG) {
			TRACE(VF_SCHEDULING, printf("delivering to %s / %d\n",
				p->p_name, p->p_endpoint););
			delivermsg(p);
		}
		ಅನ್ಯಥಾ ಯದಿ (p->p_misc_flags & MF_SC_DEFER) {
			/* Perform the system call that we deferred earlier. */

			ಪ್ರತಿಜ್ಞಾ (!(p->p_misc_flags & MF_SC_ACTIVE));

			arch_do_syscall(p);

			/* If the process is stopped for signal delivery, and
			 * not blocked sending a message after the system call,
			 * inform PM.
			 */
			ಯದಿ ((p->p_misc_flags & MF_SIG_DELAY) &&
					!RTS_ISSET(p, RTS_SENDING))
				sig_delay_done(p);
		}
		ಅನ್ಯಥಾ ಯದಿ (p->p_misc_flags & MF_SC_TRACE) {
			/* Trigger a system call leave event if this was a
			 * system call. We must do this after processing the
			 * other flags above, both for tracing correctness and
			 * to be able to use 'break'.
			 */
			ಯದಿ (!(p->p_misc_flags & MF_SC_ACTIVE))
				ಭಙ್ಗ;

			p->p_misc_flags &=
				~(MF_SC_TRACE | MF_SC_ACTIVE);

			/* Signal the "leave system call" event.
			 * Block the process.
			 */
			cause_sig(proc_nr(p), SIGTRAP);
		}
		ಅನ್ಯಥಾ ಯದಿ (p->p_misc_flags & MF_SC_ACTIVE) {
			/* If MF_SC_ACTIVE was set, remove it now:
			 * we're leaving the system call.
			 */
			p->p_misc_flags &= ~MF_SC_ACTIVE;

			ಭಙ್ಗ;
		}

		/*
		 * the selected process might not be runnable anymore. We have
		 * to checkit and schedule another one
		 */
		ಯದಿ (!proc_is_runnable(p))
			ಗಚ್ಛ not_runnable_pick_new;
	}
	/*
	 * check the quantum left before it runs again. We must do it only here
	 * as we are sure that a possible out-of-quantum message to the
	 * scheduler will not collide with the regular ipc
	 */
	ಯದಿ (!p->p_cpu_time_ಎಡ)
		proc_no_time(p);
	/*
	 * After handling the misc flags the selected process might not be
	 * runnable anymore. We have to checkit and schedule another one
	 */
	ಯದಿ (!proc_is_runnable(p))
		ಗಚ್ಛ not_runnable_pick_new;

	TRACE(VF_SCHEDULING, printf("cpu %d starting %s / %d "
				"pc 0x%08x\n",
		cpuid, p->p_name, p->p_endpoint, p->p_reg.pc););
#ಯದಿ DEBUG_TRACE
	p->p_schedules++;
#endif

	p = arch_finish_switch_to_user();
	ಪ್ರತಿಜ್ಞಾ(p->p_cpu_time_ಎಡ);

	context_stop(proc_addr(KERNEL));

	/* If the process isn't the owner of FPU, enable the FPU exception */
	ಯದಿ (get_cpulocal_var(fpu_owner) != p)
		enable_fpu_exception();
	ಅನ್ಯಥಾ
		disable_fpu_exception();

	/* If MF_CONTEXT_SET is set, don't clobber process state within
	 * the kernel. The next kernel entry is OK again though.
	 */
	p->p_misc_flags &= ~MF_CONTEXT_SET;

#ಯದಿ defined(__i386__)
  	ಪ್ರತಿಜ್ಞಾ(p->p_seg.p_cr3 != 0);
#ನೋಚೇದ್ಯದಿ defined(__arm__)
	ಪ್ರತಿಜ್ಞಾ(p->p_seg.p_ttbr != 0);
#endif
#ifdef CONFIG_SMP
	ಯದಿ (p->p_misc_flags & MF_FLUSH_TLB) {
		ಯದಿ (tlb_must_refresh)
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_sync_ipc(ಸಂರಚನಾ proc * caller_ptr, /* who made the call */
			ಪೂರ್ಣಾಙ್ಕ call_nr,	/* system call number and flags */
			endpoint_t src_dst_e,	/* src or dst of the call */
			message *m_ptr)	/* users pointer to a message */
{
  ಪೂರ್ಣಾಙ್ಕ result;					/* the system call's result */
  ಪೂರ್ಣಾಙ್ಕ src_dst_p;				/* Process slot number */
  ವರ್ಣ *callname;

  /* Check destination. RECEIVE is the only call that accepts ANY (in addition
   * to a real endpoint). The other calls (SEND, SENDREC, and NOTIFY) require an
   * endpoint to corresponds to a process. In addition, it is necessary to check
   * whether a process is allowed to send to a given destination.
   */
  ಪ್ರತಿಜ್ಞಾ(call_nr != SENDA);

  /* Only allow non-negative call_nr values less than 32 */
  ಯದಿ (call_nr < 0 || call_nr > IPCNO_HIGHEST || call_nr >= 32
      || !(callname = ipc_call_names[call_nr])) {
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          call_nr, proc_nr(caller_ptr), src_dst_e);
#endif
	ಪ್ರತ್ಯಾಗಮ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ಯದಿ (src_dst_e == ANY)
  {
	ಯದಿ (call_nr != RECEIVE)
	{
#ಯದಿ 0
		printf("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#endif
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	}
	src_dst_p = (ಪೂರ್ಣಾಙ್ಕ) src_dst_e;
  }
  ಅನ್ಯಥಾ
  {
	/* Require a valid source and/or destination process. */
	ಯದಿ(!isokendpt(src_dst_e, &src_dst_p)) {
#ಯದಿ 0
		printf("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#endif
		ಪ್ರತ್ಯಾಗಮ EDEADSRCDST;
	}

	/* If the call is to send to a process, i.e., for SEND, SENDNB,
	 * SENDREC or NOTIFY, verify that the caller is allowed to send to
	 * the given destination. 
	 */
	ಯದಿ (call_nr != RECEIVE)
	{
		ಯದಿ (!may_send_to(caller_ptr, src_dst_p)) {
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
			printf(
			"sys_call: ipc mask denied %s from %d to %d\n",
				callname,
				caller_ptr->p_endpoint, src_dst_e);
#endif
			ಪ್ರತ್ಯಾಗಮ(ECALLDENIED);	/* call denied by ipc mask */
		}
	}
  }

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  ಯದಿ (!(priv(caller_ptr)->s_trap_mask & (1 << call_nr))) {
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: %s not allowed, caller %d, src_dst %d\n", 
          callname, proc_nr(caller_ptr), src_dst_p);
#endif
	ಪ್ರತ್ಯಾಗಮ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ಯದಿ (call_nr != SENDREC && call_nr != RECEIVE && iskerneln(src_dst_p)) {
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
      printf("sys_call: trap %s not allowed, caller %d, src_dst %d\n",
           callname, proc_nr(caller_ptr), src_dst_e);
#endif
	ಪ್ರತ್ಯಾಗಮ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ವಿಕಲ್ಪ(call_nr) {
  ಪಕ್ಷ SENDREC:
	/* A flag is set so that notifications cannot interrupt SENDREC. */
	caller_ptr->p_misc_flags |= MF_REPLY_PEND;
	/* fall through */
  ಪಕ್ಷ SEND:			
	result = mini_send(caller_ptr, src_dst_e, m_ptr, 0);
	ಯದಿ (call_nr == SEND || result != OK)
		ಭಙ್ಗ;				/* done, or SEND failed */
	/* fall through for SENDREC */
  ಪಕ್ಷ RECEIVE:			
	ಯದಿ (call_nr == RECEIVE) {
		caller_ptr->p_misc_flags &= ~MF_REPLY_PEND;
		IPC_STATUS_CLEAR(caller_ptr);  /* clear IPC status code */
	}
	result = mini_receive(caller_ptr, src_dst_e, m_ptr, 0);
	ಭಙ್ಗ;
  ಪಕ್ಷ NOTIFY:
	result = mini_notify(caller_ptr, src_dst_e);
	ಭಙ್ಗ;
  ಪಕ್ಷ SENDNB:
        result = mini_send(caller_ptr, src_dst_e, m_ptr, NON_BLOCKING);
        ಭಙ್ಗ;
  ಸ್ವಭಾವ:
	result = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  ಪ್ರತ್ಯಾಗಮ(result);
}

ಪೂರ್ಣಾಙ್ಕ do_ipc(reg_t r1, reg_t r2, reg_t r3)
{
  ಸಂರಚನಾ proc *ಧ್ರುವ caller_ptr = get_cpulocal_var(proc_ptr);	/* get pointer to caller */
  ಪೂರ್ಣಾಙ್ಕ call_nr = (ಪೂರ್ಣಾಙ್ಕ) r1;

  ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(caller_ptr, RTS_SLOT_FREE));

  /* bill kernel time to this process. */
  kbill_ipc = caller_ptr;

  /* If this process is subject to system call tracing, handle that first. */
  ಯದಿ (caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
	/* Are we tracing this process, and is it the first sys_call entry? */
	ಯದಿ ((caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) ==
							MF_SC_TRACE) {
		/* We must notify the tracer before processing the actual
		 * system call. If we don't, the tracer could not obtain the
		 * input message. Postpone the entire system call.
		 */
		caller_ptr->p_misc_flags &= ~MF_SC_TRACE;
		ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_SC_DEFER));
		caller_ptr->p_misc_flags |= MF_SC_DEFER;
		caller_ptr->p_defer.r1 = r1;
		caller_ptr->p_defer.r2 = r2;
		caller_ptr->p_defer.r3 = r3;

		/* Signal the "enter system call" event. Block the process. */
		cause_sig(proc_nr(caller_ptr), SIGTRAP);

		/* Preserve the return register's value. */
		ಪ್ರತ್ಯಾಗಮ caller_ptr->p_reg.retreg;
	}

	/* If the MF_SC_DEFER flag is set, the syscall is now being resumed. */
	caller_ptr->p_misc_flags &= ~MF_SC_DEFER;

	ಪ್ರತಿಜ್ಞಾ (!(caller_ptr->p_misc_flags & MF_SC_ACTIVE));

	/* Set a flag to allow reliable tracing of leaving the system call. */
	caller_ptr->p_misc_flags |= MF_SC_ACTIVE;
  }

  ಯದಿ(caller_ptr->p_misc_flags & MF_DELIVERMSG) {
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
  ವಿಕಲ್ಪ(call_nr) {
  	ಪಕ್ಷ SENDREC:
  	ಪಕ್ಷ SEND:			
  	ಪಕ್ಷ RECEIVE:			
  	ಪಕ್ಷ NOTIFY:
  	ಪಕ್ಷ SENDNB:
  	{
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_sync++;

  	    ಪ್ರತ್ಯಾಗಮ do_sync_ipc(caller_ptr, call_nr, (endpoint_t) r2,
			    (message *) r3);
  	}
  	ಪಕ್ಷ SENDA:
  	{
 	    /*
  	     * Get and check the size of the argument in bytes as it is a
  	     * table
  	     */
  	    size_t ಸಂದೇಶ_size = (size_t) r2;
  
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_async++;
 
  	    /* Limit size to something reasonable. An arbitrary choice is 16
  	     * times the number of process table entries.
  	     */
  	    ಯದಿ (ಸಂದೇಶ_size > 16*(NR_TASKS + NR_PROCS))
	        ಪ್ರತ್ಯಾಗಮ EDOM;
  	    ಪ್ರತ್ಯಾಗಮ mini_senda(caller_ptr, (asynmsg_t *) r3, ಸಂದೇಶ_size);
  	}
  	ಪಕ್ಷ MINIX_KERNINFO:
	{
		/* It might not be initialized yet. */
	  	ಯದಿ(!minix_kerninfo_user) {
			ಪ್ರತ್ಯಾಗಮ EBADCALL;
		}

  		arch_set_secondary_ipc_return(caller_ptr, minix_kerninfo_user);
  		ಪ್ರತ್ಯಾಗಮ OK;
	}
  	ಸ್ವಭಾವ:
	ಪ್ರತ್ಯಾಗಮ EBADCALL;		/* illegal system call */
  }
}

/*===========================================================================*
 *				deadlock				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ deadlock(
  ಪೂರ್ಣಾಙ್ಕ function,				/* trap number */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *cp,		/* pointer to caller */
  endpoint_t src_dst_e			/* src or dst process */
)
{
/* Check for deadlock. This can happen if 'caller_ptr' and 'src_dst' have
 * a cyclic dependency of blocking send and receive calls. The only cyclic 
 * dependency that is not fatal is if the caller and target directly SEND(REC)
 * and RECEIVE to each other. If a deadlock is found, the group size is 
 * returned. Otherwise zero is returned. 
 */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *xp;			/* process pointer */
  ಪೂರ್ಣಾಙ್ಕ group_size = 1;				/* start with only caller */
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
  ಸ್ಥಿರ ಸಂರಚನಾ proc *processes[NR_PROCS + NR_TASKS];
  processes[0] = cp;
#endif

  ಯಾವತ್ (src_dst_e != ANY) { 			/* check while process nr */
      ಪೂರ್ಣಾಙ್ಕ src_dst_slot;
      okendpt(src_dst_e, &src_dst_slot);
      xp = proc_addr(src_dst_slot);		/* follow chain of processes */
      ಪ್ರತಿಜ್ಞಾ(proc_ptr_ok(xp));
      ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(xp, RTS_SLOT_FREE));
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
      processes[group_size] = xp;
#endif
      group_size ++;				/* extra process in group */

      /* Check whether the last process in the chain has a dependency. If it 
       * has not, the cycle cannot be closed and we are done.
       */
      ಯದಿ((src_dst_e = P_BLOCKEDON(xp)) == NONE)
	ಪ್ರತ್ಯಾಗಮ 0;

      /* Now check if there is a cyclic dependency. For group sizes of two,  
       * a combination of SEND(REC) and RECEIVE is not fatal. Larger groups
       * or other combinations indicate a deadlock.  
       */
      ಯದಿ (src_dst_e == cp->p_endpoint) {	/* possible deadlock */
	  ಯದಿ (group_size == 2) {		/* caller and src_dst */
	      /* The function number is magically converted to flags. */
	      ಯದಿ ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING) { 
	          ಪ್ರತ್ಯಾಗಮ(0);			/* not a deadlock */
	      }
	  }
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
	  {
		ಪೂರ್ಣಾಙ್ಕ i;
		printf("deadlock between these processes:\n");
		ಪ್ರತಿ(i = 0; i < group_size; i++) {
			printf(" %10s ", processes[i]->p_name);
		}
		printf("\n\n");
		ಪ್ರತಿ(i = 0; i < group_size; i++) {
			print_proc(processes[i]);
			proc_stacktrace(processes[i]);
		}
	  }
#endif
          ಪ್ರತ್ಯಾಗಮ(group_size);			/* deadlock found */
      }
  }
  ಪ್ರತ್ಯಾಗಮ(0);					/* not a deadlock */
}

/*===========================================================================*
 *				has_pending				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ has_pending(sys_map_t *map, ಪೂರ್ಣಾಙ್ಕ src_p, ಪೂರ್ಣಾಙ್ಕ asynm)
{
/* Check to see if there is a pending message from the desired source
 * available.
 */

  ಪೂರ್ಣಾಙ್ಕ src_id;
  sys_id_t id = NULL_PRIV_ID;
#ifdef CONFIG_SMP
  ಸಂರಚನಾ proc * p;
#endif

  /* Either check a specific bit in the mask map, or find the first bit set in
   * it (if any), depending on whether the receive was called on a specific
   * source endpoint.
   */
  ಯದಿ (src_p != ANY) {
	src_id = nr_to_id(src_p);
	ಯದಿ (get_sys_bit(*map, src_id)) {
#ifdef CONFIG_SMP
		p = proc_addr(id_to_nr(src_id));
		ಯದಿ (asynm && RTS_ISSET(p, RTS_VMINHIBIT))
			p->p_misc_flags |= MF_SENDA_VM_MISS;
		ಅನ್ಯಥಾ
#endif
			id = src_id;
	}
  } ಅನ್ಯಥಾ {
	/* Find a source with a pending message */
	ಪ್ರತಿ (src_id = 0; src_id < NR_SYS_PROCS; src_id += BITCHUNK_BITS) {
		ಯದಿ (get_sys_bits(*map, src_id) != 0) {
#ifdef CONFIG_SMP
			ಯಾವತ್ (src_id < NR_SYS_PROCS) {
				ಯಾವತ್ (!get_sys_bit(*map, src_id)) {
					ಯದಿ (src_id == NR_SYS_PROCS)
						ಗಚ್ಛ quit_ಹುಡುಕು;
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
				ಯದಿ (asynm && RTS_ISSET(p, RTS_VMINHIBIT)) {
					p->p_misc_flags |= MF_SENDA_VM_MISS;
					src_id++;
				} ಅನ್ಯಥಾ
					ಗಚ್ಛ quit_ಹುಡುಕು;
			}
#ಅನ್ಯಥಾ
			ಯಾವತ್ (!get_sys_bit(*map, src_id)) src_id++;
			ಗಚ್ಛ quit_ಹುಡುಕು;
#endif
		}
	}

quit_ಹುಡುಕು:
	ಯದಿ (src_id < NR_SYS_PROCS)	/* Found one */
		id = src_id;
  }

  ಪ್ರತ್ಯಾಗಮ(id);
}

/*===========================================================================*
 *				has_pending_notify			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ has_pending_notify(ಸಂರಚನಾ proc * caller, ಪೂರ್ಣಾಙ್ಕ src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	ಪ್ರತ್ಯಾಗಮ has_pending(map, src_p, 0);
}

/*===========================================================================*
 *				has_pending_asend			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ has_pending_asend(ಸಂರಚನಾ proc * caller, ಪೂರ್ಣಾಙ್ಕ src_p)
{
	sys_map_t * map = &priv(caller)->s_asyn_pending;
	ಪ್ರತ್ಯಾಗಮ has_pending(map, src_p, 1);
}

/*===========================================================================*
 *				unset_notify_pending			     *
 *===========================================================================*/
ಶೂನ್ಯ unset_notify_pending(ಸಂರಚನಾ proc * caller, ಪೂರ್ಣಾಙ್ಕ src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	unset_sys_bit(*map, src_p);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ mini_send(
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *caller_ptr,	/* who is trying to send a message? */
  endpoint_t dst_e,			/* to whom is message being sent? */
  message *m_ptr,			/* pointer to message buffer */
  ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ flags
)
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *dst_ptr;
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc **xpp;
  ಪೂರ್ಣಾಙ್ಕ dst_p;
  dst_p = _ENDPOINT_P(dst_e);
  dst_ptr = proc_addr(dst_p);

  ಯದಿ (RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT))
  {
	ಪ್ರತ್ಯಾಗಮ EDEADSRCDST;
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * RTS_SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  ಯದಿ (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, (vir_bytes)m_ptr, ರಿಕ್ತ)) {
	ಪೂರ್ಣಾಙ್ಕ call;
	/* Destination is indeed waiting for this message. */
	ಪ್ರತಿಜ್ಞಾ(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));	

	ಯದಿ (!(flags & FROM_KERNEL)) {
		ಯದಿ(copy_ಸಂದೇಶ_from_user(m_ptr, &dst_ptr->p_delivermsg))
			ಪ್ರತ್ಯಾಗಮ EFAULT;
	} ಅನ್ಯಥಾ {
		dst_ptr->p_delivermsg = *m_ptr;
		IPC_STATUS_ADD_FLAGS(dst_ptr, IPC_FLG_ಸಂದೇಶ_FROM_KERNEL);
	}

	dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;

	call = (caller_ptr->p_misc_flags & MF_REPLY_PEND ? SENDREC
		: (flags & NON_BLOCKING ? SENDNB : SEND));
	IPC_STATUS_ADD_CALL(dst_ptr, call);

	ಯದಿ (dst_ptr->p_misc_flags & MF_REPLY_PEND)
		dst_ptr->p_misc_flags &= ~MF_REPLY_PEND;

	RTS_UNSET(dst_ptr, RTS_RECEIVING);

#ಯದಿ DEBUG_IPC_HOOK
	hook_ipc_msgsend(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#endif
  } ಅನ್ಯಥಾ {
	ಯದಿ(flags & NON_BLOCKING) {
		ಪ್ರತ್ಯಾಗಮ(ENOTREADY);
	}

	/* Check for a possible deadlock before actually blocking. */
	ಯದಿ (deadlock(SEND, caller_ptr, dst_e)) {
		ಪ್ರತ್ಯಾಗಮ(ELOCKED);
	}

	/* Destination is not waiting.  Block and dequeue caller. */
	ಯದಿ (!(flags & FROM_KERNEL)) {
		ಯದಿ(copy_ಸಂದೇಶ_from_user(m_ptr, &caller_ptr->p_sendmsg))
			ಪ್ರತ್ಯಾಗಮ EFAULT;
	} ಅನ್ಯಥಾ {
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
	ಪ್ರತಿಜ್ಞಾ(caller_ptr->p_q_link == ರಿಕ್ತ);
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	ಯಾವತ್ (*xpp) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */

#ಯದಿ DEBUG_IPC_HOOK
	hook_ipc_msgsend(&caller_ptr->p_sendmsg, caller_ptr, dst_ptr);
#endif
  }
  ಪ್ರತ್ಯಾಗಮ(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mini_receive(ಸಂರಚನಾ proc * caller_ptr,
			endpoint_t src_e, /* which message source is wanted */
			message * m_buff_usr, /* pointer to message buffer */
			ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ flags)
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller.
 */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc **xpp;
  ಪೂರ್ಣಾಙ್ಕ r, src_id, found, src_proc_nr, src_p;
  endpoint_t sender_e;

  ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));

  /* This is where we want our message. */
  caller_ptr->p_delivermsg_vir = (vir_bytes) m_buff_usr;

  ಯದಿ(src_e == ANY) src_p = ANY;
  ಅನ್ಯಥಾ
  {
	okendpt(src_e, &src_p);
	ಯದಿ (RTS_ISSET(proc_addr(src_p), RTS_NO_ENDPOINT))
	{
		ಪ್ರತ್ಯಾಗಮ EDEADSRCDST;
	}
  }


  /* Check to see if a message from desired source is already available.  The
   * caller's RTS_SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  ಯದಿ (!RTS_ISSET(caller_ptr, RTS_SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    ಯದಿ (! (caller_ptr->p_misc_flags & MF_REPLY_PEND)) {

	/* Check for pending notifications */
        src_id = has_pending_notify(caller_ptr, src_p);
        found = src_id != NULL_PRIV_ID;
        ಯದಿ(found) {
            src_proc_nr = id_to_nr(src_id);		/* get source proc */
            sender_e = proc_addr(src_proc_nr)->p_endpoint;
        }

        ಯದಿ (found && CANRECEIVE(src_e, sender_e, caller_ptr, 0,
          &m_notify_buff)) {

#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
	    ಯದಿ(src_proc_nr == NONE) {
		printf("mini_receive: sending notify from NONE\n");
	    }
#endif
	    ಪ್ರತಿಜ್ಞಾ(src_proc_nr != NONE);
            unset_notify_pending(caller_ptr, src_id);	/* no longer pending */

            /* Found a suitable source, deliver the notification message. */
	    ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));	
	    ಪ್ರತಿಜ್ಞಾ(src_e == ANY || sender_e == src_e);

	    /* assemble message */
	    BuildNotifyMessage(&caller_ptr->p_delivermsg, src_proc_nr, caller_ptr);
	    caller_ptr->p_delivermsg.m_source = sender_e;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;

	    IPC_STATUS_ADD_CALL(caller_ptr, NOTIFY);

	    ಗಚ್ಛ receive_done;
        }
    }

    /* Check for pending asynchronous messages */
    ಯದಿ (has_pending_asend(caller_ptr, src_p) != NULL_PRIV_ID) {
        ಯದಿ (src_p != ANY)
		r = try_one(src_e, proc_addr(src_p), caller_ptr);
        ಅನ್ಯಥಾ
        	r = try_async(caller_ptr);

	ಯದಿ (r == OK) {
            IPC_STATUS_ADD_CALL(caller_ptr, SENDA);
            ಗಚ್ಛ receive_done;
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    ಯಾವತ್ (*xpp) {
	ಸಂರಚನಾ proc * sender = *xpp;
	endpoint_t sender_e = sender->p_endpoint;

        ಯದಿ (CANRECEIVE(src_e, sender_e, caller_ptr, 0, &sender->p_sendmsg)) {
            ಪೂರ್ಣಾಙ್ಕ call;
	    ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(sender, RTS_SLOT_FREE));
	    ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(sender, RTS_NO_ENDPOINT));

	    /* Found acceptable message. Copy it and update status. */
  	    ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
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
	    ಯದಿ (sender->p_misc_flags & MF_SENDING_FROM_KERNEL) {
		IPC_STATUS_ADD_FLAGS(caller_ptr, IPC_FLG_ಸಂದೇಶ_FROM_KERNEL);
		/* we can clean the flag now, not need anymore */
		sender->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
	    }
	    ಯದಿ (sender->p_misc_flags & MF_SIG_DELAY)
		sig_delay_done(sender);

#ಯದಿ DEBUG_IPC_HOOK
            hook_ipc_msgrecv(&caller_ptr->p_delivermsg, *xpp, caller_ptr);
#endif
		
            *xpp = sender->p_q_link;		/* remove from queue */
	    sender->p_q_link = ರಿಕ್ತ;
	    ಗಚ್ಛ receive_done;
	}
	xpp = &sender->p_q_link;		/* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC. 
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  ಯದಿ ( ! (flags & NON_BLOCKING)) {
      /* Check for a possible deadlock before actually blocking. */
      ಯದಿ (deadlock(RECEIVE, caller_ptr, src_e)) {
          ಪ್ರತ್ಯಾಗಮ(ELOCKED);
      }

      caller_ptr->p_getfrom_e = src_e;		
      RTS_SET(caller_ptr, RTS_RECEIVING);
      ಪ್ರತ್ಯಾಗಮ(OK);
  } ಅನ್ಯಥಾ {
	ಪ್ರತ್ಯಾಗಮ(ENOTREADY);
  }

receive_done:
  ಯದಿ (caller_ptr->p_misc_flags & MF_REPLY_PEND)
	  caller_ptr->p_misc_flags &= ~MF_REPLY_PEND;
  ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ mini_notify(
  ಧ್ರುವ ಸಂರಚನಾ proc *caller_ptr,	/* sender of the notification */
  endpoint_t dst_e			/* which process to notify */
)
{
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *dst_ptr;
  ಪೂರ್ಣಾಙ್ಕ src_id;				/* source id for late delivery */
  ಪೂರ್ಣಾಙ್ಕ dst_p;

  ಯದಿ (!isokendpt(dst_e, &dst_p)) {
	util_stacktrace();
	printf("mini_notify: bogus endpoint %d\n", dst_e);
	ಪ್ರತ್ಯಾಗಮ EDEADSRCDST;
  }

  dst_ptr = proc_addr(dst_p);

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  ಯದಿ (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, 0, &m_notify_buff) &&
    !(dst_ptr->p_misc_flags & MF_REPLY_PEND)) {
      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      ಪ್ರತಿಜ್ಞಾ(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));

      BuildNotifyMessage(&dst_ptr->p_delivermsg, proc_nr(caller_ptr), dst_ptr);
      dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;

      IPC_STATUS_ADD_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING);

      ಪ್ರತ್ಯಾಗಮ(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the privilege id
   * instead of the process number is used in the pending bit map.
   */ 
  src_id = priv(caller_ptr)->s_id;
  set_sys_bit(priv(dst_ptr)->s_notify_pending, src_id); 
  ಪ್ರತ್ಯಾಗಮ(OK);
}

#define ASCOMPLAIN(caller, entry, field)	\
	printf("kernel:%s:%d: asyn failed for %s in %s "	\
	"(%d/%zu, tab 0x%lx)\n",__FILE__,__LINE__,	\
field, caller->p_name, entry, priv(caller)->s_asynsize, priv(caller)->s_asyntab)

#define A_RETR(entry) ಕುರು {			\
  ಯದಿ (data_copy(				\
  		caller_ptr->p_endpoint, table_v + (entry)*ಪರಿಮಾಣ(asynmsg_t),\
  		KERNEL, (vir_bytes) &tabent,	\
  		ಪರಿಮಾಣ(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, entry, "message entry");	\
  			r = EFAULT;		\
	                ಗಚ್ಛ asyn_error; \
  }						\
  ಅನ್ಯಥಾ ಯದಿ(tabent.dst == SELF) { \
      tabent.dst = caller_ptr->p_endpoint; \
  } \
  			 } ಯಾವತ್(0)

#define A_INSRT(entry) ಕುರು {			\
  ಯದಿ (data_copy(KERNEL, (vir_bytes) &tabent,	\
  		caller_ptr->p_endpoint, table_v + (entry)*ಪರಿಮಾಣ(asynmsg_t),\
  		ಪರಿಮಾಣ(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, entry, "message entry");	\
			/* Do NOT set r or goto asyn_error here! */ \
  }						\
  			  } ಯಾವತ್(0)	

/*===========================================================================*
 *				try_deliver_senda			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ try_deliver_senda(ಸಂರಚನಾ proc *caller_ptr,
				asynmsg_t *table,
				size_t size)
{
  ಪೂರ್ಣಾಙ್ಕ r, dst_p, done, do_notify;
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  unsigned flags;
  endpoint_t dst;
  ಸಂರಚನಾ proc *dst_ptr;
  ಸಂರಚನಾ priv *privp;
  asynmsg_t tabent;
  ಧ್ರುವ vir_bytes table_v = (vir_bytes) table;
  message *m_ptr = ರಿಕ್ತ;

  privp = priv(caller_ptr);

  /* Clear table */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  privp->s_asynendpoint = caller_ptr->p_endpoint;

  ಯದಿ (size == 0) ಪ್ರತ್ಯಾಗಮ(OK);  /* Nothing to do, just return */

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;

  /* Limit size to something reasonable. An arbitrary choice is 16
   * times the number of process table entries.
   *
   * (this check has been duplicated in sys_call but is left here
   * as a sanity check)
   */
  ಯದಿ (size > 16*(NR_TASKS + NR_PROCS)) {
    r = EDOM;
    ಪ್ರತ್ಯಾಗಮ r;
  }

  ಪ್ರತಿ (i = 0; i < size; i++) {
	/* Process each entry in the table and store the result in the table.
	 * If we're done handling a message, copy the result to the sender. */

	dst = NONE;
	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ಯದಿ (flags == 0) ಅನುವರ್ತ; /* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ಯದಿ(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR)) {
		r = EINVAL;
		ಗಚ್ಛ asyn_error;
	}
	ಯದಿ (!(flags & AMF_VALID)) { /* Must contain message */
		r = EINVAL;
		ಗಚ್ಛ asyn_error;
	}
	ಯದಿ (flags & AMF_DONE) ಅನುವರ್ತ;	/* Already done processing */

	r = OK;
	ಯದಿ (!isokendpt(tabent.dst, &dst_p)) 
		r = EDEADSRCDST; /* Bad destination, report the error */
	ಅನ್ಯಥಾ ಯದಿ (iskerneln(dst_p)) 
		r = ECALLDENIED; /* Asyn sends to the kernel are not allowed */
	ಅನ್ಯಥಾ ಯದಿ (!may_asynsend_to(caller_ptr, dst_p))
		r = ECALLDENIED; /* Send denied by IPC mask */
	ಅನ್ಯಥಾ 	/* r == OK */
		dst_ptr = proc_addr(dst_p);

	/* XXX: RTS_NO_ENDPOINT should be removed */
	ಯದಿ (r == OK && RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT)) {
		r = EDEADSRCDST;
	}

	/* Check if 'dst' is blocked waiting for this message.
	 * If AMF_NOREPLY is set, do not satisfy the receiving part of
	 * a SENDREC.
	 */
	ಯದಿ (r == OK && WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr,
	    (vir_bytes)&table[i].ಸಂದೇಶ, ರಿಕ್ತ) &&
	    (!(flags&AMF_NOREPLY) || !(dst_ptr->p_misc_flags&MF_REPLY_PEND))) {
		/* Destination is indeed waiting for this message. */
		dst_ptr->p_delivermsg = tabent.ಸಂದೇಶ;
		dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
		dst_ptr->p_misc_flags |= MF_DELIVERMSG;
		IPC_STATUS_ADD_CALL(dst_ptr, SENDA);
		RTS_UNSET(dst_ptr, RTS_RECEIVING);
#ಯದಿ DEBUG_IPC_HOOK
		hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#endif
	} ಅನ್ಯಥಾ ಯದಿ (r == OK) {
		/* Inform receiver that something is pending */
		set_sys_bit(priv(dst_ptr)->s_asyn_pending, 
			    priv(caller_ptr)->s_id); 
		done = FALSE;
		ಅನುವರ್ತ;
	} 

	/* Store results */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	ಯದಿ (flags & AMF_NOTIFY)
		do_notify = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (r != OK && (flags & AMF_NOTIFY_ERR))
		do_notify = TRUE;
	A_INSRT(i);	/* Copy results to caller; ignore errors */
	ಅನುವರ್ತ;

asyn_error:
	ಯದಿ (dst != NONE)
		printf("KERNEL senda error %d to %d\n", r, dst);
	ಅನ್ಯಥಾ
		printf("KERNEL senda error %d\n", r);
  }

  ಯದಿ (do_notify) 
	mini_notify(proc_addr(ASYNCM), caller_ptr->p_endpoint);

  ಯದಿ (!done) {
	privp->s_asyntab = (vir_bytes) table;
	privp->s_asynsize = size;
  }

  ಪ್ರತ್ಯಾಗಮ(OK);
}

/*===========================================================================*
 *				mini_senda				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mini_senda(ಸಂರಚನಾ proc *caller_ptr, asynmsg_t *table, size_t size)
{
  ಸಂರಚನಾ priv *privp;

  privp = priv(caller_ptr);
  ಯದಿ (!(privp->s_flags & SYS_PROC)) {
	printf( "mini_senda: warning caller has no privilege structure\n");
	ಪ್ರತ್ಯಾಗಮ(EPERM);
  }

  ಪ್ರತ್ಯಾಗಮ try_deliver_senda(caller_ptr, table, size);
}


/*===========================================================================*
 *				try_async				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ try_async(ಸಂರಚನಾ proc * caller_ptr)
{
  ಪೂರ್ಣಾಙ್ಕ r;
  ಸಂರಚನಾ priv *privp;
  ಸಂರಚನಾ proc *src_ptr;
  sys_map_t *map;

  map = &priv(caller_ptr)->s_asyn_pending;

  /* Try all privilege structures */
  ಪ್ರತಿ (privp = BEG_PRIV_ADDR; privp < END_PRIV_ADDR; ++privp)  {
	ಯದಿ (privp->s_proc_nr == NONE)
		ಅನುವರ್ತ;

	ಯದಿ (!get_sys_bit(*map, privp->s_id)) 
		ಅನುವರ್ತ;

	src_ptr = proc_addr(privp->s_proc_nr);

#ifdef CONFIG_SMP
	/*
	 * Do not copy from a process which does not have a stable address space
	 * due to VM fiddling with it
	 */
	ಯದಿ (RTS_ISSET(src_ptr, RTS_VMINHIBIT)) {
		src_ptr->p_misc_flags |= MF_SENDA_VM_MISS;
		ಅನುವರ್ತ;
	}
#endif

	ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	ಯದಿ ((r = try_one(ANY, src_ptr, caller_ptr)) == OK)
		ಪ್ರತ್ಯಾಗಮ(r);
  }

  ಪ್ರತ್ಯಾಗಮ(ESRCH);
}


/*===========================================================================*
 *				try_one					     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ try_one(endpoint_t receive_e, ಸಂರಚನಾ proc *src_ptr,
    ಸಂರಚನಾ proc *dst_ptr)
{
/* Try to receive an asynchronous message from 'src_ptr' */
  ಪೂರ್ಣಾಙ್ಕ r = EAGAIN, done, do_notify;
  unsigned ಪೂರ್ಣಾಙ್ಕ flags, i;
  size_t size;
  endpoint_t dst, src_e;
  ಸಂರಚನಾ proc *caller_ptr;
  ಸಂರಚನಾ priv *privp;
  asynmsg_t tabent;
  vir_bytes table_v;

  privp = priv(src_ptr);
  ಯದಿ (!(privp->s_flags & SYS_PROC)) ಪ್ರತ್ಯಾಗಮ(EPERM);
  size = privp->s_asynsize;
  table_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  unset_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  ಯದಿ (size == 0) ಪ್ರತ್ಯಾಗಮ(EAGAIN);
  ಯದಿ (privp->s_asynendpoint != src_ptr->p_endpoint) ಪ್ರತ್ಯಾಗಮ EAGAIN;
  ಯದಿ (!may_asynsend_to(src_ptr, proc_nr(dst_ptr))) ಪ್ರತ್ಯಾಗಮ (ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */
  src_e = src_ptr->p_endpoint;

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;

  ಪ್ರತಿ (i = 0; i < size; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the meantime.
  	 */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ಯದಿ (flags == 0) ಅನುವರ್ತ;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ಯದಿ(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	ಅನ್ಯಥಾ ಯದಿ (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	ಅನ್ಯಥಾ ಯದಿ (flags & AMF_DONE) ಅನುವರ್ತ; /* Already done processing */

	/* Clear done flag. The sender is done sending when all messages in the
	 * table are marked done or empty. However, we will know that only
	 * the next time we enter this function or when the sender decides to
	 * send additional asynchronous messages and manages to deliver them
	 * all.
	 */
	done = FALSE;

	ಯದಿ (r == EINVAL)
		ಗಚ್ಛ store_result;

	/* Message must be directed at receiving end */
	ಯದಿ (dst != dst_ptr->p_endpoint) ಅನುವರ್ತ;

	ಯದಿ (!CANRECEIVE(receive_e, src_e, dst_ptr,
		table_v + i*ಪರಿಮಾಣ(asynmsg_t) + offsetof(ಸಂರಚನಾ asynmsg,ಸಂದೇಶ),
		ರಿಕ್ತ)) {
		ಅನುವರ್ತ;
	}

	/* If AMF_NOREPLY is set, then this message is not a reply to a
	 * SENDREC and thus should not satisfy the receiving part of the
	 * SENDREC. This message is to be delivered later.
	 */
	ಯದಿ ((flags & AMF_NOREPLY) && (dst_ptr->p_misc_flags & MF_REPLY_PEND)) 
		ಅನುವರ್ತ;

	/* Destination is ready to receive the message; deliver it */
	r = OK;
	dst_ptr->p_delivermsg = tabent.ಸಂದೇಶ;
	dst_ptr->p_delivermsg.m_source = src_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;
#ಯದಿ DEBUG_IPC_HOOK
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, src_ptr, dst_ptr);
#endif

store_result:
	/* Store results for sender. We may just have started delivering a
	 * message, so we must not return an error to the caller in the case
	 * that storing the results triggers an error!
	 */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	ಯದಿ (flags & AMF_NOTIFY) do_notify = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */

	ಭಙ್ಗ;
  }

  ಯದಿ (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  ಯದಿ (done) {
	privp->s_asyntab = -1;
	privp->s_asynsize = 0;
  } ಅನ್ಯಥಾ {
	set_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);
  }

asyn_error:
  ಪ್ರತ್ಯಾಗಮ(r);
}

/*===========================================================================*
 *				cancel_async				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ cancel_async(ಸಂರಚನಾ proc *src_ptr, ಸಂರಚನಾ proc *dst_ptr)
{
/* Cancel asynchronous messages from src to dst, because dst is not interested
 * in them (e.g., dst has been restarted) */
  ಪೂರ್ಣಾಙ್ಕ done, do_notify;
  unsigned ಪೂರ್ಣಾಙ್ಕ flags, i;
  size_t size;
  endpoint_t dst;
  ಸಂರಚನಾ proc *caller_ptr;
  ಸಂರಚನಾ priv *privp;
  asynmsg_t tabent;
  vir_bytes table_v;

  privp = priv(src_ptr);
  ಯದಿ (!(privp->s_flags & SYS_PROC)) ಪ್ರತ್ಯಾಗಮ(EPERM);
  size = privp->s_asynsize;
  table_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  unset_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  ಯದಿ (size == 0) ಪ್ರತ್ಯಾಗಮ(EAGAIN);
  ಯದಿ (!may_send_to(src_ptr, proc_nr(dst_ptr))) ಪ್ರತ್ಯಾಗಮ(ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;


  ಪ್ರತಿ (i = 0; i < size; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the mean time.
  	 */

  	ಪೂರ್ಣಾಙ್ಕ r = EDEADSRCDST;	/* Cancel delivery due to dead dst */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ಯದಿ (flags == 0) ಅನುವರ್ತ;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ಯದಿ(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	ಅನ್ಯಥಾ ಯದಿ (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	ಅನ್ಯಥಾ ಯದಿ (flags & AMF_DONE) ಅನುವರ್ತ; /* Already done processing */

	/* Message must be directed at receiving end */
	ಯದಿ (dst != dst_ptr->p_endpoint) {
		done = FALSE;
		ಅನುವರ್ತ;
	}

	/* Store results for sender */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	ಯದಿ (flags & AMF_NOTIFY) do_notify = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */
  }

  ಯದಿ (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  ಯದಿ (!done) {
	privp->s_asyntab = table_v;
	privp->s_asynsize = size;
  }

asyn_error:
  ಪ್ರತ್ಯಾಗಮ(OK);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
ಶೂನ್ಯ enqueue(
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp	/* this process is now runnable */
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
  ಪೂರ್ಣಾಙ್ಕ q = rp->p_priority;	 		/* scheduling queue to use */
  ಸಂರಚನಾ proc **rdy_head, **rdy_tail;
  
  ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(rp));

  ಪ್ರತಿಜ್ಞಾ(q >= 0);

  rdy_head = get_cpu_var(rp->p_cpu, run_q_head);
  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now add the process to the queue. */
  ಯದಿ (!rdy_head[q]) {		/* add to empty queue */
      rdy_head[q] = rdy_tail[q] = rp; 		/* create a new queue */
      rp->p_nextready = ರಿಕ್ತ;		/* mark new end */
  } 
  ಅನ್ಯಥಾ {					/* add to tail of queue */
      rdy_tail[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_tail[q] = rp;				/* set new queue tail */
      rp->p_nextready = ರಿಕ್ತ;		/* mark new end */
  }

  ಯದಿ (cpuid == rp->p_cpu) {
	  /*
	   * enqueueing a process with a higher priority than the current one,
	   * it gets preempted. The current process must be preemptible. Testing
	   * the priority also makes sure that a process does not preempt itself
	   */
	  ಸಂರಚನಾ proc * p;
	  p = get_cpulocal_var(proc_ptr);
	  ಪ್ರತಿಜ್ಞಾ(p);
	  ಯದಿ((p->p_priority > rp->p_priority) &&
			  (priv(p)->s_flags & PREEMPTIBLE))
		  RTS_SET(p, RTS_PREEMPTED); /* calls dequeue() */
  }
#ifdef CONFIG_SMP
  /*
   * if the process was enqueued on a different cpu and the cpu is idle, i.e.
   * the time is off, we need to wake up that cpu and let it schedule this new
   * process
   */
  ಅನ್ಯಥಾ ಯದಿ (get_cpu_var(rp->p_cpu, cpu_is_idle)) {
	  smp_schedule(rp->p_cpu);
  }
#endif

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr)->p_accounting.enter_queue));


#ಯದಿ DEBUG_SANITYCHECKS
  ಪ್ರತಿಜ್ಞಾ(runqueues_ok_local());
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
ಸ್ಥಿರ ಶೂನ್ಯ enqueue_head(ಸಂರಚನಾ proc *rp)
{
  ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ q = rp->p_priority;	 		/* scheduling queue to use */

  ಸಂರಚನಾ proc **rdy_head, **rdy_tail;

  ಪ್ರತಿಜ್ಞಾ(proc_ptr_ok(rp));
  ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(rp));

  /*
   * the process was runnable without its quantum expired when dequeued. A
   * process with no time left should have been handled else and differently
   */
  ಪ್ರತಿಜ್ಞಾ(rp->p_cpu_time_ಎಡ);

  ಪ್ರತಿಜ್ಞಾ(q >= 0);


  rdy_head = get_cpu_var(rp->p_cpu, run_q_head);
  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now add the process to the queue. */
  ಯದಿ (!rdy_head[q]) {		/* add to empty queue */
	rdy_head[q] = rdy_tail[q] = rp; 	/* create a new queue */
	rp->p_nextready = ರಿಕ್ತ;			/* mark new end */
  } ಅನ್ಯಥಾ {					/* add to head of queue */
	rp->p_nextready = rdy_head[q];		/* chain head of queue */
	rdy_head[q] = rp;			/* set new queue head */
  }

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr->p_accounting.enter_queue)));


  /* Process accounting for scheduling */
  rp->p_accounting.dequeues--;
  rp->p_accounting.preempted++;

#ಯದಿ DEBUG_SANITYCHECKS
  ಪ್ರತಿಜ್ಞಾ(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
ಶೂನ್ಯ dequeue(ಸಂರಚನಾ proc *rp)
/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 *
 * This function can operate x-cpu as it always removes the process from the
 * queue of the cpu the process is currently assigned to.
 */
  ಪೂರ್ಣಾಙ್ಕ q = rp->p_priority;		/* queue to use */
  ಸಂರಚನಾ proc **xpp;			/* iterate over queue */
  ಸಂರಚನಾ proc *prev_xp;
  u64_t tsc, tsc_delta;

  ಸಂರಚನಾ proc **rdy_tail;

  ಪ್ರತಿಜ್ಞಾ(proc_ptr_ok(rp));
  ಪ್ರತಿಜ್ಞಾ(!proc_is_runnable(rp));

  /* Side-effect for kernel: check if the task's stack still is ok? */
  ಪ್ರತಿಜ್ಞಾ (!iskernelp(rp) || *priv(rp)->s_ರಾಶಿ_guard == ರಾಶಿ_GUARD);

  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  prev_xp = ರಿಕ್ತ;				
  ಪ್ರತಿ (xpp = get_cpu_var_ptr(rp->p_cpu, run_q_head[q]); *xpp;
		  xpp = &(*xpp)->p_nextready) {
      ಯದಿ (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          ಯದಿ (rp == rdy_tail[q]) {		/* queue tail removed */
              rdy_tail[q] = prev_xp;		/* set new tail */
	  }

          ಭಙ್ಗ;
      }
      prev_xp = *xpp;				/* save previous in chain */
  }

	
  /* Process accounting for scheduling */
  rp->p_accounting.dequeues++;

  /* this is not all that accurate on virtual machines, especially with
     IO bound processes that only spend a short amount of time in the queue
     at a time. */
  ಯದಿ (rp->p_accounting.enter_queue) {
	read_tsc_64(&tsc);
	tsc_delta = tsc - rp->p_accounting.enter_queue;
	rp->p_accounting.time_in_queue = rp->p_accounting.time_in_queue +
		tsc_delta;
	rp->p_accounting.enter_queue = 0;
  }

  /* For ps(1), remember when the process was last dequeued. */
  rp->p_dequeued = get_monotonic();

#ಯದಿ DEBUG_SANITYCHECKS
  ಪ್ರತಿಜ್ಞಾ(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಸಂರಚನಾ proc * pick_proc(ಶೂನ್ಯ)
{
/* Decide who to run now.  A new process is selected and returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp;			/* process to run */
  ಸಂರಚನಾ proc **rdy_head;
  ಪೂರ್ಣಾಙ್ಕ q;				/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the task table.
   * If there are no processes ready to run, return NULL.
   */
  rdy_head = get_cpulocal_var(run_q_head);
  ಪ್ರತಿ (q=0; q < NR_SCHED_QUEUES; q++) {	
	ಯದಿ(!(rp = rdy_head[q])) {
		TRACE(VF_PICKPROC, printf("cpu %d queue %d empty\n", cpuid, q););
		ಅನುವರ್ತ;
	}
	ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(rp));
	ಯದಿ (priv(rp)->s_flags & BILLABLE)	 	
		get_cpulocal_var(bill_ptr) = rp; /* bill for system time */
	ಪ್ರತ್ಯಾಗಮ rp;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*===========================================================================*
 *				endpoint_lookup				     *
 *===========================================================================*/
ಸಂರಚನಾ proc *endpoint_lookup(endpoint_t e)
{
	ಪೂರ್ಣಾಙ್ಕ n;

	ಯದಿ(!isokendpt(e, &n)) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ proc_addr(n);
}

/*===========================================================================*
 *				isokendpt_f				     *
 *===========================================================================*/
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
ಪೂರ್ಣಾಙ್ಕ isokendpt_f(ಧ್ರುವ ವರ್ಣ * file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, endpoint_t e, ಪೂರ್ಣಾಙ್ಕ * p,
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ fatalflag)
#ಅನ್ಯಥಾ
ಪೂರ್ಣಾಙ್ಕ isokendpt_f(endpoint_t e, ಪೂರ್ಣಾಙ್ಕ * p, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ fatalflag)
#endif
{
	ಪೂರ್ಣಾಙ್ಕ ok = 0;
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
	ಯದಿ(isokprocn(*p) && !isemptyn(*p) && proc_addr(*p)->p_endpoint == e)
		ok = 1;
	ಯದಿ(!ok && fatalflag)
		panic("invalid endpoint: %d",  e);
	ಪ್ರತ್ಯಾಗಮ ok;
}

ಸ್ಥಿರ ಶೂನ್ಯ notify_scheduler(ಸಂರಚನಾ proc *p)
{
	message m_no_quantum;
	ಪೂರ್ಣಾಙ್ಕ err;

	ಪ್ರತಿಜ್ಞಾ(!proc_kernel_scheduler(p));

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

	ಯದಿ ((err = mini_send(p, p->p_scheduler->p_endpoint,
					&m_no_quantum, FROM_KERNEL))) {
		panic("WARNING: Scheduling: mini_send returned %d\n", err);
	}
}

ಶೂನ್ಯ proc_no_time(ಸಂರಚನಾ proc * p)
{
	ಯದಿ (!proc_kernel_scheduler(p) && priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		notify_scheduler(p);
	}
	ಅನ್ಯಥಾ {
		/*
		 * non-preemptible processes only need their quantum to
		 * be renewed. In fact, they by pass scheduling
		 */
		p->p_cpu_time_ಎಡ = ms_2_cpu_time(p->p_quantum_size_ms);
#ಯದಿ DEBUG_RACE
		RTS_SET(p, RTS_PREEMPTED);
		RTS_UNSET(p, RTS_PREEMPTED);
#endif
	}
}

ಶೂನ್ಯ reset_proc_accounting(ಸಂರಚನಾ proc *p)
{
  p->p_accounting.preempted = 0;
  p->p_accounting.ipc_sync  = 0;
  p->p_accounting.ipc_async = 0;
  p->p_accounting.dequeues  = 0;
  p->p_accounting.time_in_queue = 0;
  p->p_accounting.enter_queue = 0;
}
	
ಶೂನ್ಯ copr_not_available_handler(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ proc * p;
	ಸಂರಚನಾ proc ** local_fpu_owner;
	/*
	 * Disable the FPU exception (both for the kernel and for the process
	 * once it's scheduled), and initialize or restore the FPU state.
	 */

	disable_fpu_exception();

	p = get_cpulocal_var(proc_ptr);

	/* if FPU is not owned by anyone, do not store anything */
	local_fpu_owner = get_cpulocal_var_ptr(fpu_owner);
	ಯದಿ (*local_fpu_owner != ರಿಕ್ತ) {
		ಪ್ರತಿಜ್ಞಾ(*local_fpu_owner != p);
		save_local_fpu(*local_fpu_owner, FALSE /*retain*/);
	}

	/*
	 * restore the current process' state and let it run again, do not
	 * schedule!
	 */
	ಯದಿ (restore_fpu(p) != OK) {
		/* Restoring FPU state failed. This is always the process's own
		 * fault. Send a signal, and schedule another process instead.
		 */
		*local_fpu_owner = ರಿಕ್ತ;		/* release FPU */
		cause_sig(proc_nr(p), SIGFPE);
		ಪ್ರತ್ಯಾಗಮ;
	}

	*local_fpu_owner = p;
	context_stop(proc_addr(KERNEL));
	restore_user_context(p);
	NOT_REACHABLE;
}

ಶೂನ್ಯ release_fpu(ಸಂರಚನಾ proc * p) {
	ಸಂರಚನಾ proc ** fpu_owner_ptr;

	fpu_owner_ptr = get_cpu_var_ptr(p->p_cpu, fpu_owner);

	ಯದಿ (*fpu_owner_ptr == p)
		*fpu_owner_ptr = ರಿಕ್ತ;
}

ಶೂನ್ಯ ser_dump_proc(ಶೂನ್ಯ)
{
        ಸಂರಚನಾ proc *pp;

        ಪ್ರತಿ (pp= BEG_PROC_ADDR; pp < END_PROC_ADDR; pp++)
        {
                ಯದಿ (isemptyp(pp))
                        ಅನುವರ್ತ;
                print_proc_recursive(pp);
        }
}
