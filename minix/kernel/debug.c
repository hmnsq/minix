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

#परिभाषय MAX_LOOP (NR_PROCS + NR_TASKS)

पूर्णाङ्क runqueues_ok_cpu(अचिह्नित cpu)
{
  पूर्णाङ्क q, l = 0;
  पञ्जिका संरचना proc *xp;
  संरचना proc **rdy_शीर्ष, **rdy_पुच्छ;

  rdy_शीर्ष = get_cpu_var(cpu, सततम्_q_शीर्ष);
  rdy_पुच्छ = get_cpu_var(cpu, सततम्_q_पुच्छ);

  प्रति (xp = BEG_PROC_ADDR; xp < अन्तः_PROC_ADDR; ++xp) {
	xp->p_प्राप्तम् = 0;
	यदि (l++ > MAX_LOOP) panic("check error");
  }

  प्रति (q=l=0; q < NR_SCHED_QUEUES; q++) {
    यदि (rdy_शीर्ष[q] && !rdy_पुच्छ[q]) {
	प्रारूप_लिख("head but no tail in %d\n", q);
	प्रत्यागम 0;
    }
    यदि (!rdy_शीर्ष[q] && rdy_पुच्छ[q]) {
	प्रारूप_लिख("tail but no head in %d\n", q);
	प्रत्यागम 0;
    }
    यदि (rdy_पुच्छ[q] && rdy_पुच्छ[q]->p_nextready) {
	प्रारूप_लिख("tail and tail->next not null in %d\n", q);
	प्रत्यागम 0;
    }
    प्रति(xp = rdy_शीर्ष[q]; xp; xp = xp->p_nextready) {
	ध्रुव vir_अष्टकानि vxp = (vir_अष्टकानि) xp;
	vir_अष्टकानि dxp;
	यदि(vxp < (vir_अष्टकानि) BEG_PROC_ADDR || vxp >= (vir_अष्टकानि) अन्तः_PROC_ADDR) {
  		प्रारूप_लिख("xp out of range\n");
		प्रत्यागम 0;
	}
	dxp = vxp - (vir_अष्टकानि) BEG_PROC_ADDR;
	यदि(dxp % परिमाण(संरचना proc)) {
  		प्रारूप_लिख("xp not a real pointer");
		प्रत्यागम 0;
	}
	यदि(!proc_ptr_ok(xp)) {
  		प्रारूप_लिख("xp bogus pointer");
		प्रत्यागम 0;
	}
	यदि (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		प्रारूप_लिख("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		प्रत्यागम 0;
	}
        यदि (!proc_is_runnable(xp)) {
		प्रारूप_लिख("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		प्रत्यागम 0;
        }
        यदि (xp->p_प्राथमिकता != q) {
		प्रारूप_लिख("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		प्रत्यागम 0;
	}
	यदि (xp->p_प्राप्तम्) {
		प्रारूप_लिख("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		प्रत्यागम 0;
	}
	xp->p_प्राप्तम् = 1;
	यदि (!xp->p_nextready && rdy_पुच्छ[q] != xp) {
		प्रारूप_लिख("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		प्रत्यागम 0;
	}
	यदि (l++ > MAX_LOOP) {
		प्रारूप_लिख("loop in schedule queue?");
		प्रत्यागम 0;
	}
    }
  }	

  प्रति (xp = BEG_PROC_ADDR; xp < अन्तः_PROC_ADDR; ++xp) {
	यदि(!proc_ptr_ok(xp)) {
		प्रारूप_लिख("xp bogus pointer in proc table\n");
		प्रत्यागम 0;
	}
	यदि (isemptyp(xp))
		अनुवर्त;
	यदि(proc_is_runnable(xp) && !xp->p_प्राप्तम्) {
		प्रारूप_लिख("sched error: ready proc %d not on queue\n", xp->p_nr);
		प्रत्यागम 0;
	}
  }

  /* All is ok. */
  प्रत्यागम 1;
}

#यदि_परिभाषितम् CONFIG_SMP
स्थिर पूर्णाङ्क runqueues_ok_all(शून्य)
{
	अचिह्नित c;

	प्रति (c = 0 ; c < ncpus; c++) {
		यदि (!runqueues_ok_cpu(c))
			प्रत्यागम 0;
	}
	प्रत्यागम 1;	
}

पूर्णाङ्क runqueues_ok(शून्य)
{
	प्रत्यागम runqueues_ok_all();
}

#अन्यथा

पूर्णाङ्क runqueues_ok(शून्य)
{
	प्रत्यागम runqueues_ok_cpu(0);
}


#यदि_अन्तः

वर्ण *
rtsflagstr(ध्रुव u32_t flags)
{
	स्थिर वर्ण str[100];
	str[0] = '\0';

#परिभाषय FLAG(n) यदि(flags & n) { strlcat(str, #n " ", परिमाण(str)); }

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
	FLAG(RTS_NO_कालांशः);

	प्रत्यागम str;
}

वर्ण *
miscflagstr(ध्रुव u32_t flags)
{
	स्थिर वर्ण str[100];
	str[0] = '\0';

	FLAG(MF_प्रत्युत्तरम्_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	प्रत्यागम str;
}

वर्ण *
schedulerstr(संरचना proc *scheduler)
{
	यदि (scheduler != रिक्त)
	{
		प्रत्यागम scheduler->p_name;
	}

	प्रत्यागम "KERNEL";
}

स्थिर शून्य
print_proc_name(संरचना proc *pp)
{
	वर्ण *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	यदि(name) {
		प्रारूप_लिख("%s(%d)", name, ep);
	}
	अन्यथा {
		प्रारूप_लिख("%d", ep);
	}
}

स्थिर शून्य
print_endpoint(endpoint_t ep)
{
	पूर्णाङ्क proc_nr;
	संरचना proc *pp = रिक्त;

	विकल्प(ep) {
	पक्ष ANY:
		प्रारूप_लिख("ANY");
	भङ्ग;
	पक्ष SELF:
		प्रारूप_लिख("SELF");
	भङ्ग;
	पक्ष NONE:
		प्रारूप_लिख("NONE");
	भङ्ग;
	स्वभाव:
		यदि(!isokendpt(ep, &proc_nr)) {
			प्रारूप_लिख("??? %d\n", ep);
		}
		अन्यथा {
			pp = proc_addr(proc_nr);
			यदि(isemptyp(pp)) {
				प्रारूप_लिख("??? empty slot %d\n", proc_nr);
			}
			अन्यथा {
				print_proc_name(pp);
			}
		}
	भङ्ग;
	}
}

स्थिर शून्य
print_sigmgr(संरचना proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	यदि(sig_mgr == NONE) { प्रारूप_लिख("no sigmgr"); प्रत्यागम; }
	प्रारूप_लिख("sigmgr ");
	print_endpoint(sig_mgr);
	यदि(bak_sig_mgr != NONE) {
		प्रारूप_लिख(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

शून्य print_proc(संरचना proc *pp)
{
	endpoint_t dep;

	प्रारूप_लिख("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_प्राथमिकता, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_आवृत्तयः),
		ex64lo(pp->p_आवृत्तयः), pp->p_cpu,
#यदि defined(__i386__)
		pp->p_seg.p_cr3,
#नोचेद्यदि defined(__arm__)
		pp->p_seg.p_ttbr,
#यदि_अन्तः
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	यदि(dep != NONE) {
		प्रारूप_लिख(" blocked on: ");
		print_endpoint(dep);
	}
	प्रारूप_लिख("\n");
}

स्थिर शून्य print_proc_depends(संरचना proc *pp, ध्रुव पूर्णाङ्क level)
{
	संरचना proc *depproc = रिक्त;
	endpoint_t dep;
#परिभाषय COL { पूर्णाङ्क i; प्रति(i = 0; i < level; i++) प्रारूप_लिख("> "); }

	यदि(level >= NR_PROCS) {
		प्रारूप_लिख("loop??\n");
		प्रत्यागम;
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
स्थिर ध्रुव वर्ण *mtypename(पूर्णाङ्क mtype, पूर्णाङ्क *possible_callname)
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
#परिभाषय IDENT(x) पक्ष x: callname = #x; *possible_callname = 1; भङ्ग;
#अन्तर्भाव "kernel/extracted-mtype.h"
#अपरिभाषय IDENT
	}
	विकल्प(mtype) {
#परिभाषय IDENT(x) पक्ष x: errname = #x; भङ्ग;
#अन्तर्भाव "kernel/extracted-errno.h"
#अपरिभाषय IDENT
	}

	/* no match */
	यदि(!errname && !callname)
		प्रत्यागम रिक्त;

	/* 2 matches */
	यदि(errname && callname) {
		स्थिर वर्ण typename[100];
		वाक्यं_प्रतिलिख(typename, errname);
		वाक्यं_योजय(typename, " / ");
		वाक्यं_योजय(typename, callname);
		प्रत्यागम typename;
	}

	यदि(errname) प्रत्यागम errname;

	प्रतिज्ञा(callname);
	प्रत्यागम callname;
}

स्थिर शून्य printproc(संरचना proc *rp)
{
	यदि (rp)
		प्रारूप_लिख(" %s(%d)", rp->p_name, rp - proc);
	अन्यथा
		प्रारूप_लिख(" kernel");
}

स्थिर शून्य printparam(ध्रुव वर्ण *name, ध्रुव शून्य *दत्त, size_t आकार)
{
	प्रारूप_लिख(" %s=", name);
	विकल्प (आकार) {
		पक्ष परिमाण(वर्ण):	प्रारूप_लिख("%d", *(वर्ण *) दत्त);	भङ्ग;
		पक्ष परिमाण(ह्रस्व):	प्रारूप_लिख("%d", *(ह्रस्व *) दत्त);	भङ्ग;
		पक्ष परिमाण(पूर्णाङ्क):	प्रारूप_लिख("%d", *(पूर्णाङ्क *) दत्त);	भङ्ग;
		स्वभाव:		प्रारूप_लिख("(%u bytes)", आकार);	भङ्ग;
	}
}

#यदि_परिभाषितम् DEBUG_DUMPIPC_नामानि
स्थिर पूर्णाङ्क namematch(वर्ण **नामानि, पूर्णाङ्क nnames, वर्ण *name)
{
	पूर्णाङ्क i;
	प्रति(i = 0; i < nnames; i++)
		यदि(!वाक्यं_तुलय(नामानि[i], name))
			प्रत्यागम 1;
	प्रत्यागम 0;
}
#यदि_अन्तः

शून्य printmsg(सन्देशः *सन्देशः_2, संरचना proc *src, संरचना proc *dst,
	वर्ण operation, पूर्णाङ्क printparams)
{
	ध्रुव वर्ण *name;
	पूर्णाङ्क mtype = सन्देशः_2->m_type, mightbecall = 0;

#यदि_परिभाषितम् DEBUG_DUMPIPC_नामानि
  {
	वर्ण *नामानि[] = DEBUG_DUMPIPC_नामानि;
	पूर्णाङ्क nnames = परिमाण(नामानि)/परिमाण(नामानि[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	यदि(!(src && namematch(नामानि, nnames, src->p_name)) &&
	   !(dst && namematch(नामानि, nnames, dst->p_name))) {
		प्रत्यागम;
	}
  }
#यदि_अन्तः

	/* source, destination and message type */
	प्रारूप_लिख("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	यदि (name) {
		प्रारूप_लिख(" %s(%d/0x%x)", name, mtype, mtype);
	} अन्यथा {
		प्रारूप_लिख(" %d/0x%x", mtype, mtype);
	}

	यदि (mightbecall && printparams) {
#परिभाषय IDENT(x, y) यदि (mtype == x) printparam(#y, &सन्देशः_2->y, परिमाण(सन्देशः_2->y));
#अन्तर्भाव "kernel/extracted-mfield.h"
#अपरिभाषय IDENT
	}
	प्रारूप_लिख("\n");
}
#यदि_अन्तः

#यदि DEBUG_IPCSTATS
#परिभाषय IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#परिभाषय KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
स्थिर पूर्णाङ्क messages[IPCPROCS][IPCPROCS];

#परिभाषय PRINTSLOTS 20
स्थिर संरचना {
	पूर्णाङ्क src, dst, messages;
} winners[PRINTSLOTS];
स्थिर पूर्णाङ्क योग, goodslots;

स्थिर शून्य printstats(पूर्णाङ्क ticks)
{
	पूर्णाङ्क i;
	प्रति(i = 0; i < goodslots; i++) {
#परिभाषय name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#परिभाषय persec(n) (system_hz*(n)/ticks)
		वर्ण	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		प्रारूप_लिख("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	प्रारूप_लिख("total %d/s\n", persec(योग));
}

स्थिर शून्य sortstats(शून्य)
{
	/* Print top message senders/receivers. */
	पूर्णाङ्क src_slot, dst_slot;
	योग = goodslots = 0;
	प्रति(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		प्रति(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			पूर्णाङ्क w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			योग += n;
			यावत्(w > 0 && n > winners[w-1].messages)
				w--;
			यदि(w >= PRINTSLOTS) अनुवर्त;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			प्रतिज्ञा(rem >= 0);
			प्रतिज्ञा(rem < PRINTSLOTS);
			यदि(rem > 0) {
				प्रतिज्ञा(w+1 <= PRINTSLOTS-1);
				प्रतिज्ञा(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*परिमाण(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			यदि(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#परिभाषय proc2slot(p, s) { \
	यदि(p) { s = p->p_nr; } \
	अन्यथा { s = KERNELIPC; } \
	प्रतिज्ञा(s >= 0 && s < IPCPROCS); \
}

स्थिर शून्य statmsg(सन्देशः *सन्देशः_2, संरचना proc *srcp, संरचना proc *dstp)
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
		स्मृतिं_पूरय(winners, 0, परिमाण(winners));
		sortstats();
		printstats(dt);
		स्मृतिं_पूरय(messages, 0, परिमाण(messages));
		lastprint = now;
	}
}
#यदि_अन्तः

#यदि DEBUG_IPC_HOOK
शून्य hook_ipc_msgkcall(सन्देशः *सन्देशः_2, संरचना proc *proc)
{
#यदि DEBUG_DUMPIPC
	printmsg(सन्देशः_2, proc, रिक्त, 'k', 1);
#यदि_अन्तः
}

शून्य hook_ipc_msgkresult(सन्देशः *सन्देशः_2, संरचना proc *proc)
{
#यदि DEBUG_DUMPIPC
	printmsg(सन्देशः_2, रिक्त, proc, 'k', 0);
#यदि_अन्तः
#यदि DEBUG_IPCSTATS
	statmsg(सन्देशः_2, proc, रिक्त);
#यदि_अन्तः
}

शून्य hook_ipc_msgrecv(सन्देशः *सन्देशः_2, संरचना proc *src, संरचना proc *dst)
{
#यदि DEBUG_DUMPIPC
	printmsg(सन्देशः_2, src, dst, 'r', 0);
#यदि_अन्तः
#यदि DEBUG_IPCSTATS
	statmsg(सन्देशः_2, src, dst);
#यदि_अन्तः
}

शून्य hook_ipc_msgsend(सन्देशः *सन्देशः_2, संरचना proc *src, संरचना proc *dst)
{
#यदि DEBUG_DUMPIPC
	printmsg(सन्देशः_2, src, dst, 's', 1);
#यदि_अन्तः
}

शून्य hook_ipc_clear(संरचना proc *p)
{
#यदि DEBUG_IPCSTATS
	पूर्णाङ्क slot, i;
	प्रतिज्ञा(p);
	proc2slot(p, slot);
	प्रति(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#यदि_अन्तः
}
#यदि_अन्तः
