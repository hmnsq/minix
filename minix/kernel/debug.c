/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#समावेश "kernel/kernel.h"

#समावेश <minix/callnr.h>
#समावेश <minix/u64.h>
#समावेश <limits.h>
#समावेश <string.h>
#समावेश <assert.h>

#घोषणा MAX_LOOP (NR_PROCS + NR_TASKS)

पूर्णांक runqueues_ok_cpu(अचिन्हित cpu)
{
  पूर्णांक q, l = 0;
  रेजिस्टर काष्ठा proc *xp;
  काष्ठा proc **rdy_शीर्ष, **rdy_पुच्छ;

  rdy_शीर्ष = get_cpu_var(cpu, run_q_शीर्ष);
  rdy_पुच्छ = get_cpu_var(cpu, run_q_पुच्छ);

  क्रम (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	अगर (l++ > MAX_LOOP) panic("check error");
  }

  क्रम (q=l=0; q < NR_SCHED_QUEUES; q++) {
    अगर (rdy_शीर्ष[q] && !rdy_पुच्छ[q]) {
	म_लिखो("head but no tail in %d\n", q);
	वापस 0;
    }
    अगर (!rdy_शीर्ष[q] && rdy_पुच्छ[q]) {
	म_लिखो("tail but no head in %d\n", q);
	वापस 0;
    }
    अगर (rdy_पुच्छ[q] && rdy_पुच्छ[q]->p_nextready) {
	म_लिखो("tail and tail->next not null in %d\n", q);
	वापस 0;
    }
    क्रम(xp = rdy_शीर्ष[q]; xp; xp = xp->p_nextready) {
	स्थिर vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	अगर(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		म_लिखो("xp out of range\n");
		वापस 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	अगर(dxp % माप(काष्ठा proc)) {
  		म_लिखो("xp not a real pointer");
		वापस 0;
	}
	अगर(!proc_ptr_ok(xp)) {
  		म_लिखो("xp bogus pointer");
		वापस 0;
	}
	अगर (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		म_लिखो("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		वापस 0;
	}
        अगर (!proc_is_runnable(xp)) {
		म_लिखो("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		वापस 0;
        }
        अगर (xp->p_priority != q) {
		म_लिखो("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		वापस 0;
	}
	अगर (xp->p_found) {
		म_लिखो("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		वापस 0;
	}
	xp->p_found = 1;
	अगर (!xp->p_nextready && rdy_पुच्छ[q] != xp) {
		म_लिखो("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		वापस 0;
	}
	अगर (l++ > MAX_LOOP) {
		म_लिखो("loop in schedule queue?");
		वापस 0;
	}
    }
  }	

  क्रम (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	अगर(!proc_ptr_ok(xp)) {
		म_लिखो("xp bogus pointer in proc table\n");
		वापस 0;
	}
	अगर (isemptyp(xp))
		जारी;
	अगर(proc_is_runnable(xp) && !xp->p_found) {
		म_लिखो("sched error: ready proc %d not on queue\n", xp->p_nr);
		वापस 0;
	}
  }

  /* All is ok. */
  वापस 1;
}

#अगर_घोषित CONFIG_SMP
अटल पूर्णांक runqueues_ok_all(व्योम)
{
	अचिन्हित c;

	क्रम (c = 0 ; c < ncpus; c++) {
		अगर (!runqueues_ok_cpu(c))
			वापस 0;
	}
	वापस 1;	
}

पूर्णांक runqueues_ok(व्योम)
{
	वापस runqueues_ok_all();
}

#अन्यथा

पूर्णांक runqueues_ok(व्योम)
{
	वापस runqueues_ok_cpu(0);
}


#पूर्ण_अगर

अक्षर *
rtsflagstr(स्थिर u32_t flags)
{
	अटल अक्षर str[100];
	str[0] = '\0';

#घोषणा FLAG(n) अगर(flags & n) { strlcat(str, #n " ", माप(str)); }

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

	वापस str;
}

अक्षर *
miscflagstr(स्थिर u32_t flags)
{
	अटल अक्षर str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	वापस str;
}

अक्षर *
schedulerstr(काष्ठा proc *scheduler)
{
	अगर (scheduler != शून्य)
	{
		वापस scheduler->p_name;
	}

	वापस "KERNEL";
}

अटल व्योम
print_proc_name(काष्ठा proc *pp)
{
	अक्षर *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	अगर(name) {
		म_लिखो("%s(%d)", name, ep);
	}
	अन्यथा {
		म_लिखो("%d", ep);
	}
}

अटल व्योम
print_endpoint(endpoint_t ep)
{
	पूर्णांक proc_nr;
	काष्ठा proc *pp = शून्य;

	चयन(ep) {
	हाल ANY:
		म_लिखो("ANY");
	अवरोध;
	हाल SELF:
		म_लिखो("SELF");
	अवरोध;
	हाल NONE:
		म_लिखो("NONE");
	अवरोध;
	शेष:
		अगर(!isokendpt(ep, &proc_nr)) {
			म_लिखो("??? %d\n", ep);
		}
		अन्यथा {
			pp = proc_addr(proc_nr);
			अगर(isemptyp(pp)) {
				म_लिखो("??? empty slot %d\n", proc_nr);
			}
			अन्यथा {
				print_proc_name(pp);
			}
		}
	अवरोध;
	}
}

अटल व्योम
print_sigmgr(काष्ठा proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	अगर(sig_mgr == NONE) { म_लिखो("no sigmgr"); वापस; }
	म_लिखो("sigmgr ");
	print_endpoint(sig_mgr);
	अगर(bak_sig_mgr != NONE) {
		म_लिखो(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

व्योम print_proc(काष्ठा proc *pp)
{
	endpoint_t dep;

	म_लिखो("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#अगर defined(__i386__)
		pp->p_seg.p_cr3,
#या_अगर defined(__arm__)
		pp->p_seg.p_ttbr,
#पूर्ण_अगर
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	अगर(dep != NONE) {
		म_लिखो(" blocked on: ");
		print_endpoint(dep);
	}
	म_लिखो("\n");
}

अटल व्योम print_proc_depends(काष्ठा proc *pp, स्थिर पूर्णांक level)
{
	काष्ठा proc *depproc = शून्य;
	endpoint_t dep;
#घोषणा COL { पूर्णांक i; क्रम(i = 0; i < level; i++) म_लिखो("> "); }

	अगर(level >= NR_PROCS) {
		म_लिखो("loop??\n");
		वापस;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	अगर(dep != NONE && dep != ANY) {
		पूर्णांक procno;
		अगर(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			अगर(isemptyp(depproc))
				depproc = शून्य;
		}
		अगर (depproc)
			print_proc_depends(depproc, level+1);
	}
}

व्योम print_proc_recursive(काष्ठा proc *pp)
{
	print_proc_depends(pp, 0);
}

#अगर DEBUG_DUMPIPC || DEBUG_DUMPIPCF
अटल स्थिर अक्षर *mtypename(पूर्णांक mtype, पूर्णांक *possible_callname)
{
	अक्षर *callname = शून्य, *errname = शून्य;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	चयन(mtype) {
#घोषणा IDENT(x) हाल x: callname = #x; *possible_callname = 1; अवरोध;
#समावेश "kernel/extracted-mtype.h"
#अघोषित IDENT
	}
	चयन(mtype) {
#घोषणा IDENT(x) हाल x: errname = #x; अवरोध;
#समावेश "kernel/extracted-errno.h"
#अघोषित IDENT
	}

	/* no match */
	अगर(!errname && !callname)
		वापस शून्य;

	/* 2 matches */
	अगर(errname && callname) {
		अटल अक्षर typename[100];
		म_नकल(typename, errname);
		म_जोड़ो(typename, " / ");
		म_जोड़ो(typename, callname);
		वापस typename;
	}

	अगर(errname) वापस errname;

	निश्चित(callname);
	वापस callname;
}

अटल व्योम printproc(काष्ठा proc *rp)
{
	अगर (rp)
		म_लिखो(" %s(%d)", rp->p_name, rp - proc);
	अन्यथा
		म_लिखो(" kernel");
}

अटल व्योम printparam(स्थिर अक्षर *name, स्थिर व्योम *तथ्य, माप_प्रकार आकार)
{
	म_लिखो(" %s=", name);
	चयन (आकार) {
		हाल माप(अक्षर):	म_लिखो("%d", *(अक्षर *) तथ्य);	अवरोध;
		हाल माप(लघु):	म_लिखो("%d", *(लघु *) तथ्य);	अवरोध;
		हाल माप(पूर्णांक):	म_लिखो("%d", *(पूर्णांक *) तथ्य);	अवरोध;
		शेष:		म_लिखो("(%u bytes)", आकार);	अवरोध;
	}
}

#अगर_घोषित DEBUG_DUMPIPC_NAMES
अटल पूर्णांक namematch(अक्षर **names, पूर्णांक nnames, अक्षर *name)
{
	पूर्णांक i;
	क्रम(i = 0; i < nnames; i++)
		अगर(!म_भेद(names[i], name))
			वापस 1;
	वापस 0;
}
#पूर्ण_अगर

व्योम printmsg(message *संदेश, काष्ठा proc *src, काष्ठा proc *dst,
	अक्षर operation, पूर्णांक printparams)
{
	स्थिर अक्षर *name;
	पूर्णांक mtype = संदेश->m_type, mightbecall = 0;

#अगर_घोषित DEBUG_DUMPIPC_NAMES
  {
	अक्षर *names[] = DEBUG_DUMPIPC_NAMES;
	पूर्णांक nnames = माप(names)/माप(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	अगर(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		वापस;
	}
  }
#पूर्ण_अगर

	/* source, destination and message type */
	म_लिखो("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	अगर (name) {
		म_लिखो(" %s(%d/0x%x)", name, mtype, mtype);
	} अन्यथा {
		म_लिखो(" %d/0x%x", mtype, mtype);
	}

	अगर (mightbecall && printparams) {
#घोषणा IDENT(x, y) अगर (mtype == x) printparam(#y, &संदेश->y, माप(संदेश->y));
#समावेश "kernel/extracted-mfield.h"
#अघोषित IDENT
	}
	म_लिखो("\n");
}
#पूर्ण_अगर

#अगर DEBUG_IPCSTATS
#घोषणा IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#घोषणा KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
अटल पूर्णांक messages[IPCPROCS][IPCPROCS];

#घोषणा PRINTSLOTS 20
अटल काष्ठा {
	पूर्णांक src, dst, messages;
} winners[PRINTSLOTS];
अटल पूर्णांक योग, goodslots;

अटल व्योम printstats(पूर्णांक ticks)
{
	पूर्णांक i;
	क्रम(i = 0; i < goodslots; i++) {
#घोषणा name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#घोषणा persec(n) (system_hz*(n)/ticks)
		अक्षर	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		म_लिखो("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	म_लिखो("total %d/s\n", persec(योग));
}

अटल व्योम sortstats(व्योम)
{
	/* Print top message senders/receivers. */
	पूर्णांक src_slot, dst_slot;
	योग = goodslots = 0;
	क्रम(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		क्रम(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			पूर्णांक w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			योग += n;
			जबतक(w > 0 && n > winners[w-1].messages)
				w--;
			अगर(w >= PRINTSLOTS) जारी;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			निश्चित(rem >= 0);
			निश्चित(rem < PRINTSLOTS);
			अगर(rem > 0) {
				निश्चित(w+1 <= PRINTSLOTS-1);
				निश्चित(w >= 0);
				स_हटाओ(&winners[w+1], &winners[w],
					rem*माप(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			अगर(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#घोषणा proc2slot(p, s) { \
	अगर(p) { s = p->p_nr; } \
	अन्यथा { s = KERNELIPC; } \
	निश्चित(s >= 0 && s < IPCPROCS); \
}

अटल व्योम statmsg(message *संदेश, काष्ठा proc *srcp, काष्ठा proc *dstp)
{
	पूर्णांक src, dst, now, secs, dt;
	अटल पूर्णांक lastprint;

	/* Stat message. */
	निश्चित(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	अगर(secs >= 30) {
		स_रखो(winners, 0, माप(winners));
		sortstats();
		printstats(dt);
		स_रखो(messages, 0, माप(messages));
		lastprint = now;
	}
}
#पूर्ण_अगर

#अगर DEBUG_IPC_HOOK
व्योम hook_ipc_msgkcall(message *संदेश, काष्ठा proc *proc)
{
#अगर DEBUG_DUMPIPC
	printmsg(संदेश, proc, शून्य, 'k', 1);
#पूर्ण_अगर
}

व्योम hook_ipc_msgkresult(message *संदेश, काष्ठा proc *proc)
{
#अगर DEBUG_DUMPIPC
	printmsg(संदेश, शून्य, proc, 'k', 0);
#पूर्ण_अगर
#अगर DEBUG_IPCSTATS
	statmsg(संदेश, proc, शून्य);
#पूर्ण_अगर
}

व्योम hook_ipc_msgrecv(message *संदेश, काष्ठा proc *src, काष्ठा proc *dst)
{
#अगर DEBUG_DUMPIPC
	printmsg(संदेश, src, dst, 'r', 0);
#पूर्ण_अगर
#अगर DEBUG_IPCSTATS
	statmsg(संदेश, src, dst);
#पूर्ण_अगर
}

व्योम hook_ipc_msgsend(message *संदेश, काष्ठा proc *src, काष्ठा proc *dst)
{
#अगर DEBUG_DUMPIPC
	printmsg(संदेश, src, dst, 's', 1);
#पूर्ण_अगर
}

व्योम hook_ipc_clear(काष्ठा proc *p)
{
#अगर DEBUG_IPCSTATS
	पूर्णांक slot, i;
	निश्चित(p);
	proc2slot(p, slot);
	क्रम(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#पूर्ण_अगर
}
#पूर्ण_अगर
