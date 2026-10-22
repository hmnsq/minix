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

#تعریف_کړه MAX_LOOP (NR_PROCS + NR_TASKS)

صحيح runqueues_ok_cpu(بینښه cpu)
{
  صحيح q, l = 0;
  سجل بنية proc *xp;
  بنية proc **rdy_سر, **rdy_لکۍ;

  rdy_سر = get_cpu_var(cpu, پرلپسې_q_سر);
  rdy_لکۍ = get_cpu_var(cpu, پرلپسې_q_لکۍ);

  لكل (xp = BEG_PROC_ADDR; xp < پای_ټکی_PROC_ADDR; ++xp) {
	xp->p_وموندل = 0;
	إذا (l++ > MAX_LOOP) panic("check error");
  }

  لكل (q=l=0; q < NR_SCHED_QUEUES; q++) {
    إذا (rdy_سر[q] && !rdy_لکۍ[q]) {
	چاپ_کړه("head but no tail in %d\n", q);
	أرجع 0;
    }
    إذا (!rdy_سر[q] && rdy_لکۍ[q]) {
	چاپ_کړه("tail but no head in %d\n", q);
	أرجع 0;
    }
    إذا (rdy_لکۍ[q] && rdy_لکۍ[q]->p_nextready) {
	چاپ_کړه("tail and tail->next not null in %d\n", q);
	أرجع 0;
    }
    لكل(xp = rdy_سر[q]; xp; xp = xp->p_nextready) {
	ثابت vir_بایټونه vxp = (vir_بایټونه) xp;
	vir_بایټونه dxp;
	إذا(vxp < (vir_بایټونه) BEG_PROC_ADDR || vxp >= (vir_بایټونه) پای_ټکی_PROC_ADDR) {
  		چاپ_کړه("xp out of range\n");
		أرجع 0;
	}
	dxp = vxp - (vir_بایټونه) BEG_PROC_ADDR;
	إذا(dxp % حجم(بنية proc)) {
  		چاپ_کړه("xp not a real pointer");
		أرجع 0;
	}
	إذا(!proc_ptr_ok(xp)) {
  		چاپ_کړه("xp bogus pointer");
		أرجع 0;
	}
	إذا (RTS_ISSET(xp, RTS_خونه_FREE)) {
		چاپ_کړه("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		أرجع 0;
	}
        إذا (!proc_is_runnable(xp)) {
		چاپ_کړه("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
        }
        إذا (xp->p_لومړیتوب != q) {
		چاپ_کړه("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		أرجع 0;
	}
	إذا (xp->p_وموندل) {
		چاپ_کړه("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	xp->p_وموندل = 1;
	إذا (!xp->p_nextready && rdy_لکۍ[q] != xp) {
		چاپ_کړه("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	إذا (l++ > MAX_LOOP) {
		چاپ_کړه("loop in schedule queue?");
		أرجع 0;
	}
    }
  }	

  لكل (xp = BEG_PROC_ADDR; xp < پای_ټکی_PROC_ADDR; ++xp) {
	إذا(!proc_ptr_ok(xp)) {
		چاپ_کړه("xp bogus pointer in proc table\n");
		أرجع 0;
	}
	إذا (isemptyp(xp))
		استمر;
	إذا(proc_is_runnable(xp) && !xp->p_وموندل) {
		چاپ_کړه("sched error: ready proc %d not on queue\n", xp->p_nr);
		أرجع 0;
	}
  }

  /* All is ok. */
  أرجع 1;
}

#که_تعریف_شوی CONFIG_SMP
ساكن صحيح runqueues_ok_all(فراغ)
{
	بینښه c;

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


#د_که_پای

حرف *
rtsflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

#تعریف_کړه FLAG(n) إذا(flags & n) { strlcat(str, #n " ", حجم(str)); }

	FLAG(RTS_خونه_FREE);
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
	FLAG(RTS_NO_وخت_ټوټه);

	أرجع str;
}

حرف *
miscflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

	FLAG(MF_ځواب_PEND);
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
		چاپ_کړه("%s(%d)", name, ep);
	}
	وإلا {
		چاپ_کړه("%d", ep);
	}
}

ساكن فراغ
print_endpoint(endpoint_t ep)
{
	صحيح proc_nr;
	بنية proc *pp = عدم;

	بدل(ep) {
	حالة ANY:
		چاپ_کړه("ANY");
	اكسر;
	حالة SELF:
		چاپ_کړه("SELF");
	اكسر;
	حالة NONE:
		چاپ_کړه("NONE");
	اكسر;
	افتراضي:
		إذا(!isokendpt(ep, &proc_nr)) {
			چاپ_کړه("??? %d\n", ep);
		}
		وإلا {
			pp = proc_addr(proc_nr);
			إذا(isemptyp(pp)) {
				چاپ_کړه("??? empty slot %d\n", proc_nr);
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
	إذا(sig_mgr == NONE) { چاپ_کړه("no sigmgr"); أرجع; }
	چاپ_کړه("sigmgr ");
	print_endpoint(sig_mgr);
	إذا(bak_sig_mgr != NONE) {
		چاپ_کړه(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

فراغ print_proc(بنية proc *pp)
{
	endpoint_t dep;

	چاپ_کړه("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_لومړیتوب, pp->p_user_وخت,
		pp->p_sys_وخت, ex64hi(pp->p_دورې),
		ex64lo(pp->p_دورې), pp->p_cpu,
#إذا defined(__i386__)
		pp->p_seg.p_cr3,
#وإلاإذا defined(__arm__)
		pp->p_seg.p_ttbr,
#د_که_پای
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	إذا(dep != NONE) {
		چاپ_کړه(" blocked on: ");
		print_endpoint(dep);
	}
	چاپ_کړه("\n");
}

ساكن فراغ print_proc_depends(بنية proc *pp, ثابت صحيح level)
{
	بنية proc *depproc = عدم;
	endpoint_t dep;
#تعریف_کړه ستون_2 { صحيح i; لكل(i = 0; i < level; i++) چاپ_کړه("> "); }

	إذا(level >= NR_PROCS) {
		چاپ_کړه("loop??\n");
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
#تعریف_کړه IDENT(x) حالة x: callname = #x; *possible_callname = 1; اكسر;
#ضمن "kernel/extracted-mtype.h"
#تعریف_لغوه IDENT
	}
	بدل(mtype) {
#تعریف_کړه IDENT(x) حالة x: errname = #x; اكسر;
#ضمن "kernel/extracted-errno.h"
#تعریف_لغوه IDENT
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
		چاپ_کړه(" %s(%d)", rp->p_name, rp - proc);
	وإلا
		چاپ_کړه(" kernel");
}

ساكن فراغ printparam(ثابت حرف *name, ثابت فراغ *معلومات, size_t اندازه)
{
	چاپ_کړه(" %s=", name);
	بدل (اندازه) {
		حالة حجم(حرف):	چاپ_کړه("%d", *(حرف *) معلومات);	اكسر;
		حالة حجم(لنډ):	چاپ_کړه("%d", *(لنډ *) معلومات);	اكسر;
		حالة حجم(صحيح):	چاپ_کړه("%d", *(صحيح *) معلومات);	اكسر;
		افتراضي:		چاپ_کړه("(%u bytes)", اندازه);	اكسر;
	}
}

#که_تعریف_شوی DEBUG_DUMPIPC_نومونه
ساكن صحيح namematch(حرف **نومونه, صحيح nnames, حرف *name)
{
	صحيح i;
	لكل(i = 0; i < nnames; i++)
		إذا(!strcmp(نومونه[i], name))
			أرجع 1;
	أرجع 0;
}
#د_که_پای

فراغ printmsg(پیغام *پیغام_2, بنية proc *src, بنية proc *dst,
	حرف operation, صحيح printparams)
{
	ثابت حرف *name;
	صحيح mtype = پیغام_2->m_type, mightbecall = 0;

#که_تعریف_شوی DEBUG_DUMPIPC_نومونه
  {
	حرف *نومونه[] = DEBUG_DUMPIPC_نومونه;
	صحيح nnames = حجم(نومونه)/حجم(نومونه[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	إذا(!(src && namematch(نومونه, nnames, src->p_name)) &&
	   !(dst && namematch(نومونه, nnames, dst->p_name))) {
		أرجع;
	}
  }
#د_که_پای

	/* source, destination and message type */
	چاپ_کړه("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	إذا (name) {
		چاپ_کړه(" %s(%d/0x%x)", name, mtype, mtype);
	} وإلا {
		چاپ_کړه(" %d/0x%x", mtype, mtype);
	}

	إذا (mightbecall && printparams) {
#تعریف_کړه IDENT(x, y) إذا (mtype == x) printparam(#y, &پیغام_2->y, حجم(پیغام_2->y));
#ضمن "kernel/extracted-mfield.h"
#تعریف_لغوه IDENT
	}
	چاپ_کړه("\n");
}
#د_که_پای

#إذا DEBUG_IPCSTATS
#تعریف_کړه IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#تعریف_کړه KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ساكن صحيح messages[IPCPROCS][IPCPROCS];

#تعریف_کړه PRINTSLOTS 20
ساكن بنية {
	صحيح src, dst, messages;
} winners[PRINTSLOTS];
ساكن صحيح ټول, goodslots;

ساكن فراغ printstats(صحيح ticks)
{
	صحيح i;
	لكل(i = 0; i < goodslots; i++) {
#تعریف_کړه name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#تعریف_کړه persec(n) (system_hz*(n)/ticks)
		حرف	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		چاپ_کړه("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	چاپ_کړه("total %d/s\n", persec(ټول));
}

ساكن فراغ sortstats(فراغ)
{
	/* Print top message senders/receivers. */
	صحيح src_خونه, dst_خونه;
	ټول = goodslots = 0;
	لكل(src_خونه = 0; src_خونه < IPCPROCS; src_خونه++) {
		لكل(dst_خونه = 0; dst_خونه < IPCPROCS; dst_خونه++) {
			صحيح w = PRINTSLOTS, rem,
				n = messages[src_خونه][dst_خونه];
			ټول += n;
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
			winners[w].src = src_خونه;
			winners[w].dst = dst_خونه;
			winners[w].messages = n;
			إذا(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#تعریف_کړه proc2slot(p, s) { \
	إذا(p) { s = p->p_nr; } \
	وإلا { s = KERNELIPC; } \
	أكد(s >= 0 && s < IPCPROCS); \
}

ساكن فراغ statmsg(پیغام *پیغام_2, بنية proc *srcp, بنية proc *dstp)
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
#د_که_پای

#إذا DEBUG_IPC_HOOK
فراغ hook_ipc_msgkcall(پیغام *پیغام_2, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیغام_2, proc, عدم, 'k', 1);
#د_که_پای
}

فراغ hook_ipc_msgkresult(پیغام *پیغام_2, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیغام_2, عدم, proc, 'k', 0);
#د_که_پای
#إذا DEBUG_IPCSTATS
	statmsg(پیغام_2, proc, عدم);
#د_که_پای
}

فراغ hook_ipc_msgrecv(پیغام *پیغام_2, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیغام_2, src, dst, 'r', 0);
#د_که_پای
#إذا DEBUG_IPCSTATS
	statmsg(پیغام_2, src, dst);
#د_که_پای
}

فراغ hook_ipc_msgsend(پیغام *پیغام_2, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیغام_2, src, dst, 's', 1);
#د_که_پای
}

فراغ hook_ipc_clear(بنية proc *p)
{
#إذا DEBUG_IPCSTATS
	صحيح خونه, i;
	أكد(p);
	proc2slot(p, خونه);
	لكل(i = 0; i < IPCPROCS; i++)
		messages[خونه][i] = messages[i][خونه] = 0;
#د_که_پای
}
#د_که_پای
