/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#അന്തര്ഭാവ "kernel/kernel.h"

#അന്തര്ഭാവ <minix/callnr.h>
#അന്തര്ഭാവ <minix/u64.h>
#അന്തര്ഭാവ <limits.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

പൂർണ്ണസംഖ്യ runqueues_ok_cpu(unsigned cpu)
{
  പൂർണ്ണസംഖ്യ q, l = 0;
  രജിസ്റ്റർ ഘടന proc *xp;
  ഘടന proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  വേണ്ടി (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	എങ്കിൽ (l++ > MAX_LOOP) panic("check error");
  }

  വേണ്ടി (q=l=0; q < NR_SCHED_QUEUES; q++) {
    എങ്കിൽ (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	തിരികെ 0;
    }
    എങ്കിൽ (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	തിരികെ 0;
    }
    എങ്കിൽ (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	തിരികെ 0;
    }
    വേണ്ടി(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	സ്ഥിരം vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	എങ്കിൽ(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		തിരികെ 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	എങ്കിൽ(dxp % വലുപ്പം(ഘടന proc)) {
  		printf("xp not a real pointer");
		തിരികെ 0;
	}
	എങ്കിൽ(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		തിരികെ 0;
	}
	എങ്കിൽ (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		തിരികെ 0;
	}
        എങ്കിൽ (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		തിരികെ 0;
        }
        എങ്കിൽ (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		തിരികെ 0;
	}
	എങ്കിൽ (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		തിരികെ 0;
	}
	xp->p_found = 1;
	എങ്കിൽ (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		തിരികെ 0;
	}
	എങ്കിൽ (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		തിരികെ 0;
	}
    }
  }	

  വേണ്ടി (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	എങ്കിൽ(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		തിരികെ 0;
	}
	എങ്കിൽ (isemptyp(xp))
		തുടരുക;
	എങ്കിൽ(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		തിരികെ 0;
	}
  }

  /* All is ok. */
  തിരികെ 1;
}

#ifdef CONFIG_SMP
സ്ഥിതം പൂർണ്ണസംഖ്യ runqueues_ok_all(ശൂന്യം)
{
	unsigned c;

	വേണ്ടി (c = 0 ; c < ncpus; c++) {
		എങ്കിൽ (!runqueues_ok_cpu(c))
			തിരികെ 0;
	}
	തിരികെ 1;	
}

പൂർണ്ണസംഖ്യ runqueues_ok(ശൂന്യം)
{
	തിരികെ runqueues_ok_all();
}

#അല്ലെങ്കിൽ

പൂർണ്ണസംഖ്യ runqueues_ok(ശൂന്യം)
{
	തിരികെ runqueues_ok_cpu(0);
}


#endif

അക്ഷരം *
rtsflagstr(സ്ഥിരം u32_t flags)
{
	സ്ഥിതം അക്ഷരം str[100];
	str[0] = '\0';

#define FLAG(n) എങ്കിൽ(flags & n) { strlcat(str, #n " ", വലുപ്പം(str)); }

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

	തിരികെ str;
}

അക്ഷരം *
miscflagstr(സ്ഥിരം u32_t flags)
{
	സ്ഥിതം അക്ഷരം str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	തിരികെ str;
}

അക്ഷരം *
schedulerstr(ഘടന proc *scheduler)
{
	എങ്കിൽ (scheduler != ശൂന്യ)
	{
		തിരികെ scheduler->p_name;
	}

	തിരികെ "KERNEL";
}

സ്ഥിതം ശൂന്യം
print_proc_name(ഘടന proc *pp)
{
	അക്ഷരം *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	എങ്കിൽ(name) {
		printf("%s(%d)", name, ep);
	}
	അല്ലെങ്കിൽ {
		printf("%d", ep);
	}
}

സ്ഥിതം ശൂന്യം
print_endpoint(endpoint_t ep)
{
	പൂർണ്ണസംഖ്യ proc_nr;
	ഘടന proc *pp = ശൂന്യ;

	തിരഞ്ഞെടുപ്പ്(ep) {
	അവസ്ഥ ANY:
		printf("ANY");
	നിർത്തുക;
	അവസ്ഥ SELF:
		printf("SELF");
	നിർത്തുക;
	അവസ്ഥ NONE:
		printf("NONE");
	നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		എങ്കിൽ(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		അല്ലെങ്കിൽ {
			pp = proc_addr(proc_nr);
			എങ്കിൽ(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			അല്ലെങ്കിൽ {
				print_proc_name(pp);
			}
		}
	നിർത്തുക;
	}
}

സ്ഥിതം ശൂന്യം
print_sigmgr(ഘടന proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	എങ്കിൽ(sig_mgr == NONE) { printf("no sigmgr"); തിരികെ; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	എങ്കിൽ(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

ശൂന്യം print_proc(ഘടന proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#എങ്കിൽ defined(__i386__)
		pp->p_seg.p_cr3,
#അല്ലെങ്കിലെങ്കിൽ defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	എങ്കിൽ(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

സ്ഥിതം ശൂന്യം print_proc_depends(ഘടന proc *pp, സ്ഥിരം പൂർണ്ണസംഖ്യ level)
{
	ഘടന proc *depproc = ശൂന്യ;
	endpoint_t dep;
#define COL { പൂർണ്ണസംഖ്യ i; വേണ്ടി(i = 0; i < level; i++) printf("> "); }

	എങ്കിൽ(level >= NR_PROCS) {
		printf("loop??\n");
		തിരികെ;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	എങ്കിൽ(dep != NONE && dep != ANY) {
		പൂർണ്ണസംഖ്യ procno;
		എങ്കിൽ(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			എങ്കിൽ(isemptyp(depproc))
				depproc = ശൂന്യ;
		}
		എങ്കിൽ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

ശൂന്യം print_proc_recursive(ഘടന proc *pp)
{
	print_proc_depends(pp, 0);
}

#എങ്കിൽ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
സ്ഥിതം സ്ഥിരം അക്ഷരം *mtypename(പൂർണ്ണസംഖ്യ mtype, പൂർണ്ണസംഖ്യ *possible_callname)
{
	അക്ഷരം *callname = ശൂന്യ, *errname = ശൂന്യ;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	തിരഞ്ഞെടുപ്പ്(mtype) {
#define IDENT(x) അവസ്ഥ x: callname = #x; *possible_callname = 1; നിർത്തുക;
#അന്തര്ഭാവ "kernel/extracted-mtype.h"
#undef IDENT
	}
	തിരഞ്ഞെടുപ്പ്(mtype) {
#define IDENT(x) അവസ്ഥ x: errname = #x; നിർത്തുക;
#അന്തര്ഭാവ "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	എങ്കിൽ(!errname && !callname)
		തിരികെ ശൂന്യ;

	/* 2 matches */
	എങ്കിൽ(errname && callname) {
		സ്ഥിതം അക്ഷരം typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		തിരികെ typename;
	}

	എങ്കിൽ(errname) തിരികെ errname;

	പ്രതിജ്ഞാ(callname);
	തിരികെ callname;
}

സ്ഥിതം ശൂന്യം printproc(ഘടന proc *rp)
{
	എങ്കിൽ (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	അല്ലെങ്കിൽ
		printf(" kernel");
}

സ്ഥിതം ശൂന്യം printparam(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം ശൂന്യം *data, size_t size)
{
	printf(" %s=", name);
	തിരഞ്ഞെടുപ്പ് (size) {
		അവസ്ഥ വലുപ്പം(അക്ഷരം):	printf("%d", *(അക്ഷരം *) data);	നിർത്തുക;
		അവസ്ഥ വലുപ്പം(short):	printf("%d", *(short *) data);	നിർത്തുക;
		അവസ്ഥ വലുപ്പം(പൂർണ്ണസംഖ്യ):	printf("%d", *(പൂർണ്ണസംഖ്യ *) data);	നിർത്തുക;
		സ്ഥിരസ്ഥിതി:		printf("(%u bytes)", size);	നിർത്തുക;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
സ്ഥിതം പൂർണ്ണസംഖ്യ namematch(അക്ഷരം **names, പൂർണ്ണസംഖ്യ nnames, അക്ഷരം *name)
{
	പൂർണ്ണസംഖ്യ i;
	വേണ്ടി(i = 0; i < nnames; i++)
		എങ്കിൽ(!strcmp(names[i], name))
			തിരികെ 1;
	തിരികെ 0;
}
#endif

ശൂന്യം printmsg(message *സന്ദേശം, ഘടന proc *src, ഘടന proc *dst,
	അക്ഷരം operation, പൂർണ്ണസംഖ്യ printparams)
{
	സ്ഥിരം അക്ഷരം *name;
	പൂർണ്ണസംഖ്യ mtype = സന്ദേശം->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	അക്ഷരം *names[] = DEBUG_DUMPIPC_NAMES;
	പൂർണ്ണസംഖ്യ nnames = വലുപ്പം(names)/വലുപ്പം(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	എങ്കിൽ(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		തിരികെ;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	എങ്കിൽ (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} അല്ലെങ്കിൽ {
		printf(" %d/0x%x", mtype, mtype);
	}

	എങ്കിൽ (mightbecall && printparams) {
#define IDENT(x, y) എങ്കിൽ (mtype == x) printparam(#y, &സന്ദേശം->y, വലുപ്പം(സന്ദേശം->y));
#അന്തര്ഭാവ "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#എങ്കിൽ DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
സ്ഥിതം പൂർണ്ണസംഖ്യ messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
സ്ഥിതം ഘടന {
	പൂർണ്ണസംഖ്യ src, dst, messages;
} winners[PRINTSLOTS];
സ്ഥിതം പൂർണ്ണസംഖ്യ total, goodslots;

സ്ഥിതം ശൂന്യം printstats(പൂർണ്ണസംഖ്യ ticks)
{
	പൂർണ്ണസംഖ്യ i;
	വേണ്ടി(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		അക്ഷരം	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

സ്ഥിതം ശൂന്യം sortstats(ശൂന്യം)
{
	/* Print top message senders/receivers. */
	പൂർണ്ണസംഖ്യ src_slot, dst_slot;
	total = goodslots = 0;
	വേണ്ടി(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		വേണ്ടി(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			പൂർണ്ണസംഖ്യ w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			ആകുമ്പോൾ(w > 0 && n > winners[w-1].messages)
				w--;
			എങ്കിൽ(w >= PRINTSLOTS) തുടരുക;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			പ്രതിജ്ഞാ(rem >= 0);
			പ്രതിജ്ഞാ(rem < PRINTSLOTS);
			എങ്കിൽ(rem > 0) {
				പ്രതിജ്ഞാ(w+1 <= PRINTSLOTS-1);
				പ്രതിജ്ഞാ(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*വലുപ്പം(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			എങ്കിൽ(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	എങ്കിൽ(p) { s = p->p_nr; } \
	അല്ലെങ്കിൽ { s = KERNELIPC; } \
	പ്രതിജ്ഞാ(s >= 0 && s < IPCPROCS); \
}

സ്ഥിതം ശൂന്യം statmsg(message *സന്ദേശം, ഘടന proc *srcp, ഘടന proc *dstp)
{
	പൂർണ്ണസംഖ്യ src, dst, now, secs, dt;
	സ്ഥിതം പൂർണ്ണസംഖ്യ lastprint;

	/* Stat message. */
	പ്രതിജ്ഞാ(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	എങ്കിൽ(secs >= 30) {
		memset(winners, 0, വലുപ്പം(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, വലുപ്പം(messages));
		lastprint = now;
	}
}
#endif

#എങ്കിൽ DEBUG_IPC_HOOK
ശൂന്യം hook_ipc_msgkcall(message *സന്ദേശം, ഘടന proc *proc)
{
#എങ്കിൽ DEBUG_DUMPIPC
	printmsg(സന്ദേശം, proc, ശൂന്യ, 'k', 1);
#endif
}

ശൂന്യം hook_ipc_msgkresult(message *സന്ദേശം, ഘടന proc *proc)
{
#എങ്കിൽ DEBUG_DUMPIPC
	printmsg(സന്ദേശം, ശൂന്യ, proc, 'k', 0);
#endif
#എങ്കിൽ DEBUG_IPCSTATS
	statmsg(സന്ദേശം, proc, ശൂന്യ);
#endif
}

ശൂന്യം hook_ipc_msgrecv(message *സന്ദേശം, ഘടന proc *src, ഘടന proc *dst)
{
#എങ്കിൽ DEBUG_DUMPIPC
	printmsg(സന്ദേശം, src, dst, 'r', 0);
#endif
#എങ്കിൽ DEBUG_IPCSTATS
	statmsg(സന്ദേശം, src, dst);
#endif
}

ശൂന്യം hook_ipc_msgsend(message *സന്ദേശം, ഘടന proc *src, ഘടന proc *dst)
{
#എങ്കിൽ DEBUG_DUMPIPC
	printmsg(സന്ദേശം, src, dst, 's', 1);
#endif
}

ശൂന്യം hook_ipc_clear(ഘടന proc *p)
{
#എങ്കിൽ DEBUG_IPCSTATS
	പൂർണ്ണസംഖ്യ slot, i;
	പ്രതിജ്ഞാ(p);
	proc2slot(p, slot);
	വേണ്ടി(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
