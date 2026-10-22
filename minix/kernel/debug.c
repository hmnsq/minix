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

पूर्णांक runqueues_ok_cpu(अचिन्हित cpu)
{
  पूर्णांक q, l = 0;
  नोंदणी संरचना proc *xp;
  संरचना proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  साठी (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	जर (l++ > MAX_LOOP) panic("check error");
  }

  साठी (q=l=0; q < NR_SCHED_QUEUES; q++) {
    जर (rdy_head[q] && !rdy_tail[q]) {
	म_लिखो("head but no tail in %d\n", q);
	परत 0;
    }
    जर (!rdy_head[q] && rdy_tail[q]) {
	म_लिखो("tail but no head in %d\n", q);
	परत 0;
    }
    जर (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	म_लिखो("tail and tail->next not null in %d\n", q);
	परत 0;
    }
    साठी(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	स्थिरांक vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	जर(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		म_लिखो("xp out of range\n");
		परत 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	जर(dxp % आकार(संरचना proc)) {
  		म_लिखो("xp not a real pointer");
		परत 0;
	}
	जर(!proc_ptr_ok(xp)) {
  		म_लिखो("xp bogus pointer");
		परत 0;
	}
	जर (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		म_लिखो("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		परत 0;
	}
        जर (!proc_is_runnable(xp)) {
		म_लिखो("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		परत 0;
        }
        जर (xp->p_priority != q) {
		म_लिखो("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		परत 0;
	}
	जर (xp->p_found) {
		म_लिखो("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		परत 0;
	}
	xp->p_found = 1;
	जर (!xp->p_nextready && rdy_tail[q] != xp) {
		म_लिखो("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		परत 0;
	}
	जर (l++ > MAX_LOOP) {
		म_लिखो("loop in schedule queue?");
		परत 0;
	}
    }
  }	

  साठी (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	जर(!proc_ptr_ok(xp)) {
		म_लिखो("xp bogus pointer in proc table\n");
		परत 0;
	}
	जर (isemptyp(xp))
		सुरूठेवा;
	जर(proc_is_runnable(xp) && !xp->p_found) {
		म_लिखो("sched error: ready proc %d not on queue\n", xp->p_nr);
		परत 0;
	}
  }

  /* All is ok. */
  परत 1;
}

#ifdef CONFIG_SMP
स्थिर पूर्णांक runqueues_ok_all(शून्य)
{
	अचिन्हित c;

	साठी (c = 0 ; c < ncpus; c++) {
		जर (!runqueues_ok_cpu(c))
			परत 0;
	}
	परत 1;	
}

पूर्णांक runqueues_ok(शून्य)
{
	परत runqueues_ok_all();
}

#अन्यथा

पूर्णांक runqueues_ok(शून्य)
{
	परत runqueues_ok_cpu(0);
}


#endif

अक्षर *
rtsflagstr(स्थिरांक u32_t flags)
{
	स्थिर अक्षर str[100];
	str[0] = '\0';

#define FLAG(n) जर(flags & n) { strlcat(str, #n " ", आकार(str)); }

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

	परत str;
}

अक्षर *
miscflagstr(स्थिरांक u32_t flags)
{
	स्थिर अक्षर str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	परत str;
}

अक्षर *
schedulerstr(संरचना proc *scheduler)
{
	जर (scheduler != रिक्त)
	{
		परत scheduler->p_name;
	}

	परत "KERNEL";
}

स्थिर शून्य
print_proc_name(संरचना proc *pp)
{
	अक्षर *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	जर(name) {
		म_लिखो("%s(%d)", name, ep);
	}
	अन्यथा {
		म_लिखो("%d", ep);
	}
}

स्थिर शून्य
print_endpoint(endpoint_t ep)
{
	पूर्णांक proc_nr;
	संरचना proc *pp = रिक्त;

	निवड(ep) {
	स्थिती ANY:
		म_लिखो("ANY");
	थांबा;
	स्थिती SELF:
		म_लिखो("SELF");
	थांबा;
	स्थिती NONE:
		म_लिखो("NONE");
	थांबा;
	पूर्वनिर्धारित:
		जर(!isokendpt(ep, &proc_nr)) {
			म_लिखो("??? %d\n", ep);
		}
		अन्यथा {
			pp = proc_addr(proc_nr);
			जर(isemptyp(pp)) {
				म_लिखो("??? empty slot %d\n", proc_nr);
			}
			अन्यथा {
				print_proc_name(pp);
			}
		}
	थांबा;
	}
}

स्थिर शून्य
print_sigmgr(संरचना proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	जर(sig_mgr == NONE) { म_लिखो("no sigmgr"); परत; }
	म_लिखो("sigmgr ");
	print_endpoint(sig_mgr);
	जर(bak_sig_mgr != NONE) {
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
#जर defined(__i386__)
		pp->p_seg.p_cr3,
#किंवाजर defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	जर(dep != NONE) {
		म_लिखो(" blocked on: ");
		print_endpoint(dep);
	}
	म_लिखो("\n");
}

स्थिर शून्य print_proc_depends(संरचना proc *pp, स्थिरांक पूर्णांक level)
{
	संरचना proc *depproc = रिक्त;
	endpoint_t dep;
#define COL { पूर्णांक i; साठी(i = 0; i < level; i++) म_लिखो("> "); }

	जर(level >= NR_PROCS) {
		म_लिखो("loop??\n");
		परत;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	जर(dep != NONE && dep != ANY) {
		पूर्णांक procno;
		जर(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			जर(isemptyp(depproc))
				depproc = रिक्त;
		}
		जर (depproc)
			print_proc_depends(depproc, level+1);
	}
}

शून्य print_proc_recursive(संरचना proc *pp)
{
	print_proc_depends(pp, 0);
}

#जर DEBUG_DUMPIPC || DEBUG_DUMPIPCF
स्थिर स्थिरांक अक्षर *mtypename(पूर्णांक mtype, पूर्णांक *possible_callname)
{
	अक्षर *callname = रिक्त, *errname = रिक्त;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	निवड(mtype) {
#define IDENT(x) स्थिती x: callname = #x; *possible_callname = 1; थांबा;
#अन्तर्भाव "kernel/extracted-mtype.h"
#undef IDENT
	}
	निवड(mtype) {
#define IDENT(x) स्थिती x: errname = #x; थांबा;
#अन्तर्भाव "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	जर(!errname && !callname)
		परत रिक्त;

	/* 2 matches */
	जर(errname && callname) {
		स्थिर अक्षर typename[100];
		म_नकल(typename, errname);
		म_जोड़ो(typename, " / ");
		म_जोड़ो(typename, callname);
		परत typename;
	}

	जर(errname) परत errname;

	प्रतिज्ञा(callname);
	परत callname;
}

स्थिर शून्य printproc(संरचना proc *rp)
{
	जर (rp)
		म_लिखो(" %s(%d)", rp->p_name, rp - proc);
	अन्यथा
		म_लिखो(" kernel");
}

स्थिर शून्य printparam(स्थिरांक अक्षर *name, स्थिरांक शून्य *data, माप_प्रकार size)
{
	म_लिखो(" %s=", name);
	निवड (size) {
		स्थिती आकार(अक्षर):	म_लिखो("%d", *(अक्षर *) data);	थांबा;
		स्थिती आकार(लघु):	म_लिखो("%d", *(लघु *) data);	थांबा;
		स्थिती आकार(पूर्णांक):	म_लिखो("%d", *(पूर्णांक *) data);	थांबा;
		पूर्वनिर्धारित:		म_लिखो("(%u bytes)", size);	थांबा;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
स्थिर पूर्णांक namematch(अक्षर **names, पूर्णांक nnames, अक्षर *name)
{
	पूर्णांक i;
	साठी(i = 0; i < nnames; i++)
		जर(!म_भेद(names[i], name))
			परत 1;
	परत 0;
}
#endif

शून्य printmsg(message *संदेश, संरचना proc *src, संरचना proc *dst,
	अक्षर operation, पूर्णांक printparams)
{
	स्थिरांक अक्षर *name;
	पूर्णांक mtype = संदेश->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	अक्षर *names[] = DEBUG_DUMPIPC_NAMES;
	पूर्णांक nnames = आकार(names)/आकार(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	जर(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		परत;
	}
  }
#endif

	/* source, destination and message type */
	म_लिखो("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	जर (name) {
		म_लिखो(" %s(%d/0x%x)", name, mtype, mtype);
	} अन्यथा {
		म_लिखो(" %d/0x%x", mtype, mtype);
	}

	जर (mightbecall && printparams) {
#define IDENT(x, y) जर (mtype == x) printparam(#y, &संदेश->y, आकार(संदेश->y));
#अन्तर्भाव "kernel/extracted-mfield.h"
#undef IDENT
	}
	म_लिखो("\n");
}
#endif

#जर DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
स्थिर पूर्णांक messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
स्थिर संरचना {
	पूर्णांक src, dst, messages;
} winners[PRINTSLOTS];
स्थिर पूर्णांक total, goodslots;

स्थिर शून्य printstats(पूर्णांक ticks)
{
	पूर्णांक i;
	साठी(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		अक्षर	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		म_लिखो("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	म_लिखो("total %d/s\n", persec(total));
}

स्थिर शून्य sortstats(शून्य)
{
	/* Print top message senders/receivers. */
	पूर्णांक src_slot, dst_slot;
	total = goodslots = 0;
	साठी(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		साठी(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			पूर्णांक w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			पर्यंत(w > 0 && n > winners[w-1].messages)
				w--;
			जर(w >= PRINTSLOTS) सुरूठेवा;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			प्रतिज्ञा(rem >= 0);
			प्रतिज्ञा(rem < PRINTSLOTS);
			जर(rem > 0) {
				प्रतिज्ञा(w+1 <= PRINTSLOTS-1);
				प्रतिज्ञा(w >= 0);
				स_हटाओ(&winners[w+1], &winners[w],
					rem*आकार(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			जर(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	जर(p) { s = p->p_nr; } \
	अन्यथा { s = KERNELIPC; } \
	प्रतिज्ञा(s >= 0 && s < IPCPROCS); \
}

स्थिर शून्य statmsg(message *संदेश, संरचना proc *srcp, संरचना proc *dstp)
{
	पूर्णांक src, dst, now, secs, dt;
	स्थिर पूर्णांक lastprint;

	/* Stat message. */
	प्रतिज्ञा(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	जर(secs >= 30) {
		स_रखो(winners, 0, आकार(winners));
		sortstats();
		printstats(dt);
		स_रखो(messages, 0, आकार(messages));
		lastprint = now;
	}
}
#endif

#जर DEBUG_IPC_HOOK
शून्य hook_ipc_msgkcall(message *संदेश, संरचना proc *proc)
{
#जर DEBUG_DUMPIPC
	printmsg(संदेश, proc, रिक्त, 'k', 1);
#endif
}

शून्य hook_ipc_msgkresult(message *संदेश, संरचना proc *proc)
{
#जर DEBUG_DUMPIPC
	printmsg(संदेश, रिक्त, proc, 'k', 0);
#endif
#जर DEBUG_IPCSTATS
	statmsg(संदेश, proc, रिक्त);
#endif
}

शून्य hook_ipc_msgrecv(message *संदेश, संरचना proc *src, संरचना proc *dst)
{
#जर DEBUG_DUMPIPC
	printmsg(संदेश, src, dst, 'r', 0);
#endif
#जर DEBUG_IPCSTATS
	statmsg(संदेश, src, dst);
#endif
}

शून्य hook_ipc_msgsend(message *संदेश, संरचना proc *src, संरचना proc *dst)
{
#जर DEBUG_DUMPIPC
	printmsg(संदेश, src, dst, 's', 1);
#endif
}

शून्य hook_ipc_clear(संरचना proc *p)
{
#जर DEBUG_IPCSTATS
	पूर्णांक slot, i;
	प्रतिज्ञा(p);
	proc2slot(p, slot);
	साठी(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
