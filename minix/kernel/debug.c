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

#تعریف_کن MAX_LOOP (NR_PROCS + NR_TASKS)

صحيح runqueues_ok_cpu(بیعلامت cpu)
{
  صحيح q, l = 0;
  سجل بنية proc *xp;
  بنية proc **rdy_سر, **rdy_دم;

  rdy_سر = get_cpu_var(cpu, پیاپی_q_سر);
  rdy_دم = get_cpu_var(cpu, پیاپی_q_دم);

  لكل (xp = BEG_PROC_ADDR; xp < انتها_PROC_ADDR; ++xp) {
	xp->p_یافت = 0;
	إذا (l++ > MAX_LOOP) panic("check error");
  }

  لكل (q=l=0; q < NR_SCHED_QUEUES; q++) {
    إذا (rdy_سر[q] && !rdy_دم[q]) {
	چاپ_کن("head but no tail in %d\n", q);
	أرجع 0;
    }
    إذا (!rdy_سر[q] && rdy_دم[q]) {
	چاپ_کن("tail but no head in %d\n", q);
	أرجع 0;
    }
    إذا (rdy_دم[q] && rdy_دم[q]->p_nextready) {
	چاپ_کن("tail and tail->next not null in %d\n", q);
	أرجع 0;
    }
    لكل(xp = rdy_سر[q]; xp; xp = xp->p_nextready) {
	ثابت vir_بایتها vxp = (vir_بایتها) xp;
	vir_بایتها dxp;
	إذا(vxp < (vir_بایتها) BEG_PROC_ADDR || vxp >= (vir_بایتها) انتها_PROC_ADDR) {
  		چاپ_کن("xp out of range\n");
		أرجع 0;
	}
	dxp = vxp - (vir_بایتها) BEG_PROC_ADDR;
	إذا(dxp % حجم(بنية proc)) {
  		چاپ_کن("xp not a real pointer");
		أرجع 0;
	}
	إذا(!proc_ptr_ok(xp)) {
  		چاپ_کن("xp bogus pointer");
		أرجع 0;
	}
	إذا (RTS_ISSET(xp, RTS_جایگاه_FREE)) {
		چاپ_کن("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		أرجع 0;
	}
        إذا (!proc_is_runnable(xp)) {
		چاپ_کن("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
        }
        إذا (xp->p_اولویت != q) {
		چاپ_کن("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		أرجع 0;
	}
	إذا (xp->p_یافت) {
		چاپ_کن("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	xp->p_یافت = 1;
	إذا (!xp->p_nextready && rdy_دم[q] != xp) {
		چاپ_کن("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	إذا (l++ > MAX_LOOP) {
		چاپ_کن("loop in schedule queue?");
		أرجع 0;
	}
    }
  }	

  لكل (xp = BEG_PROC_ADDR; xp < انتها_PROC_ADDR; ++xp) {
	إذا(!proc_ptr_ok(xp)) {
		چاپ_کن("xp bogus pointer in proc table\n");
		أرجع 0;
	}
	إذا (isemptyp(xp))
		استمر;
	إذا(proc_is_runnable(xp) && !xp->p_یافت) {
		چاپ_کن("sched error: ready proc %d not on queue\n", xp->p_nr);
		أرجع 0;
	}
  }

  /* All is ok. */
  أرجع 1;
}

#اگر_تعریف_شده CONFIG_SMP
ساكن صحيح runqueues_ok_all(فراغ)
{
	بیعلامت c;

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


#پایان_اگر

حرف *
rtsflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

#تعریف_کن FLAG(n) إذا(flags & n) { strlcat(str, #n " ", حجم(str)); }

	FLAG(RTS_جایگاه_FREE);
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
	FLAG(RTS_NO_برش_زمانی);

	أرجع str;
}

حرف *
miscflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

	FLAG(MF_پاسخ_PEND);
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
		چاپ_کن("%s(%d)", name, ep);
	}
	وإلا {
		چاپ_کن("%d", ep);
	}
}

ساكن فراغ
print_endpoint(endpoint_t ep)
{
	صحيح proc_nr;
	بنية proc *pp = عدم;

	بدل(ep) {
	حالة ANY:
		چاپ_کن("ANY");
	اكسر;
	حالة SELF:
		چاپ_کن("SELF");
	اكسر;
	حالة NONE:
		چاپ_کن("NONE");
	اكسر;
	افتراضي:
		إذا(!isokendpt(ep, &proc_nr)) {
			چاپ_کن("??? %d\n", ep);
		}
		وإلا {
			pp = proc_addr(proc_nr);
			إذا(isemptyp(pp)) {
				چاپ_کن("??? empty slot %d\n", proc_nr);
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
	إذا(sig_mgr == NONE) { چاپ_کن("no sigmgr"); أرجع; }
	چاپ_کن("sigmgr ");
	print_endpoint(sig_mgr);
	إذا(bak_sig_mgr != NONE) {
		چاپ_کن(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

فراغ print_proc(بنية proc *pp)
{
	endpoint_t dep;

	چاپ_کن("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_اولویت, pp->p_user_زمان,
		pp->p_sys_زمان, ex64hi(pp->p_چرخهها),
		ex64lo(pp->p_چرخهها), pp->p_cpu,
#إذا defined(__i386__)
		pp->p_seg.p_cr3,
#وإلاإذا defined(__arm__)
		pp->p_seg.p_ttbr,
#پایان_اگر
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	إذا(dep != NONE) {
		چاپ_کن(" blocked on: ");
		print_endpoint(dep);
	}
	چاپ_کن("\n");
}

ساكن فراغ print_proc_depends(بنية proc *pp, ثابت صحيح level)
{
	بنية proc *depproc = عدم;
	endpoint_t dep;
#تعریف_کن ستون_2 { صحيح i; لكل(i = 0; i < level; i++) چاپ_کن("> "); }

	إذا(level >= NR_PROCS) {
		چاپ_کن("loop??\n");
		أرجع;
	}

	ستون_2

	print_proc(pp);

	ستون_2
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
#تعریف_کن IDENT(x) حالة x: callname = #x; *possible_callname = 1; اكسر;
#ضمن "kernel/extracted-mtype.h"
#لغو_تعریف IDENT
	}
	بدل(mtype) {
#تعریف_کن IDENT(x) حالة x: errname = #x; اكسر;
#ضمن "kernel/extracted-errno.h"
#لغو_تعریف IDENT
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
		چاپ_کن(" %s(%d)", rp->p_name, rp - proc);
	وإلا
		چاپ_کن(" kernel");
}

ساكن فراغ printparam(ثابت حرف *name, ثابت فراغ *داده, size_t اندازه)
{
	چاپ_کن(" %s=", name);
	بدل (اندازه) {
		حالة حجم(حرف):	چاپ_کن("%d", *(حرف *) داده);	اكسر;
		حالة حجم(کوتاه):	چاپ_کن("%d", *(کوتاه *) داده);	اكسر;
		حالة حجم(صحيح):	چاپ_کن("%d", *(صحيح *) داده);	اكسر;
		افتراضي:		چاپ_کن("(%u bytes)", اندازه);	اكسر;
	}
}

#اگر_تعریف_شده DEBUG_DUMPIPC_نامها
ساكن صحيح namematch(حرف **نامها, صحيح nnames, حرف *name)
{
	صحيح i;
	لكل(i = 0; i < nnames; i++)
		إذا(!strcmp(نامها[i], name))
			أرجع 1;
	أرجع 0;
}
#پایان_اگر

فراغ printmsg(پیام *پیام_2, بنية proc *src, بنية proc *dst,
	حرف operation, صحيح printparams)
{
	ثابت حرف *name;
	صحيح mtype = پیام_2->m_type, mightbecall = 0;

#اگر_تعریف_شده DEBUG_DUMPIPC_نامها
  {
	حرف *نامها[] = DEBUG_DUMPIPC_نامها;
	صحيح nnames = حجم(نامها)/حجم(نامها[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	إذا(!(src && namematch(نامها, nnames, src->p_name)) &&
	   !(dst && namematch(نامها, nnames, dst->p_name))) {
		أرجع;
	}
  }
#پایان_اگر

	/* source, destination and message type */
	چاپ_کن("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	إذا (name) {
		چاپ_کن(" %s(%d/0x%x)", name, mtype, mtype);
	} وإلا {
		چاپ_کن(" %d/0x%x", mtype, mtype);
	}

	إذا (mightbecall && printparams) {
#تعریف_کن IDENT(x, y) إذا (mtype == x) printparam(#y, &پیام_2->y, حجم(پیام_2->y));
#ضمن "kernel/extracted-mfield.h"
#لغو_تعریف IDENT
	}
	چاپ_کن("\n");
}
#پایان_اگر

#إذا DEBUG_IPCSTATS
#تعریف_کن IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#تعریف_کن KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ساكن صحيح messages[IPCPROCS][IPCPROCS];

#تعریف_کن PRINTSLOTS 20
ساكن بنية {
	صحيح src, dst, messages;
} winners[PRINTSLOTS];
ساكن صحيح مجموع, goodslots;

ساكن فراغ printstats(صحيح ticks)
{
	صحيح i;
	لكل(i = 0; i < goodslots; i++) {
#تعریف_کن name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#تعریف_کن persec(n) (system_hz*(n)/ticks)
		حرف	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		چاپ_کن("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	چاپ_کن("total %d/s\n", persec(مجموع));
}

ساكن فراغ sortstats(فراغ)
{
	/* Print top message senders/receivers. */
	صحيح src_جایگاه, dst_جایگاه;
	مجموع = goodslots = 0;
	لكل(src_جایگاه = 0; src_جایگاه < IPCPROCS; src_جایگاه++) {
		لكل(dst_جایگاه = 0; dst_جایگاه < IPCPROCS; dst_جایگاه++) {
			صحيح w = PRINTSLOTS, rem,
				n = messages[src_جایگاه][dst_جایگاه];
			مجموع += n;
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
			winners[w].src = src_جایگاه;
			winners[w].dst = dst_جایگاه;
			winners[w].messages = n;
			إذا(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#تعریف_کن proc2slot(p, s) { \
	إذا(p) { s = p->p_nr; } \
	وإلا { s = KERNELIPC; } \
	أكد(s >= 0 && s < IPCPROCS); \
}

ساكن فراغ statmsg(پیام *پیام_2, بنية proc *srcp, بنية proc *dstp)
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
#پایان_اگر

#إذا DEBUG_IPC_HOOK
فراغ hook_ipc_msgkcall(پیام *پیام_2, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیام_2, proc, عدم, 'k', 1);
#پایان_اگر
}

فراغ hook_ipc_msgkresult(پیام *پیام_2, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیام_2, عدم, proc, 'k', 0);
#پایان_اگر
#إذا DEBUG_IPCSTATS
	statmsg(پیام_2, proc, عدم);
#پایان_اگر
}

فراغ hook_ipc_msgrecv(پیام *پیام_2, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیام_2, src, dst, 'r', 0);
#پایان_اگر
#إذا DEBUG_IPCSTATS
	statmsg(پیام_2, src, dst);
#پایان_اگر
}

فراغ hook_ipc_msgsend(پیام *پیام_2, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیام_2, src, dst, 's', 1);
#پایان_اگر
}

فراغ hook_ipc_clear(بنية proc *p)
{
#إذا DEBUG_IPCSTATS
	صحيح جایگاه, i;
	أكد(p);
	proc2slot(p, جایگاه);
	لكل(i = 0; i < IPCPROCS; i++)
		messages[جایگاه][i] = messages[i][جایگاه] = 0;
#پایان_اگر
}
#پایان_اگر
