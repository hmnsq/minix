/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#अन्तर्भाव "kernel/kernel.h"

#अन्तर्भाव <minix/callnr.h>
#अन्तर्भाव <minix/u64.h>
#अन्तर्भाव <limits.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

पूर्णाङ्क runqueues_ok_cpu(अचिन्हित cpu)
{
  पूर्णाङ्क q, l = 0;
  पञ्जिका संरचना proc *xp;
  संरचना proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  लागि (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	यदि (l++ > MAX_LOOP) panic("check error");
  }

  लागि (q=l=0; q < NR_SCHED_QUEUES; q++) {
    यदि (rdy_head[q] && !rdy_tail[q]) {
	म_लिखो("head but no tail in %d\n", q);
	फर्काऊ 0;
    }
    यदि (!rdy_head[q] && rdy_tail[q]) {
	म_लिखो("tail but no head in %d\n", q);
	फर्काऊ 0;
    }
    यदि (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	म_लिखो("tail and tail->next not null in %d\n", q);
	फर्काऊ 0;
    }
    लागि(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	अचर vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	यदि(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		म_लिखो("xp out of range\n");
		फर्काऊ 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	यदि(dxp % परिमाण(संरचना proc)) {
  		म_लिखो("xp not a real pointer");
		फर्काऊ 0;
	}
	यदि(!proc_ptr_ok(xp)) {
  		म_लिखो("xp bogus pointer");
		फर्काऊ 0;
	}
	यदि (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		म_लिखो("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		फर्काऊ 0;
	}
        यदि (!proc_is_runnable(xp)) {
		म_लिखो("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		फर्काऊ 0;
        }
        यदि (xp->p_priority != q) {
		म_लिखो("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		फर्काऊ 0;
	}
	यदि (xp->p_found) {
		म_लिखो("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		फर्काऊ 0;
	}
	xp->p_found = 1;
	यदि (!xp->p_nextready && rdy_tail[q] != xp) {
		म_लिखो("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		फर्काऊ 0;
	}
	यदि (l++ > MAX_LOOP) {
		म_लिखो("loop in schedule queue?");
		फर्काऊ 0;
	}
    }
  }	

  लागि (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	यदि(!proc_ptr_ok(xp)) {
		म_लिखो("xp bogus pointer in proc table\n");
		फर्काऊ 0;
	}
	यदि (isemptyp(xp))
		जारी;
	यदि(proc_is_runnable(xp) && !xp->p_found) {
		म_लिखो("sched error: ready proc %d not on queue\n", xp->p_nr);
		फर्काऊ 0;
	}
  }

  /* All is ok. */
  फर्काऊ 1;
}

#ifdef CONFIG_SMP
स्थिर पूर्णाङ्क runqueues_ok_all(शून्य)
{
	अचिन्हित c;

	लागि (c = 0 ; c < ncpus; c++) {
		यदि (!runqueues_ok_cpu(c))
			फर्काऊ 0;
	}
	फर्काऊ 1;	
}

पूर्णाङ्क runqueues_ok(शून्य)
{
	फर्काऊ runqueues_ok_all();
}

#अन्यथा

पूर्णाङ्क runqueues_ok(शून्य)
{
	फर्काऊ runqueues_ok_cpu(0);
}


#endif

वर्ण *
rtsflagstr(अचर u32_t flags)
{
	स्थिर वर्ण str[100];
	str[0] = '\0';

#define FLAG(n) यदि(flags & n) { strlcat(str, #n " ", परिमाण(str)); }

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

	फर्काऊ str;
}

वर्ण *
miscflagstr(अचर u32_t flags)
{
	स्थिर वर्ण str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	फर्काऊ str;
}

वर्ण *
schedulerstr(संरचना proc *scheduler)
{
	यदि (scheduler != रिक्त)
	{
		फर्काऊ scheduler->p_name;
	}

	फर्काऊ "KERNEL";
}

स्थिर शून्य
print_proc_name(संरचना proc *pp)
{
	वर्ण *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	यदि(name) {
		म_लिखो("%s(%d)", name, ep);
	}
	अन्यथा {
		म_लिखो("%d", ep);
	}
}

स्थिर शून्य
print_endpoint(endpoint_t ep)
{
	पूर्णाङ्क proc_nr;
	संरचना proc *pp = रिक्त;

	विकल्प(ep) {
	पक्ष ANY:
		म_लिखो("ANY");
	रोक;
	पक्ष SELF:
		म_लिखो("SELF");
	रोक;
	पक्ष NONE:
		म_लिखो("NONE");
	रोक;
	स्वभाव:
		यदि(!isokendpt(ep, &proc_nr)) {
			म_लिखो("??? %d\n", ep);
		}
		अन्यथा {
			pp = proc_addr(proc_nr);
			यदि(isemptyp(pp)) {
				म_लिखो("??? empty slot %d\n", proc_nr);
			}
			अन्यथा {
				print_proc_name(pp);
			}
		}
	रोक;
	}
}

स्थिर शून्य
print_sigmgr(संरचना proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	यदि(sig_mgr == NONE) { म_लिखो("no sigmgr"); फर्काऊ; }
	म_लिखो("sigmgr ");
	print_endpoint(sig_mgr);
	यदि(bak_sig_mgr != NONE) {
		म_लिखो(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

शून्य print_proc(संरचना proc *pp)
{
	endpoint_t dep;

	म_लिखो("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#यदि defined(__i386__)
		pp->p_seg.p_cr3,
#अथवायदि defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	यदि(dep != NONE) {
		म_लिखो(" blocked on: ");
		print_endpoint(dep);
	}
	म_लिखो("\n");
}

स्थिर शून्य print_proc_depends(संरचना proc *pp, अचर पूर्णाङ्क level)
{
	संरचना proc *depproc = रिक्त;
	endpoint_t dep;
#define COL { पूर्णाङ्क i; लागि(i = 0; i < level; i++) म_लिखो("> "); }

	यदि(level >= NR_PROCS) {
		म_लिखो("loop??\n");
		फर्काऊ;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	यदि(dep != NONE && dep != ANY) {
		पूर्णाङ्क procno;
		यदि(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			यदि(isemptyp(depproc))
				depproc = रिक्त;
		}
		यदि (depproc)
			print_proc_depends(depproc, level+1);
	}
}

शून्य print_proc_recursive(संरचना proc *pp)
{
	print_proc_depends(pp, 0);
}

#यदि DEBUG_DUMPIPC || DEBUG_DUMPIPCF
स्थिर अचर वर्ण *mtypename(पूर्णाङ्क mtype, पूर्णाङ्क *possible_callname)
{
	वर्ण *callname = रिक्त, *errname = रिक्त;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	विकल्प(mtype) {
#define IDENT(x) पक्ष x: callname = #x; *possible_callname = 1; रोक;
#अन्तर्भाव "kernel/extracted-mtype.h"
#undef IDENT
	}
	विकल्प(mtype) {
#define IDENT(x) पक्ष x: errname = #x; रोक;
#अन्तर्भाव "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	यदि(!errname && !callname)
		फर्काऊ रिक्त;

	/* 2 matches */
	यदि(errname && callname) {
		स्थिर वर्ण typename[100];
		म_नकल(typename, errname);
		म_जोड़ो(typename, " / ");
		म_जोड़ो(typename, callname);
		फर्काऊ typename;
	}

	यदि(errname) फर्काऊ errname;

	प्रतिज्ञा(callname);
	फर्काऊ callname;
}

स्थिर शून्य printproc(संरचना proc *rp)
{
	यदि (rp)
		म_लिखो(" %s(%d)", rp->p_name, rp - proc);
	अन्यथा
		म_लिखो(" kernel");
}

स्थिर शून्य printparam(अचर वर्ण *name, अचर शून्य *data, माप_प्रकार size)
{
	म_लिखो(" %s=", name);
	विकल्प (size) {
		पक्ष परिमाण(वर्ण):	म_लिखो("%d", *(वर्ण *) data);	रोक;
		पक्ष परिमाण(लघु):	म_लिखो("%d", *(लघु *) data);	रोक;
		पक्ष परिमाण(पूर्णाङ्क):	म_लिखो("%d", *(पूर्णाङ्क *) data);	रोक;
		स्वभाव:		म_लिखो("(%u bytes)", size);	रोक;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
स्थिर पूर्णाङ्क namematch(वर्ण **names, पूर्णाङ्क nnames, वर्ण *name)
{
	पूर्णाङ्क i;
	लागि(i = 0; i < nnames; i++)
		यदि(!म_भेद(names[i], name))
			फर्काऊ 1;
	फर्काऊ 0;
}
#endif

शून्य printmsg(message *सन्देश, संरचना proc *src, संरचना proc *dst,
	वर्ण operation, पूर्णाङ्क printparams)
{
	अचर वर्ण *name;
	पूर्णाङ्क mtype = सन्देश->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	वर्ण *names[] = DEBUG_DUMPIPC_NAMES;
	पूर्णाङ्क nnames = परिमाण(names)/परिमाण(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	यदि(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		फर्काऊ;
	}
  }
#endif

	/* source, destination and message type */
	म_लिखो("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	यदि (name) {
		म_लिखो(" %s(%d/0x%x)", name, mtype, mtype);
	} अन्यथा {
		म_लिखो(" %d/0x%x", mtype, mtype);
	}

	यदि (mightbecall && printparams) {
#define IDENT(x, y) यदि (mtype == x) printparam(#y, &सन्देश->y, परिमाण(सन्देश->y));
#अन्तर्भाव "kernel/extracted-mfield.h"
#undef IDENT
	}
	म_लिखो("\n");
}
#endif

#यदि DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
स्थिर पूर्णाङ्क messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
स्थिर संरचना {
	पूर्णाङ्क src, dst, messages;
} winners[PRINTSLOTS];
स्थिर पूर्णाङ्क total, goodslots;

स्थिर शून्य printstats(पूर्णाङ्क ticks)
{
	पूर्णाङ्क i;
	लागि(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		वर्ण	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		म_लिखो("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	म_लिखो("total %d/s\n", persec(total));
}

स्थिर शून्य sortstats(शून्य)
{
	/* Print top message senders/receivers. */
	पूर्णाङ्क src_slot, dst_slot;
	total = goodslots = 0;
	लागि(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		लागि(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			पूर्णाङ्क w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			जबसम्म(w > 0 && n > winners[w-1].messages)
				w--;
			यदि(w >= PRINTSLOTS) जारी;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			प्रतिज्ञा(rem >= 0);
			प्रतिज्ञा(rem < PRINTSLOTS);
			यदि(rem > 0) {
				प्रतिज्ञा(w+1 <= PRINTSLOTS-1);
				प्रतिज्ञा(w >= 0);
				स_हटाओ(&winners[w+1], &winners[w],
					rem*परिमाण(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			यदि(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	यदि(p) { s = p->p_nr; } \
	अन्यथा { s = KERNELIPC; } \
	प्रतिज्ञा(s >= 0 && s < IPCPROCS); \
}

स्थिर शून्य statmsg(message *सन्देश, संरचना proc *srcp, संरचना proc *dstp)
{
	पूर्णाङ्क src, dst, now, secs, dt;
	स्थिर पूर्णाङ्क lastprint;

	/* Stat message. */
	प्रतिज्ञा(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	यदि(secs >= 30) {
		स_रखो(winners, 0, परिमाण(winners));
		sortstats();
		printstats(dt);
		स_रखो(messages, 0, परिमाण(messages));
		lastprint = now;
	}
}
#endif

#यदि DEBUG_IPC_HOOK
शून्य hook_ipc_msgkcall(message *सन्देश, संरचना proc *proc)
{
#यदि DEBUG_DUMPIPC
	printmsg(सन्देश, proc, रिक्त, 'k', 1);
#endif
}

शून्य hook_ipc_msgkresult(message *सन्देश, संरचना proc *proc)
{
#यदि DEBUG_DUMPIPC
	printmsg(सन्देश, रिक्त, proc, 'k', 0);
#endif
#यदि DEBUG_IPCSTATS
	statmsg(सन्देश, proc, रिक्त);
#endif
}

शून्य hook_ipc_msgrecv(message *सन्देश, संरचना proc *src, संरचना proc *dst)
{
#यदि DEBUG_DUMPIPC
	printmsg(सन्देश, src, dst, 'r', 0);
#endif
#यदि DEBUG_IPCSTATS
	statmsg(सन्देश, src, dst);
#endif
}

शून्य hook_ipc_msgsend(message *सन्देश, संरचना proc *src, संरचना proc *dst)
{
#यदि DEBUG_DUMPIPC
	printmsg(सन्देश, src, dst, 's', 1);
#endif
}

शून्य hook_ipc_clear(संरचना proc *p)
{
#यदि DEBUG_IPCSTATS
	पूर्णाङ्क slot, i;
	प्रतिज्ञा(p);
	proc2slot(p, slot);
	लागि(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
