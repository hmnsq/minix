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

#شامل <stddef.h>
#شامل <signal.h>
#شامل <assert.h>
#شامل <string.h>

#شامل "vm.h"
#شامل "clock.h"
#شامل "spinlock.h"
#شامل "arch_proto.h"

#شامل <minix/syslib.h>

/* Scheduling and message passing functions */
ساکن خالی idle(خالی);
/**
 * Made public for use in clock.c (for user-space scheduling)
static int mini_send(struct proc *caller_ptr, endpoint_t dst_e, message
	*m_ptr, int flags);
*/
ساکن صحیح mini_receive(ساخت proc *caller_ptr, endpoint_t src,
	پیغام *m_buff_usr, صحیح flags);
ساکن صحیح mini_senda(ساخت proc *caller_ptr, asynmsg_t *جدول, size_t
	سائز);
ساکن صحیح deadlock(صحیح function, اندراج ساخت proc *caller,
	endpoint_t src_dst_e);
ساکن صحیح try_async(ساخت proc *caller_ptr);
ساکن صحیح try_one(endpoint_t receive_e, ساخت proc *src_ptr,
	ساخت proc *dst_ptr);
ساکن ساخت proc * pick_proc(خالی);
ساکن خالی قطار_میں_ڈالو_سر(ساخت proc *rp);

/* all idles share the same idle_priv structure */
ساکن ساخت priv idle_priv;

ساکن خالی سیٹ_idle_name(حرف * name, صحیح n)
{
        صحیح i, c;
        صحیح p_z = 0;

        اگر (n > 999) 
                n = 999; 

        name[0] = 'i'; 
        name[1] = 'd'; 
        name[2] = 'l'; 
        name[3] = 'e'; 

        برائے (i = 4, c = 100; c > 0; c /= 10) {
                صحیح digit;

                digit = n / c;  
                n -= digit * c;  

                اگر (p_z || digit != 0 || c == 1) {
                        p_z = 1;
                        name[i++] = '0' + digit;
                }   
        }    

        name[i] = '\0';

}


#تعریف_کرو PICK_ANY	1
#تعریف_کرو PICK_HIGHERONLY	2

#تعریف_کرو Build_Notify_پیغام(m_ptr, src, dst_ptr) \
	memset((m_ptr), 0, حجم(*(m_ptr)));				\
	(m_ptr)->m_type = NOTIFY_پیغام;				\
	(m_ptr)->m_notify.timestamp = get_monotonic();		\
	انتخاب (src) {							\
	حالت HARDWARE:							\
		(m_ptr)->m_notify.interrupts =			\
			priv(dst_ptr)->s_int_pending;			\
		priv(dst_ptr)->s_int_pending = 0;			\
		توڑو;							\
	حالت SYSTEM:							\
		حافظہ_نقل(&(m_ptr)->m_notify.sigset,			\
			&priv(dst_ptr)->s_sig_pending,			\
			حجم(sigset_t));				\
		sigemptyset(&priv(dst_ptr)->s_sig_pending);		\
		توڑو;							\
	}

ساکن پیغام m_notify_buff = { 0, NOTIFY_پیغام };

خالی proc_init(خالی)
{
	ساخت proc * rp;
	ساخت priv *sp;
	صحیح i;

	/* Clear the process table. Announce each slot as empty and set up
	 * mappings for proc_addr() and proc_nr() macros. Do the same for the
	 * table with privilege structures for the system processes. 
	 */
	برائے (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < انتہا_PROC_ADDR; ++rp, ++i) {
		rp->p_rts_flags = RTS_خانہ_FREE;/* initialize free slot */
		rp->p_magic = PMAGIC;
		rp->p_nr = i;			/* proc number from ptr */
		rp->p_endpoint = _ENDPOINT(0, rp->p_nr); /* generation no. 0 */
		rp->p_scheduler = تہی;		/* no user space scheduler */
		rp->p_ترجیح = 0;		/* no priority */
		rp->p_وقتی_حصہ_سائز_ms = 0;	/* no quantum size */

		/* arch-specific initialization */
		arch_proc_reset(rp);
	}
	برائے (sp = BEG_PRIV_ADDR, i = 0; sp < انتہا_PRIV_ADDR; ++sp, ++i) {
		sp->s_proc_nr = NONE;		/* initialize as free */
		sp->s_id = (sys_id_t) i;	/* priv structure index */
		ppriv_addr[i] = sp;		/* priv ptr from number */
		sp->s_sig_mgr = NONE;		/* clear signal managers */
		sp->s_bak_sig_mgr = NONE;
	}

	idle_priv.s_flags = IDL_F;
	/* initialize IDLE structures for every CPU */
	برائے (i = 0; i < CONFIG_MAX_CPUS; i++) {
		ساخت proc * ip = get_cpu_var_ptr(i, idle_proc);
		ip->p_endpoint = IDLE;
		ip->p_priv = &idle_priv;
		/* must not let idle ever get scheduled */
		ip->p_rts_flags |= RTS_PROC_STOP;
		سیٹ_idle_name(ip->p_name, i);
	}
}

ساکن خالی switch_پتہ_space_idle(خالی)
{
#اگر_تعریف_شدہ CONFIG_SMP
	/*
	 * currently we bet that VM is always alive and its pages available so
	 * when the CPU wakes up the kernel is mapped and no surprises happen.
	 * This is only a problem if more than 1 cpus are available
	 */
	switch_پتہ_space(proc_addr(VM_PROC_NR));
#اگر_ختم
}

/*===========================================================================*
 *				idle					     * 
 *===========================================================================*/
ساکن خالی idle(خالی)
{
	ساخت proc * p;

	/* This function is called whenever there is no work to do.
	 * Halt the CPU, and measure how many timestamp counter ticks are
	 * spent not doing anything. This allows test setups to measure
	 * the CPU utilization of certain workloads with high precision.
	 */

	p = get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	اگر (priv(p)->s_flags & BILLABLE)
		get_cpulocal_var(bill_ptr) = p;

	switch_پتہ_space_idle();

#اگر_تعریف_شدہ CONFIG_SMP
	get_cpulocal_var(cpu_is_idle) = 1;
	/* we don't need to keep time on APs as it is handled on the BSP */
	اگر (cpuid != bsp_cpu_id)
		stop_local_timer();
	ورنہ
#اگر_ختم
	{
		/*
		 * If the timer has expired while in kernel we must
		 * rearm it before we go to sleep
		 */
		restart_local_timer();
	}

	/* start accounting for the idle time */
	context_stop(proc_addr(KERNEL));
#اگر !SPROFILE
	halt_cpu();
#ورنہ
	اگر (!sprofiling)
		halt_cpu();
	ورنہ {
		متلون صحیح * v;

		v = get_cpulocal_var_ptr(idle_interrupted);
		interrupts_enable();
		جب (!*v)
			arch_pause();
		interrupts_disable();
		*v = 0;
	}
#اگر_ختم
	/*
	 * end of accounting for the idle task does not happen here, the kernel
	 * is handling stuff for quite a while before it gets back here!
	 */
}

/*===========================================================================*
 *                              vm_suspend                                *
 *===========================================================================*/
خالی vm_suspend(ساخت proc *caller, ثابت ساخت proc *ہدف,
        ثابت vir_بائٹیں linaddr, ثابت vir_بائٹیں len, ثابت صحیح type,
        ثابت صحیح writeflag)
{
        /* This range is not OK for this process. Set parameters
         * of the request and notify VM about the pending request.
         */
        أكد(!RTS_ISSET(caller, RTS_VMREQUEST));
        أكد(!RTS_ISSET(ہدف, RTS_VMREQUEST));

        RTS_سیٹ(caller, RTS_VMREQUEST);

        caller->p_vmrequest.req_type = VMPTYPE_CHECK;
        caller->p_vmrequest.ہدف = ہدف->p_endpoint;
        caller->p_vmrequest.params.check.آغاز = linaddr;
        caller->p_vmrequest.params.check.لمبائی = len;
        caller->p_vmrequest.params.check.writeflag = writeflag;
        caller->p_vmrequest.type = type;

        /* Connect caller on vmrequest wait queue. */
        اگر(!(caller->p_vmrequest.nextrequestor = vmrequest))
                اگر(OK != send_sig(VM_PROC_NR, SIGKMEM))
                        panic("send_sig failed");
        vmrequest = caller;
}

/*===========================================================================*
 *                              delivermsg                                *
 *===========================================================================*/
ساکن خالی delivermsg(ساخت proc *rp)
{
        أكد(!RTS_ISSET(rp, RTS_VMREQUEST));
        أكد(rp->p_misc_flags & MF_DELIVERMSG);
        أكد(rp->p_delivermsg.m_ماخذ != NONE);

        اگر (copy_پیغام_مقصد_user(&rp->p_delivermsg,
                                (پیغام *) rp->p_delivermsg_vir)) {
                اگر(rp->p_misc_flags & MF_MSGFAILED) {
                        /* 2nd consecutive failure means this won't succeed */
                        لکھو("WARNING wrong user pointer 0x%08lx from "
                                "process %s / %d\n",
                                rp->p_delivermsg_vir,
                                rp->p_name,
                                rp->p_endpoint);
                        cause_sig(rp->p_nr, SIGSEGV);
                } ورنہ {
                        /* 1st failure means we have to ask VM to handle it */
                        vm_suspend(rp, rp, rp->p_delivermsg_vir,
                                حجم(پیغام), VMSTYPE_DELIVERMSG, 1);
                        rp->p_misc_flags |= MF_MSGFAILED;
                }
        } ورنہ {
                /* Indicate message has been delivered; address is 'used'. */
                rp->p_delivermsg.m_ماخذ = NONE;
                rp->p_misc_flags &= ~(MF_DELIVERMSG|MF_MSGFAILED);

                اگر(!(rp->p_misc_flags & MF_CONTEXT_سیٹ)) {
                        rp->p_reg.retreg = OK;
                }
        }
}

/*===========================================================================*
 *				switch_to_user				     * 
 *===========================================================================*/
خالی switch_مقصد_user(خالی)
{
	/* This function is called an instant before proc_ptr is
	 * to be scheduled again.
	 */
	ساخت proc * p;
#اگر_تعریف_شدہ CONFIG_SMP
	صحیح tlb_must_refresh = 0;
#اگر_ختم

	p = get_cpulocal_var(proc_ptr);
	/*
	 * if the current process is still runnable check the misc flags and let
	 * it run unless it becomes not runnable in the meantime
	 */
	اگر (proc_is_runnable(p))
		جاؤ check_misc_flags;
	/*
	 * if a process becomes not runnable while handling the misc flags, we
	 * need to pick a new one here and start from scratch. Also if the
	 * current process wasn't runnable, we pick a new one here
	 */
not_runnable_pick_new:
	اگر (proc_is_preempted(p)) {
		p->p_rts_flags &= ~RTS_PREEMPTED;
		اگر (proc_is_runnable(p)) {
			اگر (p->p_cpu_وقت_بایاں)
				قطار_میں_ڈالو_سر(p);
			ورنہ
				قطار_میں_ڈالو(p);
		}
	}

	/*
	 * if we have no process to run, set IDLE as the current process for
	 * time accounting and put the cpu in an idle state. After the next
	 * timer interrupt the execution resumes here and we can pick another
	 * process. If there is still nothing runnable we "schedule" IDLE again
	 */
	جب (!(p = pick_proc())) {
		idle();
	}

	/* update the global variable */
	get_cpulocal_var(proc_ptr) = p;

#اگر_تعریف_شدہ CONFIG_SMP
	اگر (p->p_misc_flags & MF_FLUSH_TLB && get_cpulocal_var(ptproc) == p)
		tlb_must_refresh = 1;
#اگر_ختم
	switch_پتہ_space(p);

check_misc_flags:

	أكد(p);
	أكد(proc_is_runnable(p));
	جب (p->p_misc_flags &
		(MF_KCALL_RESUME | MF_DELIVERMSG |
		 MF_SC_DEFER | MF_SC_TRACE | MF_SC_فعال)) {

		أكد(proc_is_runnable(p));
		اگر (p->p_misc_flags & MF_KCALL_RESUME) {
			kernel_call_resume(p);
		}
		ورنہ اگر (p->p_misc_flags & MF_DELIVERMSG) {
			TRACE(VF_SCHEDULING, لکھو("delivering to %s / %d\n",
				p->p_name, p->p_endpoint););
			delivermsg(p);
		}
		ورنہ اگر (p->p_misc_flags & MF_SC_DEFER) {
			/* Perform the system call that we deferred earlier. */

			أكد (!(p->p_misc_flags & MF_SC_فعال));

			arch_do_syscall(p);

			/* If the process is stopped for signal delivery, and
			 * not blocked sending a message after the system call,
			 * inform PM.
			 */
			اگر ((p->p_misc_flags & MF_SIG_DELAY) &&
					!RTS_ISSET(p, RTS_SENDING))
				sig_delay_مکمل(p);
		}
		ورنہ اگر (p->p_misc_flags & MF_SC_TRACE) {
			/* Trigger a system call leave event if this was a
			 * system call. We must do this after processing the
			 * other flags above, both for tracing correctness and
			 * to be able to use 'break'.
			 */
			اگر (!(p->p_misc_flags & MF_SC_فعال))
				توڑو;

			p->p_misc_flags &=
				~(MF_SC_TRACE | MF_SC_فعال);

			/* Signal the "leave system call" event.
			 * Block the process.
			 */
			cause_sig(proc_nr(p), SIGTRAP);
		}
		ورنہ اگر (p->p_misc_flags & MF_SC_فعال) {
			/* If MF_SC_ACTIVE was set, remove it now:
			 * we're leaving the system call.
			 */
			p->p_misc_flags &= ~MF_SC_فعال;

			توڑو;
		}

		/*
		 * the selected process might not be runnable anymore. We have
		 * to checkit and schedule another one
		 */
		اگر (!proc_is_runnable(p))
			جاؤ not_runnable_pick_new;
	}
	/*
	 * check the quantum left before it runs again. We must do it only here
	 * as we are sure that a possible out-of-quantum message to the
	 * scheduler will not collide with the regular ipc
	 */
	اگر (!p->p_cpu_وقت_بایاں)
		proc_no_وقت(p);
	/*
	 * After handling the misc flags the selected process might not be
	 * runnable anymore. We have to checkit and schedule another one
	 */
	اگر (!proc_is_runnable(p))
		جاؤ not_runnable_pick_new;

	TRACE(VF_SCHEDULING, لکھو("cpu %d starting %s / %d "
				"pc 0x%08x\n",
		cpuid, p->p_name, p->p_endpoint, p->p_reg.pc););
#اگر DEBUG_TRACE
	p->p_schedules++;
#اگر_ختم

	p = arch_اختتام_switch_مقصد_user();
	أكد(p->p_cpu_وقت_بایاں);

	context_stop(proc_addr(KERNEL));

	/* If the process isn't the owner of FPU, enable the FPU exception */
	اگر (get_cpulocal_var(fpu_owner) != p)
		enable_fpu_exception();
	ورنہ
		disable_fpu_exception();

	/* If MF_CONTEXT_SET is set, don't clobber process state within
	 * the kernel. The next kernel entry is OK again though.
	 */
	p->p_misc_flags &= ~MF_CONTEXT_سیٹ;

#اگر defined(__i386__)
  	أكد(p->p_seg.p_cr3 != 0);
#وگرنہ_اگر defined(__arm__)
	أكد(p->p_seg.p_ttbr != 0);
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_SMP
	اگر (p->p_misc_flags & MF_FLUSH_TLB) {
		اگر (tlb_must_refresh)
			refresh_tlb();
		p->p_misc_flags &= ~MF_FLUSH_TLB;
	}
#اگر_ختم
	
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
ساکن صحیح do_sync_ipc(ساخت proc * caller_ptr, /* who made the call */
			صحیح call_nr,	/* system call number and flags */
			endpoint_t src_dst_e,	/* src or dst of the call */
			پیغام *m_ptr)	/* users pointer to a message */
{
  صحیح نتیجہ;					/* the system call's result */
  صحیح src_dst_p;				/* Process slot number */
  حرف *callname;

  /* Check destination. RECEIVE is the only call that accepts ANY (in addition
   * to a real endpoint). The other calls (SEND, SENDREC, and NOTIFY) require an
   * endpoint to corresponds to a process. In addition, it is necessary to check
   * whether a process is allowed to send to a given destination.
   */
  أكد(call_nr != SENDA);

  /* Only allow non-negative call_nr values less than 32 */
  اگر (call_nr < 0 || call_nr > IPCNO_HIGHEST || call_nr >= 32
      || !(callname = ipc_call_نام[call_nr])) {
#اگر DEBUG_ENABLE_IPC_WARNINGS
      لکھو("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          call_nr, proc_nr(caller_ptr), src_dst_e);
#اگر_ختم
	واپس(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  اگر (src_dst_e == ANY)
  {
	اگر (call_nr != RECEIVE)
	{
#اگر 0
		لکھو("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#اگر_ختم
		واپس EINVAL;
	}
	src_dst_p = (صحیح) src_dst_e;
  }
  ورنہ
  {
	/* Require a valid source and/or destination process. */
	اگر(!isokendpt(src_dst_e, &src_dst_p)) {
#اگر 0
		لکھو("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#اگر_ختم
		واپس EDEADSRCDST;
	}

	/* If the call is to send to a process, i.e., for SEND, SENDNB,
	 * SENDREC or NOTIFY, verify that the caller is allowed to send to
	 * the given destination. 
	 */
	اگر (call_nr != RECEIVE)
	{
		اگر (!may_send_مقصد(caller_ptr, src_dst_p)) {
#اگر DEBUG_ENABLE_IPC_WARNINGS
			لکھو(
			"sys_call: ipc mask denied %s from %d to %d\n",
				callname,
				caller_ptr->p_endpoint, src_dst_e);
#اگر_ختم
			واپس(ECALLDENIED);	/* call denied by ipc mask */
		}
	}
  }

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  اگر (!(priv(caller_ptr)->s_trap_نقاب & (1 << call_nr))) {
#اگر DEBUG_ENABLE_IPC_WARNINGS
      لکھو("sys_call: %s not allowed, caller %d, src_dst %d\n", 
          callname, proc_nr(caller_ptr), src_dst_p);
#اگر_ختم
	واپس(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  اگر (call_nr != SENDREC && call_nr != RECEIVE && iskerneln(src_dst_p)) {
#اگر DEBUG_ENABLE_IPC_WARNINGS
      لکھو("sys_call: trap %s not allowed, caller %d, src_dst %d\n",
           callname, proc_nr(caller_ptr), src_dst_e);
#اگر_ختم
	واپس(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  انتخاب(call_nr) {
  حالت SENDREC:
	/* A flag is set so that notifications cannot interrupt SENDREC. */
	caller_ptr->p_misc_flags |= MF_جواب_PEND;
	/* fall through */
  حالت SEND:			
	نتیجہ = mini_send(caller_ptr, src_dst_e, m_ptr, 0);
	اگر (call_nr == SEND || نتیجہ != OK)
		توڑو;				/* done, or SEND failed */
	/* fall through for SENDREC */
  حالت RECEIVE:			
	اگر (call_nr == RECEIVE) {
		caller_ptr->p_misc_flags &= ~MF_جواب_PEND;
		IPC_حالت_CLEAR(caller_ptr);  /* clear IPC status code */
	}
	نتیجہ = mini_receive(caller_ptr, src_dst_e, m_ptr, 0);
	توڑو;
  حالت NOTIFY:
	نتیجہ = mini_notify(caller_ptr, src_dst_e);
	توڑو;
  حالت SENDNB:
        نتیجہ = mini_send(caller_ptr, src_dst_e, m_ptr, NON_BLOCKING);
        توڑو;
  طےشدہ:
	نتیجہ = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  واپس(نتیجہ);
}

صحیح do_ipc(reg_t r1, reg_t r2, reg_t r3)
{
  ساخت proc *ثابت caller_ptr = get_cpulocal_var(proc_ptr);	/* get pointer to caller */
  صحیح call_nr = (صحیح) r1;

  أكد(!RTS_ISSET(caller_ptr, RTS_خانہ_FREE));

  /* bill kernel time to this process. */
  kbill_ipc = caller_ptr;

  /* If this process is subject to system call tracing, handle that first. */
  اگر (caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
	/* Are we tracing this process, and is it the first sys_call entry? */
	اگر ((caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) ==
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
		واپس caller_ptr->p_reg.retreg;
	}

	/* If the MF_SC_DEFER flag is set, the syscall is now being resumed. */
	caller_ptr->p_misc_flags &= ~MF_SC_DEFER;

	أكد (!(caller_ptr->p_misc_flags & MF_SC_فعال));

	/* Set a flag to allow reliable tracing of leaving the system call. */
	caller_ptr->p_misc_flags |= MF_SC_فعال;
  }

  اگر(caller_ptr->p_misc_flags & MF_DELIVERMSG) {
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
  انتخاب(call_nr) {
  	حالت SENDREC:
  	حالت SEND:			
  	حالت RECEIVE:			
  	حالت NOTIFY:
  	حالت SENDNB:
  	{
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_sync++;

  	    واپس do_sync_ipc(caller_ptr, call_nr, (endpoint_t) r2,
			    (پیغام *) r3);
  	}
  	حالت SENDA:
  	{
 	    /*
  	     * Get and check the size of the argument in bytes as it is a
  	     * table
  	     */
  	    size_t پیغام_سائز = (size_t) r2;
  
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_async++;
 
  	    /* Limit size to something reasonable. An arbitrary choice is 16
  	     * times the number of process table entries.
  	     */
  	    اگر (پیغام_سائز > 16*(NR_TASKS + NR_PROCS))
	        واپس EDOM;
  	    واپس mini_senda(caller_ptr, (asynmsg_t *) r3, پیغام_سائز);
  	}
  	حالت MINIX_KERNINFO:
	{
		/* It might not be initialized yet. */
	  	اگر(!minix_kerninfo_user) {
			واپس EBADCALL;
		}

  		arch_سیٹ_secondary_ipc_return(caller_ptr, minix_kerninfo_user);
  		واپس OK;
	}
  	طےشدہ:
	واپس EBADCALL;		/* illegal system call */
  }
}

/*===========================================================================*
 *				deadlock				     * 
 *===========================================================================*/
ساکن صحیح deadlock(
  صحیح function,				/* trap number */
  اندراج ساخت proc *cp,		/* pointer to caller */
  endpoint_t src_dst_e			/* src or dst process */
)
{
/* Check for deadlock. This can happen if 'caller_ptr' and 'src_dst' have
 * a cyclic dependency of blocking send and receive calls. The only cyclic 
 * dependency that is not fatal is if the caller and target directly SEND(REC)
 * and RECEIVE to each other. If a deadlock is found, the group size is 
 * returned. Otherwise zero is returned. 
 */
  اندراج ساخت proc *xp;			/* process pointer */
  صحیح گروہ_سائز = 1;				/* start with only caller */
#اگر DEBUG_ENABLE_IPC_WARNINGS
  ساکن ساخت proc *processes[NR_PROCS + NR_TASKS];
  processes[0] = cp;
#اگر_ختم

  جب (src_dst_e != ANY) { 			/* check while process nr */
      صحیح src_dst_خانہ;
      okendpt(src_dst_e, &src_dst_خانہ);
      xp = proc_addr(src_dst_خانہ);		/* follow chain of processes */
      أكد(proc_ptr_ok(xp));
      أكد(!RTS_ISSET(xp, RTS_خانہ_FREE));
#اگر DEBUG_ENABLE_IPC_WARNINGS
      processes[گروہ_سائز] = xp;
#اگر_ختم
      گروہ_سائز ++;				/* extra process in group */

      /* Check whether the last process in the chain has a dependency. If it 
       * has not, the cycle cannot be closed and we are done.
       */
      اگر((src_dst_e = P_BLOCKEDON(xp)) == NONE)
	واپس 0;

      /* Now check if there is a cyclic dependency. For group sizes of two,  
       * a combination of SEND(REC) and RECEIVE is not fatal. Larger groups
       * or other combinations indicate a deadlock.  
       */
      اگر (src_dst_e == cp->p_endpoint) {	/* possible deadlock */
	  اگر (گروہ_سائز == 2) {		/* caller and src_dst */
	      /* The function number is magically converted to flags. */
	      اگر ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING) { 
	          واپس(0);			/* not a deadlock */
	      }
	  }
#اگر DEBUG_ENABLE_IPC_WARNINGS
	  {
		صحیح i;
		لکھو("deadlock between these processes:\n");
		برائے(i = 0; i < گروہ_سائز; i++) {
			لکھو(" %10s ", processes[i]->p_name);
		}
		لکھو("\n\n");
		برائے(i = 0; i < گروہ_سائز; i++) {
			print_proc(processes[i]);
			proc_stacktrace(processes[i]);
		}
	  }
#اگر_ختم
          واپس(گروہ_سائز);			/* deadlock found */
      }
  }
  واپس(0);					/* not a deadlock */
}

/*===========================================================================*
 *				has_pending				     * 
 *===========================================================================*/
ساکن صحیح has_pending(sys_map_t *map, صحیح src_p, صحیح asynm)
{
/* Check to see if there is a pending message from the desired source
 * available.
 */

  صحیح src_id;
  sys_id_t id = NULL_PRIV_ID;
#اگر_تعریف_شدہ CONFIG_SMP
  ساخت proc * p;
#اگر_ختم

  /* Either check a specific bit in the mask map, or find the first bit set in
   * it (if any), depending on whether the receive was called on a specific
   * source endpoint.
   */
  اگر (src_p != ANY) {
	src_id = nr_مقصد_id(src_p);
	اگر (get_sys_ثنائی_ہندسہ(*map, src_id)) {
#اگر_تعریف_شدہ CONFIG_SMP
		p = proc_addr(id_مقصد_nr(src_id));
		اگر (asynm && RTS_ISSET(p, RTS_VMINHIBIT))
			p->p_misc_flags |= MF_SENDA_VM_ناکامی;
		ورنہ
#اگر_ختم
			id = src_id;
	}
  } ورنہ {
	/* Find a source with a pending message */
	برائے (src_id = 0; src_id < NR_SYS_PROCS; src_id += BITCHUNK_بٹ) {
		اگر (get_sys_بٹ(*map, src_id) != 0) {
#اگر_تعریف_شدہ CONFIG_SMP
			جب (src_id < NR_SYS_PROCS) {
				جب (!get_sys_ثنائی_ہندسہ(*map, src_id)) {
					اگر (src_id == NR_SYS_PROCS)
						جاؤ quit_تلاش;
					src_id++;
				}
				p = proc_addr(id_مقصد_nr(src_id));
				/*
				 * We must not let kernel fiddle with pages of a
				 * process which are currently being changed by
				 * VM.  It is dangerous! So do not report such a
				 * process as having pending async messages.
				 * Skip it.
				 */
				اگر (asynm && RTS_ISSET(p, RTS_VMINHIBIT)) {
					p->p_misc_flags |= MF_SENDA_VM_ناکامی;
					src_id++;
				} ورنہ
					جاؤ quit_تلاش;
			}
#ورنہ
			جب (!get_sys_ثنائی_ہندسہ(*map, src_id)) src_id++;
			جاؤ quit_تلاش;
#اگر_ختم
		}
	}

quit_تلاش:
	اگر (src_id < NR_SYS_PROCS)	/* Found one */
		id = src_id;
  }

  واپس(id);
}

/*===========================================================================*
 *				has_pending_notify			     *
 *===========================================================================*/
صحیح has_pending_notify(ساخت proc * caller, صحیح src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	واپس has_pending(map, src_p, 0);
}

/*===========================================================================*
 *				has_pending_asend			     *
 *===========================================================================*/
صحیح has_pending_asend(ساخت proc * caller, صحیح src_p)
{
	sys_map_t * map = &priv(caller)->s_asyn_pending;
	واپس has_pending(map, src_p, 1);
}

/*===========================================================================*
 *				unset_notify_pending			     *
 *===========================================================================*/
خالی unset_notify_pending(ساخت proc * caller, صحیح src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	unset_sys_ثنائی_ہندسہ(*map, src_p);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
صحیح mini_send(
  اندراج ساخت proc *caller_ptr,	/* who is trying to send a message? */
  endpoint_t dst_e,			/* to whom is message being sent? */
  پیغام *m_ptr,			/* pointer to message buffer */
  ثابت صحیح flags
)
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  اندراج ساخت proc *dst_ptr;
  اندراج ساخت proc **xpp;
  صحیح dst_p;
  dst_p = _ENDPOINT_P(dst_e);
  dst_ptr = proc_addr(dst_p);

  اگر (RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT))
  {
	واپس EDEADSRCDST;
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * RTS_SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  اگر (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, (vir_بائٹیں)m_ptr, تہی)) {
	صحیح call;
	/* Destination is indeed waiting for this message. */
	أكد(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));	

	اگر (!(flags & FROM_KERNEL)) {
		اگر(copy_پیغام_from_user(m_ptr, &dst_ptr->p_delivermsg))
			واپس EFAULT;
	} ورنہ {
		dst_ptr->p_delivermsg = *m_ptr;
		IPC_حالت_جوڑو_FLAGS(dst_ptr, IPC_FLG_پیغام_FROM_KERNEL);
	}

	dst_ptr->p_delivermsg.m_ماخذ = caller_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;

	call = (caller_ptr->p_misc_flags & MF_جواب_PEND ? SENDREC
		: (flags & NON_BLOCKING ? SENDNB : SEND));
	IPC_حالت_جوڑو_CALL(dst_ptr, call);

	اگر (dst_ptr->p_misc_flags & MF_جواب_PEND)
		dst_ptr->p_misc_flags &= ~MF_جواب_PEND;

	RTS_UNSET(dst_ptr, RTS_RECEIVING);

#اگر DEBUG_IPC_HOOK
	hook_ipc_msgsend(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#اگر_ختم
  } ورنہ {
	اگر(flags & NON_BLOCKING) {
		واپس(ENOTREADY);
	}

	/* Check for a possible deadlock before actually blocking. */
	اگر (deadlock(SEND, caller_ptr, dst_e)) {
		واپس(ELOCKED);
	}

	/* Destination is not waiting.  Block and dequeue caller. */
	اگر (!(flags & FROM_KERNEL)) {
		اگر(copy_پیغام_from_user(m_ptr, &caller_ptr->p_sendmsg))
			واپس EFAULT;
	} ورنہ {
		caller_ptr->p_sendmsg = *m_ptr;
		/*
		 * we need to remember that this message is from kernel so we
		 * can set the delivery status flags when the message is
		 * actually delivered
		 */
		caller_ptr->p_misc_flags |= MF_SENDING_FROM_KERNEL;
	}

	RTS_سیٹ(caller_ptr, RTS_SENDING);
	caller_ptr->p_sendto_e = dst_e;

	/* Process is now blocked.  Put in on the destination's queue. */
	أكد(caller_ptr->p_q_link == تہی);
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	جب (*xpp) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */

#اگر DEBUG_IPC_HOOK
	hook_ipc_msgsend(&caller_ptr->p_sendmsg, caller_ptr, dst_ptr);
#اگر_ختم
  }
  واپس(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
ساکن صحیح mini_receive(ساخت proc * caller_ptr,
			endpoint_t src_e, /* which message source is wanted */
			پیغام * m_buff_usr, /* pointer to message buffer */
			ثابت صحیح flags)
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller.
 */
  اندراج ساخت proc **xpp;
  صحیح r, src_id, ملا, src_proc_nr, src_p;
  endpoint_t sender_e;

  أكد(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));

  /* This is where we want our message. */
  caller_ptr->p_delivermsg_vir = (vir_بائٹیں) m_buff_usr;

  اگر(src_e == ANY) src_p = ANY;
  ورنہ
  {
	okendpt(src_e, &src_p);
	اگر (RTS_ISSET(proc_addr(src_p), RTS_NO_ENDPOINT))
	{
		واپس EDEADSRCDST;
	}
  }


  /* Check to see if a message from desired source is already available.  The
   * caller's RTS_SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  اگر (!RTS_ISSET(caller_ptr, RTS_SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    اگر (! (caller_ptr->p_misc_flags & MF_جواب_PEND)) {

	/* Check for pending notifications */
        src_id = has_pending_notify(caller_ptr, src_p);
        ملا = src_id != NULL_PRIV_ID;
        اگر(ملا) {
            src_proc_nr = id_مقصد_nr(src_id);		/* get source proc */
            sender_e = proc_addr(src_proc_nr)->p_endpoint;
        }

        اگر (ملا && CANRECEIVE(src_e, sender_e, caller_ptr, 0,
          &m_notify_buff)) {

#اگر DEBUG_ENABLE_IPC_WARNINGS
	    اگر(src_proc_nr == NONE) {
		لکھو("mini_receive: sending notify from NONE\n");
	    }
#اگر_ختم
	    أكد(src_proc_nr != NONE);
            unset_notify_pending(caller_ptr, src_id);	/* no longer pending */

            /* Found a suitable source, deliver the notification message. */
	    أكد(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));	
	    أكد(src_e == ANY || sender_e == src_e);

	    /* assemble message */
	    Build_Notify_پیغام(&caller_ptr->p_delivermsg, src_proc_nr, caller_ptr);
	    caller_ptr->p_delivermsg.m_ماخذ = sender_e;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;

	    IPC_حالت_جوڑو_CALL(caller_ptr, NOTIFY);

	    جاؤ receive_مکمل;
        }
    }

    /* Check for pending asynchronous messages */
    اگر (has_pending_asend(caller_ptr, src_p) != NULL_PRIV_ID) {
        اگر (src_p != ANY)
		r = try_one(src_e, proc_addr(src_p), caller_ptr);
        ورنہ
        	r = try_async(caller_ptr);

	اگر (r == OK) {
            IPC_حالت_جوڑو_CALL(caller_ptr, SENDA);
            جاؤ receive_مکمل;
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    جب (*xpp) {
	ساخت proc * sender = *xpp;
	endpoint_t sender_e = sender->p_endpoint;

        اگر (CANRECEIVE(src_e, sender_e, caller_ptr, 0, &sender->p_sendmsg)) {
            صحیح call;
	    أكد(!RTS_ISSET(sender, RTS_خانہ_FREE));
	    أكد(!RTS_ISSET(sender, RTS_NO_ENDPOINT));

	    /* Found acceptable message. Copy it and update status. */
  	    أكد(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	    caller_ptr->p_delivermsg = sender->p_sendmsg;
	    caller_ptr->p_delivermsg.m_ماخذ = sender->p_endpoint;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;
	    RTS_UNSET(sender, RTS_SENDING);

	    call = (sender->p_misc_flags & MF_جواب_PEND ? SENDREC : SEND);
	    IPC_حالت_جوڑو_CALL(caller_ptr, call);

	    /*
	     * if the message is originally from the kernel on behalf of this
	     * process, we must send the status flags accordingly
	     */
	    اگر (sender->p_misc_flags & MF_SENDING_FROM_KERNEL) {
		IPC_حالت_جوڑو_FLAGS(caller_ptr, IPC_FLG_پیغام_FROM_KERNEL);
		/* we can clean the flag now, not need anymore */
		sender->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
	    }
	    اگر (sender->p_misc_flags & MF_SIG_DELAY)
		sig_delay_مکمل(sender);

#اگر DEBUG_IPC_HOOK
            hook_ipc_msgrecv(&caller_ptr->p_delivermsg, *xpp, caller_ptr);
#اگر_ختم
		
            *xpp = sender->p_q_link;		/* remove from queue */
	    sender->p_q_link = تہی;
	    جاؤ receive_مکمل;
	}
	xpp = &sender->p_q_link;		/* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC. 
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  اگر ( ! (flags & NON_BLOCKING)) {
      /* Check for a possible deadlock before actually blocking. */
      اگر (deadlock(RECEIVE, caller_ptr, src_e)) {
          واپس(ELOCKED);
      }

      caller_ptr->p_getfrom_e = src_e;		
      RTS_سیٹ(caller_ptr, RTS_RECEIVING);
      واپس(OK);
  } ورنہ {
	واپس(ENOTREADY);
  }

receive_مکمل:
  اگر (caller_ptr->p_misc_flags & MF_جواب_PEND)
	  caller_ptr->p_misc_flags &= ~MF_جواب_PEND;
  واپس OK;
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
صحیح mini_notify(
  ثابت ساخت proc *caller_ptr,	/* sender of the notification */
  endpoint_t dst_e			/* which process to notify */
)
{
  اندراج ساخت proc *dst_ptr;
  صحیح src_id;				/* source id for late delivery */
  صحیح dst_p;

  اگر (!isokendpt(dst_e, &dst_p)) {
	util_stacktrace();
	لکھو("mini_notify: bogus endpoint %d\n", dst_e);
	واپس EDEADSRCDST;
  }

  dst_ptr = proc_addr(dst_p);

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  اگر (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, 0, &m_notify_buff) &&
    !(dst_ptr->p_misc_flags & MF_جواب_PEND)) {
      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      أكد(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));

      Build_Notify_پیغام(&dst_ptr->p_delivermsg, proc_nr(caller_ptr), dst_ptr);
      dst_ptr->p_delivermsg.m_ماخذ = caller_ptr->p_endpoint;
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;

      IPC_حالت_جوڑو_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING);

      واپس(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the privilege id
   * instead of the process number is used in the pending bit map.
   */ 
  src_id = priv(caller_ptr)->s_id;
  سیٹ_sys_ثنائی_ہندسہ(priv(dst_ptr)->s_notify_pending, src_id); 
  واپس(OK);
}

#تعریف_کرو ASCOMPLAIN(caller, اندراج_2, میدان)	\
	لکھو("kernel:%s:%d: asyn failed for %s in %s "	\
	"(%d/%zu, tab 0x%lx)\n",__FILE__,__LINE__,	\
میدان, caller->p_name, اندراج_2, priv(caller)->s_asynsize, priv(caller)->s_asyntab)

#تعریف_کرو A_RETR(اندراج_2) کرو {			\
  اگر (ڈیٹا_copy(				\
  		caller_ptr->p_endpoint, جدول_v + (اندراج_2)*حجم(asynmsg_t),\
  		KERNEL, (vir_بائٹیں) &tabent,	\
  		حجم(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, اندراج_2, "message entry");	\
  			r = EFAULT;		\
	                جاؤ asyn_error; \
  }						\
  ورنہ اگر(tabent.dst == SELF) { \
      tabent.dst = caller_ptr->p_endpoint; \
  } \
  			 } جب(0)

#تعریف_کرو A_INSRT(اندراج_2) کرو {			\
  اگر (ڈیٹا_copy(KERNEL, (vir_بائٹیں) &tabent,	\
  		caller_ptr->p_endpoint, جدول_v + (اندراج_2)*حجم(asynmsg_t),\
  		حجم(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, اندراج_2, "message entry");	\
			/* Do NOT set r or goto asyn_error here! */ \
  }						\
  			  } جب(0)	

/*===========================================================================*
 *				try_deliver_senda			     *
 *===========================================================================*/
صحیح try_deliver_senda(ساخت proc *caller_ptr,
				asynmsg_t *جدول,
				size_t سائز)
{
  صحیح r, dst_p, مکمل, do_notify;
  غیرعلامتی صحیح i;
  غیرعلامتی flags;
  endpoint_t dst;
  ساخت proc *dst_ptr;
  ساخت priv *privp;
  asynmsg_t tabent;
  ثابت vir_بائٹیں جدول_v = (vir_بائٹیں) جدول;
  پیغام *m_ptr = تہی;

  privp = priv(caller_ptr);

  /* Clear table */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  privp->s_asynendpoint = caller_ptr->p_endpoint;

  اگر (سائز == 0) واپس(OK);  /* Nothing to do, just return */

  /* Scan the table */
  do_notify = FALSE;
  مکمل = TRUE;

  /* Limit size to something reasonable. An arbitrary choice is 16
   * times the number of process table entries.
   *
   * (this check has been duplicated in sys_call but is left here
   * as a sanity check)
   */
  اگر (سائز > 16*(NR_TASKS + NR_PROCS)) {
    r = EDOM;
    واپس r;
  }

  برائے (i = 0; i < سائز; i++) {
	/* Process each entry in the table and store the result in the table.
	 * If we're done handling a message, copy the result to the sender. */

	dst = NONE;
	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	اگر (flags == 0) جاری; /* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	اگر(flags & ~(AMF_VALID|AMF_مکمل|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR)) {
		r = EINVAL;
		جاؤ asyn_error;
	}
	اگر (!(flags & AMF_VALID)) { /* Must contain message */
		r = EINVAL;
		جاؤ asyn_error;
	}
	اگر (flags & AMF_مکمل) جاری;	/* Already done processing */

	r = OK;
	اگر (!isokendpt(tabent.dst, &dst_p)) 
		r = EDEADSRCDST; /* Bad destination, report the error */
	ورنہ اگر (iskerneln(dst_p)) 
		r = ECALLDENIED; /* Asyn sends to the kernel are not allowed */
	ورنہ اگر (!may_asynsend_مقصد(caller_ptr, dst_p))
		r = ECALLDENIED; /* Send denied by IPC mask */
	ورنہ 	/* r == OK */
		dst_ptr = proc_addr(dst_p);

	/* XXX: RTS_NO_ENDPOINT should be removed */
	اگر (r == OK && RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT)) {
		r = EDEADSRCDST;
	}

	/* Check if 'dst' is blocked waiting for this message.
	 * If AMF_NOREPLY is set, do not satisfy the receiving part of
	 * a SENDREC.
	 */
	اگر (r == OK && WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr,
	    (vir_بائٹیں)&جدول[i].پیغام_2, تہی) &&
	    (!(flags&AMF_NOREPLY) || !(dst_ptr->p_misc_flags&MF_جواب_PEND))) {
		/* Destination is indeed waiting for this message. */
		dst_ptr->p_delivermsg = tabent.پیغام_2;
		dst_ptr->p_delivermsg.m_ماخذ = caller_ptr->p_endpoint;
		dst_ptr->p_misc_flags |= MF_DELIVERMSG;
		IPC_حالت_جوڑو_CALL(dst_ptr, SENDA);
		RTS_UNSET(dst_ptr, RTS_RECEIVING);
#اگر DEBUG_IPC_HOOK
		hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#اگر_ختم
	} ورنہ اگر (r == OK) {
		/* Inform receiver that something is pending */
		سیٹ_sys_ثنائی_ہندسہ(priv(dst_ptr)->s_asyn_pending, 
			    priv(caller_ptr)->s_id); 
		مکمل = FALSE;
		جاری;
	} 

	/* Store results */
	tabent.نتیجہ = r;
	tabent.flags = flags | AMF_مکمل;
	اگر (flags & AMF_NOTIFY)
		do_notify = TRUE;
	ورنہ اگر (r != OK && (flags & AMF_NOTIFY_ERR))
		do_notify = TRUE;
	A_INSRT(i);	/* Copy results to caller; ignore errors */
	جاری;

asyn_error:
	اگر (dst != NONE)
		لکھو("KERNEL senda error %d to %d\n", r, dst);
	ورنہ
		لکھو("KERNEL senda error %d\n", r);
  }

  اگر (do_notify) 
	mini_notify(proc_addr(ASYNCM), caller_ptr->p_endpoint);

  اگر (!مکمل) {
	privp->s_asyntab = (vir_بائٹیں) جدول;
	privp->s_asynsize = سائز;
  }

  واپس(OK);
}

/*===========================================================================*
 *				mini_senda				     *
 *===========================================================================*/
ساکن صحیح mini_senda(ساخت proc *caller_ptr, asynmsg_t *جدول, size_t سائز)
{
  ساخت priv *privp;

  privp = priv(caller_ptr);
  اگر (!(privp->s_flags & SYS_PROC)) {
	لکھو( "mini_senda: warning caller has no privilege structure\n");
	واپس(EPERM);
  }

  واپس try_deliver_senda(caller_ptr, جدول, سائز);
}


/*===========================================================================*
 *				try_async				     * 
 *===========================================================================*/
ساکن صحیح try_async(ساخت proc * caller_ptr)
{
  صحیح r;
  ساخت priv *privp;
  ساخت proc *src_ptr;
  sys_map_t *map;

  map = &priv(caller_ptr)->s_asyn_pending;

  /* Try all privilege structures */
  برائے (privp = BEG_PRIV_ADDR; privp < انتہا_PRIV_ADDR; ++privp)  {
	اگر (privp->s_proc_nr == NONE)
		جاری;

	اگر (!get_sys_ثنائی_ہندسہ(*map, privp->s_id)) 
		جاری;

	src_ptr = proc_addr(privp->s_proc_nr);

#اگر_تعریف_شدہ CONFIG_SMP
	/*
	 * Do not copy from a process which does not have a stable address space
	 * due to VM fiddling with it
	 */
	اگر (RTS_ISSET(src_ptr, RTS_VMINHIBIT)) {
		src_ptr->p_misc_flags |= MF_SENDA_VM_ناکامی;
		جاری;
	}
#اگر_ختم

	أكد(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	اگر ((r = try_one(ANY, src_ptr, caller_ptr)) == OK)
		واپس(r);
  }

  واپس(ESRCH);
}


/*===========================================================================*
 *				try_one					     *
 *===========================================================================*/
ساکن صحیح try_one(endpoint_t receive_e, ساخت proc *src_ptr,
    ساخت proc *dst_ptr)
{
/* Try to receive an asynchronous message from 'src_ptr' */
  صحیح r = EAGAIN, مکمل, do_notify;
  غیرعلامتی صحیح flags, i;
  size_t سائز;
  endpoint_t dst, src_e;
  ساخت proc *caller_ptr;
  ساخت priv *privp;
  asynmsg_t tabent;
  vir_بائٹیں جدول_v;

  privp = priv(src_ptr);
  اگر (!(privp->s_flags & SYS_PROC)) واپس(EPERM);
  سائز = privp->s_asynsize;
  جدول_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  unset_sys_ثنائی_ہندسہ(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  اگر (سائز == 0) واپس(EAGAIN);
  اگر (privp->s_asynendpoint != src_ptr->p_endpoint) واپس EAGAIN;
  اگر (!may_asynsend_مقصد(src_ptr, proc_nr(dst_ptr))) واپس (ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */
  src_e = src_ptr->p_endpoint;

  /* Scan the table */
  do_notify = FALSE;
  مکمل = TRUE;

  برائے (i = 0; i < سائز; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the meantime.
  	 */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	اگر (flags == 0) جاری;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	اگر(flags & ~(AMF_VALID|AMF_مکمل|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	ورنہ اگر (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	ورنہ اگر (flags & AMF_مکمل) جاری; /* Already done processing */

	/* Clear done flag. The sender is done sending when all messages in the
	 * table are marked done or empty. However, we will know that only
	 * the next time we enter this function or when the sender decides to
	 * send additional asynchronous messages and manages to deliver them
	 * all.
	 */
	مکمل = FALSE;

	اگر (r == EINVAL)
		جاؤ store_نتیجہ;

	/* Message must be directed at receiving end */
	اگر (dst != dst_ptr->p_endpoint) جاری;

	اگر (!CANRECEIVE(receive_e, src_e, dst_ptr,
		جدول_v + i*حجم(asynmsg_t) + offsetof(ساخت asynmsg,پیغام_2),
		تہی)) {
		جاری;
	}

	/* If AMF_NOREPLY is set, then this message is not a reply to a
	 * SENDREC and thus should not satisfy the receiving part of the
	 * SENDREC. This message is to be delivered later.
	 */
	اگر ((flags & AMF_NOREPLY) && (dst_ptr->p_misc_flags & MF_جواب_PEND)) 
		جاری;

	/* Destination is ready to receive the message; deliver it */
	r = OK;
	dst_ptr->p_delivermsg = tabent.پیغام_2;
	dst_ptr->p_delivermsg.m_ماخذ = src_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;
#اگر DEBUG_IPC_HOOK
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, src_ptr, dst_ptr);
#اگر_ختم

store_نتیجہ:
	/* Store results for sender. We may just have started delivering a
	 * message, so we must not return an error to the caller in the case
	 * that storing the results triggers an error!
	 */
	tabent.نتیجہ = r;
	tabent.flags = flags | AMF_مکمل;
	اگر (flags & AMF_NOTIFY) do_notify = TRUE;
	ورنہ اگر (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */

	توڑو;
  }

  اگر (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  اگر (مکمل) {
	privp->s_asyntab = -1;
	privp->s_asynsize = 0;
  } ورنہ {
	سیٹ_sys_ثنائی_ہندسہ(priv(dst_ptr)->s_asyn_pending, privp->s_id);
  }

asyn_error:
  واپس(r);
}

/*===========================================================================*
 *				cancel_async				     *
 *===========================================================================*/
صحیح cancel_async(ساخت proc *src_ptr, ساخت proc *dst_ptr)
{
/* Cancel asynchronous messages from src to dst, because dst is not interested
 * in them (e.g., dst has been restarted) */
  صحیح مکمل, do_notify;
  غیرعلامتی صحیح flags, i;
  size_t سائز;
  endpoint_t dst;
  ساخت proc *caller_ptr;
  ساخت priv *privp;
  asynmsg_t tabent;
  vir_بائٹیں جدول_v;

  privp = priv(src_ptr);
  اگر (!(privp->s_flags & SYS_PROC)) واپس(EPERM);
  سائز = privp->s_asynsize;
  جدول_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  unset_sys_ثنائی_ہندسہ(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  اگر (سائز == 0) واپس(EAGAIN);
  اگر (!may_send_مقصد(src_ptr, proc_nr(dst_ptr))) واپس(ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */

  /* Scan the table */
  do_notify = FALSE;
  مکمل = TRUE;


  برائے (i = 0; i < سائز; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the mean time.
  	 */

  	صحیح r = EDEADSRCDST;	/* Cancel delivery due to dead dst */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	اگر (flags == 0) جاری;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	اگر(flags & ~(AMF_VALID|AMF_مکمل|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	ورنہ اگر (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	ورنہ اگر (flags & AMF_مکمل) جاری; /* Already done processing */

	/* Message must be directed at receiving end */
	اگر (dst != dst_ptr->p_endpoint) {
		مکمل = FALSE;
		جاری;
	}

	/* Store results for sender */
	tabent.نتیجہ = r;
	tabent.flags = flags | AMF_مکمل;
	اگر (flags & AMF_NOTIFY) do_notify = TRUE;
	ورنہ اگر (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */
  }

  اگر (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  اگر (!مکمل) {
	privp->s_asyntab = جدول_v;
	privp->s_asynsize = سائز;
  }

asyn_error:
  واپس(OK);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
خالی قطار_میں_ڈالو(
  اندراج ساخت proc *rp	/* this process is now runnable */
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
  صحیح q = rp->p_ترجیح;	 		/* scheduling queue to use */
  ساخت proc **rdy_سر, **rdy_دم;
  
  أكد(proc_is_runnable(rp));

  أكد(q >= 0);

  rdy_سر = get_cpu_var(rp->p_cpu, تسلسل_q_سر);
  rdy_دم = get_cpu_var(rp->p_cpu, تسلسل_q_دم);

  /* Now add the process to the queue. */
  اگر (!rdy_سر[q]) {		/* add to empty queue */
      rdy_سر[q] = rdy_دم[q] = rp; 		/* create a new queue */
      rp->p_nextready = تہی;		/* mark new end */
  } 
  ورنہ {					/* add to tail of queue */
      rdy_دم[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_دم[q] = rp;				/* set new queue tail */
      rp->p_nextready = تہی;		/* mark new end */
  }

  اگر (cpuid == rp->p_cpu) {
	  /*
	   * enqueueing a process with a higher priority than the current one,
	   * it gets preempted. The current process must be preemptible. Testing
	   * the priority also makes sure that a process does not preempt itself
	   */
	  ساخت proc * p;
	  p = get_cpulocal_var(proc_ptr);
	  أكد(p);
	  اگر((p->p_ترجیح > rp->p_ترجیح) &&
			  (priv(p)->s_flags & PREEMPTIBLE))
		  RTS_سیٹ(p, RTS_PREEMPTED); /* calls dequeue() */
  }
#اگر_تعریف_شدہ CONFIG_SMP
  /*
   * if the process was enqueued on a different cpu and the cpu is idle, i.e.
   * the time is off, we need to wake up that cpu and let it schedule this new
   * process
   */
  ورنہ اگر (get_cpu_var(rp->p_cpu, cpu_is_idle)) {
	  smp_جدول_وقت(rp->p_cpu);
  }
#اگر_ختم

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr)->p_accounting.enter_قطار));


#اگر DEBUG_SANITYCHECKS
  أكد(runqueues_ok_local());
#اگر_ختم
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
ساکن خالی قطار_میں_ڈالو_سر(ساخت proc *rp)
{
  ثابت صحیح q = rp->p_ترجیح;	 		/* scheduling queue to use */

  ساخت proc **rdy_سر, **rdy_دم;

  أكد(proc_ptr_ok(rp));
  أكد(proc_is_runnable(rp));

  /*
   * the process was runnable without its quantum expired when dequeued. A
   * process with no time left should have been handled else and differently
   */
  أكد(rp->p_cpu_وقت_بایاں);

  أكد(q >= 0);


  rdy_سر = get_cpu_var(rp->p_cpu, تسلسل_q_سر);
  rdy_دم = get_cpu_var(rp->p_cpu, تسلسل_q_دم);

  /* Now add the process to the queue. */
  اگر (!rdy_سر[q]) {		/* add to empty queue */
	rdy_سر[q] = rdy_دم[q] = rp; 	/* create a new queue */
	rp->p_nextready = تہی;			/* mark new end */
  } ورنہ {					/* add to head of queue */
	rp->p_nextready = rdy_سر[q];		/* chain head of queue */
	rdy_سر[q] = rp;			/* set new queue head */
  }

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr->p_accounting.enter_قطار)));


  /* Process accounting for scheduling */
  rp->p_accounting.dequeues--;
  rp->p_accounting.preempted++;

#اگر DEBUG_SANITYCHECKS
  أكد(runqueues_ok_local());
#اگر_ختم
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
خالی قطار_سے_نکالو(ساخت proc *rp)
/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 *
 * This function can operate x-cpu as it always removes the process from the
 * queue of the cpu the process is currently assigned to.
 */
  صحیح q = rp->p_ترجیح;		/* queue to use */
  ساخت proc **xpp;			/* iterate over queue */
  ساخت proc *پچھلا_xp;
  u64_t tsc, tsc_delta;

  ساخت proc **rdy_دم;

  أكد(proc_ptr_ok(rp));
  أكد(!proc_is_runnable(rp));

  /* Side-effect for kernel: check if the task's stack still is ok? */
  أكد (!iskernelp(rp) || *priv(rp)->s_ڈھیر_guard == ڈھیر_GUARD);

  rdy_دم = get_cpu_var(rp->p_cpu, تسلسل_q_دم);

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  پچھلا_xp = تہی;				
  برائے (xpp = get_cpu_var_ptr(rp->p_cpu, تسلسل_q_سر[q]); *xpp;
		  xpp = &(*xpp)->p_nextready) {
      اگر (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          اگر (rp == rdy_دم[q]) {		/* queue tail removed */
              rdy_دم[q] = پچھلا_xp;		/* set new tail */
	  }

          توڑو;
      }
      پچھلا_xp = *xpp;				/* save previous in chain */
  }

	
  /* Process accounting for scheduling */
  rp->p_accounting.dequeues++;

  /* this is not all that accurate on virtual machines, especially with
     IO bound processes that only spend a short amount of time in the queue
     at a time. */
  اگر (rp->p_accounting.enter_قطار) {
	read_tsc_64(&tsc);
	tsc_delta = tsc - rp->p_accounting.enter_قطار;
	rp->p_accounting.وقت_اندر_قطار = rp->p_accounting.وقت_اندر_قطار +
		tsc_delta;
	rp->p_accounting.enter_قطار = 0;
  }

  /* For ps(1), remember when the process was last dequeued. */
  rp->p_dequeued = get_monotonic();

#اگر DEBUG_SANITYCHECKS
  أكد(runqueues_ok_local());
#اگر_ختم
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
ساکن ساخت proc * pick_proc(خالی)
{
/* Decide who to run now.  A new process is selected and returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
  اندراج ساخت proc *rp;			/* process to run */
  ساخت proc **rdy_سر;
  صحیح q;				/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the task table.
   * If there are no processes ready to run, return NULL.
   */
  rdy_سر = get_cpulocal_var(تسلسل_q_سر);
  برائے (q=0; q < NR_SCHED_QUEUES; q++) {	
	اگر(!(rp = rdy_سر[q])) {
		TRACE(VF_PICKPROC, لکھو("cpu %d queue %d empty\n", cpuid, q););
		جاری;
	}
	أكد(proc_is_runnable(rp));
	اگر (priv(rp)->s_flags & BILLABLE)	 	
		get_cpulocal_var(bill_ptr) = rp; /* bill for system time */
	واپس rp;
  }
  واپس تہی;
}

/*===========================================================================*
 *				endpoint_lookup				     *
 *===========================================================================*/
ساخت proc *endpoint_lookup(endpoint_t e)
{
	صحیح n;

	اگر(!isokendpt(e, &n)) واپس تہی;

	واپس proc_addr(n);
}

/*===========================================================================*
 *				isokendpt_f				     *
 *===========================================================================*/
#اگر DEBUG_ENABLE_IPC_WARNINGS
صحیح isokendpt_f(ثابت حرف * file, صحیح سطر, endpoint_t e, صحیح * p,
	ثابت صحیح fatalflag)
#ورنہ
صحیح isokendpt_f(endpoint_t e, صحیح * p, ثابت صحیح fatalflag)
#اگر_ختم
{
	صحیح ok = 0;
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
	اگر(isokprocn(*p) && !isemptyn(*p) && proc_addr(*p)->p_endpoint == e)
		ok = 1;
	اگر(!ok && fatalflag)
		panic("invalid endpoint: %d",  e);
	واپس ok;
}

ساکن خالی notify_scheduler(ساخت proc *p)
{
	پیغام m_no_وقتی_حصہ;
	صحیح err;

	أكد(!proc_kernel_scheduler(p));

	/* dequeue the process */
	RTS_سیٹ(p, RTS_NO_وقتی_حصہ);
	/*
	 * Notify the process's scheduler that it has run out of
	 * quantum. This is done by sending a message to the scheduler
	 * on the process's behalf
	 */
	m_no_وقتی_حصہ.m_ماخذ = p->p_endpoint;
	m_no_وقتی_حصہ.m_type   = SCHEDULING_NO_وقتی_حصہ;
	m_no_وقتی_حصہ.m_krn_lsys_جدول_وقت.acnt_قطار = cpu_وقت_2_ms(p->p_accounting.وقت_اندر_قطار);
	m_no_وقتی_حصہ.m_krn_lsys_جدول_وقت.acnt_deqs      = p->p_accounting.dequeues;
	m_no_وقتی_حصہ.m_krn_lsys_جدول_وقت.acnt_ipc_sync  = p->p_accounting.ipc_sync;
	m_no_وقتی_حصہ.m_krn_lsys_جدول_وقت.acnt_ipc_async = p->p_accounting.ipc_async;
	m_no_وقتی_حصہ.m_krn_lsys_جدول_وقت.acnt_preempt   = p->p_accounting.preempted;
	m_no_وقتی_حصہ.m_krn_lsys_جدول_وقت.acnt_cpu       = cpuid;
	m_no_وقتی_حصہ.m_krn_lsys_جدول_وقت.acnt_cpu_بھرو  = cpu_بھرو();

	/* Reset accounting */
	reset_proc_accounting(p);

	اگر ((err = mini_send(p, p->p_scheduler->p_endpoint,
					&m_no_وقتی_حصہ, FROM_KERNEL))) {
		panic("WARNING: Scheduling: mini_send returned %d\n", err);
	}
}

خالی proc_no_وقت(ساخت proc * p)
{
	اگر (!proc_kernel_scheduler(p) && priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		notify_scheduler(p);
	}
	ورنہ {
		/*
		 * non-preemptible processes only need their quantum to
		 * be renewed. In fact, they by pass scheduling
		 */
		p->p_cpu_وقت_بایاں = ms_2_cpu_وقت(p->p_وقتی_حصہ_سائز_ms);
#اگر DEBUG_RACE
		RTS_سیٹ(p, RTS_PREEMPTED);
		RTS_UNSET(p, RTS_PREEMPTED);
#اگر_ختم
	}
}

خالی reset_proc_accounting(ساخت proc *p)
{
  p->p_accounting.preempted = 0;
  p->p_accounting.ipc_sync  = 0;
  p->p_accounting.ipc_async = 0;
  p->p_accounting.dequeues  = 0;
  p->p_accounting.وقت_اندر_قطار = 0;
  p->p_accounting.enter_قطار = 0;
}
	
خالی copr_not_دستیاب_handler(خالی)
{
	ساخت proc * p;
	ساخت proc ** local_fpu_owner;
	/*
	 * Disable the FPU exception (both for the kernel and for the process
	 * once it's scheduled), and initialize or restore the FPU state.
	 */

	disable_fpu_exception();

	p = get_cpulocal_var(proc_ptr);

	/* if FPU is not owned by anyone, do not store anything */
	local_fpu_owner = get_cpulocal_var_ptr(fpu_owner);
	اگر (*local_fpu_owner != تہی) {
		أكد(*local_fpu_owner != p);
		save_local_fpu(*local_fpu_owner, FALSE /*retain*/);
	}

	/*
	 * restore the current process' state and let it run again, do not
	 * schedule!
	 */
	اگر (restore_fpu(p) != OK) {
		/* Restoring FPU state failed. This is always the process's own
		 * fault. Send a signal, and schedule another process instead.
		 */
		*local_fpu_owner = تہی;		/* release FPU */
		cause_sig(proc_nr(p), SIGFPE);
		واپس;
	}

	*local_fpu_owner = p;
	context_stop(proc_addr(KERNEL));
	restore_user_context(p);
	NOT_REACHABLE;
}

خالی release_fpu(ساخت proc * p) {
	ساخت proc ** fpu_owner_ptr;

	fpu_owner_ptr = get_cpu_var_ptr(p->p_cpu, fpu_owner);

	اگر (*fpu_owner_ptr == p)
		*fpu_owner_ptr = تہی;
}

خالی ser_dump_proc(خالی)
{
        ساخت proc *pp;

        برائے (pp= BEG_PROC_ADDR; pp < انتہا_PROC_ADDR; pp++)
        {
                اگر (isemptyp(pp))
                        جاری;
                print_proc_recursive(pp);
        }
}
