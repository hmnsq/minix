/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#অন্তর্ভাৱ "kernel/kernel.h"

#অন্তর্ভাৱ <minix/callnr.h>
#অন্তর্ভাৱ <minix/u64.h>
#অন্তর্ভাৱ <limits.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

পূর্ণাঙ্ক runqueues_ok_cpu(unsigned cpu)
{
  পূর্ণাঙ্ক q, l = 0;
  পঞ্জিকা সংরচনা proc *xp;
  সংরচনা proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  বাবে (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	যদি (l++ > MAX_LOOP) panic("check error");
  }

  বাবে (q=l=0; q < NR_SCHED_QUEUES; q++) {
    যদি (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	ঘূৰাই 0;
    }
    যদি (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	ঘূৰাই 0;
    }
    যদি (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	ঘূৰাই 0;
    }
    বাবে(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	ধ্রুৱ vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	যদি(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		ঘূৰাই 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	যদি(dxp % পরিমাণ(সংরচনা proc)) {
  		printf("xp not a real pointer");
		ঘূৰাই 0;
	}
	যদি(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		ঘূৰাই 0;
	}
	যদি (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ঘূৰাই 0;
	}
        যদি (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ঘূৰাই 0;
        }
        যদি (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ঘূৰাই 0;
	}
	যদি (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ঘূৰাই 0;
	}
	xp->p_found = 1;
	যদি (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ঘূৰাই 0;
	}
	যদি (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		ঘূৰাই 0;
	}
    }
  }	

  বাবে (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	যদি(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		ঘূৰাই 0;
	}
	যদি (isemptyp(xp))
		অনুৱর্ত;
	যদি(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		ঘূৰাই 0;
	}
  }

  /* All is ok. */
  ঘূৰাই 1;
}

#ifdef CONFIG_SMP
স্থির পূর্ণাঙ্ক runqueues_ok_all(শূন্য)
{
	unsigned c;

	বাবে (c = 0 ; c < ncpus; c++) {
		যদি (!runqueues_ok_cpu(c))
			ঘূৰাই 0;
	}
	ঘূৰাই 1;	
}

পূর্ণাঙ্ক runqueues_ok(শূন্য)
{
	ঘূৰাই runqueues_ok_all();
}

#অন্যথা

পূর্ণাঙ্ক runqueues_ok(শূন্য)
{
	ঘূৰাই runqueues_ok_cpu(0);
}


#endif

ৱর্ণ *
rtsflagstr(ধ্রুৱ u32_t flags)
{
	স্থির ৱর্ণ str[100];
	str[0] = '\0';

#define FLAG(n) যদি(flags & n) { strlcat(str, #n " ", পরিমাণ(str)); }

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

	ঘূৰাই str;
}

ৱর্ণ *
miscflagstr(ধ্রুৱ u32_t flags)
{
	স্থির ৱর্ণ str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ঘূৰাই str;
}

ৱর্ণ *
schedulerstr(সংরচনা proc *scheduler)
{
	যদি (scheduler != রিক্ত)
	{
		ঘূৰাই scheduler->p_name;
	}

	ঘূৰাই "KERNEL";
}

স্থির শূন্য
print_proc_name(সংরচনা proc *pp)
{
	ৱর্ণ *name = pp->p_name;
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
	পূর্ণাঙ্ক proc_nr;
	সংরচনা proc *pp = রিক্ত;

	ৱিকল্প(ep) {
	পক্ষ ANY:
		printf("ANY");
	ভঙ্গ;
	পক্ষ SELF:
		printf("SELF");
	ভঙ্গ;
	পক্ষ NONE:
		printf("NONE");
	ভঙ্গ;
	স্ৱভাৱ:
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
	ভঙ্গ;
	}
}

স্থির শূন্য
print_sigmgr(সংরচনা proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	যদি(sig_mgr == NONE) { printf("no sigmgr"); ঘূৰাই; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	যদি(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

শূন্য print_proc(সংরচনা proc *pp)
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
#নোচেদ্যদি defined(__arm__)
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

স্থির শূন্য print_proc_depends(সংরচনা proc *pp, ধ্রুৱ পূর্ণাঙ্ক level)
{
	সংরচনা proc *depproc = রিক্ত;
	endpoint_t dep;
#define COL { পূর্ণাঙ্ক i; বাবে(i = 0; i < level; i++) printf("> "); }

	যদি(level >= NR_PROCS) {
		printf("loop??\n");
		ঘূৰাই;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	যদি(dep != NONE && dep != ANY) {
		পূর্ণাঙ্ক procno;
		যদি(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			যদি(isemptyp(depproc))
				depproc = রিক্ত;
		}
		যদি (depproc)
			print_proc_depends(depproc, level+1);
	}
}

শূন্য print_proc_recursive(সংরচনা proc *pp)
{
	print_proc_depends(pp, 0);
}

#যদি DEBUG_DUMPIPC || DEBUG_DUMPIPCF
স্থির ধ্রুৱ ৱর্ণ *mtypename(পূর্ণাঙ্ক mtype, পূর্ণাঙ্ক *possible_callname)
{
	ৱর্ণ *callname = রিক্ত, *errname = রিক্ত;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	ৱিকল্প(mtype) {
#define IDENT(x) পক্ষ x: callname = #x; *possible_callname = 1; ভঙ্গ;
#অন্তর্ভাৱ "kernel/extracted-mtype.h"
#undef IDENT
	}
	ৱিকল্প(mtype) {
#define IDENT(x) পক্ষ x: errname = #x; ভঙ্গ;
#অন্তর্ভাৱ "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	যদি(!errname && !callname)
		ঘূৰাই রিক্ত;

	/* 2 matches */
	যদি(errname && callname) {
		স্থির ৱর্ণ typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ঘূৰাই typename;
	}

	যদি(errname) ঘূৰাই errname;

	প্রতিজ্ঞা(callname);
	ঘূৰাই callname;
}

স্থির শূন্য printproc(সংরচনা proc *rp)
{
	যদি (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	অন্যথা
		printf(" kernel");
}

স্থির শূন্য printparam(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ শূন্য *data, size_t size)
{
	printf(" %s=", name);
	ৱিকল্প (size) {
		পক্ষ পরিমাণ(ৱর্ণ):	printf("%d", *(ৱর্ণ *) data);	ভঙ্গ;
		পক্ষ পরিমাণ(short):	printf("%d", *(short *) data);	ভঙ্গ;
		পক্ষ পরিমাণ(পূর্ণাঙ্ক):	printf("%d", *(পূর্ণাঙ্ক *) data);	ভঙ্গ;
		স্ৱভাৱ:		printf("(%u bytes)", size);	ভঙ্গ;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
স্থির পূর্ণাঙ্ক namematch(ৱর্ণ **names, পূর্ণাঙ্ক nnames, ৱর্ণ *name)
{
	পূর্ণাঙ্ক i;
	বাবে(i = 0; i < nnames; i++)
		যদি(!strcmp(names[i], name))
			ঘূৰাই 1;
	ঘূৰাই 0;
}
#endif

শূন্য printmsg(message *বাৰ্তা, সংরচনা proc *src, সংরচনা proc *dst,
	ৱর্ণ operation, পূর্ণাঙ্ক printparams)
{
	ধ্রুৱ ৱর্ণ *name;
	পূর্ণাঙ্ক mtype = বাৰ্তা->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	ৱর্ণ *names[] = DEBUG_DUMPIPC_NAMES;
	পূর্ণাঙ্ক nnames = পরিমাণ(names)/পরিমাণ(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	যদি(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		ঘূৰাই;
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
#define IDENT(x, y) যদি (mtype == x) printparam(#y, &বাৰ্তা->y, পরিমাণ(বাৰ্তা->y));
#অন্তর্ভাৱ "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#যদি DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
স্থির পূর্ণাঙ্ক messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
স্থির সংরচনা {
	পূর্ণাঙ্ক src, dst, messages;
} winners[PRINTSLOTS];
স্থির পূর্ণাঙ্ক total, goodslots;

স্থির শূন্য printstats(পূর্ণাঙ্ক ticks)
{
	পূর্ণাঙ্ক i;
	বাবে(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		ৱর্ণ	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

স্থির শূন্য sortstats(শূন্য)
{
	/* Print top message senders/receivers. */
	পূর্ণাঙ্ক src_slot, dst_slot;
	total = goodslots = 0;
	বাবে(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		বাবে(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			পূর্ণাঙ্ক w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			যেতিয়ালৈ(w > 0 && n > winners[w-1].messages)
				w--;
			যদি(w >= PRINTSLOTS) অনুৱর্ত;

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
					rem*পরিমাণ(winners[0]));
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

স্থির শূন্য statmsg(message *বাৰ্তা, সংরচনা proc *srcp, সংরচনা proc *dstp)
{
	পূর্ণাঙ্ক src, dst, now, secs, dt;
	স্থির পূর্ণাঙ্ক lastprint;

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
		memset(winners, 0, পরিমাণ(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, পরিমাণ(messages));
		lastprint = now;
	}
}
#endif

#যদি DEBUG_IPC_HOOK
শূন্য hook_ipc_msgkcall(message *বাৰ্তা, সংরচনা proc *proc)
{
#যদি DEBUG_DUMPIPC
	printmsg(বাৰ্তা, proc, রিক্ত, 'k', 1);
#endif
}

শূন্য hook_ipc_msgkresult(message *বাৰ্তা, সংরচনা proc *proc)
{
#যদি DEBUG_DUMPIPC
	printmsg(বাৰ্তা, রিক্ত, proc, 'k', 0);
#endif
#যদি DEBUG_IPCSTATS
	statmsg(বাৰ্তা, proc, রিক্ত);
#endif
}

শূন্য hook_ipc_msgrecv(message *বাৰ্তা, সংরচনা proc *src, সংরচনা proc *dst)
{
#যদি DEBUG_DUMPIPC
	printmsg(বাৰ্তা, src, dst, 'r', 0);
#endif
#যদি DEBUG_IPCSTATS
	statmsg(বাৰ্তা, src, dst);
#endif
}

শূন্য hook_ipc_msgsend(message *বাৰ্তা, সংরচনা proc *src, সংরচনা proc *dst)
{
#যদি DEBUG_DUMPIPC
	printmsg(বাৰ্তা, src, dst, 's', 1);
#endif
}

শূন্য hook_ipc_clear(সংরচনা proc *p)
{
#যদি DEBUG_IPCSTATS
	পূর্ণাঙ্ক slot, i;
	প্রতিজ্ঞা(p);
	proc2slot(p, slot);
	বাবে(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
