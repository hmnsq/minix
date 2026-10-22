/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#شامل "kernel/kernel.h"

#شامل <minix/callnr.h>
#شامل <minix/u64.h>
#شامل <limits.h>
#شامل <string.h>
#شامل <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

صحیح runqueues_ok_cpu(unsigned cpu)
{
  صحیح q, l = 0;
  ثبات ساختار proc *xp;
  ساختار proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  برای (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	اگر (l++ > MAX_LOOP) panic("check error");
  }

  برای (q=l=0; q < NR_SCHED_QUEUES; q++) {
    اگر (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	بازگشت 0;
    }
    اگر (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	بازگشت 0;
    }
    اگر (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	بازگشت 0;
    }
    برای(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	ثابت vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	اگر(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		بازگشت 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	اگر(dxp % اندازه(ساختار proc)) {
  		printf("xp not a real pointer");
		بازگشت 0;
	}
	اگر(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		بازگشت 0;
	}
	اگر (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		بازگشت 0;
	}
        اگر (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		بازگشت 0;
        }
        اگر (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		بازگشت 0;
	}
	اگر (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		بازگشت 0;
	}
	xp->p_found = 1;
	اگر (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		بازگشت 0;
	}
	اگر (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		بازگشت 0;
	}
    }
  }	

  برای (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	اگر(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		بازگشت 0;
	}
	اگر (isemptyp(xp))
		ادامه;
	اگر(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		بازگشت 0;
	}
  }

  /* All is ok. */
  بازگشت 1;
}

#ifdef CONFIG_SMP
ایستا صحیح runqueues_ok_all(تهی)
{
	unsigned c;

	برای (c = 0 ; c < ncpus; c++) {
		اگر (!runqueues_ok_cpu(c))
			بازگشت 0;
	}
	بازگشت 1;	
}

صحیح runqueues_ok(تهی)
{
	بازگشت runqueues_ok_all();
}

#وگرنه

صحیح runqueues_ok(تهی)
{
	بازگشت runqueues_ok_cpu(0);
}


#endif

نویسه *
rtsflagstr(ثابت u32_t flags)
{
	ایستا نویسه str[100];
	str[0] = '\0';

#define FLAG(n) اگر(flags & n) { strlcat(str, #n " ", اندازه(str)); }

	FLAG(RTS_SLOT_FREE);
	FLAG(RTS_PROC_STOP);
	FLAG(RTS_SENDING);
	FLAG(RTS_RECEIVING);
	FLAG(RTS_SIGNALED);
	FLAG(RTS_SIG_PENDING);
	FLAG(RTS_P_STOP);
	FLAG(RTS_NO_PRIV);
	FLAG(RTS_NO_ENDPOINT);
	FLAG(RTS_VMINHIBIT);
	FLAG(RTS_PAGEFAULT);
	FLAG(RTS_VMREQUEST);
	FLAG(RTS_VMREQTARGET);
	FLAG(RTS_PREEMPTED);
	FLAG(RTS_NO_QUANTUM);

	بازگشت str;
}

نویسه *
miscflagstr(ثابت u32_t flags)
{
	ایستا نویسه str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	بازگشت str;
}

نویسه *
schedulerstr(ساختار proc *scheduler)
{
	اگر (scheduler != پوچ)
	{
		بازگشت scheduler->p_name;
	}

	بازگشت "KERNEL";
}

ایستا تهی
print_proc_name(ساختار proc *pp)
{
	نویسه *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	اگر(name) {
		printf("%s(%d)", name, ep);
	}
	وگرنه {
		printf("%d", ep);
	}
}

ایستا تهی
print_endpoint(endpoint_t ep)
{
	صحیح proc_nr;
	ساختار proc *pp = پوچ;

	انتخاب(ep) {
	حالت ANY:
		printf("ANY");
	بشکن;
	حالت SELF:
		printf("SELF");
	بشکن;
	حالت NONE:
		printf("NONE");
	بشکن;
	پیشفرض:
		اگر(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		وگرنه {
			pp = proc_addr(proc_nr);
			اگر(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			وگرنه {
				print_proc_name(pp);
			}
		}
	بشکن;
	}
}

ایستا تهی
print_sigmgr(ساختار proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	اگر(sig_mgr == NONE) { printf("no sigmgr"); بازگشت; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	اگر(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

تهی print_proc(ساختار proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#اگر defined(__i386__)
		pp->p_seg.p_cr3,
#وگرنهاگر defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	اگر(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

ایستا تهی print_proc_depends(ساختار proc *pp, ثابت صحیح level)
{
	ساختار proc *depproc = پوچ;
	endpoint_t dep;
#define COL { صحیح i; برای(i = 0; i < level; i++) printf("> "); }

	اگر(level >= NR_PROCS) {
		printf("loop??\n");
		بازگشت;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	اگر(dep != NONE && dep != ANY) {
		صحیح procno;
		اگر(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			اگر(isemptyp(depproc))
				depproc = پوچ;
		}
		اگر (depproc)
			print_proc_depends(depproc, level+1);
	}
}

تهی print_proc_recursive(ساختار proc *pp)
{
	print_proc_depends(pp, 0);
}

#اگر DEBUG_DUMPIPC || DEBUG_DUMPIPCF
ایستا ثابت نویسه *mtypename(صحیح mtype, صحیح *possible_callname)
{
	نویسه *callname = پوچ, *errname = پوچ;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	انتخاب(mtype) {
#define IDENT(x) حالت x: callname = #x; *possible_callname = 1; بشکن;
#شامل "kernel/extracted-mtype.h"
#undef IDENT
	}
	انتخاب(mtype) {
#define IDENT(x) حالت x: errname = #x; بشکن;
#شامل "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	اگر(!errname && !callname)
		بازگشت پوچ;

	/* 2 matches */
	اگر(errname && callname) {
		ایستا نویسه typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		بازگشت typename;
	}

	اگر(errname) بازگشت errname;

	أكد(callname);
	بازگشت callname;
}

ایستا تهی printproc(ساختار proc *rp)
{
	اگر (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	وگرنه
		printf(" kernel");
}

ایستا تهی printparam(ثابت نویسه *name, ثابت تهی *data, size_t size)
{
	printf(" %s=", name);
	انتخاب (size) {
		حالت اندازه(نویسه):	printf("%d", *(نویسه *) data);	بشکن;
		حالت اندازه(short):	printf("%d", *(short *) data);	بشکن;
		حالت اندازه(صحیح):	printf("%d", *(صحیح *) data);	بشکن;
		پیشفرض:		printf("(%u bytes)", size);	بشکن;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
ایستا صحیح namematch(نویسه **names, صحیح nnames, نویسه *name)
{
	صحیح i;
	برای(i = 0; i < nnames; i++)
		اگر(!strcmp(names[i], name))
			بازگشت 1;
	بازگشت 0;
}
#endif

تهی printmsg(message *پیام, ساختار proc *src, ساختار proc *dst,
	نویسه operation, صحیح printparams)
{
	ثابت نویسه *name;
	صحیح mtype = پیام->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	نویسه *names[] = DEBUG_DUMPIPC_NAMES;
	صحیح nnames = اندازه(names)/اندازه(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	اگر(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		بازگشت;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	اگر (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} وگرنه {
		printf(" %d/0x%x", mtype, mtype);
	}

	اگر (mightbecall && printparams) {
#define IDENT(x, y) اگر (mtype == x) printparam(#y, &پیام->y, اندازه(پیام->y));
#شامل "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#اگر DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ایستا صحیح messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
ایستا ساختار {
	صحیح src, dst, messages;
} winners[PRINTSLOTS];
ایستا صحیح total, goodslots;

ایستا تهی printstats(صحیح ticks)
{
	صحیح i;
	برای(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		نویسه	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

ایستا تهی sortstats(تهی)
{
	/* Print top message senders/receivers. */
	صحیح src_slot, dst_slot;
	total = goodslots = 0;
	برای(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		برای(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			صحیح w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			وقتی(w > 0 && n > winners[w-1].messages)
				w--;
			اگر(w >= PRINTSLOTS) ادامه;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			أكد(rem >= 0);
			أكد(rem < PRINTSLOTS);
			اگر(rem > 0) {
				أكد(w+1 <= PRINTSLOTS-1);
				أكد(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*اندازه(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			اگر(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	اگر(p) { s = p->p_nr; } \
	وگرنه { s = KERNELIPC; } \
	أكد(s >= 0 && s < IPCPROCS); \
}

ایستا تهی statmsg(message *پیام, ساختار proc *srcp, ساختار proc *dstp)
{
	صحیح src, dst, now, secs, dt;
	ایستا صحیح lastprint;

	/* Stat message. */
	أكد(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	اگر(secs >= 30) {
		memset(winners, 0, اندازه(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, اندازه(messages));
		lastprint = now;
	}
}
#endif

#اگر DEBUG_IPC_HOOK
تهی hook_ipc_msgkcall(message *پیام, ساختار proc *proc)
{
#اگر DEBUG_DUMPIPC
	printmsg(پیام, proc, پوچ, 'k', 1);
#endif
}

تهی hook_ipc_msgkresult(message *پیام, ساختار proc *proc)
{
#اگر DEBUG_DUMPIPC
	printmsg(پیام, پوچ, proc, 'k', 0);
#endif
#اگر DEBUG_IPCSTATS
	statmsg(پیام, proc, پوچ);
#endif
}

تهی hook_ipc_msgrecv(message *پیام, ساختار proc *src, ساختار proc *dst)
{
#اگر DEBUG_DUMPIPC
	printmsg(پیام, src, dst, 'r', 0);
#endif
#اگر DEBUG_IPCSTATS
	statmsg(پیام, src, dst);
#endif
}

تهی hook_ipc_msgsend(message *پیام, ساختار proc *src, ساختار proc *dst)
{
#اگر DEBUG_DUMPIPC
	printmsg(پیام, src, dst, 's', 1);
#endif
}

تهی hook_ipc_clear(ساختار proc *p)
{
#اگر DEBUG_IPCSTATS
	صحیح slot, i;
	أكد(p);
	proc2slot(p, slot);
	برای(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
