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

पूर्णाङ्क runqueues_ok_cpu(unsigned cpu)
{
  पूर्णाङ्क q, l = 0;
  पञ्जिका संरचना proc *xp;
  संरचना proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  प्रति (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	जइ (l++ > MAX_LOOP) panic("check error");
  }

  प्रति (q=l=0; q < NR_SCHED_QUEUES; q++) {
    जइ (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	प्रत्यागम 0;
    }
    जइ (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	प्रत्यागम 0;
    }
    जइ (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	प्रत्यागम 0;
    }
    प्रति(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	ध्रुव vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	जइ(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		प्रत्यागम 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	जइ(dxp % परिमाण(संरचना proc)) {
  		printf("xp not a real pointer");
		प्रत्यागम 0;
	}
	जइ(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		प्रत्यागम 0;
	}
	जइ (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		प्रत्यागम 0;
	}
        जइ (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		प्रत्यागम 0;
        }
        जइ (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		प्रत्यागम 0;
	}
	जइ (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		प्रत्यागम 0;
	}
	xp->p_found = 1;
	जइ (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		प्रत्यागम 0;
	}
	जइ (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		प्रत्यागम 0;
	}
    }
  }	

  प्रति (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	जइ(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		प्रत्यागम 0;
	}
	जइ (isemptyp(xp))
		अनुवर्त;
	जइ(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		प्रत्यागम 0;
	}
  }

  /* All is ok. */
  प्रत्यागम 1;
}

#ifdef CONFIG_SMP
स्थिर पूर्णाङ्क runqueues_ok_all(शून्य)
{
	unsigned c;

	प्रति (c = 0 ; c < ncpus; c++) {
		जइ (!runqueues_ok_cpu(c))
			प्रत्यागम 0;
	}
	प्रत्यागम 1;	
}

पूर्णाङ्क runqueues_ok(शून्य)
{
	प्रत्यागम runqueues_ok_all();
}

#अण्णहा

पूर्णाङ्क runqueues_ok(शून्य)
{
	प्रत्यागम runqueues_ok_cpu(0);
}


#endif

वर्ण *
rtsflagstr(ध्रुव u32_t flags)
{
	स्थिर वर्ण str[100];
	str[0] = '\0';

#define FLAG(n) जइ(flags & n) { strlcat(str, #n " ", परिमाण(str)); }

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

	प्रत्यागम str;
}

वर्ण *
miscflagstr(ध्रुव u32_t flags)
{
	स्थिर वर्ण str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	प्रत्यागम str;
}

वर्ण *
schedulerstr(संरचना proc *scheduler)
{
	जइ (scheduler != रिक्त)
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

	जइ(name) {
		printf("%s(%d)", name, ep);
	}
	अण्णहा {
		printf("%d", ep);
	}
}

स्थिर शून्य
print_endpoint(endpoint_t ep)
{
	पूर्णाङ्क proc_nr;
	संरचना proc *pp = रिक्त;

	विकल्प(ep) {
	पक्ष ANY:
		printf("ANY");
	भङ्ग;
	पक्ष SELF:
		printf("SELF");
	भङ्ग;
	पक्ष NONE:
		printf("NONE");
	भङ्ग;
	स्वभाव:
		जइ(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		अण्णहा {
			pp = proc_addr(proc_nr);
			जइ(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			अण्णहा {
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
	जइ(sig_mgr == NONE) { printf("no sigmgr"); प्रत्यागम; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	जइ(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

शून्य print_proc(संरचना proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#जइ defined(__i386__)
		pp->p_seg.p_cr3,
#नोचेद्यदि defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	जइ(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

स्थिर शून्य print_proc_depends(संरचना proc *pp, ध्रुव पूर्णाङ्क level)
{
	संरचना proc *depproc = रिक्त;
	endpoint_t dep;
#define COL { पूर्णाङ्क i; प्रति(i = 0; i < level; i++) printf("> "); }

	जइ(level >= NR_PROCS) {
		printf("loop??\n");
		प्रत्यागम;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	जइ(dep != NONE && dep != ANY) {
		पूर्णाङ्क procno;
		जइ(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			जइ(isemptyp(depproc))
				depproc = रिक्त;
		}
		जइ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

शून्य print_proc_recursive(संरचना proc *pp)
{
	print_proc_depends(pp, 0);
}

#जइ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
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
#define IDENT(x) पक्ष x: callname = #x; *possible_callname = 1; भङ्ग;
#अन्तर्भाव "kernel/extracted-mtype.h"
#undef IDENT
	}
	विकल्प(mtype) {
#define IDENT(x) पक्ष x: errname = #x; भङ्ग;
#अन्तर्भाव "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	जइ(!errname && !callname)
		प्रत्यागम रिक्त;

	/* 2 matches */
	जइ(errname && callname) {
		स्थिर वर्ण typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		प्रत्यागम typename;
	}

	जइ(errname) प्रत्यागम errname;

	प्रतिज्ञा(callname);
	प्रत्यागम callname;
}

स्थिर शून्य printproc(संरचना proc *rp)
{
	जइ (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	अण्णहा
		printf(" kernel");
}

स्थिर शून्य printparam(ध्रुव वर्ण *name, ध्रुव शून्य *data, size_t size)
{
	printf(" %s=", name);
	विकल्प (size) {
		पक्ष परिमाण(वर्ण):	printf("%d", *(वर्ण *) data);	भङ्ग;
		पक्ष परिमाण(short):	printf("%d", *(short *) data);	भङ्ग;
		पक्ष परिमाण(पूर्णाङ्क):	printf("%d", *(पूर्णाङ्क *) data);	भङ्ग;
		स्वभाव:		printf("(%u bytes)", size);	भङ्ग;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
स्थिर पूर्णाङ्क namematch(वर्ण **names, पूर्णाङ्क nnames, वर्ण *name)
{
	पूर्णाङ्क i;
	प्रति(i = 0; i < nnames; i++)
		जइ(!strcmp(names[i], name))
			प्रत्यागम 1;
	प्रत्यागम 0;
}
#endif

शून्य printmsg(message *सन्देशः, संरचना proc *src, संरचना proc *dst,
	वर्ण operation, पूर्णाङ्क printparams)
{
	ध्रुव वर्ण *name;
	पूर्णाङ्क mtype = सन्देशः->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	वर्ण *names[] = DEBUG_DUMPIPC_NAMES;
	पूर्णाङ्क nnames = परिमाण(names)/परिमाण(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	जइ(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		प्रत्यागम;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	जइ (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} अण्णहा {
		printf(" %d/0x%x", mtype, mtype);
	}

	जइ (mightbecall && printparams) {
#define IDENT(x, y) जइ (mtype == x) printparam(#y, &सन्देशः->y, परिमाण(सन्देशः->y));
#अन्तर्भाव "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#जइ DEBUG_IPCSTATS
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
	प्रति(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		वर्ण	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

स्थिर शून्य sortstats(शून्य)
{
	/* Print top message senders/receivers. */
	पूर्णाङ्क src_slot, dst_slot;
	total = goodslots = 0;
	प्रति(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		प्रति(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			पूर्णाङ्क w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			जाव(w > 0 && n > winners[w-1].messages)
				w--;
			जइ(w >= PRINTSLOTS) अनुवर्त;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			प्रतिज्ञा(rem >= 0);
			प्रतिज्ञा(rem < PRINTSLOTS);
			जइ(rem > 0) {
				प्रतिज्ञा(w+1 <= PRINTSLOTS-1);
				प्रतिज्ञा(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*परिमाण(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			जइ(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	जइ(p) { s = p->p_nr; } \
	अण्णहा { s = KERNELIPC; } \
	प्रतिज्ञा(s >= 0 && s < IPCPROCS); \
}

स्थिर शून्य statmsg(message *सन्देशः, संरचना proc *srcp, संरचना proc *dstp)
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
	जइ(secs >= 30) {
		memset(winners, 0, परिमाण(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, परिमाण(messages));
		lastprint = now;
	}
}
#endif

#जइ DEBUG_IPC_HOOK
शून्य hook_ipc_msgkcall(message *सन्देशः, संरचना proc *proc)
{
#जइ DEBUG_DUMPIPC
	printmsg(सन्देशः, proc, रिक्त, 'k', 1);
#endif
}

शून्य hook_ipc_msgkresult(message *सन्देशः, संरचना proc *proc)
{
#जइ DEBUG_DUMPIPC
	printmsg(सन्देशः, रिक्त, proc, 'k', 0);
#endif
#जइ DEBUG_IPCSTATS
	statmsg(सन्देशः, proc, रिक्त);
#endif
}

शून्य hook_ipc_msgrecv(message *सन्देशः, संरचना proc *src, संरचना proc *dst)
{
#जइ DEBUG_DUMPIPC
	printmsg(सन्देशः, src, dst, 'r', 0);
#endif
#जइ DEBUG_IPCSTATS
	statmsg(सन्देशः, src, dst);
#endif
}

शून्य hook_ipc_msgsend(message *सन्देशः, संरचना proc *src, संरचना proc *dst)
{
#जइ DEBUG_DUMPIPC
	printmsg(सन्देशः, src, dst, 's', 1);
#endif
}

शून्य hook_ipc_clear(संरचना proc *p)
{
#जइ DEBUG_IPCSTATS
	पूर्णाङ्क slot, i;
	प्रतिज्ञा(p);
	proc2slot(p, slot);
	प्रति(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
