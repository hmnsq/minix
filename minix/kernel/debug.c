/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#અન્તર્ભાવ "kernel/kernel.h"

#અન્તર્ભાવ <minix/callnr.h>
#અન્તર્ભાવ <minix/u64.h>
#અન્તર્ભાવ <limits.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

પૂર્ણાંક runqueues_ok_cpu(unsigned cpu)
{
  પૂર્ણાંક q, l = 0;
  નોંધણી સંરચના proc *xp;
  સંરચના proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  માટે (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	જો (l++ > MAX_LOOP) panic("check error");
  }

  માટે (q=l=0; q < NR_SCHED_QUEUES; q++) {
    જો (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	પાછા 0;
    }
    જો (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	પાછા 0;
    }
    જો (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	પાછા 0;
    }
    માટે(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	અચલ vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	જો(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		પાછા 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	જો(dxp % કદ(સંરચના proc)) {
  		printf("xp not a real pointer");
		પાછા 0;
	}
	જો(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		પાછા 0;
	}
	જો (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		પાછા 0;
	}
        જો (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		પાછા 0;
        }
        જો (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		પાછા 0;
	}
	જો (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		પાછા 0;
	}
	xp->p_found = 1;
	જો (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		પાછા 0;
	}
	જો (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		પાછા 0;
	}
    }
  }	

  માટે (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	જો(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		પાછા 0;
	}
	જો (isemptyp(xp))
		ચાલુ;
	જો(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		પાછા 0;
	}
  }

  /* All is ok. */
  પાછા 1;
}

#ifdef CONFIG_SMP
સ્થિર પૂર્ણાંક runqueues_ok_all(શૂન્ય)
{
	unsigned c;

	માટે (c = 0 ; c < ncpus; c++) {
		જો (!runqueues_ok_cpu(c))
			પાછા 0;
	}
	પાછા 1;	
}

પૂર્ણાંક runqueues_ok(શૂન્ય)
{
	પાછા runqueues_ok_all();
}

#નહીંતર

પૂર્ણાંક runqueues_ok(શૂન્ય)
{
	પાછા runqueues_ok_cpu(0);
}


#endif

અક્ષર *
rtsflagstr(અચલ u32_t flags)
{
	સ્થિર અક્ષર str[100];
	str[0] = '\0';

#define FLAG(n) જો(flags & n) { strlcat(str, #n " ", કદ(str)); }

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

	પાછા str;
}

અક્ષર *
miscflagstr(અચલ u32_t flags)
{
	સ્થિર અક્ષર str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	પાછા str;
}

અક્ષર *
schedulerstr(સંરચના proc *scheduler)
{
	જો (scheduler != ખાલી)
	{
		પાછા scheduler->p_name;
	}

	પાછા "KERNEL";
}

સ્થિર શૂન્ય
print_proc_name(સંરચના proc *pp)
{
	અક્ષર *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	જો(name) {
		printf("%s(%d)", name, ep);
	}
	નહીંતર {
		printf("%d", ep);
	}
}

સ્થિર શૂન્ય
print_endpoint(endpoint_t ep)
{
	પૂર્ણાંક proc_nr;
	સંરચના proc *pp = ખાલી;

	પસંદગી(ep) {
	સ્થિતિ ANY:
		printf("ANY");
	તોડો;
	સ્થિતિ SELF:
		printf("SELF");
	તોડો;
	સ્થિતિ NONE:
		printf("NONE");
	તોડો;
	મૂળભૂત:
		જો(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		નહીંતર {
			pp = proc_addr(proc_nr);
			જો(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			નહીંતર {
				print_proc_name(pp);
			}
		}
	તોડો;
	}
}

સ્થિર શૂન્ય
print_sigmgr(સંરચના proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	જો(sig_mgr == NONE) { printf("no sigmgr"); પાછા; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	જો(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

શૂન્ય print_proc(સંરચના proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#જો defined(__i386__)
		pp->p_seg.p_cr3,
#અથવાજો defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	જો(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

સ્થિર શૂન્ય print_proc_depends(સંરચના proc *pp, અચલ પૂર્ણાંક level)
{
	સંરચના proc *depproc = ખાલી;
	endpoint_t dep;
#define COL { પૂર્ણાંક i; માટે(i = 0; i < level; i++) printf("> "); }

	જો(level >= NR_PROCS) {
		printf("loop??\n");
		પાછા;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	જો(dep != NONE && dep != ANY) {
		પૂર્ણાંક procno;
		જો(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			જો(isemptyp(depproc))
				depproc = ખાલી;
		}
		જો (depproc)
			print_proc_depends(depproc, level+1);
	}
}

શૂન્ય print_proc_recursive(સંરચના proc *pp)
{
	print_proc_depends(pp, 0);
}

#જો DEBUG_DUMPIPC || DEBUG_DUMPIPCF
સ્થિર અચલ અક્ષર *mtypename(પૂર્ણાંક mtype, પૂર્ણાંક *possible_callname)
{
	અક્ષર *callname = ખાલી, *errname = ખાલી;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	પસંદગી(mtype) {
#define IDENT(x) સ્થિતિ x: callname = #x; *possible_callname = 1; તોડો;
#અન્તર્ભાવ "kernel/extracted-mtype.h"
#undef IDENT
	}
	પસંદગી(mtype) {
#define IDENT(x) સ્થિતિ x: errname = #x; તોડો;
#અન્તર્ભાવ "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	જો(!errname && !callname)
		પાછા ખાલી;

	/* 2 matches */
	જો(errname && callname) {
		સ્થિર અક્ષર typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		પાછા typename;
	}

	જો(errname) પાછા errname;

	પ્રતિજ્ઞા(callname);
	પાછા callname;
}

સ્થિર શૂન્ય printproc(સંરચના proc *rp)
{
	જો (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	નહીંતર
		printf(" kernel");
}

સ્થિર શૂન્ય printparam(અચલ અક્ષર *name, અચલ શૂન્ય *data, size_t size)
{
	printf(" %s=", name);
	પસંદગી (size) {
		સ્થિતિ કદ(અક્ષર):	printf("%d", *(અક્ષર *) data);	તોડો;
		સ્થિતિ કદ(short):	printf("%d", *(short *) data);	તોડો;
		સ્થિતિ કદ(પૂર્ણાંક):	printf("%d", *(પૂર્ણાંક *) data);	તોડો;
		મૂળભૂત:		printf("(%u bytes)", size);	તોડો;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
સ્થિર પૂર્ણાંક namematch(અક્ષર **names, પૂર્ણાંક nnames, અક્ષર *name)
{
	પૂર્ણાંક i;
	માટે(i = 0; i < nnames; i++)
		જો(!strcmp(names[i], name))
			પાછા 1;
	પાછા 0;
}
#endif

શૂન્ય printmsg(message *સંદેશ, સંરચના proc *src, સંરચના proc *dst,
	અક્ષર operation, પૂર્ણાંક printparams)
{
	અચલ અક્ષર *name;
	પૂર્ણાંક mtype = સંદેશ->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	અક્ષર *names[] = DEBUG_DUMPIPC_NAMES;
	પૂર્ણાંક nnames = કદ(names)/કદ(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	જો(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		પાછા;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	જો (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} નહીંતર {
		printf(" %d/0x%x", mtype, mtype);
	}

	જો (mightbecall && printparams) {
#define IDENT(x, y) જો (mtype == x) printparam(#y, &સંદેશ->y, કદ(સંદેશ->y));
#અન્તર્ભાવ "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#જો DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
સ્થિર પૂર્ણાંક messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
સ્થિર સંરચના {
	પૂર્ણાંક src, dst, messages;
} winners[PRINTSLOTS];
સ્થિર પૂર્ણાંક total, goodslots;

સ્થિર શૂન્ય printstats(પૂર્ણાંક ticks)
{
	પૂર્ણાંક i;
	માટે(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		અક્ષર	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

સ્થિર શૂન્ય sortstats(શૂન્ય)
{
	/* Print top message senders/receivers. */
	પૂર્ણાંક src_slot, dst_slot;
	total = goodslots = 0;
	માટે(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		માટે(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			પૂર્ણાંક w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			જ્યારે(w > 0 && n > winners[w-1].messages)
				w--;
			જો(w >= PRINTSLOTS) ચાલુ;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			પ્રતિજ્ઞા(rem >= 0);
			પ્રતિજ્ઞા(rem < PRINTSLOTS);
			જો(rem > 0) {
				પ્રતિજ્ઞા(w+1 <= PRINTSLOTS-1);
				પ્રતિજ્ઞા(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*કદ(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			જો(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	જો(p) { s = p->p_nr; } \
	નહીંતર { s = KERNELIPC; } \
	પ્રતિજ્ઞા(s >= 0 && s < IPCPROCS); \
}

સ્થિર શૂન્ય statmsg(message *સંદેશ, સંરચના proc *srcp, સંરચના proc *dstp)
{
	પૂર્ણાંક src, dst, now, secs, dt;
	સ્થિર પૂર્ણાંક lastprint;

	/* Stat message. */
	પ્રતિજ્ઞા(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	જો(secs >= 30) {
		memset(winners, 0, કદ(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, કદ(messages));
		lastprint = now;
	}
}
#endif

#જો DEBUG_IPC_HOOK
શૂન્ય hook_ipc_msgkcall(message *સંદેશ, સંરચના proc *proc)
{
#જો DEBUG_DUMPIPC
	printmsg(સંદેશ, proc, ખાલી, 'k', 1);
#endif
}

શૂન્ય hook_ipc_msgkresult(message *સંદેશ, સંરચના proc *proc)
{
#જો DEBUG_DUMPIPC
	printmsg(સંદેશ, ખાલી, proc, 'k', 0);
#endif
#જો DEBUG_IPCSTATS
	statmsg(સંદેશ, proc, ખાલી);
#endif
}

શૂન્ય hook_ipc_msgrecv(message *સંદેશ, સંરચના proc *src, સંરચના proc *dst)
{
#જો DEBUG_DUMPIPC
	printmsg(સંદેશ, src, dst, 'r', 0);
#endif
#જો DEBUG_IPCSTATS
	statmsg(સંદેશ, src, dst);
#endif
}

શૂન્ય hook_ipc_msgsend(message *સંદેશ, સંરચના proc *src, સંરચના proc *dst)
{
#જો DEBUG_DUMPIPC
	printmsg(સંદેશ, src, dst, 's', 1);
#endif
}

શૂન્ય hook_ipc_clear(સંરચના proc *p)
{
#જો DEBUG_IPCSTATS
	પૂર્ણાંક slot, i;
	પ્રતિજ્ઞા(p);
	proc2slot(p, slot);
	માટે(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
