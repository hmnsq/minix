/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#அந்தர்பாவ "kernel/kernel.h"

#அந்தர்பாவ <minix/callnr.h>
#அந்தர்பாவ <minix/u64.h>
#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

முழு runqueues_ok_cpu(unsigned cpu)
{
  முழு q, l = 0;
  பதிவு கட்டமைப்பு proc *xp;
  கட்டமைப்பு proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  க்கு (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	எனில் (l++ > MAX_LOOP) panic("check error");
  }

  க்கு (q=l=0; q < NR_SCHED_QUEUES; q++) {
    எனில் (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	திரும்பு 0;
    }
    எனில் (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	திரும்பு 0;
    }
    எனில் (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	திரும்பு 0;
    }
    க்கு(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	மாறிலி vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	எனில்(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		திரும்பு 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	எனில்(dxp % அளவு(கட்டமைப்பு proc)) {
  		printf("xp not a real pointer");
		திரும்பு 0;
	}
	எனில்(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		திரும்பு 0;
	}
	எனில் (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		திரும்பு 0;
	}
        எனில் (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		திரும்பு 0;
        }
        எனில் (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		திரும்பு 0;
	}
	எனில் (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		திரும்பு 0;
	}
	xp->p_found = 1;
	எனில் (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		திரும்பு 0;
	}
	எனில் (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		திரும்பு 0;
	}
    }
  }	

  க்கு (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	எனில்(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		திரும்பு 0;
	}
	எனில் (isemptyp(xp))
		தொடர்;
	எனில்(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		திரும்பு 0;
	}
  }

  /* All is ok. */
  திரும்பு 1;
}

#ifdef CONFIG_SMP
நிலையான முழு runqueues_ok_all(வெறுமை)
{
	unsigned c;

	க்கு (c = 0 ; c < ncpus; c++) {
		எனில் (!runqueues_ok_cpu(c))
			திரும்பு 0;
	}
	திரும்பு 1;	
}

முழு runqueues_ok(வெறுமை)
{
	திரும்பு runqueues_ok_all();
}

#இல்லையெனில்

முழு runqueues_ok(வெறுமை)
{
	திரும்பு runqueues_ok_cpu(0);
}


#endif

எழுத்து *
rtsflagstr(மாறிலி u32_t flags)
{
	நிலையான எழுத்து str[100];
	str[0] = '\0';

#define FLAG(n) எனில்(flags & n) { strlcat(str, #n " ", அளவு(str)); }

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

	திரும்பு str;
}

எழுத்து *
miscflagstr(மாறிலி u32_t flags)
{
	நிலையான எழுத்து str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	திரும்பு str;
}

எழுத்து *
schedulerstr(கட்டமைப்பு proc *scheduler)
{
	எனில் (scheduler != வெற்று)
	{
		திரும்பு scheduler->p_name;
	}

	திரும்பு "KERNEL";
}

நிலையான வெறுமை
print_proc_name(கட்டமைப்பு proc *pp)
{
	எழுத்து *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	எனில்(name) {
		printf("%s(%d)", name, ep);
	}
	இல்லையெனில் {
		printf("%d", ep);
	}
}

நிலையான வெறுமை
print_endpoint(endpoint_t ep)
{
	முழு proc_nr;
	கட்டமைப்பு proc *pp = வெற்று;

	தேர்வு(ep) {
	நிலை ANY:
		printf("ANY");
	நிறுத்து;
	நிலை SELF:
		printf("SELF");
	நிறுத்து;
	நிலை NONE:
		printf("NONE");
	நிறுத்து;
	இயல்பு:
		எனில்(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		இல்லையெனில் {
			pp = proc_addr(proc_nr);
			எனில்(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			இல்லையெனில் {
				print_proc_name(pp);
			}
		}
	நிறுத்து;
	}
}

நிலையான வெறுமை
print_sigmgr(கட்டமைப்பு proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	எனில்(sig_mgr == NONE) { printf("no sigmgr"); திரும்பு; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	எனில்(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

வெறுமை print_proc(கட்டமைப்பு proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#எனில் defined(__i386__)
		pp->p_seg.p_cr3,
#அல்லதெனில் defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	எனில்(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

நிலையான வெறுமை print_proc_depends(கட்டமைப்பு proc *pp, மாறிலி முழு level)
{
	கட்டமைப்பு proc *depproc = வெற்று;
	endpoint_t dep;
#define COL { முழு i; க்கு(i = 0; i < level; i++) printf("> "); }

	எனில்(level >= NR_PROCS) {
		printf("loop??\n");
		திரும்பு;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	எனில்(dep != NONE && dep != ANY) {
		முழு procno;
		எனில்(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			எனில்(isemptyp(depproc))
				depproc = வெற்று;
		}
		எனில் (depproc)
			print_proc_depends(depproc, level+1);
	}
}

வெறுமை print_proc_recursive(கட்டமைப்பு proc *pp)
{
	print_proc_depends(pp, 0);
}

#எனில் DEBUG_DUMPIPC || DEBUG_DUMPIPCF
நிலையான மாறிலி எழுத்து *mtypename(முழு mtype, முழு *possible_callname)
{
	எழுத்து *callname = வெற்று, *errname = வெற்று;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	தேர்வு(mtype) {
#define IDENT(x) நிலை x: callname = #x; *possible_callname = 1; நிறுத்து;
#அந்தர்பாவ "kernel/extracted-mtype.h"
#undef IDENT
	}
	தேர்வு(mtype) {
#define IDENT(x) நிலை x: errname = #x; நிறுத்து;
#அந்தர்பாவ "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	எனில்(!errname && !callname)
		திரும்பு வெற்று;

	/* 2 matches */
	எனில்(errname && callname) {
		நிலையான எழுத்து typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		திரும்பு typename;
	}

	எனில்(errname) திரும்பு errname;

	ப்ரதிஜ்ஞா(callname);
	திரும்பு callname;
}

நிலையான வெறுமை printproc(கட்டமைப்பு proc *rp)
{
	எனில் (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	இல்லையெனில்
		printf(" kernel");
}

நிலையான வெறுமை printparam(மாறிலி எழுத்து *name, மாறிலி வெறுமை *data, size_t size)
{
	printf(" %s=", name);
	தேர்வு (size) {
		நிலை அளவு(எழுத்து):	printf("%d", *(எழுத்து *) data);	நிறுத்து;
		நிலை அளவு(short):	printf("%d", *(short *) data);	நிறுத்து;
		நிலை அளவு(முழு):	printf("%d", *(முழு *) data);	நிறுத்து;
		இயல்பு:		printf("(%u bytes)", size);	நிறுத்து;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
நிலையான முழு namematch(எழுத்து **names, முழு nnames, எழுத்து *name)
{
	முழு i;
	க்கு(i = 0; i < nnames; i++)
		எனில்(!strcmp(names[i], name))
			திரும்பு 1;
	திரும்பு 0;
}
#endif

வெறுமை printmsg(message *செய்தி, கட்டமைப்பு proc *src, கட்டமைப்பு proc *dst,
	எழுத்து operation, முழு printparams)
{
	மாறிலி எழுத்து *name;
	முழு mtype = செய்தி->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	எழுத்து *names[] = DEBUG_DUMPIPC_NAMES;
	முழு nnames = அளவு(names)/அளவு(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	எனில்(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		திரும்பு;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	எனில் (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} இல்லையெனில் {
		printf(" %d/0x%x", mtype, mtype);
	}

	எனில் (mightbecall && printparams) {
#define IDENT(x, y) எனில் (mtype == x) printparam(#y, &செய்தி->y, அளவு(செய்தி->y));
#அந்தர்பாவ "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#எனில் DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
நிலையான முழு messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
நிலையான கட்டமைப்பு {
	முழு src, dst, messages;
} winners[PRINTSLOTS];
நிலையான முழு total, goodslots;

நிலையான வெறுமை printstats(முழு ticks)
{
	முழு i;
	க்கு(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		எழுத்து	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

நிலையான வெறுமை sortstats(வெறுமை)
{
	/* Print top message senders/receivers. */
	முழு src_slot, dst_slot;
	total = goodslots = 0;
	க்கு(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		க்கு(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			முழு w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			போது(w > 0 && n > winners[w-1].messages)
				w--;
			எனில்(w >= PRINTSLOTS) தொடர்;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			ப்ரதிஜ்ஞா(rem >= 0);
			ப்ரதிஜ்ஞா(rem < PRINTSLOTS);
			எனில்(rem > 0) {
				ப்ரதிஜ்ஞா(w+1 <= PRINTSLOTS-1);
				ப்ரதிஜ்ஞா(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*அளவு(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			எனில்(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	எனில்(p) { s = p->p_nr; } \
	இல்லையெனில் { s = KERNELIPC; } \
	ப்ரதிஜ்ஞா(s >= 0 && s < IPCPROCS); \
}

நிலையான வெறுமை statmsg(message *செய்தி, கட்டமைப்பு proc *srcp, கட்டமைப்பு proc *dstp)
{
	முழு src, dst, now, secs, dt;
	நிலையான முழு lastprint;

	/* Stat message. */
	ப்ரதிஜ்ஞா(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	எனில்(secs >= 30) {
		memset(winners, 0, அளவு(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, அளவு(messages));
		lastprint = now;
	}
}
#endif

#எனில் DEBUG_IPC_HOOK
வெறுமை hook_ipc_msgkcall(message *செய்தி, கட்டமைப்பு proc *proc)
{
#எனில் DEBUG_DUMPIPC
	printmsg(செய்தி, proc, வெற்று, 'k', 1);
#endif
}

வெறுமை hook_ipc_msgkresult(message *செய்தி, கட்டமைப்பு proc *proc)
{
#எனில் DEBUG_DUMPIPC
	printmsg(செய்தி, வெற்று, proc, 'k', 0);
#endif
#எனில் DEBUG_IPCSTATS
	statmsg(செய்தி, proc, வெற்று);
#endif
}

வெறுமை hook_ipc_msgrecv(message *செய்தி, கட்டமைப்பு proc *src, கட்டமைப்பு proc *dst)
{
#எனில் DEBUG_DUMPIPC
	printmsg(செய்தி, src, dst, 'r', 0);
#endif
#எனில் DEBUG_IPCSTATS
	statmsg(செய்தி, src, dst);
#endif
}

வெறுமை hook_ipc_msgsend(message *செய்தி, கட்டமைப்பு proc *src, கட்டமைப்பு proc *dst)
{
#எனில் DEBUG_DUMPIPC
	printmsg(செய்தி, src, dst, 's', 1);
#endif
}

வெறுமை hook_ipc_clear(கட்டமைப்பு proc *p)
{
#எனில் DEBUG_IPCSTATS
	முழு slot, i;
	ப்ரதிஜ்ஞா(p);
	proc2slot(p, slot);
	க்கு(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
