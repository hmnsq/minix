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

#تعریف_کرو MAX_LOOP (NR_PROCS + NR_TASKS)

صحيح runqueues_ok_cpu(بنانشانی cpu)
{
  صحيح q, l = 0;
  سجل بنية proc *xp;
  بنية proc **rdy_سر, **rdy_پوچھ;

  rdy_سر = get_cpu_var(cpu, لگاتار_q_سر);
  rdy_پوچھ = get_cpu_var(cpu, لگاتار_q_پوچھ);

  لكل (xp = BEG_PROC_ADDR; xp < سرا_PROC_ADDR; ++xp) {
	xp->p_لبھیا = 0;
	إذا (l++ > MAX_LOOP) panic("check error");
  }

  لكل (q=l=0; q < NR_SCHED_QUEUES; q++) {
    إذا (rdy_سر[q] && !rdy_پوچھ[q]) {
	لکھو("head but no tail in %d\n", q);
	أرجع 0;
    }
    إذا (!rdy_سر[q] && rdy_پوچھ[q]) {
	لکھو("tail but no head in %d\n", q);
	أرجع 0;
    }
    إذا (rdy_پوچھ[q] && rdy_پوچھ[q]->p_nextready) {
	لکھو("tail and tail->next not null in %d\n", q);
	أرجع 0;
    }
    لكل(xp = rdy_سر[q]; xp; xp = xp->p_nextready) {
	ثابت vir_بائٹاں vxp = (vir_بائٹاں) xp;
	vir_بائٹاں dxp;
	إذا(vxp < (vir_بائٹاں) BEG_PROC_ADDR || vxp >= (vir_بائٹاں) سرا_PROC_ADDR) {
  		لکھو("xp out of range\n");
		أرجع 0;
	}
	dxp = vxp - (vir_بائٹاں) BEG_PROC_ADDR;
	إذا(dxp % حجم(بنية proc)) {
  		لکھو("xp not a real pointer");
		أرجع 0;
	}
	إذا(!proc_ptr_ok(xp)) {
  		لکھو("xp bogus pointer");
		أرجع 0;
	}
	إذا (RTS_ISSET(xp, RTS_تھاں_FREE)) {
		لکھو("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		أرجع 0;
	}
        إذا (!proc_is_runnable(xp)) {
		لکھو("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
        }
        إذا (xp->p_ترجیح != q) {
		لکھو("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		أرجع 0;
	}
	إذا (xp->p_لبھیا) {
		لکھو("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	xp->p_لبھیا = 1;
	إذا (!xp->p_nextready && rdy_پوچھ[q] != xp) {
		لکھو("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	إذا (l++ > MAX_LOOP) {
		لکھو("loop in schedule queue?");
		أرجع 0;
	}
    }
  }	

  لكل (xp = BEG_PROC_ADDR; xp < سرا_PROC_ADDR; ++xp) {
	إذا(!proc_ptr_ok(xp)) {
		لکھو("xp bogus pointer in proc table\n");
		أرجع 0;
	}
	إذا (isemptyp(xp))
		استمر;
	إذا(proc_is_runnable(xp) && !xp->p_لبھیا) {
		لکھو("sched error: ready proc %d not on queue\n", xp->p_nr);
		أرجع 0;
	}
  }

  /* All is ok. */
  أرجع 1;
}

#جے_تعریف_شدہ CONFIG_SMP
ساكن صحيح runqueues_ok_all(فراغ)
{
	بنانشانی c;

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


#جے_انت

حرف *
rtsflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

#تعریف_کرو FLAG(n) إذا(flags & n) { strlcat(str, #n " ", حجم(str)); }

	FLAG(RTS_تھاں_FREE);
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
	FLAG(RTS_NO_ویلا_حصہ);

	أرجع str;
}

حرف *
miscflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

	FLAG(MF_جواب_PEND);
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
		لکھو("%s(%d)", name, ep);
	}
	وإلا {
		لکھو("%d", ep);
	}
}

ساكن فراغ
print_endpoint(endpoint_t ep)
{
	صحيح proc_nr;
	بنية proc *pp = عدم;

	بدل(ep) {
	حالة ANY:
		لکھو("ANY");
	اكسر;
	حالة SELF:
		لکھو("SELF");
	اكسر;
	حالة NONE:
		لکھو("NONE");
	اكسر;
	افتراضي:
		إذا(!isokendpt(ep, &proc_nr)) {
			لکھو("??? %d\n", ep);
		}
		وإلا {
			pp = proc_addr(proc_nr);
			إذا(isemptyp(pp)) {
				لکھو("??? empty slot %d\n", proc_nr);
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
	إذا(sig_mgr == NONE) { لکھو("no sigmgr"); أرجع; }
	لکھو("sigmgr ");
	print_endpoint(sig_mgr);
	إذا(bak_sig_mgr != NONE) {
		لکھو(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

فراغ print_proc(بنية proc *pp)
{
	endpoint_t dep;

	لکھو("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_ترجیح, pp->p_user_ویلا,
		pp->p_sys_ویلا, ex64hi(pp->p_چکراں),
		ex64lo(pp->p_چکراں), pp->p_cpu,
#إذا defined(__i386__)
		pp->p_seg.p_cr3,
#وإلاإذا defined(__arm__)
		pp->p_seg.p_ttbr,
#جے_انت
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	إذا(dep != NONE) {
		لکھو(" blocked on: ");
		print_endpoint(dep);
	}
	لکھو("\n");
}

ساكن فراغ print_proc_depends(بنية proc *pp, ثابت صحيح level)
{
	بنية proc *depproc = عدم;
	endpoint_t dep;
#تعریف_کرو خانہ_2 { صحيح i; لكل(i = 0; i < level; i++) لکھو("> "); }

	إذا(level >= NR_PROCS) {
		لکھو("loop??\n");
		أرجع;
	}

	خانہ_2

	print_proc(pp);

	خانہ_2
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
#تعریف_کرو IDENT(x) حالة x: callname = #x; *possible_callname = 1; اكسر;
#ضمن "kernel/extracted-mtype.h"
#تعریف_رد IDENT
	}
	بدل(mtype) {
#تعریف_کرو IDENT(x) حالة x: errname = #x; اكسر;
#ضمن "kernel/extracted-errno.h"
#تعریف_رد IDENT
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
		لکھو(" %s(%d)", rp->p_name, rp - proc);
	وإلا
		لکھو(" kernel");
}

ساكن فراغ printparam(ثابت حرف *name, ثابت فراغ *انکڑے, size_t ناپ)
{
	لکھو(" %s=", name);
	بدل (ناپ) {
		حالة حجم(حرف):	لکھو("%d", *(حرف *) انکڑے);	اكسر;
		حالة حجم(چھوٹا):	لکھو("%d", *(چھوٹا *) انکڑے);	اكسر;
		حالة حجم(صحيح):	لکھو("%d", *(صحيح *) انکڑے);	اكسر;
		افتراضي:		لکھو("(%u bytes)", ناپ);	اكسر;
	}
}

#جے_تعریف_شدہ DEBUG_DUMPIPC_ناں
ساكن صحيح namematch(حرف **ناں, صحيح nnames, حرف *name)
{
	صحيح i;
	لكل(i = 0; i < nnames; i++)
		إذا(!strcmp(ناں[i], name))
			أرجع 1;
	أرجع 0;
}
#جے_انت

فراغ printmsg(سنیہا *پیغام, بنية proc *src, بنية proc *dst,
	حرف operation, صحيح printparams)
{
	ثابت حرف *name;
	صحيح mtype = پیغام->m_type, mightbecall = 0;

#جے_تعریف_شدہ DEBUG_DUMPIPC_ناں
  {
	حرف *ناں[] = DEBUG_DUMPIPC_ناں;
	صحيح nnames = حجم(ناں)/حجم(ناں[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	إذا(!(src && namematch(ناں, nnames, src->p_name)) &&
	   !(dst && namematch(ناں, nnames, dst->p_name))) {
		أرجع;
	}
  }
#جے_انت

	/* source, destination and message type */
	لکھو("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	إذا (name) {
		لکھو(" %s(%d/0x%x)", name, mtype, mtype);
	} وإلا {
		لکھو(" %d/0x%x", mtype, mtype);
	}

	إذا (mightbecall && printparams) {
#تعریف_کرو IDENT(x, y) إذا (mtype == x) printparam(#y, &پیغام->y, حجم(پیغام->y));
#ضمن "kernel/extracted-mfield.h"
#تعریف_رد IDENT
	}
	لکھو("\n");
}
#جے_انت

#إذا DEBUG_IPCSTATS
#تعریف_کرو IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#تعریف_کرو KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ساكن صحيح messages[IPCPROCS][IPCPROCS];

#تعریف_کرو PRINTSLOTS 20
ساكن بنية {
	صحيح src, dst, messages;
} winners[PRINTSLOTS];
ساكن صحيح کل, goodslots;

ساكن فراغ printstats(صحيح ticks)
{
	صحيح i;
	لكل(i = 0; i < goodslots; i++) {
#تعریف_کرو name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#تعریف_کرو persec(n) (system_hz*(n)/ticks)
		حرف	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		لکھو("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	لکھو("total %d/s\n", persec(کل));
}

ساكن فراغ sortstats(فراغ)
{
	/* Print top message senders/receivers. */
	صحيح src_تھاں, dst_تھاں;
	کل = goodslots = 0;
	لكل(src_تھاں = 0; src_تھاں < IPCPROCS; src_تھاں++) {
		لكل(dst_تھاں = 0; dst_تھاں < IPCPROCS; dst_تھاں++) {
			صحيح w = PRINTSLOTS, rem,
				n = messages[src_تھاں][dst_تھاں];
			کل += n;
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
			winners[w].src = src_تھاں;
			winners[w].dst = dst_تھاں;
			winners[w].messages = n;
			إذا(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#تعریف_کرو proc2slot(p, s) { \
	إذا(p) { s = p->p_nr; } \
	وإلا { s = KERNELIPC; } \
	أكد(s >= 0 && s < IPCPROCS); \
}

ساكن فراغ statmsg(سنیہا *پیغام, بنية proc *srcp, بنية proc *dstp)
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
#جے_انت

#إذا DEBUG_IPC_HOOK
فراغ hook_ipc_msgkcall(سنیہا *پیغام, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیغام, proc, عدم, 'k', 1);
#جے_انت
}

فراغ hook_ipc_msgkresult(سنیہا *پیغام, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیغام, عدم, proc, 'k', 0);
#جے_انت
#إذا DEBUG_IPCSTATS
	statmsg(پیغام, proc, عدم);
#جے_انت
}

فراغ hook_ipc_msgrecv(سنیہا *پیغام, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیغام, src, dst, 'r', 0);
#جے_انت
#إذا DEBUG_IPCSTATS
	statmsg(پیغام, src, dst);
#جے_انت
}

فراغ hook_ipc_msgsend(سنیہا *پیغام, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(پیغام, src, dst, 's', 1);
#جے_انت
}

فراغ hook_ipc_clear(بنية proc *p)
{
#إذا DEBUG_IPCSTATS
	صحيح تھاں, i;
	أكد(p);
	proc2slot(p, تھاں);
	لكل(i = 0; i < IPCPROCS; i++)
		messages[تھاں][i] = messages[i][تھاں] = 0;
#جے_انت
}
#جے_انت
