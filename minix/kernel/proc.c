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

#అన్తర్భావ <stddef.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "vm.h"
#అన్తర్భావ "clock.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "arch_proto.h"

#అన్తర్భావ <minix/syslib.h>

/* Scheduling and message passing functions */
స్థిర శూన్య idle(శూన్య);
/**
 * Made public for use in clock.c (for user-space scheduling)
static int mini_send(struct proc *caller_ptr, endpoint_t dst_e, message
	*m_ptr, int flags);
*/
స్థిర పూర్ణాంకం mini_receive(నిర్మాణం proc *caller_ptr, endpoint_t src,
	సందేశం *m_buff_usr, పూర్ణాంకం flags);
స్థిర పూర్ణాంకం mini_senda(నిర్మాణం proc *caller_ptr, asynmsg_t *పట్టిక, size_t
	కొలత);
స్థిర పూర్ణాంకం deadlock(పూర్ణాంకం function, నమోదు నిర్మాణం proc *caller,
	endpoint_t src_dst_e);
స్థిర పూర్ణాంకం try_async(నిర్మాణం proc *caller_ptr);
స్థిర పూర్ణాంకం try_one(endpoint_t receive_e, నిర్మాణం proc *src_ptr,
	నిర్మాణం proc *dst_ptr);
స్థిర నిర్మాణం proc * pick_proc(శూన్య);
స్థిర శూన్య వరుసలో_చేర్చు_తల(నిర్మాణం proc *rp);

/* all idles share the same idle_priv structure */
స్థిర నిర్మాణం priv idle_priv;

స్థిర శూన్య సమితి_idle_name(అక్షరం * name, పూర్ణాంకం n)
{
        పూర్ణాంకం i, c;
        పూర్ణాంకం p_z = 0;

        ఒకవేళ (n > 999) 
                n = 999; 

        name[0] = 'i'; 
        name[1] = 'd'; 
        name[2] = 'l'; 
        name[3] = 'e'; 

        కొరకు (i = 4, c = 100; c > 0; c /= 10) {
                పూర్ణాంకం digit;

                digit = n / c;  
                n -= digit * c;  

                ఒకవేళ (p_z || digit != 0 || c == 1) {
                        p_z = 1;
                        name[i++] = '0' + digit;
                }   
        }    

        name[i] = '\0';

}


#నిర్వచించు PICK_ANY	1
#నిర్వచించు PICK_HIGHERONLY	2

#నిర్వచించు Build_Notify_సందేశం(m_ptr, src, dst_ptr) \
	memset((m_ptr), 0, పరిమాణం(*(m_ptr)));				\
	(m_ptr)->m_type = NOTIFY_సందేశం;				\
	(m_ptr)->m_notify.timestamp = get_monotonic();		\
	ఎంపిక (src) {							\
	స్థితి HARDWARE:							\
		(m_ptr)->m_notify.interrupts =			\
			priv(dst_ptr)->s_int_pending;			\
		priv(dst_ptr)->s_int_pending = 0;			\
		ఆపు;							\
	స్థితి SYSTEM:							\
		జ్ఞాపకం_నకలు(&(m_ptr)->m_notify.sigset,			\
			&priv(dst_ptr)->s_sig_pending,			\
			పరిమాణం(sigset_t));				\
		sigemptyset(&priv(dst_ptr)->s_sig_pending);		\
		ఆపు;							\
	}

స్థిర సందేశం m_notify_buff = { 0, NOTIFY_సందేశం };

శూన్య proc_init(శూన్య)
{
	నిర్మాణం proc * rp;
	నిర్మాణం priv *sp;
	పూర్ణాంకం i;

	/* Clear the process table. Announce each slot as empty and set up
	 * mappings for proc_addr() and proc_nr() macros. Do the same for the
	 * table with privilege structures for the system processes. 
	 */
	కొరకు (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < చివర_PROC_ADDR; ++rp, ++i) {
		rp->p_rts_flags = RTS_స్థానం_FREE;/* initialize free slot */
		rp->p_magic = PMAGIC;
		rp->p_nr = i;			/* proc number from ptr */
		rp->p_endpoint = _ENDPOINT(0, rp->p_nr); /* generation no. 0 */
		rp->p_scheduler = ఖాళీ;		/* no user space scheduler */
		rp->p_ప్రాధాన్యత = 0;		/* no priority */
		rp->p_కాలాంశం_కొలత_ms = 0;	/* no quantum size */

		/* arch-specific initialization */
		arch_proc_reset(rp);
	}
	కొరకు (sp = BEG_PRIV_ADDR, i = 0; sp < చివర_PRIV_ADDR; ++sp, ++i) {
		sp->s_proc_nr = NONE;		/* initialize as free */
		sp->s_id = (sys_id_t) i;	/* priv structure index */
		ppriv_addr[i] = sp;		/* priv ptr from number */
		sp->s_sig_mgr = NONE;		/* clear signal managers */
		sp->s_bak_sig_mgr = NONE;
	}

	idle_priv.s_flags = IDL_F;
	/* initialize IDLE structures for every CPU */
	కొరకు (i = 0; i < CONFIG_MAX_CPUS; i++) {
		నిర్మాణం proc * ip = get_cpu_var_ptr(i, idle_proc);
		ip->p_endpoint = IDLE;
		ip->p_priv = &idle_priv;
		/* must not let idle ever get scheduled */
		ip->p_rts_flags |= RTS_PROC_STOP;
		సమితి_idle_name(ip->p_name, i);
	}
}

స్థిర శూన్య switch_చిరునామా_space_idle(శూన్య)
{
#నిర్వచితమైతే CONFIG_SMP
	/*
	 * currently we bet that VM is always alive and its pages available so
	 * when the CPU wakes up the kernel is mapped and no surprises happen.
	 * This is only a problem if more than 1 cpus are available
	 */
	switch_చిరునామా_space(proc_addr(VM_PROC_NR));
#అయితే_ముగింపు
}

/*===========================================================================*
 *				idle					     * 
 *===========================================================================*/
స్థిర శూన్య idle(శూన్య)
{
	నిర్మాణం proc * p;

	/* This function is called whenever there is no work to do.
	 * Halt the CPU, and measure how many timestamp counter ticks are
	 * spent not doing anything. This allows test setups to measure
	 * the CPU utilization of certain workloads with high precision.
	 */

	p = get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	ఒకవేళ (priv(p)->s_flags & BILLABLE)
		get_cpulocal_var(bill_ptr) = p;

	switch_చిరునామా_space_idle();

#నిర్వచితమైతే CONFIG_SMP
	get_cpulocal_var(cpu_is_idle) = 1;
	/* we don't need to keep time on APs as it is handled on the BSP */
	ఒకవేళ (cpuid != bsp_cpu_id)
		stop_local_timer();
	లేకపోతే
#అయితే_ముగింపు
	{
		/*
		 * If the timer has expired while in kernel we must
		 * rearm it before we go to sleep
		 */
		restart_local_timer();
	}

	/* start accounting for the idle time */
	context_stop(proc_addr(KERNEL));
#ఒకవేళ !SPROFILE
	halt_cpu();
#లేకపోతే
	ఒకవేళ (!sprofiling)
		halt_cpu();
	లేకపోతే {
		అస్థిర పూర్ణాంకం * v;

		v = get_cpulocal_var_ptr(idle_interrupted);
		interrupts_enable();
		అయితే (!*v)
			arch_pause();
		interrupts_disable();
		*v = 0;
	}
#అయితే_ముగింపు
	/*
	 * end of accounting for the idle task does not happen here, the kernel
	 * is handling stuff for quite a while before it gets back here!
	 */
}

/*===========================================================================*
 *                              vm_suspend                                *
 *===========================================================================*/
శూన్య vm_suspend(నిర్మాణం proc *caller, స్థిరరాశి నిర్మాణం proc *లక్ష్యం,
        స్థిరరాశి vir_బైట్లు linaddr, స్థిరరాశి vir_బైట్లు len, స్థిరరాశి పూర్ణాంకం type,
        స్థిరరాశి పూర్ణాంకం writeflag)
{
        /* This range is not OK for this process. Set parameters
         * of the request and notify VM about the pending request.
         */
        ప్రతిజ్ఞా(!RTS_ISSET(caller, RTS_VMREQUEST));
        ప్రతిజ్ఞా(!RTS_ISSET(లక్ష్యం, RTS_VMREQUEST));

        RTS_సమితి(caller, RTS_VMREQUEST);

        caller->p_vmrequest.req_type = VMPTYPE_CHECK;
        caller->p_vmrequest.లక్ష్యం = లక్ష్యం->p_endpoint;
        caller->p_vmrequest.params.check.ప్రారంభం = linaddr;
        caller->p_vmrequest.params.check.నిడివి = len;
        caller->p_vmrequest.params.check.writeflag = writeflag;
        caller->p_vmrequest.type = type;

        /* Connect caller on vmrequest wait queue. */
        ఒకవేళ(!(caller->p_vmrequest.nextrequestor = vmrequest))
                ఒకవేళ(OK != send_sig(VM_PROC_NR, SIGKMEM))
                        panic("send_sig failed");
        vmrequest = caller;
}

/*===========================================================================*
 *                              delivermsg                                *
 *===========================================================================*/
స్థిర శూన్య delivermsg(నిర్మాణం proc *rp)
{
        ప్రతిజ్ఞా(!RTS_ISSET(rp, RTS_VMREQUEST));
        ప్రతిజ్ఞా(rp->p_misc_flags & MF_DELIVERMSG);
        ప్రతిజ్ఞా(rp->p_delivermsg.m_ఆధారం != NONE);

        ఒకవేళ (copy_సందేశం_గమ్యం_user(&rp->p_delivermsg,
                                (సందేశం *) rp->p_delivermsg_vir)) {
                ఒకవేళ(rp->p_misc_flags & MF_MSGFAILED) {
                        /* 2nd consecutive failure means this won't succeed */
                        రాయి("WARNING wrong user pointer 0x%08lx from "
                                "process %s / %d\n",
                                rp->p_delivermsg_vir,
                                rp->p_name,
                                rp->p_endpoint);
                        cause_sig(rp->p_nr, SIGSEGV);
                } లేకపోతే {
                        /* 1st failure means we have to ask VM to handle it */
                        vm_suspend(rp, rp, rp->p_delivermsg_vir,
                                పరిమాణం(సందేశం), VMSTYPE_DELIVERMSG, 1);
                        rp->p_misc_flags |= MF_MSGFAILED;
                }
        } లేకపోతే {
                /* Indicate message has been delivered; address is 'used'. */
                rp->p_delivermsg.m_ఆధారం = NONE;
                rp->p_misc_flags &= ~(MF_DELIVERMSG|MF_MSGFAILED);

                ఒకవేళ(!(rp->p_misc_flags & MF_CONTEXT_సమితి)) {
                        rp->p_reg.retreg = OK;
                }
        }
}

/*===========================================================================*
 *				switch_to_user				     * 
 *===========================================================================*/
శూన్య switch_గమ్యం_user(శూన్య)
{
	/* This function is called an instant before proc_ptr is
	 * to be scheduled again.
	 */
	నిర్మాణం proc * p;
#నిర్వచితమైతే CONFIG_SMP
	పూర్ణాంకం tlb_must_refresh = 0;
#అయితే_ముగింపు

	p = get_cpulocal_var(proc_ptr);
	/*
	 * if the current process is still runnable check the misc flags and let
	 * it run unless it becomes not runnable in the meantime
	 */
	ఒకవేళ (proc_is_runnable(p))
		వెళ్ళు check_misc_flags;
	/*
	 * if a process becomes not runnable while handling the misc flags, we
	 * need to pick a new one here and start from scratch. Also if the
	 * current process wasn't runnable, we pick a new one here
	 */
not_runnable_pick_new:
	ఒకవేళ (proc_is_preempted(p)) {
		p->p_rts_flags &= ~RTS_PREEMPTED;
		ఒకవేళ (proc_is_runnable(p)) {
			ఒకవేళ (p->p_cpu_సమయం_ఎడమ)
				వరుసలో_చేర్చు_తల(p);
			లేకపోతే
				వరుసలో_చేర్చు(p);
		}
	}

	/*
	 * if we have no process to run, set IDLE as the current process for
	 * time accounting and put the cpu in an idle state. After the next
	 * timer interrupt the execution resumes here and we can pick another
	 * process. If there is still nothing runnable we "schedule" IDLE again
	 */
	అయితే (!(p = pick_proc())) {
		idle();
	}

	/* update the global variable */
	get_cpulocal_var(proc_ptr) = p;

#నిర్వచితమైతే CONFIG_SMP
	ఒకవేళ (p->p_misc_flags & MF_FLUSH_TLB && get_cpulocal_var(ptproc) == p)
		tlb_must_refresh = 1;
#అయితే_ముగింపు
	switch_చిరునామా_space(p);

check_misc_flags:

	ప్రతిజ్ఞా(p);
	ప్రతిజ్ఞా(proc_is_runnable(p));
	అయితే (p->p_misc_flags &
		(MF_KCALL_RESUME | MF_DELIVERMSG |
		 MF_SC_DEFER | MF_SC_TRACE | MF_SC_చురుకైన)) {

		ప్రతిజ్ఞా(proc_is_runnable(p));
		ఒకవేళ (p->p_misc_flags & MF_KCALL_RESUME) {
			kernel_call_resume(p);
		}
		లేకపోతే ఒకవేళ (p->p_misc_flags & MF_DELIVERMSG) {
			TRACE(VF_SCHEDULING, రాయి("delivering to %s / %d\n",
				p->p_name, p->p_endpoint););
			delivermsg(p);
		}
		లేకపోతే ఒకవేళ (p->p_misc_flags & MF_SC_DEFER) {
			/* Perform the system call that we deferred earlier. */

			ప్రతిజ్ఞా (!(p->p_misc_flags & MF_SC_చురుకైన));

			arch_do_syscall(p);

			/* If the process is stopped for signal delivery, and
			 * not blocked sending a message after the system call,
			 * inform PM.
			 */
			ఒకవేళ ((p->p_misc_flags & MF_SIG_DELAY) &&
					!RTS_ISSET(p, RTS_SENDING))
				sig_delay_పూర్తి(p);
		}
		లేకపోతే ఒకవేళ (p->p_misc_flags & MF_SC_TRACE) {
			/* Trigger a system call leave event if this was a
			 * system call. We must do this after processing the
			 * other flags above, both for tracing correctness and
			 * to be able to use 'break'.
			 */
			ఒకవేళ (!(p->p_misc_flags & MF_SC_చురుకైన))
				ఆపు;

			p->p_misc_flags &=
				~(MF_SC_TRACE | MF_SC_చురుకైన);

			/* Signal the "leave system call" event.
			 * Block the process.
			 */
			cause_sig(proc_nr(p), SIGTRAP);
		}
		లేకపోతే ఒకవేళ (p->p_misc_flags & MF_SC_చురుకైన) {
			/* If MF_SC_ACTIVE was set, remove it now:
			 * we're leaving the system call.
			 */
			p->p_misc_flags &= ~MF_SC_చురుకైన;

			ఆపు;
		}

		/*
		 * the selected process might not be runnable anymore. We have
		 * to checkit and schedule another one
		 */
		ఒకవేళ (!proc_is_runnable(p))
			వెళ్ళు not_runnable_pick_new;
	}
	/*
	 * check the quantum left before it runs again. We must do it only here
	 * as we are sure that a possible out-of-quantum message to the
	 * scheduler will not collide with the regular ipc
	 */
	ఒకవేళ (!p->p_cpu_సమయం_ఎడమ)
		proc_no_సమయం(p);
	/*
	 * After handling the misc flags the selected process might not be
	 * runnable anymore. We have to checkit and schedule another one
	 */
	ఒకవేళ (!proc_is_runnable(p))
		వెళ్ళు not_runnable_pick_new;

	TRACE(VF_SCHEDULING, రాయి("cpu %d starting %s / %d "
				"pc 0x%08x\n",
		cpuid, p->p_name, p->p_endpoint, p->p_reg.pc););
#ఒకవేళ DEBUG_TRACE
	p->p_schedules++;
#అయితే_ముగింపు

	p = arch_ముగింపు_switch_గమ్యం_user();
	ప్రతిజ్ఞా(p->p_cpu_సమయం_ఎడమ);

	context_stop(proc_addr(KERNEL));

	/* If the process isn't the owner of FPU, enable the FPU exception */
	ఒకవేళ (get_cpulocal_var(fpu_owner) != p)
		enable_fpu_exception();
	లేకపోతే
		disable_fpu_exception();

	/* If MF_CONTEXT_SET is set, don't clobber process state within
	 * the kernel. The next kernel entry is OK again though.
	 */
	p->p_misc_flags &= ~MF_CONTEXT_సమితి;

#ఒకవేళ defined(__i386__)
  	ప్రతిజ్ఞా(p->p_seg.p_cr3 != 0);
#లేదావేళ defined(__arm__)
	ప్రతిజ్ఞా(p->p_seg.p_ttbr != 0);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_SMP
	ఒకవేళ (p->p_misc_flags & MF_FLUSH_TLB) {
		ఒకవేళ (tlb_must_refresh)
			refresh_tlb();
		p->p_misc_flags &= ~MF_FLUSH_TLB;
	}
#అయితే_ముగింపు
	
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
స్థిర పూర్ణాంకం do_sync_ipc(నిర్మాణం proc * caller_ptr, /* who made the call */
			పూర్ణాంకం call_nr,	/* system call number and flags */
			endpoint_t src_dst_e,	/* src or dst of the call */
			సందేశం *m_ptr)	/* users pointer to a message */
{
  పూర్ణాంకం ఫలితం;					/* the system call's result */
  పూర్ణాంకం src_dst_p;				/* Process slot number */
  అక్షరం *callname;

  /* Check destination. RECEIVE is the only call that accepts ANY (in addition
   * to a real endpoint). The other calls (SEND, SENDREC, and NOTIFY) require an
   * endpoint to corresponds to a process. In addition, it is necessary to check
   * whether a process is allowed to send to a given destination.
   */
  ప్రతిజ్ఞా(call_nr != SENDA);

  /* Only allow non-negative call_nr values less than 32 */
  ఒకవేళ (call_nr < 0 || call_nr > IPCNO_HIGHEST || call_nr >= 32
      || !(callname = ipc_call_పేర్లు[call_nr])) {
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
      రాయి("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          call_nr, proc_nr(caller_ptr), src_dst_e);
#అయితే_ముగింపు
	తిరిగి(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ఒకవేళ (src_dst_e == ANY)
  {
	ఒకవేళ (call_nr != RECEIVE)
	{
#ఒకవేళ 0
		రాయి("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#అయితే_ముగింపు
		తిరిగి EINVAL;
	}
	src_dst_p = (పూర్ణాంకం) src_dst_e;
  }
  లేకపోతే
  {
	/* Require a valid source and/or destination process. */
	ఒకవేళ(!isokendpt(src_dst_e, &src_dst_p)) {
#ఒకవేళ 0
		రాయి("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#అయితే_ముగింపు
		తిరిగి EDEADSRCDST;
	}

	/* If the call is to send to a process, i.e., for SEND, SENDNB,
	 * SENDREC or NOTIFY, verify that the caller is allowed to send to
	 * the given destination. 
	 */
	ఒకవేళ (call_nr != RECEIVE)
	{
		ఒకవేళ (!may_send_గమ్యం(caller_ptr, src_dst_p)) {
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
			రాయి(
			"sys_call: ipc mask denied %s from %d to %d\n",
				callname,
				caller_ptr->p_endpoint, src_dst_e);
#అయితే_ముగింపు
			తిరిగి(ECALLDENIED);	/* call denied by ipc mask */
		}
	}
  }

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  ఒకవేళ (!(priv(caller_ptr)->s_trap_ముసుగు & (1 << call_nr))) {
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
      రాయి("sys_call: %s not allowed, caller %d, src_dst %d\n", 
          callname, proc_nr(caller_ptr), src_dst_p);
#అయితే_ముగింపు
	తిరిగి(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ఒకవేళ (call_nr != SENDREC && call_nr != RECEIVE && iskerneln(src_dst_p)) {
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
      రాయి("sys_call: trap %s not allowed, caller %d, src_dst %d\n",
           callname, proc_nr(caller_ptr), src_dst_e);
#అయితే_ముగింపు
	తిరిగి(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ఎంపిక(call_nr) {
  స్థితి SENDREC:
	/* A flag is set so that notifications cannot interrupt SENDREC. */
	caller_ptr->p_misc_flags |= MF_జవాబు_PEND;
	/* fall through */
  స్థితి SEND:			
	ఫలితం = mini_send(caller_ptr, src_dst_e, m_ptr, 0);
	ఒకవేళ (call_nr == SEND || ఫలితం != OK)
		ఆపు;				/* done, or SEND failed */
	/* fall through for SENDREC */
  స్థితి RECEIVE:			
	ఒకవేళ (call_nr == RECEIVE) {
		caller_ptr->p_misc_flags &= ~MF_జవాబు_PEND;
		IPC_స్థితి_CLEAR(caller_ptr);  /* clear IPC status code */
	}
	ఫలితం = mini_receive(caller_ptr, src_dst_e, m_ptr, 0);
	ఆపు;
  స్థితి NOTIFY:
	ఫలితం = mini_notify(caller_ptr, src_dst_e);
	ఆపు;
  స్థితి SENDNB:
        ఫలితం = mini_send(caller_ptr, src_dst_e, m_ptr, NON_BLOCKING);
        ఆపు;
  అప్రమేయ:
	ఫలితం = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  తిరిగి(ఫలితం);
}

పూర్ణాంకం do_ipc(reg_t r1, reg_t r2, reg_t r3)
{
  నిర్మాణం proc *స్థిరరాశి caller_ptr = get_cpulocal_var(proc_ptr);	/* get pointer to caller */
  పూర్ణాంకం call_nr = (పూర్ణాంకం) r1;

  ప్రతిజ్ఞా(!RTS_ISSET(caller_ptr, RTS_స్థానం_FREE));

  /* bill kernel time to this process. */
  kbill_ipc = caller_ptr;

  /* If this process is subject to system call tracing, handle that first. */
  ఒకవేళ (caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
	/* Are we tracing this process, and is it the first sys_call entry? */
	ఒకవేళ ((caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) ==
							MF_SC_TRACE) {
		/* We must notify the tracer before processing the actual
		 * system call. If we don't, the tracer could not obtain the
		 * input message. Postpone the entire system call.
		 */
		caller_ptr->p_misc_flags &= ~MF_SC_TRACE;
		ప్రతిజ్ఞా(!(caller_ptr->p_misc_flags & MF_SC_DEFER));
		caller_ptr->p_misc_flags |= MF_SC_DEFER;
		caller_ptr->p_defer.r1 = r1;
		caller_ptr->p_defer.r2 = r2;
		caller_ptr->p_defer.r3 = r3;

		/* Signal the "enter system call" event. Block the process. */
		cause_sig(proc_nr(caller_ptr), SIGTRAP);

		/* Preserve the return register's value. */
		తిరిగి caller_ptr->p_reg.retreg;
	}

	/* If the MF_SC_DEFER flag is set, the syscall is now being resumed. */
	caller_ptr->p_misc_flags &= ~MF_SC_DEFER;

	ప్రతిజ్ఞా (!(caller_ptr->p_misc_flags & MF_SC_చురుకైన));

	/* Set a flag to allow reliable tracing of leaving the system call. */
	caller_ptr->p_misc_flags |= MF_SC_చురుకైన;
  }

  ఒకవేళ(caller_ptr->p_misc_flags & MF_DELIVERMSG) {
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
  ఎంపిక(call_nr) {
  	స్థితి SENDREC:
  	స్థితి SEND:			
  	స్థితి RECEIVE:			
  	స్థితి NOTIFY:
  	స్థితి SENDNB:
  	{
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_sync++;

  	    తిరిగి do_sync_ipc(caller_ptr, call_nr, (endpoint_t) r2,
			    (సందేశం *) r3);
  	}
  	స్థితి SENDA:
  	{
 	    /*
  	     * Get and check the size of the argument in bytes as it is a
  	     * table
  	     */
  	    size_t సందేశం_కొలత = (size_t) r2;
  
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_async++;
 
  	    /* Limit size to something reasonable. An arbitrary choice is 16
  	     * times the number of process table entries.
  	     */
  	    ఒకవేళ (సందేశం_కొలత > 16*(NR_TASKS + NR_PROCS))
	        తిరిగి EDOM;
  	    తిరిగి mini_senda(caller_ptr, (asynmsg_t *) r3, సందేశం_కొలత);
  	}
  	స్థితి MINIX_KERNINFO:
	{
		/* It might not be initialized yet. */
	  	ఒకవేళ(!minix_kerninfo_user) {
			తిరిగి EBADCALL;
		}

  		arch_సమితి_secondary_ipc_return(caller_ptr, minix_kerninfo_user);
  		తిరిగి OK;
	}
  	అప్రమేయ:
	తిరిగి EBADCALL;		/* illegal system call */
  }
}

/*===========================================================================*
 *				deadlock				     * 
 *===========================================================================*/
స్థిర పూర్ణాంకం deadlock(
  పూర్ణాంకం function,				/* trap number */
  నమోదు నిర్మాణం proc *cp,		/* pointer to caller */
  endpoint_t src_dst_e			/* src or dst process */
)
{
/* Check for deadlock. This can happen if 'caller_ptr' and 'src_dst' have
 * a cyclic dependency of blocking send and receive calls. The only cyclic 
 * dependency that is not fatal is if the caller and target directly SEND(REC)
 * and RECEIVE to each other. If a deadlock is found, the group size is 
 * returned. Otherwise zero is returned. 
 */
  నమోదు నిర్మాణం proc *xp;			/* process pointer */
  పూర్ణాంకం సమూహం_కొలత = 1;				/* start with only caller */
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
  స్థిర నిర్మాణం proc *processes[NR_PROCS + NR_TASKS];
  processes[0] = cp;
#అయితే_ముగింపు

  అయితే (src_dst_e != ANY) { 			/* check while process nr */
      పూర్ణాంకం src_dst_స్థానం;
      okendpt(src_dst_e, &src_dst_స్థానం);
      xp = proc_addr(src_dst_స్థానం);		/* follow chain of processes */
      ప్రతిజ్ఞా(proc_ptr_ok(xp));
      ప్రతిజ్ఞా(!RTS_ISSET(xp, RTS_స్థానం_FREE));
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
      processes[సమూహం_కొలత] = xp;
#అయితే_ముగింపు
      సమూహం_కొలత ++;				/* extra process in group */

      /* Check whether the last process in the chain has a dependency. If it 
       * has not, the cycle cannot be closed and we are done.
       */
      ఒకవేళ((src_dst_e = P_BLOCKEDON(xp)) == NONE)
	తిరిగి 0;

      /* Now check if there is a cyclic dependency. For group sizes of two,  
       * a combination of SEND(REC) and RECEIVE is not fatal. Larger groups
       * or other combinations indicate a deadlock.  
       */
      ఒకవేళ (src_dst_e == cp->p_endpoint) {	/* possible deadlock */
	  ఒకవేళ (సమూహం_కొలత == 2) {		/* caller and src_dst */
	      /* The function number is magically converted to flags. */
	      ఒకవేళ ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING) { 
	          తిరిగి(0);			/* not a deadlock */
	      }
	  }
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
	  {
		పూర్ణాంకం i;
		రాయి("deadlock between these processes:\n");
		కొరకు(i = 0; i < సమూహం_కొలత; i++) {
			రాయి(" %10s ", processes[i]->p_name);
		}
		రాయి("\n\n");
		కొరకు(i = 0; i < సమూహం_కొలత; i++) {
			print_proc(processes[i]);
			proc_stacktrace(processes[i]);
		}
	  }
#అయితే_ముగింపు
          తిరిగి(సమూహం_కొలత);			/* deadlock found */
      }
  }
  తిరిగి(0);					/* not a deadlock */
}

/*===========================================================================*
 *				has_pending				     * 
 *===========================================================================*/
స్థిర పూర్ణాంకం has_pending(sys_map_t *map, పూర్ణాంకం src_p, పూర్ణాంకం asynm)
{
/* Check to see if there is a pending message from the desired source
 * available.
 */

  పూర్ణాంకం src_id;
  sys_id_t id = NULL_PRIV_ID;
#నిర్వచితమైతే CONFIG_SMP
  నిర్మాణం proc * p;
#అయితే_ముగింపు

  /* Either check a specific bit in the mask map, or find the first bit set in
   * it (if any), depending on whether the receive was called on a specific
   * source endpoint.
   */
  ఒకవేళ (src_p != ANY) {
	src_id = nr_గమ్యం_id(src_p);
	ఒకవేళ (get_sys_బిట్టు(*map, src_id)) {
#నిర్వచితమైతే CONFIG_SMP
		p = proc_addr(id_గమ్యం_nr(src_id));
		ఒకవేళ (asynm && RTS_ISSET(p, RTS_VMINHIBIT))
			p->p_misc_flags |= MF_SENDA_VM_తప్పిపోవు;
		లేకపోతే
#అయితే_ముగింపు
			id = src_id;
	}
  } లేకపోతే {
	/* Find a source with a pending message */
	కొరకు (src_id = 0; src_id < NR_SYS_PROCS; src_id += BITCHUNK_బిట్లు) {
		ఒకవేళ (get_sys_బిట్లు(*map, src_id) != 0) {
#నిర్వచితమైతే CONFIG_SMP
			అయితే (src_id < NR_SYS_PROCS) {
				అయితే (!get_sys_బిట్టు(*map, src_id)) {
					ఒకవేళ (src_id == NR_SYS_PROCS)
						వెళ్ళు quit_శోధన;
					src_id++;
				}
				p = proc_addr(id_గమ్యం_nr(src_id));
				/*
				 * We must not let kernel fiddle with pages of a
				 * process which are currently being changed by
				 * VM.  It is dangerous! So do not report such a
				 * process as having pending async messages.
				 * Skip it.
				 */
				ఒకవేళ (asynm && RTS_ISSET(p, RTS_VMINHIBIT)) {
					p->p_misc_flags |= MF_SENDA_VM_తప్పిపోవు;
					src_id++;
				} లేకపోతే
					వెళ్ళు quit_శోధన;
			}
#లేకపోతే
			అయితే (!get_sys_బిట్టు(*map, src_id)) src_id++;
			వెళ్ళు quit_శోధన;
#అయితే_ముగింపు
		}
	}

quit_శోధన:
	ఒకవేళ (src_id < NR_SYS_PROCS)	/* Found one */
		id = src_id;
  }

  తిరిగి(id);
}

/*===========================================================================*
 *				has_pending_notify			     *
 *===========================================================================*/
పూర్ణాంకం has_pending_notify(నిర్మాణం proc * caller, పూర్ణాంకం src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	తిరిగి has_pending(map, src_p, 0);
}

/*===========================================================================*
 *				has_pending_asend			     *
 *===========================================================================*/
పూర్ణాంకం has_pending_asend(నిర్మాణం proc * caller, పూర్ణాంకం src_p)
{
	sys_map_t * map = &priv(caller)->s_asyn_pending;
	తిరిగి has_pending(map, src_p, 1);
}

/*===========================================================================*
 *				unset_notify_pending			     *
 *===========================================================================*/
శూన్య unset_notify_pending(నిర్మాణం proc * caller, పూర్ణాంకం src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	unset_sys_బిట్టు(*map, src_p);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
పూర్ణాంకం mini_send(
  నమోదు నిర్మాణం proc *caller_ptr,	/* who is trying to send a message? */
  endpoint_t dst_e,			/* to whom is message being sent? */
  సందేశం *m_ptr,			/* pointer to message buffer */
  స్థిరరాశి పూర్ణాంకం flags
)
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  నమోదు నిర్మాణం proc *dst_ptr;
  నమోదు నిర్మాణం proc **xpp;
  పూర్ణాంకం dst_p;
  dst_p = _ENDPOINT_P(dst_e);
  dst_ptr = proc_addr(dst_p);

  ఒకవేళ (RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT))
  {
	తిరిగి EDEADSRCDST;
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * RTS_SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  ఒకవేళ (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, (vir_బైట్లు)m_ptr, ఖాళీ)) {
	పూర్ణాంకం call;
	/* Destination is indeed waiting for this message. */
	ప్రతిజ్ఞా(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));	

	ఒకవేళ (!(flags & FROM_KERNEL)) {
		ఒకవేళ(copy_సందేశం_from_user(m_ptr, &dst_ptr->p_delivermsg))
			తిరిగి EFAULT;
	} లేకపోతే {
		dst_ptr->p_delivermsg = *m_ptr;
		IPC_స్థితి_కూడు_FLAGS(dst_ptr, IPC_FLG_సందేశం_FROM_KERNEL);
	}

	dst_ptr->p_delivermsg.m_ఆధారం = caller_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;

	call = (caller_ptr->p_misc_flags & MF_జవాబు_PEND ? SENDREC
		: (flags & NON_BLOCKING ? SENDNB : SEND));
	IPC_స్థితి_కూడు_CALL(dst_ptr, call);

	ఒకవేళ (dst_ptr->p_misc_flags & MF_జవాబు_PEND)
		dst_ptr->p_misc_flags &= ~MF_జవాబు_PEND;

	RTS_UNSET(dst_ptr, RTS_RECEIVING);

#ఒకవేళ DEBUG_IPC_HOOK
	hook_ipc_msgsend(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#అయితే_ముగింపు
  } లేకపోతే {
	ఒకవేళ(flags & NON_BLOCKING) {
		తిరిగి(ENOTREADY);
	}

	/* Check for a possible deadlock before actually blocking. */
	ఒకవేళ (deadlock(SEND, caller_ptr, dst_e)) {
		తిరిగి(ELOCKED);
	}

	/* Destination is not waiting.  Block and dequeue caller. */
	ఒకవేళ (!(flags & FROM_KERNEL)) {
		ఒకవేళ(copy_సందేశం_from_user(m_ptr, &caller_ptr->p_sendmsg))
			తిరిగి EFAULT;
	} లేకపోతే {
		caller_ptr->p_sendmsg = *m_ptr;
		/*
		 * we need to remember that this message is from kernel so we
		 * can set the delivery status flags when the message is
		 * actually delivered
		 */
		caller_ptr->p_misc_flags |= MF_SENDING_FROM_KERNEL;
	}

	RTS_సమితి(caller_ptr, RTS_SENDING);
	caller_ptr->p_sendto_e = dst_e;

	/* Process is now blocked.  Put in on the destination's queue. */
	ప్రతిజ్ఞా(caller_ptr->p_q_link == ఖాళీ);
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	అయితే (*xpp) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */

#ఒకవేళ DEBUG_IPC_HOOK
	hook_ipc_msgsend(&caller_ptr->p_sendmsg, caller_ptr, dst_ptr);
#అయితే_ముగింపు
  }
  తిరిగి(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
స్థిర పూర్ణాంకం mini_receive(నిర్మాణం proc * caller_ptr,
			endpoint_t src_e, /* which message source is wanted */
			సందేశం * m_buff_usr, /* pointer to message buffer */
			స్థిరరాశి పూర్ణాంకం flags)
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller.
 */
  నమోదు నిర్మాణం proc **xpp;
  పూర్ణాంకం r, src_id, దొరికింది, src_proc_nr, src_p;
  endpoint_t sender_e;

  ప్రతిజ్ఞా(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));

  /* This is where we want our message. */
  caller_ptr->p_delivermsg_vir = (vir_బైట్లు) m_buff_usr;

  ఒకవేళ(src_e == ANY) src_p = ANY;
  లేకపోతే
  {
	okendpt(src_e, &src_p);
	ఒకవేళ (RTS_ISSET(proc_addr(src_p), RTS_NO_ENDPOINT))
	{
		తిరిగి EDEADSRCDST;
	}
  }


  /* Check to see if a message from desired source is already available.  The
   * caller's RTS_SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  ఒకవేళ (!RTS_ISSET(caller_ptr, RTS_SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    ఒకవేళ (! (caller_ptr->p_misc_flags & MF_జవాబు_PEND)) {

	/* Check for pending notifications */
        src_id = has_pending_notify(caller_ptr, src_p);
        దొరికింది = src_id != NULL_PRIV_ID;
        ఒకవేళ(దొరికింది) {
            src_proc_nr = id_గమ్యం_nr(src_id);		/* get source proc */
            sender_e = proc_addr(src_proc_nr)->p_endpoint;
        }

        ఒకవేళ (దొరికింది && CANRECEIVE(src_e, sender_e, caller_ptr, 0,
          &m_notify_buff)) {

#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
	    ఒకవేళ(src_proc_nr == NONE) {
		రాయి("mini_receive: sending notify from NONE\n");
	    }
#అయితే_ముగింపు
	    ప్రతిజ్ఞా(src_proc_nr != NONE);
            unset_notify_pending(caller_ptr, src_id);	/* no longer pending */

            /* Found a suitable source, deliver the notification message. */
	    ప్రతిజ్ఞా(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));	
	    ప్రతిజ్ఞా(src_e == ANY || sender_e == src_e);

	    /* assemble message */
	    Build_Notify_సందేశం(&caller_ptr->p_delivermsg, src_proc_nr, caller_ptr);
	    caller_ptr->p_delivermsg.m_ఆధారం = sender_e;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;

	    IPC_స్థితి_కూడు_CALL(caller_ptr, NOTIFY);

	    వెళ్ళు receive_పూర్తి;
        }
    }

    /* Check for pending asynchronous messages */
    ఒకవేళ (has_pending_asend(caller_ptr, src_p) != NULL_PRIV_ID) {
        ఒకవేళ (src_p != ANY)
		r = try_one(src_e, proc_addr(src_p), caller_ptr);
        లేకపోతే
        	r = try_async(caller_ptr);

	ఒకవేళ (r == OK) {
            IPC_స్థితి_కూడు_CALL(caller_ptr, SENDA);
            వెళ్ళు receive_పూర్తి;
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    అయితే (*xpp) {
	నిర్మాణం proc * sender = *xpp;
	endpoint_t sender_e = sender->p_endpoint;

        ఒకవేళ (CANRECEIVE(src_e, sender_e, caller_ptr, 0, &sender->p_sendmsg)) {
            పూర్ణాంకం call;
	    ప్రతిజ్ఞా(!RTS_ISSET(sender, RTS_స్థానం_FREE));
	    ప్రతిజ్ఞా(!RTS_ISSET(sender, RTS_NO_ENDPOINT));

	    /* Found acceptable message. Copy it and update status. */
  	    ప్రతిజ్ఞా(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	    caller_ptr->p_delivermsg = sender->p_sendmsg;
	    caller_ptr->p_delivermsg.m_ఆధారం = sender->p_endpoint;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;
	    RTS_UNSET(sender, RTS_SENDING);

	    call = (sender->p_misc_flags & MF_జవాబు_PEND ? SENDREC : SEND);
	    IPC_స్థితి_కూడు_CALL(caller_ptr, call);

	    /*
	     * if the message is originally from the kernel on behalf of this
	     * process, we must send the status flags accordingly
	     */
	    ఒకవేళ (sender->p_misc_flags & MF_SENDING_FROM_KERNEL) {
		IPC_స్థితి_కూడు_FLAGS(caller_ptr, IPC_FLG_సందేశం_FROM_KERNEL);
		/* we can clean the flag now, not need anymore */
		sender->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
	    }
	    ఒకవేళ (sender->p_misc_flags & MF_SIG_DELAY)
		sig_delay_పూర్తి(sender);

#ఒకవేళ DEBUG_IPC_HOOK
            hook_ipc_msgrecv(&caller_ptr->p_delivermsg, *xpp, caller_ptr);
#అయితే_ముగింపు
		
            *xpp = sender->p_q_link;		/* remove from queue */
	    sender->p_q_link = ఖాళీ;
	    వెళ్ళు receive_పూర్తి;
	}
	xpp = &sender->p_q_link;		/* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC. 
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  ఒకవేళ ( ! (flags & NON_BLOCKING)) {
      /* Check for a possible deadlock before actually blocking. */
      ఒకవేళ (deadlock(RECEIVE, caller_ptr, src_e)) {
          తిరిగి(ELOCKED);
      }

      caller_ptr->p_getfrom_e = src_e;		
      RTS_సమితి(caller_ptr, RTS_RECEIVING);
      తిరిగి(OK);
  } లేకపోతే {
	తిరిగి(ENOTREADY);
  }

receive_పూర్తి:
  ఒకవేళ (caller_ptr->p_misc_flags & MF_జవాబు_PEND)
	  caller_ptr->p_misc_flags &= ~MF_జవాబు_PEND;
  తిరిగి OK;
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
పూర్ణాంకం mini_notify(
  స్థిరరాశి నిర్మాణం proc *caller_ptr,	/* sender of the notification */
  endpoint_t dst_e			/* which process to notify */
)
{
  నమోదు నిర్మాణం proc *dst_ptr;
  పూర్ణాంకం src_id;				/* source id for late delivery */
  పూర్ణాంకం dst_p;

  ఒకవేళ (!isokendpt(dst_e, &dst_p)) {
	util_stacktrace();
	రాయి("mini_notify: bogus endpoint %d\n", dst_e);
	తిరిగి EDEADSRCDST;
  }

  dst_ptr = proc_addr(dst_p);

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  ఒకవేళ (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, 0, &m_notify_buff) &&
    !(dst_ptr->p_misc_flags & MF_జవాబు_PEND)) {
      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      ప్రతిజ్ఞా(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));

      Build_Notify_సందేశం(&dst_ptr->p_delivermsg, proc_nr(caller_ptr), dst_ptr);
      dst_ptr->p_delivermsg.m_ఆధారం = caller_ptr->p_endpoint;
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;

      IPC_స్థితి_కూడు_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING);

      తిరిగి(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the privilege id
   * instead of the process number is used in the pending bit map.
   */ 
  src_id = priv(caller_ptr)->s_id;
  సమితి_sys_బిట్టు(priv(dst_ptr)->s_notify_pending, src_id); 
  తిరిగి(OK);
}

#నిర్వచించు ASCOMPLAIN(caller, నమోదు_2, క్షేత్రం)	\
	రాయి("kernel:%s:%d: asyn failed for %s in %s "	\
	"(%d/%zu, tab 0x%lx)\n",__FILE__,__LINE__,	\
క్షేత్రం, caller->p_name, నమోదు_2, priv(caller)->s_asynsize, priv(caller)->s_asyntab)

#నిర్వచించు A_RETR(నమోదు_2) చేయి {			\
  ఒకవేళ (దత్తాంశం_copy(				\
  		caller_ptr->p_endpoint, పట్టిక_v + (నమోదు_2)*పరిమాణం(asynmsg_t),\
  		KERNEL, (vir_బైట్లు) &tabent,	\
  		పరిమాణం(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, నమోదు_2, "message entry");	\
  			r = EFAULT;		\
	                వెళ్ళు asyn_error; \
  }						\
  లేకపోతే ఒకవేళ(tabent.dst == SELF) { \
      tabent.dst = caller_ptr->p_endpoint; \
  } \
  			 } అయితే(0)

#నిర్వచించు A_INSRT(నమోదు_2) చేయి {			\
  ఒకవేళ (దత్తాంశం_copy(KERNEL, (vir_బైట్లు) &tabent,	\
  		caller_ptr->p_endpoint, పట్టిక_v + (నమోదు_2)*పరిమాణం(asynmsg_t),\
  		పరిమాణం(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, నమోదు_2, "message entry");	\
			/* Do NOT set r or goto asyn_error here! */ \
  }						\
  			  } అయితే(0)	

/*===========================================================================*
 *				try_deliver_senda			     *
 *===========================================================================*/
పూర్ణాంకం try_deliver_senda(నిర్మాణం proc *caller_ptr,
				asynmsg_t *పట్టిక,
				size_t కొలత)
{
  పూర్ణాంకం r, dst_p, పూర్తి, do_notify;
  గుర్తులేని పూర్ణాంకం i;
  గుర్తులేని flags;
  endpoint_t dst;
  నిర్మాణం proc *dst_ptr;
  నిర్మాణం priv *privp;
  asynmsg_t tabent;
  స్థిరరాశి vir_బైట్లు పట్టిక_v = (vir_బైట్లు) పట్టిక;
  సందేశం *m_ptr = ఖాళీ;

  privp = priv(caller_ptr);

  /* Clear table */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  privp->s_asynendpoint = caller_ptr->p_endpoint;

  ఒకవేళ (కొలత == 0) తిరిగి(OK);  /* Nothing to do, just return */

  /* Scan the table */
  do_notify = FALSE;
  పూర్తి = TRUE;

  /* Limit size to something reasonable. An arbitrary choice is 16
   * times the number of process table entries.
   *
   * (this check has been duplicated in sys_call but is left here
   * as a sanity check)
   */
  ఒకవేళ (కొలత > 16*(NR_TASKS + NR_PROCS)) {
    r = EDOM;
    తిరిగి r;
  }

  కొరకు (i = 0; i < కొలత; i++) {
	/* Process each entry in the table and store the result in the table.
	 * If we're done handling a message, copy the result to the sender. */

	dst = NONE;
	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ఒకవేళ (flags == 0) కొనసాగు; /* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ఒకవేళ(flags & ~(AMF_VALID|AMF_పూర్తి|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR)) {
		r = EINVAL;
		వెళ్ళు asyn_error;
	}
	ఒకవేళ (!(flags & AMF_VALID)) { /* Must contain message */
		r = EINVAL;
		వెళ్ళు asyn_error;
	}
	ఒకవేళ (flags & AMF_పూర్తి) కొనసాగు;	/* Already done processing */

	r = OK;
	ఒకవేళ (!isokendpt(tabent.dst, &dst_p)) 
		r = EDEADSRCDST; /* Bad destination, report the error */
	లేకపోతే ఒకవేళ (iskerneln(dst_p)) 
		r = ECALLDENIED; /* Asyn sends to the kernel are not allowed */
	లేకపోతే ఒకవేళ (!may_asynsend_గమ్యం(caller_ptr, dst_p))
		r = ECALLDENIED; /* Send denied by IPC mask */
	లేకపోతే 	/* r == OK */
		dst_ptr = proc_addr(dst_p);

	/* XXX: RTS_NO_ENDPOINT should be removed */
	ఒకవేళ (r == OK && RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT)) {
		r = EDEADSRCDST;
	}

	/* Check if 'dst' is blocked waiting for this message.
	 * If AMF_NOREPLY is set, do not satisfy the receiving part of
	 * a SENDREC.
	 */
	ఒకవేళ (r == OK && WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr,
	    (vir_బైట్లు)&పట్టిక[i].సందేశం_2, ఖాళీ) &&
	    (!(flags&AMF_NOREPLY) || !(dst_ptr->p_misc_flags&MF_జవాబు_PEND))) {
		/* Destination is indeed waiting for this message. */
		dst_ptr->p_delivermsg = tabent.సందేశం_2;
		dst_ptr->p_delivermsg.m_ఆధారం = caller_ptr->p_endpoint;
		dst_ptr->p_misc_flags |= MF_DELIVERMSG;
		IPC_స్థితి_కూడు_CALL(dst_ptr, SENDA);
		RTS_UNSET(dst_ptr, RTS_RECEIVING);
#ఒకవేళ DEBUG_IPC_HOOK
		hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#అయితే_ముగింపు
	} లేకపోతే ఒకవేళ (r == OK) {
		/* Inform receiver that something is pending */
		సమితి_sys_బిట్టు(priv(dst_ptr)->s_asyn_pending, 
			    priv(caller_ptr)->s_id); 
		పూర్తి = FALSE;
		కొనసాగు;
	} 

	/* Store results */
	tabent.ఫలితం = r;
	tabent.flags = flags | AMF_పూర్తి;
	ఒకవేళ (flags & AMF_NOTIFY)
		do_notify = TRUE;
	లేకపోతే ఒకవేళ (r != OK && (flags & AMF_NOTIFY_ERR))
		do_notify = TRUE;
	A_INSRT(i);	/* Copy results to caller; ignore errors */
	కొనసాగు;

asyn_error:
	ఒకవేళ (dst != NONE)
		రాయి("KERNEL senda error %d to %d\n", r, dst);
	లేకపోతే
		రాయి("KERNEL senda error %d\n", r);
  }

  ఒకవేళ (do_notify) 
	mini_notify(proc_addr(ASYNCM), caller_ptr->p_endpoint);

  ఒకవేళ (!పూర్తి) {
	privp->s_asyntab = (vir_బైట్లు) పట్టిక;
	privp->s_asynsize = కొలత;
  }

  తిరిగి(OK);
}

/*===========================================================================*
 *				mini_senda				     *
 *===========================================================================*/
స్థిర పూర్ణాంకం mini_senda(నిర్మాణం proc *caller_ptr, asynmsg_t *పట్టిక, size_t కొలత)
{
  నిర్మాణం priv *privp;

  privp = priv(caller_ptr);
  ఒకవేళ (!(privp->s_flags & SYS_PROC)) {
	రాయి( "mini_senda: warning caller has no privilege structure\n");
	తిరిగి(EPERM);
  }

  తిరిగి try_deliver_senda(caller_ptr, పట్టిక, కొలత);
}


/*===========================================================================*
 *				try_async				     * 
 *===========================================================================*/
స్థిర పూర్ణాంకం try_async(నిర్మాణం proc * caller_ptr)
{
  పూర్ణాంకం r;
  నిర్మాణం priv *privp;
  నిర్మాణం proc *src_ptr;
  sys_map_t *map;

  map = &priv(caller_ptr)->s_asyn_pending;

  /* Try all privilege structures */
  కొరకు (privp = BEG_PRIV_ADDR; privp < చివర_PRIV_ADDR; ++privp)  {
	ఒకవేళ (privp->s_proc_nr == NONE)
		కొనసాగు;

	ఒకవేళ (!get_sys_బిట్టు(*map, privp->s_id)) 
		కొనసాగు;

	src_ptr = proc_addr(privp->s_proc_nr);

#నిర్వచితమైతే CONFIG_SMP
	/*
	 * Do not copy from a process which does not have a stable address space
	 * due to VM fiddling with it
	 */
	ఒకవేళ (RTS_ISSET(src_ptr, RTS_VMINHIBIT)) {
		src_ptr->p_misc_flags |= MF_SENDA_VM_తప్పిపోవు;
		కొనసాగు;
	}
#అయితే_ముగింపు

	ప్రతిజ్ఞా(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	ఒకవేళ ((r = try_one(ANY, src_ptr, caller_ptr)) == OK)
		తిరిగి(r);
  }

  తిరిగి(ESRCH);
}


/*===========================================================================*
 *				try_one					     *
 *===========================================================================*/
స్థిర పూర్ణాంకం try_one(endpoint_t receive_e, నిర్మాణం proc *src_ptr,
    నిర్మాణం proc *dst_ptr)
{
/* Try to receive an asynchronous message from 'src_ptr' */
  పూర్ణాంకం r = EAGAIN, పూర్తి, do_notify;
  గుర్తులేని పూర్ణాంకం flags, i;
  size_t కొలత;
  endpoint_t dst, src_e;
  నిర్మాణం proc *caller_ptr;
  నిర్మాణం priv *privp;
  asynmsg_t tabent;
  vir_బైట్లు పట్టిక_v;

  privp = priv(src_ptr);
  ఒకవేళ (!(privp->s_flags & SYS_PROC)) తిరిగి(EPERM);
  కొలత = privp->s_asynsize;
  పట్టిక_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  unset_sys_బిట్టు(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  ఒకవేళ (కొలత == 0) తిరిగి(EAGAIN);
  ఒకవేళ (privp->s_asynendpoint != src_ptr->p_endpoint) తిరిగి EAGAIN;
  ఒకవేళ (!may_asynsend_గమ్యం(src_ptr, proc_nr(dst_ptr))) తిరిగి (ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */
  src_e = src_ptr->p_endpoint;

  /* Scan the table */
  do_notify = FALSE;
  పూర్తి = TRUE;

  కొరకు (i = 0; i < కొలత; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the meantime.
  	 */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ఒకవేళ (flags == 0) కొనసాగు;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ఒకవేళ(flags & ~(AMF_VALID|AMF_పూర్తి|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	లేకపోతే ఒకవేళ (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	లేకపోతే ఒకవేళ (flags & AMF_పూర్తి) కొనసాగు; /* Already done processing */

	/* Clear done flag. The sender is done sending when all messages in the
	 * table are marked done or empty. However, we will know that only
	 * the next time we enter this function or when the sender decides to
	 * send additional asynchronous messages and manages to deliver them
	 * all.
	 */
	పూర్తి = FALSE;

	ఒకవేళ (r == EINVAL)
		వెళ్ళు store_ఫలితం;

	/* Message must be directed at receiving end */
	ఒకవేళ (dst != dst_ptr->p_endpoint) కొనసాగు;

	ఒకవేళ (!CANRECEIVE(receive_e, src_e, dst_ptr,
		పట్టిక_v + i*పరిమాణం(asynmsg_t) + offsetof(నిర్మాణం asynmsg,సందేశం_2),
		ఖాళీ)) {
		కొనసాగు;
	}

	/* If AMF_NOREPLY is set, then this message is not a reply to a
	 * SENDREC and thus should not satisfy the receiving part of the
	 * SENDREC. This message is to be delivered later.
	 */
	ఒకవేళ ((flags & AMF_NOREPLY) && (dst_ptr->p_misc_flags & MF_జవాబు_PEND)) 
		కొనసాగు;

	/* Destination is ready to receive the message; deliver it */
	r = OK;
	dst_ptr->p_delivermsg = tabent.సందేశం_2;
	dst_ptr->p_delivermsg.m_ఆధారం = src_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;
#ఒకవేళ DEBUG_IPC_HOOK
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, src_ptr, dst_ptr);
#అయితే_ముగింపు

store_ఫలితం:
	/* Store results for sender. We may just have started delivering a
	 * message, so we must not return an error to the caller in the case
	 * that storing the results triggers an error!
	 */
	tabent.ఫలితం = r;
	tabent.flags = flags | AMF_పూర్తి;
	ఒకవేళ (flags & AMF_NOTIFY) do_notify = TRUE;
	లేకపోతే ఒకవేళ (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */

	ఆపు;
  }

  ఒకవేళ (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  ఒకవేళ (పూర్తి) {
	privp->s_asyntab = -1;
	privp->s_asynsize = 0;
  } లేకపోతే {
	సమితి_sys_బిట్టు(priv(dst_ptr)->s_asyn_pending, privp->s_id);
  }

asyn_error:
  తిరిగి(r);
}

/*===========================================================================*
 *				cancel_async				     *
 *===========================================================================*/
పూర్ణాంకం cancel_async(నిర్మాణం proc *src_ptr, నిర్మాణం proc *dst_ptr)
{
/* Cancel asynchronous messages from src to dst, because dst is not interested
 * in them (e.g., dst has been restarted) */
  పూర్ణాంకం పూర్తి, do_notify;
  గుర్తులేని పూర్ణాంకం flags, i;
  size_t కొలత;
  endpoint_t dst;
  నిర్మాణం proc *caller_ptr;
  నిర్మాణం priv *privp;
  asynmsg_t tabent;
  vir_బైట్లు పట్టిక_v;

  privp = priv(src_ptr);
  ఒకవేళ (!(privp->s_flags & SYS_PROC)) తిరిగి(EPERM);
  కొలత = privp->s_asynsize;
  పట్టిక_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  unset_sys_బిట్టు(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  ఒకవేళ (కొలత == 0) తిరిగి(EAGAIN);
  ఒకవేళ (!may_send_గమ్యం(src_ptr, proc_nr(dst_ptr))) తిరిగి(ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */

  /* Scan the table */
  do_notify = FALSE;
  పూర్తి = TRUE;


  కొరకు (i = 0; i < కొలత; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the mean time.
  	 */

  	పూర్ణాంకం r = EDEADSRCDST;	/* Cancel delivery due to dead dst */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ఒకవేళ (flags == 0) కొనసాగు;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ఒకవేళ(flags & ~(AMF_VALID|AMF_పూర్తి|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	లేకపోతే ఒకవేళ (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	లేకపోతే ఒకవేళ (flags & AMF_పూర్తి) కొనసాగు; /* Already done processing */

	/* Message must be directed at receiving end */
	ఒకవేళ (dst != dst_ptr->p_endpoint) {
		పూర్తి = FALSE;
		కొనసాగు;
	}

	/* Store results for sender */
	tabent.ఫలితం = r;
	tabent.flags = flags | AMF_పూర్తి;
	ఒకవేళ (flags & AMF_NOTIFY) do_notify = TRUE;
	లేకపోతే ఒకవేళ (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */
  }

  ఒకవేళ (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  ఒకవేళ (!పూర్తి) {
	privp->s_asyntab = పట్టిక_v;
	privp->s_asynsize = కొలత;
  }

asyn_error:
  తిరిగి(OK);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
శూన్య వరుసలో_చేర్చు(
  నమోదు నిర్మాణం proc *rp	/* this process is now runnable */
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
  పూర్ణాంకం q = rp->p_ప్రాధాన్యత;	 		/* scheduling queue to use */
  నిర్మాణం proc **rdy_తల, **rdy_తోక;
  
  ప్రతిజ్ఞా(proc_is_runnable(rp));

  ప్రతిజ్ఞా(q >= 0);

  rdy_తల = get_cpu_var(rp->p_cpu, నిరంతరం_q_తల);
  rdy_తోక = get_cpu_var(rp->p_cpu, నిరంతరం_q_తోక);

  /* Now add the process to the queue. */
  ఒకవేళ (!rdy_తల[q]) {		/* add to empty queue */
      rdy_తల[q] = rdy_తోక[q] = rp; 		/* create a new queue */
      rp->p_nextready = ఖాళీ;		/* mark new end */
  } 
  లేకపోతే {					/* add to tail of queue */
      rdy_తోక[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_తోక[q] = rp;				/* set new queue tail */
      rp->p_nextready = ఖాళీ;		/* mark new end */
  }

  ఒకవేళ (cpuid == rp->p_cpu) {
	  /*
	   * enqueueing a process with a higher priority than the current one,
	   * it gets preempted. The current process must be preemptible. Testing
	   * the priority also makes sure that a process does not preempt itself
	   */
	  నిర్మాణం proc * p;
	  p = get_cpulocal_var(proc_ptr);
	  ప్రతిజ్ఞా(p);
	  ఒకవేళ((p->p_ప్రాధాన్యత > rp->p_ప్రాధాన్యత) &&
			  (priv(p)->s_flags & PREEMPTIBLE))
		  RTS_సమితి(p, RTS_PREEMPTED); /* calls dequeue() */
  }
#నిర్వచితమైతే CONFIG_SMP
  /*
   * if the process was enqueued on a different cpu and the cpu is idle, i.e.
   * the time is off, we need to wake up that cpu and let it schedule this new
   * process
   */
  లేకపోతే ఒకవేళ (get_cpu_var(rp->p_cpu, cpu_is_idle)) {
	  smp_కాలపట్టిక(rp->p_cpu);
  }
#అయితే_ముగింపు

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr)->p_accounting.enter_వరుస));


#ఒకవేళ DEBUG_SANITYCHECKS
  ప్రతిజ్ఞా(runqueues_ok_local());
#అయితే_ముగింపు
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
స్థిర శూన్య వరుసలో_చేర్చు_తల(నిర్మాణం proc *rp)
{
  స్థిరరాశి పూర్ణాంకం q = rp->p_ప్రాధాన్యత;	 		/* scheduling queue to use */

  నిర్మాణం proc **rdy_తల, **rdy_తోక;

  ప్రతిజ్ఞా(proc_ptr_ok(rp));
  ప్రతిజ్ఞా(proc_is_runnable(rp));

  /*
   * the process was runnable without its quantum expired when dequeued. A
   * process with no time left should have been handled else and differently
   */
  ప్రతిజ్ఞా(rp->p_cpu_సమయం_ఎడమ);

  ప్రతిజ్ఞా(q >= 0);


  rdy_తల = get_cpu_var(rp->p_cpu, నిరంతరం_q_తల);
  rdy_తోక = get_cpu_var(rp->p_cpu, నిరంతరం_q_తోక);

  /* Now add the process to the queue. */
  ఒకవేళ (!rdy_తల[q]) {		/* add to empty queue */
	rdy_తల[q] = rdy_తోక[q] = rp; 	/* create a new queue */
	rp->p_nextready = ఖాళీ;			/* mark new end */
  } లేకపోతే {					/* add to head of queue */
	rp->p_nextready = rdy_తల[q];		/* chain head of queue */
	rdy_తల[q] = rp;			/* set new queue head */
  }

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr->p_accounting.enter_వరుస)));


  /* Process accounting for scheduling */
  rp->p_accounting.dequeues--;
  rp->p_accounting.preempted++;

#ఒకవేళ DEBUG_SANITYCHECKS
  ప్రతిజ్ఞా(runqueues_ok_local());
#అయితే_ముగింపు
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
శూన్య వరుస_నుండి_తీయి(నిర్మాణం proc *rp)
/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 *
 * This function can operate x-cpu as it always removes the process from the
 * queue of the cpu the process is currently assigned to.
 */
  పూర్ణాంకం q = rp->p_ప్రాధాన్యత;		/* queue to use */
  నిర్మాణం proc **xpp;			/* iterate over queue */
  నిర్మాణం proc *prev_xp;
  u64_t tsc, tsc_delta;

  నిర్మాణం proc **rdy_తోక;

  ప్రతిజ్ఞా(proc_ptr_ok(rp));
  ప్రతిజ్ఞా(!proc_is_runnable(rp));

  /* Side-effect for kernel: check if the task's stack still is ok? */
  ప్రతిజ్ఞా (!iskernelp(rp) || *priv(rp)->s_దొంతర_guard == దొంతర_GUARD);

  rdy_తోక = get_cpu_var(rp->p_cpu, నిరంతరం_q_తోక);

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  prev_xp = ఖాళీ;				
  కొరకు (xpp = get_cpu_var_ptr(rp->p_cpu, నిరంతరం_q_తల[q]); *xpp;
		  xpp = &(*xpp)->p_nextready) {
      ఒకవేళ (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          ఒకవేళ (rp == rdy_తోక[q]) {		/* queue tail removed */
              rdy_తోక[q] = prev_xp;		/* set new tail */
	  }

          ఆపు;
      }
      prev_xp = *xpp;				/* save previous in chain */
  }

	
  /* Process accounting for scheduling */
  rp->p_accounting.dequeues++;

  /* this is not all that accurate on virtual machines, especially with
     IO bound processes that only spend a short amount of time in the queue
     at a time. */
  ఒకవేళ (rp->p_accounting.enter_వరుస) {
	read_tsc_64(&tsc);
	tsc_delta = tsc - rp->p_accounting.enter_వరుస;
	rp->p_accounting.సమయం_లోపల_వరుస = rp->p_accounting.సమయం_లోపల_వరుస +
		tsc_delta;
	rp->p_accounting.enter_వరుస = 0;
  }

  /* For ps(1), remember when the process was last dequeued. */
  rp->p_dequeued = get_monotonic();

#ఒకవేళ DEBUG_SANITYCHECKS
  ప్రతిజ్ఞా(runqueues_ok_local());
#అయితే_ముగింపు
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
స్థిర నిర్మాణం proc * pick_proc(శూన్య)
{
/* Decide who to run now.  A new process is selected and returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
  నమోదు నిర్మాణం proc *rp;			/* process to run */
  నిర్మాణం proc **rdy_తల;
  పూర్ణాంకం q;				/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the task table.
   * If there are no processes ready to run, return NULL.
   */
  rdy_తల = get_cpulocal_var(నిరంతరం_q_తల);
  కొరకు (q=0; q < NR_SCHED_QUEUES; q++) {	
	ఒకవేళ(!(rp = rdy_తల[q])) {
		TRACE(VF_PICKPROC, రాయి("cpu %d queue %d empty\n", cpuid, q););
		కొనసాగు;
	}
	ప్రతిజ్ఞా(proc_is_runnable(rp));
	ఒకవేళ (priv(rp)->s_flags & BILLABLE)	 	
		get_cpulocal_var(bill_ptr) = rp; /* bill for system time */
	తిరిగి rp;
  }
  తిరిగి ఖాళీ;
}

/*===========================================================================*
 *				endpoint_lookup				     *
 *===========================================================================*/
నిర్మాణం proc *endpoint_lookup(endpoint_t e)
{
	పూర్ణాంకం n;

	ఒకవేళ(!isokendpt(e, &n)) తిరిగి ఖాళీ;

	తిరిగి proc_addr(n);
}

/*===========================================================================*
 *				isokendpt_f				     *
 *===========================================================================*/
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
పూర్ణాంకం isokendpt_f(స్థిరరాశి అక్షరం * file, పూర్ణాంకం పంక్తి, endpoint_t e, పూర్ణాంకం * p,
	స్థిరరాశి పూర్ణాంకం fatalflag)
#లేకపోతే
పూర్ణాంకం isokendpt_f(endpoint_t e, పూర్ణాంకం * p, స్థిరరాశి పూర్ణాంకం fatalflag)
#అయితే_ముగింపు
{
	పూర్ణాంకం ok = 0;
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
	ఒకవేళ(isokprocn(*p) && !isemptyn(*p) && proc_addr(*p)->p_endpoint == e)
		ok = 1;
	ఒకవేళ(!ok && fatalflag)
		panic("invalid endpoint: %d",  e);
	తిరిగి ok;
}

స్థిర శూన్య notify_scheduler(నిర్మాణం proc *p)
{
	సందేశం m_no_కాలాంశం;
	పూర్ణాంకం err;

	ప్రతిజ్ఞా(!proc_kernel_scheduler(p));

	/* dequeue the process */
	RTS_సమితి(p, RTS_NO_కాలాంశం);
	/*
	 * Notify the process's scheduler that it has run out of
	 * quantum. This is done by sending a message to the scheduler
	 * on the process's behalf
	 */
	m_no_కాలాంశం.m_ఆధారం = p->p_endpoint;
	m_no_కాలాంశం.m_type   = SCHEDULING_NO_కాలాంశం;
	m_no_కాలాంశం.m_krn_lsys_కాలపట్టిక.acnt_వరుస = cpu_సమయం_2_ms(p->p_accounting.సమయం_లోపల_వరుస);
	m_no_కాలాంశం.m_krn_lsys_కాలపట్టిక.acnt_deqs      = p->p_accounting.dequeues;
	m_no_కాలాంశం.m_krn_lsys_కాలపట్టిక.acnt_ipc_sync  = p->p_accounting.ipc_sync;
	m_no_కాలాంశం.m_krn_lsys_కాలపట్టిక.acnt_ipc_async = p->p_accounting.ipc_async;
	m_no_కాలాంశం.m_krn_lsys_కాలపట్టిక.acnt_preempt   = p->p_accounting.preempted;
	m_no_కాలాంశం.m_krn_lsys_కాలపట్టిక.acnt_cpu       = cpuid;
	m_no_కాలాంశం.m_krn_lsys_కాలపట్టిక.acnt_cpu_ఎక్కించు  = cpu_ఎక్కించు();

	/* Reset accounting */
	reset_proc_accounting(p);

	ఒకవేళ ((err = mini_send(p, p->p_scheduler->p_endpoint,
					&m_no_కాలాంశం, FROM_KERNEL))) {
		panic("WARNING: Scheduling: mini_send returned %d\n", err);
	}
}

శూన్య proc_no_సమయం(నిర్మాణం proc * p)
{
	ఒకవేళ (!proc_kernel_scheduler(p) && priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		notify_scheduler(p);
	}
	లేకపోతే {
		/*
		 * non-preemptible processes only need their quantum to
		 * be renewed. In fact, they by pass scheduling
		 */
		p->p_cpu_సమయం_ఎడమ = ms_2_cpu_సమయం(p->p_కాలాంశం_కొలత_ms);
#ఒకవేళ DEBUG_RACE
		RTS_సమితి(p, RTS_PREEMPTED);
		RTS_UNSET(p, RTS_PREEMPTED);
#అయితే_ముగింపు
	}
}

శూన్య reset_proc_accounting(నిర్మాణం proc *p)
{
  p->p_accounting.preempted = 0;
  p->p_accounting.ipc_sync  = 0;
  p->p_accounting.ipc_async = 0;
  p->p_accounting.dequeues  = 0;
  p->p_accounting.సమయం_లోపల_వరుస = 0;
  p->p_accounting.enter_వరుస = 0;
}
	
శూన్య copr_not_అందుబాటు_handler(శూన్య)
{
	నిర్మాణం proc * p;
	నిర్మాణం proc ** local_fpu_owner;
	/*
	 * Disable the FPU exception (both for the kernel and for the process
	 * once it's scheduled), and initialize or restore the FPU state.
	 */

	disable_fpu_exception();

	p = get_cpulocal_var(proc_ptr);

	/* if FPU is not owned by anyone, do not store anything */
	local_fpu_owner = get_cpulocal_var_ptr(fpu_owner);
	ఒకవేళ (*local_fpu_owner != ఖాళీ) {
		ప్రతిజ్ఞా(*local_fpu_owner != p);
		save_local_fpu(*local_fpu_owner, FALSE /*retain*/);
	}

	/*
	 * restore the current process' state and let it run again, do not
	 * schedule!
	 */
	ఒకవేళ (restore_fpu(p) != OK) {
		/* Restoring FPU state failed. This is always the process's own
		 * fault. Send a signal, and schedule another process instead.
		 */
		*local_fpu_owner = ఖాళీ;		/* release FPU */
		cause_sig(proc_nr(p), SIGFPE);
		తిరిగి;
	}

	*local_fpu_owner = p;
	context_stop(proc_addr(KERNEL));
	restore_user_context(p);
	NOT_REACHABLE;
}

శూన్య release_fpu(నిర్మాణం proc * p) {
	నిర్మాణం proc ** fpu_owner_ptr;

	fpu_owner_ptr = get_cpu_var_ptr(p->p_cpu, fpu_owner);

	ఒకవేళ (*fpu_owner_ptr == p)
		*fpu_owner_ptr = ఖాళీ;
}

శూన్య ser_dump_proc(శూన్య)
{
        నిర్మాణం proc *pp;

        కొరకు (pp= BEG_PROC_ADDR; pp < చివర_PROC_ADDR; pp++)
        {
                ఒకవేళ (isemptyp(pp))
                        కొనసాగు;
                print_proc_recursive(pp);
        }
}
