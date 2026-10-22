/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#ضمن "kernel/kernel.h"

#ضمن <minix/callnr.h>
#ضمن <minix/u64.h>
#ضمن <limits.h>
#ضمن <string.h>
#ضمن <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

صحيح runqueues_ok_cpu(unsigned cpu)
{
  صحيح q, l = 0;
  سجل بنية proc *xp;
  بنية proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  لكل (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	إذا (l++ > MAX_LOOP) panic("check error");
  }

  لكل (q=l=0; q < NR_SCHED_QUEUES; q++) {
    إذا (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	أرجع 0;
    }
    إذا (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	أرجع 0;
    }
    إذا (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	أرجع 0;
    }
    لكل(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	ثابت vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	إذا(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		أرجع 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	إذا(dxp % حجم(بنية proc)) {
  		printf("xp not a real pointer");
		أرجع 0;
	}
	إذا(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		أرجع 0;
	}
	إذا (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		أرجع 0;
	}
        إذا (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
        }
        إذا (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		أرجع 0;
	}
	إذا (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	xp->p_found = 1;
	إذا (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	إذا (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		أرجع 0;
	}
    }
  }	

  لكل (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	إذا(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		أرجع 0;
	}
	إذا (isemptyp(xp))
		استمر;
	إذا(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		أرجع 0;
	}
  }

  /* All is ok. */
  أرجع 1;
}

#ifdef CONFIG_SMP
ساكن صحيح runqueues_ok_all(فراغ)
{
	unsigned c;

	لكل (c = 0 ; c < ncpus; c++) {
		إذا (!runqueues_ok_cpu(c))
			أرجع 0;
	}
	أرجع 1;	
}

صحيح runqueues_ok(فراغ)
{
	أرجع runqueues_ok_all();
}

#وإلا

صحيح runqueues_ok(فراغ)
{
	أرجع runqueues_ok_cpu(0);
}


#endif

حرف *
rtsflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

#define FLAG(n) إذا(flags & n) { strlcat(str, #n " ", حجم(str)); }

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

	أرجع str;
}

حرف *
miscflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	أرجع str;
}

حرف *
schedulerstr(بنية proc *scheduler)
{
	إذا (scheduler != عدم)
	{
		أرجع scheduler->p_name;
	}

	أرجع "KERNEL";
}

ساكن فراغ
print_proc_name(بنية proc *pp)
{
	حرف *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	إذا(name) {
		printf("%s(%d)", name, ep);
	}
	وإلا {
		printf("%d", ep);
	}
}

ساكن فراغ
print_endpoint(endpoint_t ep)
{
	صحيح proc_nr;
	بنية proc *pp = عدم;

	بدل(ep) {
	حالة ANY:
		printf("ANY");
	اكسر;
	حالة SELF:
		printf("SELF");
	اكسر;
	حالة NONE:
		printf("NONE");
	اكسر;
	افتراضي:
		إذا(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		وإلا {
			pp = proc_addr(proc_nr);
			إذا(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			وإلا {
				print_proc_name(pp);
			}
		}
	اكسر;
	}
}

ساكن فراغ
print_sigmgr(بنية proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	إذا(sig_mgr == NONE) { printf("no sigmgr"); أرجع; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	إذا(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

فراغ print_proc(بنية proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#إذا defined(__i386__)
		pp->p_seg.p_cr3,
#وإلاإذا defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	إذا(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

ساكن فراغ print_proc_depends(بنية proc *pp, ثابت صحيح level)
{
	بنية proc *depproc = عدم;
	endpoint_t dep;
#define COL { صحيح i; لكل(i = 0; i < level; i++) printf("> "); }

	إذا(level >= NR_PROCS) {
		printf("loop??\n");
		أرجع;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	إذا(dep != NONE && dep != ANY) {
		صحيح procno;
		إذا(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			إذا(isemptyp(depproc))
				depproc = عدم;
		}
		إذا (depproc)
			print_proc_depends(depproc, level+1);
	}
}

فراغ print_proc_recursive(بنية proc *pp)
{
	print_proc_depends(pp, 0);
}

#إذا DEBUG_DUMPIPC || DEBUG_DUMPIPCF
ساكن ثابت حرف *mtypename(صحيح mtype, صحيح *possible_callname)
{
	حرف *callname = عدم, *errname = عدم;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	بدل(mtype) {
#define IDENT(x) حالة x: callname = #x; *possible_callname = 1; اكسر;
#ضمن "kernel/extracted-mtype.h"
#undef IDENT
	}
	بدل(mtype) {
#define IDENT(x) حالة x: errname = #x; اكسر;
#ضمن "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	إذا(!errname && !callname)
		أرجع عدم;

	/* 2 matches */
	إذا(errname && callname) {
		ساكن حرف typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		أرجع typename;
	}

	إذا(errname) أرجع errname;

	أكد(callname);
	أرجع callname;
}

ساكن فراغ printproc(بنية proc *rp)
{
	إذا (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	وإلا
		printf(" kernel");
}

ساكن فراغ printparam(ثابت حرف *name, ثابت فراغ *data, size_t size)
{
	printf(" %s=", name);
	بدل (size) {
		حالة حجم(حرف):	printf("%d", *(حرف *) data);	اكسر;
		حالة حجم(short):	printf("%d", *(short *) data);	اكسر;
		حالة حجم(صحيح):	printf("%d", *(صحيح *) data);	اكسر;
		افتراضي:		printf("(%u bytes)", size);	اكسر;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
ساكن صحيح namematch(حرف **names, صحيح nnames, حرف *name)
{
	صحيح i;
	لكل(i = 0; i < nnames; i++)
		إذا(!strcmp(names[i], name))
			أرجع 1;
	أرجع 0;
}
#endif

فراغ printmsg(message *ئۇچۇر, بنية proc *src, بنية proc *dst,
	حرف operation, صحيح printparams)
{
	ثابت حرف *name;
	صحيح mtype = ئۇچۇر->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	حرف *names[] = DEBUG_DUMPIPC_NAMES;
	صحيح nnames = حجم(names)/حجم(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	إذا(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		أرجع;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	إذا (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} وإلا {
		printf(" %d/0x%x", mtype, mtype);
	}

	إذا (mightbecall && printparams) {
#define IDENT(x, y) إذا (mtype == x) printparam(#y, &ئۇچۇر->y, حجم(ئۇچۇر->y));
#ضمن "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#إذا DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ساكن صحيح messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
ساكن بنية {
	صحيح src, dst, messages;
} winners[PRINTSLOTS];
ساكن صحيح total, goodslots;

ساكن فراغ printstats(صحيح ticks)
{
	صحيح i;
	لكل(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		حرف	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

ساكن فراغ sortstats(فراغ)
{
	/* Print top message senders/receivers. */
	صحيح src_slot, dst_slot;
	total = goodslots = 0;
	لكل(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		لكل(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			صحيح w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			طالما(w > 0 && n > winners[w-1].messages)
				w--;
			إذا(w >= PRINTSLOTS) استمر;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			أكد(rem >= 0);
			أكد(rem < PRINTSLOTS);
			إذا(rem > 0) {
				أكد(w+1 <= PRINTSLOTS-1);
				أكد(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*حجم(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			إذا(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	إذا(p) { s = p->p_nr; } \
	وإلا { s = KERNELIPC; } \
	أكد(s >= 0 && s < IPCPROCS); \
}

ساكن فراغ statmsg(message *ئۇچۇر, بنية proc *srcp, بنية proc *dstp)
{
	صحيح src, dst, now, secs, dt;
	ساكن صحيح lastprint;

	/* Stat message. */
	أكد(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	إذا(secs >= 30) {
		memset(winners, 0, حجم(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, حجم(messages));
		lastprint = now;
	}
}
#endif

#إذا DEBUG_IPC_HOOK
فراغ hook_ipc_msgkcall(message *ئۇچۇر, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(ئۇچۇر, proc, عدم, 'k', 1);
#endif
}

فراغ hook_ipc_msgkresult(message *ئۇچۇر, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(ئۇچۇر, عدم, proc, 'k', 0);
#endif
#إذا DEBUG_IPCSTATS
	statmsg(ئۇچۇر, proc, عدم);
#endif
}

فراغ hook_ipc_msgrecv(message *ئۇچۇر, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(ئۇچۇر, src, dst, 'r', 0);
#endif
#إذا DEBUG_IPCSTATS
	statmsg(ئۇچۇر, src, dst);
#endif
}

فراغ hook_ipc_msgsend(message *ئۇچۇر, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(ئۇچۇر, src, dst, 's', 1);
#endif
}

فراغ hook_ipc_clear(بنية proc *p)
{
#إذا DEBUG_IPCSTATS
	صحيح slot, i;
	أكد(p);
	proc2slot(p, slot);
	لكل(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
