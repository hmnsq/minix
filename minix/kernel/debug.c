/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#অন্তর্ভাব "kernel/kernel.h"

#অন্তর্ভাব <minix/callnr.h>
#অন্তর্ভাব <minix/u64.h>
#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

পূর্ণসংখ্যা runqueues_ok_cpu(unsigned cpu)
{
  পূর্ণসংখ্যা q, l = 0;
  নিবন্ধ কাঠামো proc *xp;
  কাঠামো proc **rdy_শীর্ষ, **rdy_লেজ;

  rdy_শীর্ষ = get_cpu_var(cpu, run_q_শীর্ষ);
  rdy_লেজ = get_cpu_var(cpu, run_q_লেজ);

  জন্য (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	যদি (l++ > MAX_LOOP) panic("check error");
  }

  জন্য (q=l=0; q < NR_SCHED_QUEUES; q++) {
    যদি (rdy_শীর্ষ[q] && !rdy_লেজ[q]) {
	printf("head but no tail in %d\n", q);
	ফেরত 0;
    }
    যদি (!rdy_শীর্ষ[q] && rdy_লেজ[q]) {
	printf("tail but no head in %d\n", q);
	ফেরত 0;
    }
    যদি (rdy_লেজ[q] && rdy_লেজ[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	ফেরত 0;
    }
    জন্য(xp = rdy_শীর্ষ[q]; xp; xp = xp->p_nextready) {
	ধ্রুবক vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	যদি(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		ফেরত 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	যদি(dxp % আকার(কাঠামো proc)) {
  		printf("xp not a real pointer");
		ফেরত 0;
	}
	যদি(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		ফেরত 0;
	}
	যদি (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ফেরত 0;
	}
        যদি (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ফেরত 0;
        }
        যদি (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ফেরত 0;
	}
	যদি (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ফেরত 0;
	}
	xp->p_found = 1;
	যদি (!xp->p_nextready && rdy_লেজ[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ফেরত 0;
	}
	যদি (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		ফেরত 0;
	}
    }
  }	

  জন্য (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	যদি(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		ফেরত 0;
	}
	যদি (isemptyp(xp))
		চালু;
	যদি(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		ফেরত 0;
	}
  }

  /* All is ok. */
  ফেরত 1;
}

#ifdef CONFIG_SMP
স্থির পূর্ণসংখ্যা runqueues_ok_all(শূন্য)
{
	unsigned c;

	জন্য (c = 0 ; c < ncpus; c++) {
		যদি (!runqueues_ok_cpu(c))
			ফেরত 0;
	}
	ফেরত 1;	
}

পূর্ণসংখ্যা runqueues_ok(শূন্য)
{
	ফেরত runqueues_ok_all();
}

#অন্যথা

পূর্ণসংখ্যা runqueues_ok(শূন্য)
{
	ফেরত runqueues_ok_cpu(0);
}


#endif

অক্ষর *
rtsflagstr(ধ্রুবক u32_t flags)
{
	স্থির অক্ষর str[100];
	str[0] = '\0';

#define FLAG(n) যদি(flags & n) { strlcat(str, #n " ", আকার(str)); }

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

	ফেরত str;
}

অক্ষর *
miscflagstr(ধ্রুবক u32_t flags)
{
	স্থির অক্ষর str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ফেরত str;
}

অক্ষর *
schedulerstr(কাঠামো proc *scheduler)
{
	যদি (scheduler != ফাঁকা)
	{
		ফেরত scheduler->p_name;
	}

	ফেরত "KERNEL";
}

স্থির শূন্য
print_proc_name(কাঠামো proc *pp)
{
	অক্ষর *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	যদি(name) {
		printf("%s(%d)", name, ep);
	}
	অন্যথা {
		printf("%d", ep);
	}
}

স্থির শূন্য
print_endpoint(endpoint_t ep)
{
	পূর্ণসংখ্যা proc_nr;
	কাঠামো proc *pp = ফাঁকা;

	নির্বাচন(ep) {
	ক্ষেত্র ANY:
		printf("ANY");
	বিরতি;
	ক্ষেত্র SELF:
		printf("SELF");
	বিরতি;
	ক্ষেত্র NONE:
		printf("NONE");
	বিরতি;
	পূর্বনির্ধারিত:
		যদি(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		অন্যথা {
			pp = proc_addr(proc_nr);
			যদি(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			অন্যথা {
				print_proc_name(pp);
			}
		}
	বিরতি;
	}
}

স্থির শূন্য
print_sigmgr(কাঠামো proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	যদি(sig_mgr == NONE) { printf("no sigmgr"); ফেরত; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	যদি(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

শূন্য print_proc(কাঠামো proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#যদি defined(__i386__)
		pp->p_seg.p_cr3,
#নতুবা_যদি defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	যদি(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

স্থির শূন্য print_proc_depends(কাঠামো proc *pp, ধ্রুবক পূর্ণসংখ্যা level)
{
	কাঠামো proc *depproc = ফাঁকা;
	endpoint_t dep;
#define COL { পূর্ণসংখ্যা i; জন্য(i = 0; i < level; i++) printf("> "); }

	যদি(level >= NR_PROCS) {
		printf("loop??\n");
		ফেরত;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	যদি(dep != NONE && dep != ANY) {
		পূর্ণসংখ্যা procno;
		যদি(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			যদি(isemptyp(depproc))
				depproc = ফাঁকা;
		}
		যদি (depproc)
			print_proc_depends(depproc, level+1);
	}
}

শূন্য print_proc_recursive(কাঠামো proc *pp)
{
	print_proc_depends(pp, 0);
}

#যদি DEBUG_DUMPIPC || DEBUG_DUMPIPCF
স্থির ধ্রুবক অক্ষর *mtypename(পূর্ণসংখ্যা mtype, পূর্ণসংখ্যা *possible_callname)
{
	অক্ষর *callname = ফাঁকা, *errname = ফাঁকা;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	নির্বাচন(mtype) {
#define IDENT(x) ক্ষেত্র x: callname = #x; *possible_callname = 1; বিরতি;
#অন্তর্ভাব "kernel/extracted-mtype.h"
#undef IDENT
	}
	নির্বাচন(mtype) {
#define IDENT(x) ক্ষেত্র x: errname = #x; বিরতি;
#অন্তর্ভাব "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	যদি(!errname && !callname)
		ফেরত ফাঁকা;

	/* 2 matches */
	যদি(errname && callname) {
		স্থির অক্ষর typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ফেরত typename;
	}

	যদি(errname) ফেরত errname;

	প্রতিজ্ঞা(callname);
	ফেরত callname;
}

স্থির শূন্য printproc(কাঠামো proc *rp)
{
	যদি (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	অন্যথা
		printf(" kernel");
}

স্থির শূন্য printparam(ধ্রুবক অক্ষর *name, ধ্রুবক শূন্য *তথ্য, size_t আকার_2)
{
	printf(" %s=", name);
	নির্বাচন (আকার_2) {
		ক্ষেত্র আকার(অক্ষর):	printf("%d", *(অক্ষর *) তথ্য);	বিরতি;
		ক্ষেত্র আকার(short):	printf("%d", *(short *) তথ্য);	বিরতি;
		ক্ষেত্র আকার(পূর্ণসংখ্যা):	printf("%d", *(পূর্ণসংখ্যা *) তথ্য);	বিরতি;
		পূর্বনির্ধারিত:		printf("(%u bytes)", আকার_2);	বিরতি;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
স্থির পূর্ণসংখ্যা namematch(অক্ষর **names, পূর্ণসংখ্যা nnames, অক্ষর *name)
{
	পূর্ণসংখ্যা i;
	জন্য(i = 0; i < nnames; i++)
		যদি(!strcmp(names[i], name))
			ফেরত 1;
	ফেরত 0;
}
#endif

শূন্য printmsg(message *বার্তা, কাঠামো proc *src, কাঠামো proc *dst,
	অক্ষর operation, পূর্ণসংখ্যা printparams)
{
	ধ্রুবক অক্ষর *name;
	পূর্ণসংখ্যা mtype = বার্তা->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	অক্ষর *names[] = DEBUG_DUMPIPC_NAMES;
	পূর্ণসংখ্যা nnames = আকার(names)/আকার(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	যদি(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		ফেরত;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	যদি (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} অন্যথা {
		printf(" %d/0x%x", mtype, mtype);
	}

	যদি (mightbecall && printparams) {
#define IDENT(x, y) যদি (mtype == x) printparam(#y, &বার্তা->y, আকার(বার্তা->y));
#অন্তর্ভাব "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#যদি DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
স্থির পূর্ণসংখ্যা messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
স্থির কাঠামো {
	পূর্ণসংখ্যা src, dst, messages;
} winners[PRINTSLOTS];
স্থির পূর্ণসংখ্যা যোগ, goodslots;

স্থির শূন্য printstats(পূর্ণসংখ্যা ticks)
{
	পূর্ণসংখ্যা i;
	জন্য(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		অক্ষর	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(যোগ));
}

স্থির শূন্য sortstats(শূন্য)
{
	/* Print top message senders/receivers. */
	পূর্ণসংখ্যা src_slot, dst_slot;
	যোগ = goodslots = 0;
	জন্য(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		জন্য(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			পূর্ণসংখ্যা w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			যোগ += n;
			যখন(w > 0 && n > winners[w-1].messages)
				w--;
			যদি(w >= PRINTSLOTS) চালু;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			প্রতিজ্ঞা(rem >= 0);
			প্রতিজ্ঞা(rem < PRINTSLOTS);
			যদি(rem > 0) {
				প্রতিজ্ঞা(w+1 <= PRINTSLOTS-1);
				প্রতিজ্ঞা(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*আকার(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			যদি(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	যদি(p) { s = p->p_nr; } \
	অন্যথা { s = KERNELIPC; } \
	প্রতিজ্ঞা(s >= 0 && s < IPCPROCS); \
}

স্থির শূন্য statmsg(message *বার্তা, কাঠামো proc *srcp, কাঠামো proc *dstp)
{
	পূর্ণসংখ্যা src, dst, now, secs, dt;
	স্থির পূর্ণসংখ্যা lastprint;

	/* Stat message. */
	প্রতিজ্ঞা(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	যদি(secs >= 30) {
		memset(winners, 0, আকার(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, আকার(messages));
		lastprint = now;
	}
}
#endif

#যদি DEBUG_IPC_HOOK
শূন্য hook_ipc_msgkcall(message *বার্তা, কাঠামো proc *proc)
{
#যদি DEBUG_DUMPIPC
	printmsg(বার্তা, proc, ফাঁকা, 'k', 1);
#endif
}

শূন্য hook_ipc_msgkresult(message *বার্তা, কাঠামো proc *proc)
{
#যদি DEBUG_DUMPIPC
	printmsg(বার্তা, ফাঁকা, proc, 'k', 0);
#endif
#যদি DEBUG_IPCSTATS
	statmsg(বার্তা, proc, ফাঁকা);
#endif
}

শূন্য hook_ipc_msgrecv(message *বার্তা, কাঠামো proc *src, কাঠামো proc *dst)
{
#যদি DEBUG_DUMPIPC
	printmsg(বার্তা, src, dst, 'r', 0);
#endif
#যদি DEBUG_IPCSTATS
	statmsg(বার্তা, src, dst);
#endif
}

শূন্য hook_ipc_msgsend(message *বার্তা, কাঠামো proc *src, কাঠামো proc *dst)
{
#যদি DEBUG_DUMPIPC
	printmsg(বার্তা, src, dst, 's', 1);
#endif
}

শূন্য hook_ipc_clear(কাঠামো proc *p)
{
#যদি DEBUG_IPCSTATS
	পূর্ণসংখ্যা slot, i;
	প্রতিজ্ঞা(p);
	proc2slot(p, slot);
	জন্য(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
