/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#అన్తర్భావ "kernel/kernel.h"

#అన్తర్భావ <minix/callnr.h>
#అన్తర్భావ <minix/u64.h>
#అన్తర్భావ <limits.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <assert.h>

#నిర్వచించు MAX_LOOP (NR_PROCS + NR_TASKS)

పూర్ణాంకం runqueues_ok_cpu(గుర్తులేని cpu)
{
  పూర్ణాంకం q, l = 0;
  నమోదు నిర్మాణం proc *xp;
  నిర్మాణం proc **rdy_తల, **rdy_తోక;

  rdy_తల = get_cpu_var(cpu, నిరంతరం_q_తల);
  rdy_తోక = get_cpu_var(cpu, నిరంతరం_q_తోక);

  కొరకు (xp = BEG_PROC_ADDR; xp < చివర_PROC_ADDR; ++xp) {
	xp->p_దొరికింది = 0;
	ఒకవేళ (l++ > MAX_LOOP) panic("check error");
  }

  కొరకు (q=l=0; q < NR_SCHED_QUEUES; q++) {
    ఒకవేళ (rdy_తల[q] && !rdy_తోక[q]) {
	రాయి("head but no tail in %d\n", q);
	తిరిగి 0;
    }
    ఒకవేళ (!rdy_తల[q] && rdy_తోక[q]) {
	రాయి("tail but no head in %d\n", q);
	తిరిగి 0;
    }
    ఒకవేళ (rdy_తోక[q] && rdy_తోక[q]->p_nextready) {
	రాయి("tail and tail->next not null in %d\n", q);
	తిరిగి 0;
    }
    కొరకు(xp = rdy_తల[q]; xp; xp = xp->p_nextready) {
	స్థిరరాశి vir_బైట్లు vxp = (vir_బైట్లు) xp;
	vir_బైట్లు dxp;
	ఒకవేళ(vxp < (vir_బైట్లు) BEG_PROC_ADDR || vxp >= (vir_బైట్లు) చివర_PROC_ADDR) {
  		రాయి("xp out of range\n");
		తిరిగి 0;
	}
	dxp = vxp - (vir_బైట్లు) BEG_PROC_ADDR;
	ఒకవేళ(dxp % పరిమాణం(నిర్మాణం proc)) {
  		రాయి("xp not a real pointer");
		తిరిగి 0;
	}
	ఒకవేళ(!proc_ptr_ok(xp)) {
  		రాయి("xp bogus pointer");
		తిరిగి 0;
	}
	ఒకవేళ (RTS_ISSET(xp, RTS_స్థానం_FREE)) {
		రాయి("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		తిరిగి 0;
	}
        ఒకవేళ (!proc_is_runnable(xp)) {
		రాయి("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		తిరిగి 0;
        }
        ఒకవేళ (xp->p_ప్రాధాన్యత != q) {
		రాయి("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		తిరిగి 0;
	}
	ఒకవేళ (xp->p_దొరికింది) {
		రాయి("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		తిరిగి 0;
	}
	xp->p_దొరికింది = 1;
	ఒకవేళ (!xp->p_nextready && rdy_తోక[q] != xp) {
		రాయి("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		తిరిగి 0;
	}
	ఒకవేళ (l++ > MAX_LOOP) {
		రాయి("loop in schedule queue?");
		తిరిగి 0;
	}
    }
  }	

  కొరకు (xp = BEG_PROC_ADDR; xp < చివర_PROC_ADDR; ++xp) {
	ఒకవేళ(!proc_ptr_ok(xp)) {
		రాయి("xp bogus pointer in proc table\n");
		తిరిగి 0;
	}
	ఒకవేళ (isemptyp(xp))
		కొనసాగు;
	ఒకవేళ(proc_is_runnable(xp) && !xp->p_దొరికింది) {
		రాయి("sched error: ready proc %d not on queue\n", xp->p_nr);
		తిరిగి 0;
	}
  }

  /* All is ok. */
  తిరిగి 1;
}

#నిర్వచితమైతే CONFIG_SMP
స్థిర పూర్ణాంకం runqueues_ok_all(శూన్య)
{
	గుర్తులేని c;

	కొరకు (c = 0 ; c < ncpus; c++) {
		ఒకవేళ (!runqueues_ok_cpu(c))
			తిరిగి 0;
	}
	తిరిగి 1;	
}

పూర్ణాంకం runqueues_ok(శూన్య)
{
	తిరిగి runqueues_ok_all();
}

#లేకపోతే

పూర్ణాంకం runqueues_ok(శూన్య)
{
	తిరిగి runqueues_ok_cpu(0);
}


#అయితే_ముగింపు

అక్షరం *
rtsflagstr(స్థిరరాశి u32_t flags)
{
	స్థిర అక్షరం str[100];
	str[0] = '\0';

#నిర్వచించు FLAG(n) ఒకవేళ(flags & n) { strlcat(str, #n " ", పరిమాణం(str)); }

	FLAG(RTS_స్థానం_FREE);
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
	FLAG(RTS_NO_కాలాంశం);

	తిరిగి str;
}

అక్షరం *
miscflagstr(స్థిరరాశి u32_t flags)
{
	స్థిర అక్షరం str[100];
	str[0] = '\0';

	FLAG(MF_జవాబు_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	తిరిగి str;
}

అక్షరం *
schedulerstr(నిర్మాణం proc *scheduler)
{
	ఒకవేళ (scheduler != ఖాళీ)
	{
		తిరిగి scheduler->p_name;
	}

	తిరిగి "KERNEL";
}

స్థిర శూన్య
print_proc_name(నిర్మాణం proc *pp)
{
	అక్షరం *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	ఒకవేళ(name) {
		రాయి("%s(%d)", name, ep);
	}
	లేకపోతే {
		రాయి("%d", ep);
	}
}

స్థిర శూన్య
print_endpoint(endpoint_t ep)
{
	పూర్ణాంకం proc_nr;
	నిర్మాణం proc *pp = ఖాళీ;

	ఎంపిక(ep) {
	స్థితి ANY:
		రాయి("ANY");
	ఆపు;
	స్థితి SELF:
		రాయి("SELF");
	ఆపు;
	స్థితి NONE:
		రాయి("NONE");
	ఆపు;
	అప్రమేయ:
		ఒకవేళ(!isokendpt(ep, &proc_nr)) {
			రాయి("??? %d\n", ep);
		}
		లేకపోతే {
			pp = proc_addr(proc_nr);
			ఒకవేళ(isemptyp(pp)) {
				రాయి("??? empty slot %d\n", proc_nr);
			}
			లేకపోతే {
				print_proc_name(pp);
			}
		}
	ఆపు;
	}
}

స్థిర శూన్య
print_sigmgr(నిర్మాణం proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	ఒకవేళ(sig_mgr == NONE) { రాయి("no sigmgr"); తిరిగి; }
	రాయి("sigmgr ");
	print_endpoint(sig_mgr);
	ఒకవేళ(bak_sig_mgr != NONE) {
		రాయి(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

శూన్య print_proc(నిర్మాణం proc *pp)
{
	endpoint_t dep;

	రాయి("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_ప్రాధాన్యత, pp->p_user_సమయం,
		pp->p_sys_సమయం, ex64hi(pp->p_చక్రాలు),
		ex64lo(pp->p_చక్రాలు), pp->p_cpu,
#ఒకవేళ defined(__i386__)
		pp->p_seg.p_cr3,
#లేదావేళ defined(__arm__)
		pp->p_seg.p_ttbr,
#అయితే_ముగింపు
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	ఒకవేళ(dep != NONE) {
		రాయి(" blocked on: ");
		print_endpoint(dep);
	}
	రాయి("\n");
}

స్థిర శూన్య print_proc_depends(నిర్మాణం proc *pp, స్థిరరాశి పూర్ణాంకం level)
{
	నిర్మాణం proc *depproc = ఖాళీ;
	endpoint_t dep;
#నిర్వచించు నిలువు_2 { పూర్ణాంకం i; కొరకు(i = 0; i < level; i++) రాయి("> "); }

	ఒకవేళ(level >= NR_PROCS) {
		రాయి("loop??\n");
		తిరిగి;
	}

	నిలువు_2

	print_proc(pp);

	నిలువు_2
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	ఒకవేళ(dep != NONE && dep != ANY) {
		పూర్ణాంకం procno;
		ఒకవేళ(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			ఒకవేళ(isemptyp(depproc))
				depproc = ఖాళీ;
		}
		ఒకవేళ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

శూన్య print_proc_recursive(నిర్మాణం proc *pp)
{
	print_proc_depends(pp, 0);
}

#ఒకవేళ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
స్థిర స్థిరరాశి అక్షరం *mtypename(పూర్ణాంకం mtype, పూర్ణాంకం *possible_callname)
{
	అక్షరం *callname = ఖాళీ, *errname = ఖాళీ;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	ఎంపిక(mtype) {
#నిర్వచించు IDENT(x) స్థితి x: callname = #x; *possible_callname = 1; ఆపు;
#అన్తర్భావ "kernel/extracted-mtype.h"
#నిర్వచనం_రద్దు IDENT
	}
	ఎంపిక(mtype) {
#నిర్వచించు IDENT(x) స్థితి x: errname = #x; ఆపు;
#అన్తర్భావ "kernel/extracted-errno.h"
#నిర్వచనం_రద్దు IDENT
	}

	/* no match */
	ఒకవేళ(!errname && !callname)
		తిరిగి ఖాళీ;

	/* 2 matches */
	ఒకవేళ(errname && callname) {
		స్థిర అక్షరం typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		తిరిగి typename;
	}

	ఒకవేళ(errname) తిరిగి errname;

	ప్రతిజ్ఞా(callname);
	తిరిగి callname;
}

స్థిర శూన్య printproc(నిర్మాణం proc *rp)
{
	ఒకవేళ (rp)
		రాయి(" %s(%d)", rp->p_name, rp - proc);
	లేకపోతే
		రాయి(" kernel");
}

స్థిర శూన్య printparam(స్థిరరాశి అక్షరం *name, స్థిరరాశి శూన్య *దత్తాంశం, size_t కొలత)
{
	రాయి(" %s=", name);
	ఎంపిక (కొలత) {
		స్థితి పరిమాణం(అక్షరం):	రాయి("%d", *(అక్షరం *) దత్తాంశం);	ఆపు;
		స్థితి పరిమాణం(పొట్టి):	రాయి("%d", *(పొట్టి *) దత్తాంశం);	ఆపు;
		స్థితి పరిమాణం(పూర్ణాంకం):	రాయి("%d", *(పూర్ణాంకం *) దత్తాంశం);	ఆపు;
		అప్రమేయ:		రాయి("(%u bytes)", కొలత);	ఆపు;
	}
}

#నిర్వచితమైతే DEBUG_DUMPIPC_పేర్లు
స్థిర పూర్ణాంకం namematch(అక్షరం **పేర్లు, పూర్ణాంకం nnames, అక్షరం *name)
{
	పూర్ణాంకం i;
	కొరకు(i = 0; i < nnames; i++)
		ఒకవేళ(!strcmp(పేర్లు[i], name))
			తిరిగి 1;
	తిరిగి 0;
}
#అయితే_ముగింపు

శూన్య printmsg(సందేశం *సందేశం_2, నిర్మాణం proc *src, నిర్మాణం proc *dst,
	అక్షరం operation, పూర్ణాంకం printparams)
{
	స్థిరరాశి అక్షరం *name;
	పూర్ణాంకం mtype = సందేశం_2->m_type, mightbecall = 0;

#నిర్వచితమైతే DEBUG_DUMPIPC_పేర్లు
  {
	అక్షరం *పేర్లు[] = DEBUG_DUMPIPC_పేర్లు;
	పూర్ణాంకం nnames = పరిమాణం(పేర్లు)/పరిమాణం(పేర్లు[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	ఒకవేళ(!(src && namematch(పేర్లు, nnames, src->p_name)) &&
	   !(dst && namematch(పేర్లు, nnames, dst->p_name))) {
		తిరిగి;
	}
  }
#అయితే_ముగింపు

	/* source, destination and message type */
	రాయి("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	ఒకవేళ (name) {
		రాయి(" %s(%d/0x%x)", name, mtype, mtype);
	} లేకపోతే {
		రాయి(" %d/0x%x", mtype, mtype);
	}

	ఒకవేళ (mightbecall && printparams) {
#నిర్వచించు IDENT(x, y) ఒకవేళ (mtype == x) printparam(#y, &సందేశం_2->y, పరిమాణం(సందేశం_2->y));
#అన్తర్భావ "kernel/extracted-mfield.h"
#నిర్వచనం_రద్దు IDENT
	}
	రాయి("\n");
}
#అయితే_ముగింపు

#ఒకవేళ DEBUG_IPCSTATS
#నిర్వచించు IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#నిర్వచించు KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
స్థిర పూర్ణాంకం messages[IPCPROCS][IPCPROCS];

#నిర్వచించు PRINTSLOTS 20
స్థిర నిర్మాణం {
	పూర్ణాంకం src, dst, messages;
} winners[PRINTSLOTS];
స్థిర పూర్ణాంకం మొత్తం, goodslots;

స్థిర శూన్య printstats(పూర్ణాంకం ticks)
{
	పూర్ణాంకం i;
	కొరకు(i = 0; i < goodslots; i++) {
#నిర్వచించు name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#నిర్వచించు persec(n) (system_hz*(n)/ticks)
		అక్షరం	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		రాయి("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	రాయి("total %d/s\n", persec(మొత్తం));
}

స్థిర శూన్య sortstats(శూన్య)
{
	/* Print top message senders/receivers. */
	పూర్ణాంకం src_స్థానం, dst_స్థానం;
	మొత్తం = goodslots = 0;
	కొరకు(src_స్థానం = 0; src_స్థానం < IPCPROCS; src_స్థానం++) {
		కొరకు(dst_స్థానం = 0; dst_స్థానం < IPCPROCS; dst_స్థానం++) {
			పూర్ణాంకం w = PRINTSLOTS, rem,
				n = messages[src_స్థానం][dst_స్థానం];
			మొత్తం += n;
			అయితే(w > 0 && n > winners[w-1].messages)
				w--;
			ఒకవేళ(w >= PRINTSLOTS) కొనసాగు;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			ప్రతిజ్ఞా(rem >= 0);
			ప్రతిజ్ఞా(rem < PRINTSLOTS);
			ఒకవేళ(rem > 0) {
				ప్రతిజ్ఞా(w+1 <= PRINTSLOTS-1);
				ప్రతిజ్ఞా(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*పరిమాణం(winners[0]));
			}
			winners[w].src = src_స్థానం;
			winners[w].dst = dst_స్థానం;
			winners[w].messages = n;
			ఒకవేళ(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#నిర్వచించు proc2slot(p, s) { \
	ఒకవేళ(p) { s = p->p_nr; } \
	లేకపోతే { s = KERNELIPC; } \
	ప్రతిజ్ఞా(s >= 0 && s < IPCPROCS); \
}

స్థిర శూన్య statmsg(సందేశం *సందేశం_2, నిర్మాణం proc *srcp, నిర్మాణం proc *dstp)
{
	పూర్ణాంకం src, dst, now, secs, dt;
	స్థిర పూర్ణాంకం lastprint;

	/* Stat message. */
	ప్రతిజ్ఞా(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	ఒకవేళ(secs >= 30) {
		memset(winners, 0, పరిమాణం(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, పరిమాణం(messages));
		lastprint = now;
	}
}
#అయితే_ముగింపు

#ఒకవేళ DEBUG_IPC_HOOK
శూన్య hook_ipc_msgkcall(సందేశం *సందేశం_2, నిర్మాణం proc *proc)
{
#ఒకవేళ DEBUG_DUMPIPC
	printmsg(సందేశం_2, proc, ఖాళీ, 'k', 1);
#అయితే_ముగింపు
}

శూన్య hook_ipc_msgkresult(సందేశం *సందేశం_2, నిర్మాణం proc *proc)
{
#ఒకవేళ DEBUG_DUMPIPC
	printmsg(సందేశం_2, ఖాళీ, proc, 'k', 0);
#అయితే_ముగింపు
#ఒకవేళ DEBUG_IPCSTATS
	statmsg(సందేశం_2, proc, ఖాళీ);
#అయితే_ముగింపు
}

శూన్య hook_ipc_msgrecv(సందేశం *సందేశం_2, నిర్మాణం proc *src, నిర్మాణం proc *dst)
{
#ఒకవేళ DEBUG_DUMPIPC
	printmsg(సందేశం_2, src, dst, 'r', 0);
#అయితే_ముగింపు
#ఒకవేళ DEBUG_IPCSTATS
	statmsg(సందేశం_2, src, dst);
#అయితే_ముగింపు
}

శూన్య hook_ipc_msgsend(సందేశం *సందేశం_2, నిర్మాణం proc *src, నిర్మాణం proc *dst)
{
#ఒకవేళ DEBUG_DUMPIPC
	printmsg(సందేశం_2, src, dst, 's', 1);
#అయితే_ముగింపు
}

శూన్య hook_ipc_clear(నిర్మాణం proc *p)
{
#ఒకవేళ DEBUG_IPCSTATS
	పూర్ణాంకం స్థానం, i;
	ప్రతిజ్ఞా(p);
	proc2slot(p, స్థానం);
	కొరకు(i = 0; i < IPCPROCS; i++)
		messages[స్థానం][i] = messages[i][స్థానం] = 0;
#అయితే_ముగింపు
}
#అయితే_ముగింపు
