/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

#ଅନ୍ତର୍ଭାଵ <minix/callnr.h>
#ଅନ୍ତର୍ଭାଵ <minix/u64.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା runqueues_ok_cpu(unsigned cpu)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା q, l = 0;
  ପଞ୍ଜିକରଣ ସଂରଚନା proc *xp;
  ସଂରଚନା proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  ପାଇଁ (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	ଯଦି (l++ > MAX_LOOP) panic("check error");
  }

  ପାଇଁ (q=l=0; q < NR_SCHED_QUEUES; q++) {
    ଯଦି (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	ଫେରନ୍ତୁ 0;
    }
    ଯଦି (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	ଫେରନ୍ତୁ 0;
    }
    ଯଦି (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	ଫେରନ୍ତୁ 0;
    }
    ପାଇଁ(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	ଅଚଳ vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	ଯଦି(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		ଫେରନ୍ତୁ 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	ଯଦି(dxp % ଆକାର(ସଂରଚନା proc)) {
  		printf("xp not a real pointer");
		ଫେରନ୍ତୁ 0;
	}
	ଯଦି(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		ଫେରନ୍ତୁ 0;
	}
	ଯଦି (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ଫେରନ୍ତୁ 0;
	}
        ଯଦି (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ଫେରନ୍ତୁ 0;
        }
        ଯଦି (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ଫେରନ୍ତୁ 0;
	}
	ଯଦି (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ଫେରନ୍ତୁ 0;
	}
	xp->p_found = 1;
	ଯଦି (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ଫେରନ୍ତୁ 0;
	}
	ଯଦି (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		ଫେରନ୍ତୁ 0;
	}
    }
  }	

  ପାଇଁ (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	ଯଦି(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		ଫେରନ୍ତୁ 0;
	}
	ଯଦି (isemptyp(xp))
		ଜାରି;
	ଯଦି(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		ଫେରନ୍ତୁ 0;
	}
  }

  /* All is ok. */
  ଫେରନ୍ତୁ 1;
}

#ifdef CONFIG_SMP
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା runqueues_ok_all(ଶୂନ୍ୟ)
{
	unsigned c;

	ପାଇଁ (c = 0 ; c < ncpus; c++) {
		ଯଦି (!runqueues_ok_cpu(c))
			ଫେରନ୍ତୁ 0;
	}
	ଫେରନ୍ତୁ 1;	
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା runqueues_ok(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ runqueues_ok_all();
}

#ଅନ୍ୟଥା

ପୂର୍ଣ୍ଣସଂଖ୍ୟା runqueues_ok(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ runqueues_ok_cpu(0);
}


#endif

ଅକ୍ଷର *
rtsflagstr(ଅଚଳ u32_t flags)
{
	ସ୍ଥିର ଅକ୍ଷର str[100];
	str[0] = '\0';

#define FLAG(n) ଯଦି(flags & n) { strlcat(str, #n " ", ଆକାର(str)); }

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

	ଫେରନ୍ତୁ str;
}

ଅକ୍ଷର *
miscflagstr(ଅଚଳ u32_t flags)
{
	ସ୍ଥିର ଅକ୍ଷର str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ଫେରନ୍ତୁ str;
}

ଅକ୍ଷର *
schedulerstr(ସଂରଚନା proc *scheduler)
{
	ଯଦି (scheduler != ଖାଲି)
	{
		ଫେରନ୍ତୁ scheduler->p_name;
	}

	ଫେରନ୍ତୁ "KERNEL";
}

ସ୍ଥିର ଶୂନ୍ୟ
print_proc_name(ସଂରଚନା proc *pp)
{
	ଅକ୍ଷର *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	ଯଦି(name) {
		printf("%s(%d)", name, ep);
	}
	ଅନ୍ୟଥା {
		printf("%d", ep);
	}
}

ସ୍ଥିର ଶୂନ୍ୟ
print_endpoint(endpoint_t ep)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;
	ସଂରଚନା proc *pp = ଖାଲି;

	ବାଛ(ep) {
	ସ୍ଥିତି ANY:
		printf("ANY");
	ଭାଙ୍ଗ;
	ସ୍ଥିତି SELF:
		printf("SELF");
	ଭାଙ୍ଗ;
	ସ୍ଥିତି NONE:
		printf("NONE");
	ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଯଦି(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		ଅନ୍ୟଥା {
			pp = proc_addr(proc_nr);
			ଯଦି(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			ଅନ୍ୟଥା {
				print_proc_name(pp);
			}
		}
	ଭାଙ୍ଗ;
	}
}

ସ୍ଥିର ଶୂନ୍ୟ
print_sigmgr(ସଂରଚନା proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	ଯଦି(sig_mgr == NONE) { printf("no sigmgr"); ଫେରନ୍ତୁ; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	ଯଦି(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

ଶୂନ୍ୟ print_proc(ସଂରଚନା proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#ଯଦି defined(__i386__)
		pp->p_seg.p_cr3,
#ନଚେତଯଦି defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	ଯଦି(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

ସ୍ଥିର ଶୂନ୍ୟ print_proc_depends(ସଂରଚନା proc *pp, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା level)
{
	ସଂରଚନା proc *depproc = ଖାଲି;
	endpoint_t dep;
#define COL { ପୂର୍ଣ୍ଣସଂଖ୍ୟା i; ପାଇଁ(i = 0; i < level; i++) printf("> "); }

	ଯଦି(level >= NR_PROCS) {
		printf("loop??\n");
		ଫେରନ୍ତୁ;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	ଯଦି(dep != NONE && dep != ANY) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା procno;
		ଯଦି(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			ଯଦି(isemptyp(depproc))
				depproc = ଖାଲି;
		}
		ଯଦି (depproc)
			print_proc_depends(depproc, level+1);
	}
}

ଶୂନ୍ୟ print_proc_recursive(ସଂରଚନା proc *pp)
{
	print_proc_depends(pp, 0);
}

#ଯଦି DEBUG_DUMPIPC || DEBUG_DUMPIPCF
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *mtypename(ପୂର୍ଣ୍ଣସଂଖ୍ୟା mtype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *possible_callname)
{
	ଅକ୍ଷର *callname = ଖାଲି, *errname = ଖାଲି;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	ବାଛ(mtype) {
#define IDENT(x) ସ୍ଥିତି x: callname = #x; *possible_callname = 1; ଭାଙ୍ଗ;
#ଅନ୍ତର୍ଭାଵ "kernel/extracted-mtype.h"
#undef IDENT
	}
	ବାଛ(mtype) {
#define IDENT(x) ସ୍ଥିତି x: errname = #x; ଭାଙ୍ଗ;
#ଅନ୍ତର୍ଭାଵ "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	ଯଦି(!errname && !callname)
		ଫେରନ୍ତୁ ଖାଲି;

	/* 2 matches */
	ଯଦି(errname && callname) {
		ସ୍ଥିର ଅକ୍ଷର typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ଫେରନ୍ତୁ typename;
	}

	ଯଦି(errname) ଫେରନ୍ତୁ errname;

	ପ୍ରତିଜ୍ଞା(callname);
	ଫେରନ୍ତୁ callname;
}

ସ୍ଥିର ଶୂନ୍ୟ printproc(ସଂରଚନା proc *rp)
{
	ଯଦି (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	ଅନ୍ୟଥା
		printf(" kernel");
}

ସ୍ଥିର ଶୂନ୍ୟ printparam(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଶୂନ୍ୟ *data, size_t size)
{
	printf(" %s=", name);
	ବାଛ (size) {
		ସ୍ଥିତି ଆକାର(ଅକ୍ଷର):	printf("%d", *(ଅକ୍ଷର *) data);	ଭାଙ୍ଗ;
		ସ୍ଥିତି ଆକାର(short):	printf("%d", *(short *) data);	ଭାଙ୍ଗ;
		ସ୍ଥିତି ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା):	printf("%d", *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *) data);	ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:		printf("(%u bytes)", size);	ଭାଙ୍ଗ;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା namematch(ଅକ୍ଷର **names, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nnames, ଅକ୍ଷର *name)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପାଇଁ(i = 0; i < nnames; i++)
		ଯଦି(!strcmp(names[i], name))
			ଫେରନ୍ତୁ 1;
	ଫେରନ୍ତୁ 0;
}
#endif

ଶୂନ୍ୟ printmsg(message *ସନ୍ଦେଶ, ସଂରଚନା proc *src, ସଂରଚନା proc *dst,
	ଅକ୍ଷର operation, ପୂର୍ଣ୍ଣସଂଖ୍ୟା printparams)
{
	ଅଚଳ ଅକ୍ଷର *name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mtype = ସନ୍ଦେଶ->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	ଅକ୍ଷର *names[] = DEBUG_DUMPIPC_NAMES;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nnames = ଆକାର(names)/ଆକାର(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	ଯଦି(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		ଫେରନ୍ତୁ;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	ଯଦି (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} ଅନ୍ୟଥା {
		printf(" %d/0x%x", mtype, mtype);
	}

	ଯଦି (mightbecall && printparams) {
#define IDENT(x, y) ଯଦି (mtype == x) printparam(#y, &ସନ୍ଦେଶ->y, ଆକାର(ସନ୍ଦେଶ->y));
#ଅନ୍ତର୍ଭାଵ "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#ଯଦି DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
ସ୍ଥିର ସଂରଚନା {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା src, dst, messages;
} winners[PRINTSLOTS];
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା total, goodslots;

ସ୍ଥିର ଶୂନ୍ୟ printstats(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ticks)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପାଇଁ(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		ଅକ୍ଷର	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

ସ୍ଥିର ଶୂନ୍ୟ sortstats(ଶୂନ୍ୟ)
{
	/* Print top message senders/receivers. */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା src_slot, dst_slot;
	total = goodslots = 0;
	ପାଇଁ(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		ପାଇଁ(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			ଯେତେବେଳେ(w > 0 && n > winners[w-1].messages)
				w--;
			ଯଦି(w >= PRINTSLOTS) ଜାରି;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			ପ୍ରତିଜ୍ଞା(rem >= 0);
			ପ୍ରତିଜ୍ଞା(rem < PRINTSLOTS);
			ଯଦି(rem > 0) {
				ପ୍ରତିଜ୍ଞା(w+1 <= PRINTSLOTS-1);
				ପ୍ରତିଜ୍ଞା(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*ଆକାର(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			ଯଦି(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	ଯଦି(p) { s = p->p_nr; } \
	ଅନ୍ୟଥା { s = KERNELIPC; } \
	ପ୍ରତିଜ୍ଞା(s >= 0 && s < IPCPROCS); \
}

ସ୍ଥିର ଶୂନ୍ୟ statmsg(message *ସନ୍ଦେଶ, ସଂରଚନା proc *srcp, ସଂରଚନା proc *dstp)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା src, dst, now, secs, dt;
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା lastprint;

	/* Stat message. */
	ପ୍ରତିଜ୍ଞା(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	ଯଦି(secs >= 30) {
		memset(winners, 0, ଆକାର(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, ଆକାର(messages));
		lastprint = now;
	}
}
#endif

#ଯଦି DEBUG_IPC_HOOK
ଶୂନ୍ୟ hook_ipc_msgkcall(message *ସନ୍ଦେଶ, ସଂରଚନା proc *proc)
{
#ଯଦି DEBUG_DUMPIPC
	printmsg(ସନ୍ଦେଶ, proc, ଖାଲି, 'k', 1);
#endif
}

ଶୂନ୍ୟ hook_ipc_msgkresult(message *ସନ୍ଦେଶ, ସଂରଚନା proc *proc)
{
#ଯଦି DEBUG_DUMPIPC
	printmsg(ସନ୍ଦେଶ, ଖାଲି, proc, 'k', 0);
#endif
#ଯଦି DEBUG_IPCSTATS
	statmsg(ସନ୍ଦେଶ, proc, ଖାଲି);
#endif
}

ଶୂନ୍ୟ hook_ipc_msgrecv(message *ସନ୍ଦେଶ, ସଂରଚନା proc *src, ସଂରଚନା proc *dst)
{
#ଯଦି DEBUG_DUMPIPC
	printmsg(ସନ୍ଦେଶ, src, dst, 'r', 0);
#endif
#ଯଦି DEBUG_IPCSTATS
	statmsg(ସନ୍ଦେଶ, src, dst);
#endif
}

ଶୂନ୍ୟ hook_ipc_msgsend(message *ସନ୍ଦେଶ, ସଂରଚନା proc *src, ସଂରଚନା proc *dst)
{
#ଯଦି DEBUG_DUMPIPC
	printmsg(ସନ୍ଦେଶ, src, dst, 's', 1);
#endif
}

ଶୂନ୍ୟ hook_ipc_clear(ସଂରଚନା proc *p)
{
#ଯଦି DEBUG_IPCSTATS
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା slot, i;
	ପ୍ରତିଜ୍ଞା(p);
	proc2slot(p, slot);
	ପାଇଁ(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
