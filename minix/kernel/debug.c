/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಅನ್ತರ್ಭಾವ <minix/callnr.h>
#ಅನ್ತರ್ಭಾವ <minix/u64.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

ಪೂರ್ಣಾಙ್ಕ runqueues_ok_cpu(unsigned cpu)
{
  ಪೂರ್ಣಾಙ್ಕ q, l = 0;
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *xp;
  ಸಂರಚನಾ proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  ಪ್ರತಿ (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	ಯದಿ (l++ > MAX_LOOP) panic("check error");
  }

  ಪ್ರತಿ (q=l=0; q < NR_SCHED_QUEUES; q++) {
    ಯದಿ (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತಿ(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	ಧ್ರುವ vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	ಯದಿ(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	ಯದಿ(dxp % ಪರಿಮಾಣ(ಸಂರಚನಾ proc)) {
  		printf("xp not a real pointer");
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಯದಿ(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಯದಿ (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ಪ್ರತ್ಯಾಗಮ 0;
	}
        ಯದಿ (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಯದಿ (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	xp->p_found = 1;
	ಯದಿ (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಯದಿ (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		ಪ್ರತ್ಯಾಗಮ 0;
	}
    }
  }	

  ಪ್ರತಿ (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	ಯದಿ(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಯದಿ (isemptyp(xp))
		ಅನುವರ್ತ;
	ಯದಿ(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		ಪ್ರತ್ಯಾಗಮ 0;
	}
  }

  /* All is ok. */
  ಪ್ರತ್ಯಾಗಮ 1;
}

#ifdef CONFIG_SMP
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ runqueues_ok_all(ಶೂನ್ಯ)
{
	unsigned c;

	ಪ್ರತಿ (c = 0 ; c < ncpus; c++) {
		ಯದಿ (!runqueues_ok_cpu(c))
			ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಪ್ರತ್ಯಾಗಮ 1;	
}

ಪೂರ್ಣಾಙ್ಕ runqueues_ok(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ runqueues_ok_all();
}

#ಅನ್ಯಥಾ

ಪೂರ್ಣಾಙ್ಕ runqueues_ok(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ runqueues_ok_cpu(0);
}


#endif

ವರ್ಣ *
rtsflagstr(ಧ್ರುವ u32_t flags)
{
	ಸ್ಥಿರ ವರ್ಣ str[100];
	str[0] = '\0';

#define FLAG(n) ಯದಿ(flags & n) { strlcat(str, #n " ", ಪರಿಮಾಣ(str)); }

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

	ಪ್ರತ್ಯಾಗಮ str;
}

ವರ್ಣ *
miscflagstr(ಧ್ರುವ u32_t flags)
{
	ಸ್ಥಿರ ವರ್ಣ str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ಪ್ರತ್ಯಾಗಮ str;
}

ವರ್ಣ *
schedulerstr(ಸಂರಚನಾ proc *scheduler)
{
	ಯದಿ (scheduler != ರಿಕ್ತ)
	{
		ಪ್ರತ್ಯಾಗಮ scheduler->p_name;
	}

	ಪ್ರತ್ಯಾಗಮ "KERNEL";
}

ಸ್ಥಿರ ಶೂನ್ಯ
print_proc_name(ಸಂರಚನಾ proc *pp)
{
	ವರ್ಣ *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	ಯದಿ(name) {
		printf("%s(%d)", name, ep);
	}
	ಅನ್ಯಥಾ {
		printf("%d", ep);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ
print_endpoint(endpoint_t ep)
{
	ಪೂರ್ಣಾಙ್ಕ proc_nr;
	ಸಂರಚನಾ proc *pp = ರಿಕ್ತ;

	ವಿಕಲ್ಪ(ep) {
	ಪಕ್ಷ ANY:
		printf("ANY");
	ಭಙ್ಗ;
	ಪಕ್ಷ SELF:
		printf("SELF");
	ಭಙ್ಗ;
	ಪಕ್ಷ NONE:
		printf("NONE");
	ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಯದಿ(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		ಅನ್ಯಥಾ {
			pp = proc_addr(proc_nr);
			ಯದಿ(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			ಅನ್ಯಥಾ {
				print_proc_name(pp);
			}
		}
	ಭಙ್ಗ;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ
print_sigmgr(ಸಂರಚನಾ proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	ಯದಿ(sig_mgr == NONE) { printf("no sigmgr"); ಪ್ರತ್ಯಾಗಮ; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	ಯದಿ(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

ಶೂನ್ಯ print_proc(ಸಂರಚನಾ proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#ಯದಿ defined(__i386__)
		pp->p_seg.p_cr3,
#ನೋಚೇದ್ಯದಿ defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	ಯದಿ(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ print_proc_depends(ಸಂರಚನಾ proc *pp, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ level)
{
	ಸಂರಚನಾ proc *depproc = ರಿಕ್ತ;
	endpoint_t dep;
#define COL { ಪೂರ್ಣಾಙ್ಕ i; ಪ್ರತಿ(i = 0; i < level; i++) printf("> "); }

	ಯದಿ(level >= NR_PROCS) {
		printf("loop??\n");
		ಪ್ರತ್ಯಾಗಮ;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	ಯದಿ(dep != NONE && dep != ANY) {
		ಪೂರ್ಣಾಙ್ಕ procno;
		ಯದಿ(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			ಯದಿ(isemptyp(depproc))
				depproc = ರಿಕ್ತ;
		}
		ಯದಿ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

ಶೂನ್ಯ print_proc_recursive(ಸಂರಚನಾ proc *pp)
{
	print_proc_depends(pp, 0);
}

#ಯದಿ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *mtypename(ಪೂರ್ಣಾಙ್ಕ mtype, ಪೂರ್ಣಾಙ್ಕ *possible_callname)
{
	ವರ್ಣ *callname = ರಿಕ್ತ, *errname = ರಿಕ್ತ;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	ವಿಕಲ್ಪ(mtype) {
#define IDENT(x) ಪಕ್ಷ x: callname = #x; *possible_callname = 1; ಭಙ್ಗ;
#ಅನ್ತರ್ಭಾವ "kernel/extracted-mtype.h"
#undef IDENT
	}
	ವಿಕಲ್ಪ(mtype) {
#define IDENT(x) ಪಕ್ಷ x: errname = #x; ಭಙ್ಗ;
#ಅನ್ತರ್ಭಾವ "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	ಯದಿ(!errname && !callname)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	/* 2 matches */
	ಯದಿ(errname && callname) {
		ಸ್ಥಿರ ವರ್ಣ typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ಪ್ರತ್ಯಾಗಮ typename;
	}

	ಯದಿ(errname) ಪ್ರತ್ಯಾಗಮ errname;

	ಪ್ರತಿಜ್ಞಾ(callname);
	ಪ್ರತ್ಯಾಗಮ callname;
}

ಸ್ಥಿರ ಶೂನ್ಯ printproc(ಸಂರಚನಾ proc *rp)
{
	ಯದಿ (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	ಅನ್ಯಥಾ
		printf(" kernel");
}

ಸ್ಥಿರ ಶೂನ್ಯ printparam(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ಶೂನ್ಯ *data, size_t size)
{
	printf(" %s=", name);
	ವಿಕಲ್ಪ (size) {
		ಪಕ್ಷ ಪರಿಮಾಣ(ವರ್ಣ):	printf("%d", *(ವರ್ಣ *) data);	ಭಙ್ಗ;
		ಪಕ್ಷ ಪರಿಮಾಣ(short):	printf("%d", *(short *) data);	ಭಙ್ಗ;
		ಪಕ್ಷ ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ):	printf("%d", *(ಪೂರ್ಣಾಙ್ಕ *) data);	ಭಙ್ಗ;
		ಸ್ವಭಾವ:		printf("(%u bytes)", size);	ಭಙ್ಗ;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ namematch(ವರ್ಣ **names, ಪೂರ್ಣಾಙ್ಕ nnames, ವರ್ಣ *name)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪ್ರತಿ(i = 0; i < nnames; i++)
		ಯದಿ(!strcmp(names[i], name))
			ಪ್ರತ್ಯಾಗಮ 1;
	ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

ಶೂನ್ಯ printmsg(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *src, ಸಂರಚನಾ proc *dst,
	ವರ್ಣ operation, ಪೂರ್ಣಾಙ್ಕ printparams)
{
	ಧ್ರುವ ವರ್ಣ *name;
	ಪೂರ್ಣಾಙ್ಕ mtype = ಸಂದೇಶ->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	ವರ್ಣ *names[] = DEBUG_DUMPIPC_NAMES;
	ಪೂರ್ಣಾಙ್ಕ nnames = ಪರಿಮಾಣ(names)/ಪರಿಮಾಣ(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	ಯದಿ(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		ಪ್ರತ್ಯಾಗಮ;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	ಯದಿ (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} ಅನ್ಯಥಾ {
		printf(" %d/0x%x", mtype, mtype);
	}

	ಯದಿ (mightbecall && printparams) {
#define IDENT(x, y) ಯದಿ (mtype == x) printparam(#y, &ಸಂದೇಶ->y, ಪರಿಮಾಣ(ಸಂದೇಶ->y));
#ಅನ್ತರ್ಭಾವ "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#ಯದಿ DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
ಸ್ಥಿರ ಸಂರಚನಾ {
	ಪೂರ್ಣಾಙ್ಕ src, dst, messages;
} winners[PRINTSLOTS];
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ total, goodslots;

ಸ್ಥಿರ ಶೂನ್ಯ printstats(ಪೂರ್ಣಾಙ್ಕ ticks)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪ್ರತಿ(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		ವರ್ಣ	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

ಸ್ಥಿರ ಶೂನ್ಯ sortstats(ಶೂನ್ಯ)
{
	/* Print top message senders/receivers. */
	ಪೂರ್ಣಾಙ್ಕ src_slot, dst_slot;
	total = goodslots = 0;
	ಪ್ರತಿ(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		ಪ್ರತಿ(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			ಪೂರ್ಣಾಙ್ಕ w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			ಯಾವತ್(w > 0 && n > winners[w-1].messages)
				w--;
			ಯದಿ(w >= PRINTSLOTS) ಅನುವರ್ತ;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			ಪ್ರತಿಜ್ಞಾ(rem >= 0);
			ಪ್ರತಿಜ್ಞಾ(rem < PRINTSLOTS);
			ಯದಿ(rem > 0) {
				ಪ್ರತಿಜ್ಞಾ(w+1 <= PRINTSLOTS-1);
				ಪ್ರತಿಜ್ಞಾ(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*ಪರಿಮಾಣ(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			ಯದಿ(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	ಯದಿ(p) { s = p->p_nr; } \
	ಅನ್ಯಥಾ { s = KERNELIPC; } \
	ಪ್ರತಿಜ್ಞಾ(s >= 0 && s < IPCPROCS); \
}

ಸ್ಥಿರ ಶೂನ್ಯ statmsg(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *srcp, ಸಂರಚನಾ proc *dstp)
{
	ಪೂರ್ಣಾಙ್ಕ src, dst, now, secs, dt;
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lastprint;

	/* Stat message. */
	ಪ್ರತಿಜ್ಞಾ(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	ಯದಿ(secs >= 30) {
		memset(winners, 0, ಪರಿಮಾಣ(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, ಪರಿಮಾಣ(messages));
		lastprint = now;
	}
}
#endif

#ಯದಿ DEBUG_IPC_HOOK
ಶೂನ್ಯ hook_ipc_msgkcall(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *proc)
{
#ಯದಿ DEBUG_DUMPIPC
	printmsg(ಸಂದೇಶ, proc, ರಿಕ್ತ, 'k', 1);
#endif
}

ಶೂನ್ಯ hook_ipc_msgkresult(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *proc)
{
#ಯದಿ DEBUG_DUMPIPC
	printmsg(ಸಂದೇಶ, ರಿಕ್ತ, proc, 'k', 0);
#endif
#ಯದಿ DEBUG_IPCSTATS
	statmsg(ಸಂದೇಶ, proc, ರಿಕ್ತ);
#endif
}

ಶೂನ್ಯ hook_ipc_msgrecv(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *src, ಸಂರಚನಾ proc *dst)
{
#ಯದಿ DEBUG_DUMPIPC
	printmsg(ಸಂದೇಶ, src, dst, 'r', 0);
#endif
#ಯದಿ DEBUG_IPCSTATS
	statmsg(ಸಂದೇಶ, src, dst);
#endif
}

ಶೂನ್ಯ hook_ipc_msgsend(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *src, ಸಂರಚನಾ proc *dst)
{
#ಯದಿ DEBUG_DUMPIPC
	printmsg(ಸಂದೇಶ, src, dst, 's', 1);
#endif
}

ಶೂನ್ಯ hook_ipc_clear(ಸಂರಚನಾ proc *p)
{
#ಯದಿ DEBUG_IPCSTATS
	ಪೂರ್ಣಾಙ್ಕ slot, i;
	ಪ್ರತಿಜ್ಞಾ(p);
	proc2slot(p, slot);
	ಪ್ರತಿ(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
