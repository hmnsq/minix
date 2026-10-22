/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#شامل "kernel/kernel.h"

#شامل <minix/callnr.h>
#شامل <minix/u64.h>
#شامل <limits.h>
#شامل <string.h>
#شامل <assert.h>

#تعریف_کرو MAX_LOOP (NR_PROCS + NR_TASKS)

صحیح runqueues_ok_cpu(غیرعلامتی cpu)
{
  صحیح q, l = 0;
  اندراج ساخت proc *xp;
  ساخت proc **rdy_سر, **rdy_دم;

  rdy_سر = get_cpu_var(cpu, تسلسل_q_سر);
  rdy_دم = get_cpu_var(cpu, تسلسل_q_دم);

  برائے (xp = BEG_PROC_ADDR; xp < انتہا_PROC_ADDR; ++xp) {
	xp->p_ملا = 0;
	اگر (l++ > MAX_LOOP) panic("check error");
  }

  برائے (q=l=0; q < NR_SCHED_QUEUES; q++) {
    اگر (rdy_سر[q] && !rdy_دم[q]) {
	لکھو("head but no tail in %d\n", q);
	واپس 0;
    }
    اگر (!rdy_سر[q] && rdy_دم[q]) {
	لکھو("tail but no head in %d\n", q);
	واپس 0;
    }
    اگر (rdy_دم[q] && rdy_دم[q]->p_nextready) {
	لکھو("tail and tail->next not null in %d\n", q);
	واپس 0;
    }
    برائے(xp = rdy_سر[q]; xp; xp = xp->p_nextready) {
	ثابت vir_بائٹیں vxp = (vir_بائٹیں) xp;
	vir_بائٹیں dxp;
	اگر(vxp < (vir_بائٹیں) BEG_PROC_ADDR || vxp >= (vir_بائٹیں) انتہا_PROC_ADDR) {
  		لکھو("xp out of range\n");
		واپس 0;
	}
	dxp = vxp - (vir_بائٹیں) BEG_PROC_ADDR;
	اگر(dxp % حجم(ساخت proc)) {
  		لکھو("xp not a real pointer");
		واپس 0;
	}
	اگر(!proc_ptr_ok(xp)) {
  		لکھو("xp bogus pointer");
		واپس 0;
	}
	اگر (RTS_ISSET(xp, RTS_خانہ_FREE)) {
		لکھو("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		واپس 0;
	}
        اگر (!proc_is_runnable(xp)) {
		لکھو("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		واپس 0;
        }
        اگر (xp->p_ترجیح != q) {
		لکھو("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		واپس 0;
	}
	اگر (xp->p_ملا) {
		لکھو("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		واپس 0;
	}
	xp->p_ملا = 1;
	اگر (!xp->p_nextready && rdy_دم[q] != xp) {
		لکھو("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		واپس 0;
	}
	اگر (l++ > MAX_LOOP) {
		لکھو("loop in schedule queue?");
		واپس 0;
	}
    }
  }	

  برائے (xp = BEG_PROC_ADDR; xp < انتہا_PROC_ADDR; ++xp) {
	اگر(!proc_ptr_ok(xp)) {
		لکھو("xp bogus pointer in proc table\n");
		واپس 0;
	}
	اگر (isemptyp(xp))
		جاری;
	اگر(proc_is_runnable(xp) && !xp->p_ملا) {
		لکھو("sched error: ready proc %d not on queue\n", xp->p_nr);
		واپس 0;
	}
  }

  /* All is ok. */
  واپس 1;
}

#اگر_تعریف_شدہ CONFIG_SMP
ساکن صحیح runqueues_ok_all(خالی)
{
	غیرعلامتی c;

	برائے (c = 0 ; c < ncpus; c++) {
		اگر (!runqueues_ok_cpu(c))
			واپس 0;
	}
	واپس 1;	
}

صحیح runqueues_ok(خالی)
{
	واپس runqueues_ok_all();
}

#ورنہ

صحیح runqueues_ok(خالی)
{
	واپس runqueues_ok_cpu(0);
}


#اگر_ختم

حرف *
rtsflagstr(ثابت u32_t flags)
{
	ساکن حرف str[100];
	str[0] = '\0';

#تعریف_کرو FLAG(n) اگر(flags & n) { strlcat(str, #n " ", حجم(str)); }

	FLAG(RTS_خانہ_FREE);
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
	FLAG(RTS_NO_وقتی_حصہ);

	واپس str;
}

حرف *
miscflagstr(ثابت u32_t flags)
{
	ساکن حرف str[100];
	str[0] = '\0';

	FLAG(MF_جواب_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	واپس str;
}

حرف *
schedulerstr(ساخت proc *scheduler)
{
	اگر (scheduler != تہی)
	{
		واپس scheduler->p_name;
	}

	واپس "KERNEL";
}

ساکن خالی
print_proc_name(ساخت proc *pp)
{
	حرف *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	اگر(name) {
		لکھو("%s(%d)", name, ep);
	}
	ورنہ {
		لکھو("%d", ep);
	}
}

ساکن خالی
print_endpoint(endpoint_t ep)
{
	صحیح proc_nr;
	ساخت proc *pp = تہی;

	انتخاب(ep) {
	حالت ANY:
		لکھو("ANY");
	توڑو;
	حالت SELF:
		لکھو("SELF");
	توڑو;
	حالت NONE:
		لکھو("NONE");
	توڑو;
	طےشدہ:
		اگر(!isokendpt(ep, &proc_nr)) {
			لکھو("??? %d\n", ep);
		}
		ورنہ {
			pp = proc_addr(proc_nr);
			اگر(isemptyp(pp)) {
				لکھو("??? empty slot %d\n", proc_nr);
			}
			ورنہ {
				print_proc_name(pp);
			}
		}
	توڑو;
	}
}

ساکن خالی
print_sigmgr(ساخت proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	اگر(sig_mgr == NONE) { لکھو("no sigmgr"); واپس; }
	لکھو("sigmgr ");
	print_endpoint(sig_mgr);
	اگر(bak_sig_mgr != NONE) {
		لکھو(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

خالی print_proc(ساخت proc *pp)
{
	endpoint_t dep;

	لکھو("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_ترجیح, pp->p_user_وقت,
		pp->p_sys_وقت, ex64hi(pp->p_ادوار),
		ex64lo(pp->p_ادوار), pp->p_cpu,
#اگر defined(__i386__)
		pp->p_seg.p_cr3,
#وگرنہ_اگر defined(__arm__)
		pp->p_seg.p_ttbr,
#اگر_ختم
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	اگر(dep != NONE) {
		لکھو(" blocked on: ");
		print_endpoint(dep);
	}
	لکھو("\n");
}

ساکن خالی print_proc_depends(ساخت proc *pp, ثابت صحیح level)
{
	ساخت proc *depproc = تہی;
	endpoint_t dep;
#تعریف_کرو ستون_2 { صحیح i; برائے(i = 0; i < level; i++) لکھو("> "); }

	اگر(level >= NR_PROCS) {
		لکھو("loop??\n");
		واپس;
	}

	ستون_2

	print_proc(pp);

	ستون_2
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	اگر(dep != NONE && dep != ANY) {
		صحیح procno;
		اگر(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			اگر(isemptyp(depproc))
				depproc = تہی;
		}
		اگر (depproc)
			print_proc_depends(depproc, level+1);
	}
}

خالی print_proc_recursive(ساخت proc *pp)
{
	print_proc_depends(pp, 0);
}

#اگر DEBUG_DUMPIPC || DEBUG_DUMPIPCF
ساکن ثابت حرف *mtypename(صحیح mtype, صحیح *possible_callname)
{
	حرف *callname = تہی, *errname = تہی;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	انتخاب(mtype) {
#تعریف_کرو IDENT(x) حالت x: callname = #x; *possible_callname = 1; توڑو;
#شامل "kernel/extracted-mtype.h"
#تعریف_منسوخ IDENT
	}
	انتخاب(mtype) {
#تعریف_کرو IDENT(x) حالت x: errname = #x; توڑو;
#شامل "kernel/extracted-errno.h"
#تعریف_منسوخ IDENT
	}

	/* no match */
	اگر(!errname && !callname)
		واپس تہی;

	/* 2 matches */
	اگر(errname && callname) {
		ساکن حرف typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		واپس typename;
	}

	اگر(errname) واپس errname;

	أكد(callname);
	واپس callname;
}

ساکن خالی printproc(ساخت proc *rp)
{
	اگر (rp)
		لکھو(" %s(%d)", rp->p_name, rp - proc);
	ورنہ
		لکھو(" kernel");
}

ساکن خالی printparam(ثابت حرف *name, ثابت خالی *ڈیٹا, size_t سائز)
{
	لکھو(" %s=", name);
	انتخاب (سائز) {
		حالت حجم(حرف):	لکھو("%d", *(حرف *) ڈیٹا);	توڑو;
		حالت حجم(مختصر):	لکھو("%d", *(مختصر *) ڈیٹا);	توڑو;
		حالت حجم(صحیح):	لکھو("%d", *(صحیح *) ڈیٹا);	توڑو;
		طےشدہ:		لکھو("(%u bytes)", سائز);	توڑو;
	}
}

#اگر_تعریف_شدہ DEBUG_DUMPIPC_نام
ساکن صحیح namematch(حرف **نام, صحیح nnames, حرف *name)
{
	صحیح i;
	برائے(i = 0; i < nnames; i++)
		اگر(!strcmp(نام[i], name))
			واپس 1;
	واپس 0;
}
#اگر_ختم

خالی printmsg(پیغام *پیغام_2, ساخت proc *src, ساخت proc *dst,
	حرف operation, صحیح printparams)
{
	ثابت حرف *name;
	صحیح mtype = پیغام_2->m_type, mightbecall = 0;

#اگر_تعریف_شدہ DEBUG_DUMPIPC_نام
  {
	حرف *نام[] = DEBUG_DUMPIPC_نام;
	صحیح nnames = حجم(نام)/حجم(نام[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	اگر(!(src && namematch(نام, nnames, src->p_name)) &&
	   !(dst && namematch(نام, nnames, dst->p_name))) {
		واپس;
	}
  }
#اگر_ختم

	/* source, destination and message type */
	لکھو("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	اگر (name) {
		لکھو(" %s(%d/0x%x)", name, mtype, mtype);
	} ورنہ {
		لکھو(" %d/0x%x", mtype, mtype);
	}

	اگر (mightbecall && printparams) {
#تعریف_کرو IDENT(x, y) اگر (mtype == x) printparam(#y, &پیغام_2->y, حجم(پیغام_2->y));
#شامل "kernel/extracted-mfield.h"
#تعریف_منسوخ IDENT
	}
	لکھو("\n");
}
#اگر_ختم

#اگر DEBUG_IPCSTATS
#تعریف_کرو IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#تعریف_کرو KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ساکن صحیح messages[IPCPROCS][IPCPROCS];

#تعریف_کرو PRINTSLOTS 20
ساکن ساخت {
	صحیح src, dst, messages;
} winners[PRINTSLOTS];
ساکن صحیح کل, goodslots;

ساکن خالی printstats(صحیح ticks)
{
	صحیح i;
	برائے(i = 0; i < goodslots; i++) {
#تعریف_کرو name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#تعریف_کرو persec(n) (system_hz*(n)/ticks)
		حرف	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		لکھو("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	لکھو("total %d/s\n", persec(کل));
}

ساکن خالی sortstats(خالی)
{
	/* Print top message senders/receivers. */
	صحیح src_خانہ, dst_خانہ;
	کل = goodslots = 0;
	برائے(src_خانہ = 0; src_خانہ < IPCPROCS; src_خانہ++) {
		برائے(dst_خانہ = 0; dst_خانہ < IPCPROCS; dst_خانہ++) {
			صحیح w = PRINTSLOTS, rem,
				n = messages[src_خانہ][dst_خانہ];
			کل += n;
			جب(w > 0 && n > winners[w-1].messages)
				w--;
			اگر(w >= PRINTSLOTS) جاری;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			أكد(rem >= 0);
			أكد(rem < PRINTSLOTS);
			اگر(rem > 0) {
				أكد(w+1 <= PRINTSLOTS-1);
				أكد(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*حجم(winners[0]));
			}
			winners[w].src = src_خانہ;
			winners[w].dst = dst_خانہ;
			winners[w].messages = n;
			اگر(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#تعریف_کرو proc2slot(p, s) { \
	اگر(p) { s = p->p_nr; } \
	ورنہ { s = KERNELIPC; } \
	أكد(s >= 0 && s < IPCPROCS); \
}

ساکن خالی statmsg(پیغام *پیغام_2, ساخت proc *srcp, ساخت proc *dstp)
{
	صحیح src, dst, now, secs, dt;
	ساکن صحیح lastprint;

	/* Stat message. */
	أكد(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	اگر(secs >= 30) {
		memset(winners, 0, حجم(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, حجم(messages));
		lastprint = now;
	}
}
#اگر_ختم

#اگر DEBUG_IPC_HOOK
خالی hook_ipc_msgkcall(پیغام *پیغام_2, ساخت proc *proc)
{
#اگر DEBUG_DUMPIPC
	printmsg(پیغام_2, proc, تہی, 'k', 1);
#اگر_ختم
}

خالی hook_ipc_msgkresult(پیغام *پیغام_2, ساخت proc *proc)
{
#اگر DEBUG_DUMPIPC
	printmsg(پیغام_2, تہی, proc, 'k', 0);
#اگر_ختم
#اگر DEBUG_IPCSTATS
	statmsg(پیغام_2, proc, تہی);
#اگر_ختم
}

خالی hook_ipc_msgrecv(پیغام *پیغام_2, ساخت proc *src, ساخت proc *dst)
{
#اگر DEBUG_DUMPIPC
	printmsg(پیغام_2, src, dst, 'r', 0);
#اگر_ختم
#اگر DEBUG_IPCSTATS
	statmsg(پیغام_2, src, dst);
#اگر_ختم
}

خالی hook_ipc_msgsend(پیغام *پیغام_2, ساخت proc *src, ساخت proc *dst)
{
#اگر DEBUG_DUMPIPC
	printmsg(پیغام_2, src, dst, 's', 1);
#اگر_ختم
}

خالی hook_ipc_clear(ساخت proc *p)
{
#اگر DEBUG_IPCSTATS
	صحیح خانہ, i;
	أكد(p);
	proc2slot(p, خانہ);
	برائے(i = 0; i < IPCPROCS; i++)
		messages[خانہ][i] = messages[i][خانہ] = 0;
#اگر_ختم
}
#اگر_ختم
