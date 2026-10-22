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

#وصف_ڪريو MAX_LOOP (NR_PROCS + NR_TASKS)

صحيح runqueues_ok_cpu(بنانشان cpu)
{
  صحيح q, l = 0;
  سجل بنية proc *xp;
  بنية proc **rdy_سر, **rdy_پڇ;

  rdy_سر = get_cpu_var(cpu, لاڳيتو_q_سر);
  rdy_پڇ = get_cpu_var(cpu, لاڳيتو_q_پڇ);

  لكل (xp = BEG_PROC_ADDR; xp < پڇاڙي_PROC_ADDR; ++xp) {
	xp->p_مليو = 0;
	إذا (l++ > MAX_LOOP) panic("check error");
  }

  لكل (q=l=0; q < NR_SCHED_QUEUES; q++) {
    إذا (rdy_سر[q] && !rdy_پڇ[q]) {
	لکو("head but no tail in %d\n", q);
	أرجع 0;
    }
    إذا (!rdy_سر[q] && rdy_پڇ[q]) {
	لکو("tail but no head in %d\n", q);
	أرجع 0;
    }
    إذا (rdy_پڇ[q] && rdy_پڇ[q]->p_nextready) {
	لکو("tail and tail->next not null in %d\n", q);
	أرجع 0;
    }
    لكل(xp = rdy_سر[q]; xp; xp = xp->p_nextready) {
	ثابت vir_بائيٽون vxp = (vir_بائيٽون) xp;
	vir_بائيٽون dxp;
	إذا(vxp < (vir_بائيٽون) BEG_PROC_ADDR || vxp >= (vir_بائيٽون) پڇاڙي_PROC_ADDR) {
  		لکو("xp out of range\n");
		أرجع 0;
	}
	dxp = vxp - (vir_بائيٽون) BEG_PROC_ADDR;
	إذا(dxp % حجم(بنية proc)) {
  		لکو("xp not a real pointer");
		أرجع 0;
	}
	إذا(!proc_ptr_ok(xp)) {
  		لکو("xp bogus pointer");
		أرجع 0;
	}
	إذا (RTS_ISSET(xp, RTS_جڳهه_FREE)) {
		لکو("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		أرجع 0;
	}
        إذا (!proc_is_runnable(xp)) {
		لکو("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
        }
        إذا (xp->p_ترجيح != q) {
		لکو("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		أرجع 0;
	}
	إذا (xp->p_مليو) {
		لکو("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	xp->p_مليو = 1;
	إذا (!xp->p_nextready && rdy_پڇ[q] != xp) {
		لکو("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		أرجع 0;
	}
	إذا (l++ > MAX_LOOP) {
		لکو("loop in schedule queue?");
		أرجع 0;
	}
    }
  }	

  لكل (xp = BEG_PROC_ADDR; xp < پڇاڙي_PROC_ADDR; ++xp) {
	إذا(!proc_ptr_ok(xp)) {
		لکو("xp bogus pointer in proc table\n");
		أرجع 0;
	}
	إذا (isemptyp(xp))
		استمر;
	إذا(proc_is_runnable(xp) && !xp->p_مليو) {
		لکو("sched error: ready proc %d not on queue\n", xp->p_nr);
		أرجع 0;
	}
  }

  /* All is ok. */
  أرجع 1;
}

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
ساكن صحيح runqueues_ok_all(فراغ)
{
	بنانشان c;

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


#جيڪڏهن_پورو

حرف *
rtsflagstr(ثابت u32_t flags)
{
	ساكن حرف str[100];
	str[0] = '\0';

#وصف_ڪريو FLAG(n) إذا(flags & n) { strlcat(str, #n " ", حجم(str)); }

	FLAG(RTS_جڳهه_FREE);
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
	FLAG(RTS_NO_وقتي_حصو);

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
		لکو("%s(%d)", name, ep);
	}
	وإلا {
		لکو("%d", ep);
	}
}

ساكن فراغ
print_endpoint(endpoint_t ep)
{
	صحيح proc_nr;
	بنية proc *pp = عدم;

	بدل(ep) {
	حالة ANY:
		لکو("ANY");
	اكسر;
	حالة SELF:
		لکو("SELF");
	اكسر;
	حالة NONE:
		لکو("NONE");
	اكسر;
	افتراضي:
		إذا(!isokendpt(ep, &proc_nr)) {
			لکو("??? %d\n", ep);
		}
		وإلا {
			pp = proc_addr(proc_nr);
			إذا(isemptyp(pp)) {
				لکو("??? empty slot %d\n", proc_nr);
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
	إذا(sig_mgr == NONE) { لکو("no sigmgr"); أرجع; }
	لکو("sigmgr ");
	print_endpoint(sig_mgr);
	إذا(bak_sig_mgr != NONE) {
		لکو(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

فراغ print_proc(بنية proc *pp)
{
	endpoint_t dep;

	لکو("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_ترجيح, pp->p_user_وقت,
		pp->p_sys_وقت, ex64hi(pp->p_چڪرن),
		ex64lo(pp->p_چڪرن), pp->p_cpu,
#إذا defined(__i386__)
		pp->p_seg.p_cr3,
#وإلاإذا defined(__arm__)
		pp->p_seg.p_ttbr,
#جيڪڏهن_پورو
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	إذا(dep != NONE) {
		لکو(" blocked on: ");
		print_endpoint(dep);
	}
	لکو("\n");
}

ساكن فراغ print_proc_depends(بنية proc *pp, ثابت صحيح level)
{
	بنية proc *depproc = عدم;
	endpoint_t dep;
#وصف_ڪريو کانو_2 { صحيح i; لكل(i = 0; i < level; i++) لکو("> "); }

	إذا(level >= NR_PROCS) {
		لکو("loop??\n");
		أرجع;
	}

	کانو_2

	print_proc(pp);

	کانو_2
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
#وصف_ڪريو IDENT(x) حالة x: callname = #x; *possible_callname = 1; اكسر;
#ضمن "kernel/extracted-mtype.h"
#وصف_رد IDENT
	}
	بدل(mtype) {
#وصف_ڪريو IDENT(x) حالة x: errname = #x; اكسر;
#ضمن "kernel/extracted-errno.h"
#وصف_رد IDENT
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
		لکو(" %s(%d)", rp->p_name, rp - proc);
	وإلا
		لکو(" kernel");
}

ساكن فراغ printparam(ثابت حرف *name, ثابت فراغ *معلومات, size_t سائيز)
{
	لکو(" %s=", name);
	بدل (سائيز) {
		حالة حجم(حرف):	لکو("%d", *(حرف *) معلومات);	اكسر;
		حالة حجم(ننڍو):	لکو("%d", *(ننڍو *) معلومات);	اكسر;
		حالة حجم(صحيح):	لکو("%d", *(صحيح *) معلومات);	اكسر;
		افتراضي:		لکو("(%u bytes)", سائيز);	اكسر;
	}
}

#جيڪڏهن_وصف_ٿيل DEBUG_DUMPIPC_نالا
ساكن صحيح namematch(حرف **نالا, صحيح nnames, حرف *name)
{
	صحيح i;
	لكل(i = 0; i < nnames; i++)
		إذا(!strcmp(نالا[i], name))
			أرجع 1;
	أرجع 0;
}
#جيڪڏهن_پورو

فراغ printmsg(پيغام *نياپو, بنية proc *src, بنية proc *dst,
	حرف operation, صحيح printparams)
{
	ثابت حرف *name;
	صحيح mtype = نياپو->m_type, mightbecall = 0;

#جيڪڏهن_وصف_ٿيل DEBUG_DUMPIPC_نالا
  {
	حرف *نالا[] = DEBUG_DUMPIPC_نالا;
	صحيح nnames = حجم(نالا)/حجم(نالا[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	إذا(!(src && namematch(نالا, nnames, src->p_name)) &&
	   !(dst && namematch(نالا, nnames, dst->p_name))) {
		أرجع;
	}
  }
#جيڪڏهن_پورو

	/* source, destination and message type */
	لکو("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	إذا (name) {
		لکو(" %s(%d/0x%x)", name, mtype, mtype);
	} وإلا {
		لکو(" %d/0x%x", mtype, mtype);
	}

	إذا (mightbecall && printparams) {
#وصف_ڪريو IDENT(x, y) إذا (mtype == x) printparam(#y, &نياپو->y, حجم(نياپو->y));
#ضمن "kernel/extracted-mfield.h"
#وصف_رد IDENT
	}
	لکو("\n");
}
#جيڪڏهن_پورو

#إذا DEBUG_IPCSTATS
#وصف_ڪريو IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#وصف_ڪريو KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ساكن صحيح messages[IPCPROCS][IPCPROCS];

#وصف_ڪريو PRINTSLOTS 20
ساكن بنية {
	صحيح src, dst, messages;
} winners[PRINTSLOTS];
ساكن صحيح ڪل, goodslots;

ساكن فراغ printstats(صحيح ticks)
{
	صحيح i;
	لكل(i = 0; i < goodslots; i++) {
#وصف_ڪريو name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#وصف_ڪريو persec(n) (system_hz*(n)/ticks)
		حرف	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		لکو("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	لکو("total %d/s\n", persec(ڪل));
}

ساكن فراغ sortstats(فراغ)
{
	/* Print top message senders/receivers. */
	صحيح src_جڳهه, dst_جڳهه;
	ڪل = goodslots = 0;
	لكل(src_جڳهه = 0; src_جڳهه < IPCPROCS; src_جڳهه++) {
		لكل(dst_جڳهه = 0; dst_جڳهه < IPCPROCS; dst_جڳهه++) {
			صحيح w = PRINTSLOTS, rem,
				n = messages[src_جڳهه][dst_جڳهه];
			ڪل += n;
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
			winners[w].src = src_جڳهه;
			winners[w].dst = dst_جڳهه;
			winners[w].messages = n;
			إذا(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#وصف_ڪريو proc2slot(p, s) { \
	إذا(p) { s = p->p_nr; } \
	وإلا { s = KERNELIPC; } \
	أكد(s >= 0 && s < IPCPROCS); \
}

ساكن فراغ statmsg(پيغام *نياپو, بنية proc *srcp, بنية proc *dstp)
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
#جيڪڏهن_پورو

#إذا DEBUG_IPC_HOOK
فراغ hook_ipc_msgkcall(پيغام *نياپو, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(نياپو, proc, عدم, 'k', 1);
#جيڪڏهن_پورو
}

فراغ hook_ipc_msgkresult(پيغام *نياپو, بنية proc *proc)
{
#إذا DEBUG_DUMPIPC
	printmsg(نياپو, عدم, proc, 'k', 0);
#جيڪڏهن_پورو
#إذا DEBUG_IPCSTATS
	statmsg(نياپو, proc, عدم);
#جيڪڏهن_پورو
}

فراغ hook_ipc_msgrecv(پيغام *نياپو, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(نياپو, src, dst, 'r', 0);
#جيڪڏهن_پورو
#إذا DEBUG_IPCSTATS
	statmsg(نياپو, src, dst);
#جيڪڏهن_پورو
}

فراغ hook_ipc_msgsend(پيغام *نياپو, بنية proc *src, بنية proc *dst)
{
#إذا DEBUG_DUMPIPC
	printmsg(نياپو, src, dst, 's', 1);
#جيڪڏهن_پورو
}

فراغ hook_ipc_clear(بنية proc *p)
{
#إذا DEBUG_IPCSTATS
	صحيح جڳهه, i;
	أكد(p);
	proc2slot(p, جڳهه);
	لكل(i = 0; i < IPCPROCS; i++)
		messages[جڳهه][i] = messages[i][جڳهه] = 0;
#جيڪڏهن_پورو
}
#جيڪڏهن_پورو
