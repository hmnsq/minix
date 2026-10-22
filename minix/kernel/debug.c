/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/callnr.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_LOOP (NR_PROCS + NR_TASKS)

ਪੂਰਨ runqueues_ok_cpu(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
  ਪੂਰਨ q, l = 0;
  ਰਜਿਸਟਰ ਬਣਤਰ proc *xp;
  ਬਣਤਰ proc **rdy_ਸਿਰ, **rdy_ਪੂਛ;

  rdy_ਸਿਰ = get_cpu_var(cpu, ਲੜੀਵਾਰ_q_ਸਿਰ);
  rdy_ਪੂਛ = get_cpu_var(cpu, ਲੜੀਵਾਰ_q_ਪੂਛ);

  ਲਈ (xp = BEG_PROC_ADDR; xp < ਸਿਰਾ_PROC_ADDR; ++xp) {
	xp->p_ਮਿਲਿਆ = 0;
	ਜੇ (l++ > MAX_LOOP) panic("check error");
  }

  ਲਈ (q=l=0; q < NR_SCHED_QUEUES; q++) {
    ਜੇ (rdy_ਸਿਰ[q] && !rdy_ਪੂਛ[q]) {
	ਲਿਖੋ("head but no tail in %d\n", q);
	ਵਾਪਸ 0;
    }
    ਜੇ (!rdy_ਸਿਰ[q] && rdy_ਪੂਛ[q]) {
	ਲਿਖੋ("tail but no head in %d\n", q);
	ਵਾਪਸ 0;
    }
    ਜੇ (rdy_ਪੂਛ[q] && rdy_ਪੂਛ[q]->p_nextready) {
	ਲਿਖੋ("tail and tail->next not null in %d\n", q);
	ਵਾਪਸ 0;
    }
    ਲਈ(xp = rdy_ਸਿਰ[q]; xp; xp = xp->p_nextready) {
	ਅਚਲ vir_ਬਾਈਟਾਂ vxp = (vir_ਬਾਈਟਾਂ) xp;
	vir_ਬਾਈਟਾਂ dxp;
	ਜੇ(vxp < (vir_ਬਾਈਟਾਂ) BEG_PROC_ADDR || vxp >= (vir_ਬਾਈਟਾਂ) ਸਿਰਾ_PROC_ADDR) {
  		ਲਿਖੋ("xp out of range\n");
		ਵਾਪਸ 0;
	}
	dxp = vxp - (vir_ਬਾਈਟਾਂ) BEG_PROC_ADDR;
	ਜੇ(dxp % ਆਕਾਰ(ਬਣਤਰ proc)) {
  		ਲਿਖੋ("xp not a real pointer");
		ਵਾਪਸ 0;
	}
	ਜੇ(!proc_ptr_ok(xp)) {
  		ਲਿਖੋ("xp bogus pointer");
		ਵਾਪਸ 0;
	}
	ਜੇ (RTS_ISSET(xp, RTS_ਥਾਂ_FREE)) {
		ਲਿਖੋ("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ਵਾਪਸ 0;
	}
        ਜੇ (!proc_is_runnable(xp)) {
		ਲਿਖੋ("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ਵਾਪਸ 0;
        }
        ਜੇ (xp->p_ਤਰਜੀਹ != q) {
		ਲਿਖੋ("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ਵਾਪਸ 0;
	}
	ਜੇ (xp->p_ਮਿਲਿਆ) {
		ਲਿਖੋ("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ਵਾਪਸ 0;
	}
	xp->p_ਮਿਲਿਆ = 1;
	ਜੇ (!xp->p_nextready && rdy_ਪੂਛ[q] != xp) {
		ਲਿਖੋ("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ਵਾਪਸ 0;
	}
	ਜੇ (l++ > MAX_LOOP) {
		ਲਿਖੋ("loop in schedule queue?");
		ਵਾਪਸ 0;
	}
    }
  }	

  ਲਈ (xp = BEG_PROC_ADDR; xp < ਸਿਰਾ_PROC_ADDR; ++xp) {
	ਜੇ(!proc_ptr_ok(xp)) {
		ਲਿਖੋ("xp bogus pointer in proc table\n");
		ਵਾਪਸ 0;
	}
	ਜੇ (isemptyp(xp))
		ਜਾਰੀ;
	ਜੇ(proc_is_runnable(xp) && !xp->p_ਮਿਲਿਆ) {
		ਲਿਖੋ("sched error: ready proc %d not on queue\n", xp->p_nr);
		ਵਾਪਸ 0;
	}
  }

  /* All is ok. */
  ਵਾਪਸ 1;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
ਸਥਿਰ ਪੂਰਨ runqueues_ok_all(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ c;

	ਲਈ (c = 0 ; c < ncpus; c++) {
		ਜੇ (!runqueues_ok_cpu(c))
			ਵਾਪਸ 0;
	}
	ਵਾਪਸ 1;	
}

ਪੂਰਨ runqueues_ok(ਖਾਲੀ)
{
	ਵਾਪਸ runqueues_ok_all();
}

#ਨਹੀਂਤਾਂ

ਪੂਰਨ runqueues_ok(ਖਾਲੀ)
{
	ਵਾਪਸ runqueues_ok_cpu(0);
}


#ਜੇ_ਅੰਤ

ਅੱਖਰ *
rtsflagstr(ਅਚਲ u32_t flags)
{
	ਸਥਿਰ ਅੱਖਰ str[100];
	str[0] = '\0';

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLAG(n) ਜੇ(flags & n) { strlcat(str, #n " ", ਆਕਾਰ(str)); }

	FLAG(RTS_ਥਾਂ_FREE);
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
	FLAG(RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ);

	ਵਾਪਸ str;
}

ਅੱਖਰ *
miscflagstr(ਅਚਲ u32_t flags)
{
	ਸਥਿਰ ਅੱਖਰ str[100];
	str[0] = '\0';

	FLAG(MF_ਜਵਾਬ_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ਵਾਪਸ str;
}

ਅੱਖਰ *
schedulerstr(ਬਣਤਰ proc *scheduler)
{
	ਜੇ (scheduler != ਨਿਰਾ)
	{
		ਵਾਪਸ scheduler->p_name;
	}

	ਵਾਪਸ "KERNEL";
}

ਸਥਿਰ ਖਾਲੀ
print_proc_name(ਬਣਤਰ proc *pp)
{
	ਅੱਖਰ *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	ਜੇ(name) {
		ਲਿਖੋ("%s(%d)", name, ep);
	}
	ਨਹੀਂਤਾਂ {
		ਲਿਖੋ("%d", ep);
	}
}

ਸਥਿਰ ਖਾਲੀ
print_endpoint(endpoint_t ep)
{
	ਪੂਰਨ proc_nr;
	ਬਣਤਰ proc *pp = ਨਿਰਾ;

	ਚੋਣ(ep) {
	ਹਾਲਤ ANY:
		ਲਿਖੋ("ANY");
	ਰੋਕੋ;
	ਹਾਲਤ SELF:
		ਲਿਖੋ("SELF");
	ਰੋਕੋ;
	ਹਾਲਤ NONE:
		ਲਿਖੋ("NONE");
	ਰੋਕੋ;
	ਮੂਲ:
		ਜੇ(!isokendpt(ep, &proc_nr)) {
			ਲਿਖੋ("??? %d\n", ep);
		}
		ਨਹੀਂਤਾਂ {
			pp = proc_addr(proc_nr);
			ਜੇ(isemptyp(pp)) {
				ਲਿਖੋ("??? empty slot %d\n", proc_nr);
			}
			ਨਹੀਂਤਾਂ {
				print_proc_name(pp);
			}
		}
	ਰੋਕੋ;
	}
}

ਸਥਿਰ ਖਾਲੀ
print_sigmgr(ਬਣਤਰ proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	ਜੇ(sig_mgr == NONE) { ਲਿਖੋ("no sigmgr"); ਵਾਪਸ; }
	ਲਿਖੋ("sigmgr ");
	print_endpoint(sig_mgr);
	ਜੇ(bak_sig_mgr != NONE) {
		ਲਿਖੋ(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

ਖਾਲੀ print_proc(ਬਣਤਰ proc *pp)
{
	endpoint_t dep;

	ਲਿਖੋ("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_ਤਰਜੀਹ, pp->p_user_ਸਮਾਂ,
		pp->p_sys_ਸਮਾਂ, ex64hi(pp->p_ਚੱਕਰਾਂ),
		ex64lo(pp->p_ਚੱਕਰਾਂ), pp->p_cpu,
#ਜੇ defined(__i386__)
		pp->p_seg.p_cr3,
#ਜਾਂਜੇ defined(__arm__)
		pp->p_seg.p_ttbr,
#ਜੇ_ਅੰਤ
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	ਜੇ(dep != NONE) {
		ਲਿਖੋ(" blocked on: ");
		print_endpoint(dep);
	}
	ਲਿਖੋ("\n");
}

ਸਥਿਰ ਖਾਲੀ print_proc_depends(ਬਣਤਰ proc *pp, ਅਚਲ ਪੂਰਨ level)
{
	ਬਣਤਰ proc *depproc = ਨਿਰਾ;
	endpoint_t dep;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਨਾ_2 { ਪੂਰਨ i; ਲਈ(i = 0; i < level; i++) ਲਿਖੋ("> "); }

	ਜੇ(level >= NR_PROCS) {
		ਲਿਖੋ("loop??\n");
		ਵਾਪਸ;
	}

	ਖਾਨਾ_2

	print_proc(pp);

	ਖਾਨਾ_2
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	ਜੇ(dep != NONE && dep != ANY) {
		ਪੂਰਨ procno;
		ਜੇ(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			ਜੇ(isemptyp(depproc))
				depproc = ਨਿਰਾ;
		}
		ਜੇ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

ਖਾਲੀ print_proc_recursive(ਬਣਤਰ proc *pp)
{
	print_proc_depends(pp, 0);
}

#ਜੇ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *mtypename(ਪੂਰਨ mtype, ਪੂਰਨ *possible_callname)
{
	ਅੱਖਰ *callname = ਨਿਰਾ, *errname = ਨਿਰਾ;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	ਚੋਣ(mtype) {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDENT(x) ਹਾਲਤ x: callname = #x; *possible_callname = 1; ਰੋਕੋ;
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/extracted-mtype.h"
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ IDENT
	}
	ਚੋਣ(mtype) {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDENT(x) ਹਾਲਤ x: errname = #x; ਰੋਕੋ;
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/extracted-errno.h"
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ IDENT
	}

	/* no match */
	ਜੇ(!errname && !callname)
		ਵਾਪਸ ਨਿਰਾ;

	/* 2 matches */
	ਜੇ(errname && callname) {
		ਸਥਿਰ ਅੱਖਰ typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ਵਾਪਸ typename;
	}

	ਜੇ(errname) ਵਾਪਸ errname;

	ਪ੍ਰਤਿਜ੍ਞਾ(callname);
	ਵਾਪਸ callname;
}

ਸਥਿਰ ਖਾਲੀ printproc(ਬਣਤਰ proc *rp)
{
	ਜੇ (rp)
		ਲਿਖੋ(" %s(%d)", rp->p_name, rp - proc);
	ਨਹੀਂਤਾਂ
		ਲਿਖੋ(" kernel");
}

ਸਥਿਰ ਖਾਲੀ printparam(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t ਮਾਪ)
{
	ਲਿਖੋ(" %s=", name);
	ਚੋਣ (ਮਾਪ) {
		ਹਾਲਤ ਆਕਾਰ(ਅੱਖਰ):	ਲਿਖੋ("%d", *(ਅੱਖਰ *) ਅੰਕੜੇ);	ਰੋਕੋ;
		ਹਾਲਤ ਆਕਾਰ(ਛੋਟਾ):	ਲਿਖੋ("%d", *(ਛੋਟਾ *) ਅੰਕੜੇ);	ਰੋਕੋ;
		ਹਾਲਤ ਆਕਾਰ(ਪੂਰਨ):	ਲਿਖੋ("%d", *(ਪੂਰਨ *) ਅੰਕੜੇ);	ਰੋਕੋ;
		ਮੂਲ:		ਲਿਖੋ("(%u bytes)", ਮਾਪ);	ਰੋਕੋ;
	}
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_DUMPIPC_ਨਾਂ
ਸਥਿਰ ਪੂਰਨ namematch(ਅੱਖਰ **ਨਾਂ, ਪੂਰਨ nnames, ਅੱਖਰ *name)
{
	ਪੂਰਨ i;
	ਲਈ(i = 0; i < nnames; i++)
		ਜੇ(!strcmp(ਨਾਂ[i], name))
			ਵਾਪਸ 1;
	ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ printmsg(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *src, ਬਣਤਰ proc *dst,
	ਅੱਖਰ operation, ਪੂਰਨ printparams)
{
	ਅਚਲ ਅੱਖਰ *name;
	ਪੂਰਨ mtype = ਸੁਨੇਹਾ_2->m_type, mightbecall = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_DUMPIPC_ਨਾਂ
  {
	ਅੱਖਰ *ਨਾਂ[] = DEBUG_DUMPIPC_ਨਾਂ;
	ਪੂਰਨ nnames = ਆਕਾਰ(ਨਾਂ)/ਆਕਾਰ(ਨਾਂ[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	ਜੇ(!(src && namematch(ਨਾਂ, nnames, src->p_name)) &&
	   !(dst && namematch(ਨਾਂ, nnames, dst->p_name))) {
		ਵਾਪਸ;
	}
  }
#ਜੇ_ਅੰਤ

	/* source, destination and message type */
	ਲਿਖੋ("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	ਜੇ (name) {
		ਲਿਖੋ(" %s(%d/0x%x)", name, mtype, mtype);
	} ਨਹੀਂਤਾਂ {
		ਲਿਖੋ(" %d/0x%x", mtype, mtype);
	}

	ਜੇ (mightbecall && printparams) {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDENT(x, y) ਜੇ (mtype == x) printparam(#y, &ਸੁਨੇਹਾ_2->y, ਆਕਾਰ(ਸੁਨੇਹਾ_2->y));
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/extracted-mfield.h"
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ IDENT
	}
	ਲਿਖੋ("\n");
}
#ਜੇ_ਅੰਤ

#ਜੇ DEBUG_IPCSTATS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ਸਥਿਰ ਪੂਰਨ messages[IPCPROCS][IPCPROCS];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRINTSLOTS 20
ਸਥਿਰ ਬਣਤਰ {
	ਪੂਰਨ src, dst, messages;
} winners[PRINTSLOTS];
ਸਥਿਰ ਪੂਰਨ ਕੁੱਲ, goodslots;

ਸਥਿਰ ਖਾਲੀ printstats(ਪੂਰਨ ticks)
{
	ਪੂਰਨ i;
	ਲਈ(i = 0; i < goodslots; i++) {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ persec(n) (system_hz*(n)/ticks)
		ਅੱਖਰ	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		ਲਿਖੋ("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	ਲਿਖੋ("total %d/s\n", persec(ਕੁੱਲ));
}

ਸਥਿਰ ਖਾਲੀ sortstats(ਖਾਲੀ)
{
	/* Print top message senders/receivers. */
	ਪੂਰਨ src_ਥਾਂ, dst_ਥਾਂ;
	ਕੁੱਲ = goodslots = 0;
	ਲਈ(src_ਥਾਂ = 0; src_ਥਾਂ < IPCPROCS; src_ਥਾਂ++) {
		ਲਈ(dst_ਥਾਂ = 0; dst_ਥਾਂ < IPCPROCS; dst_ਥਾਂ++) {
			ਪੂਰਨ w = PRINTSLOTS, rem,
				n = messages[src_ਥਾਂ][dst_ਥਾਂ];
			ਕੁੱਲ += n;
			ਜਦੋਂ(w > 0 && n > winners[w-1].messages)
				w--;
			ਜੇ(w >= PRINTSLOTS) ਜਾਰੀ;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			ਪ੍ਰਤਿਜ੍ਞਾ(rem >= 0);
			ਪ੍ਰਤਿਜ੍ਞਾ(rem < PRINTSLOTS);
			ਜੇ(rem > 0) {
				ਪ੍ਰਤਿਜ੍ਞਾ(w+1 <= PRINTSLOTS-1);
				ਪ੍ਰਤਿਜ੍ਞਾ(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*ਆਕਾਰ(winners[0]));
			}
			winners[w].src = src_ਥਾਂ;
			winners[w].dst = dst_ਥਾਂ;
			winners[w].messages = n;
			ਜੇ(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc2slot(p, s) { \
	ਜੇ(p) { s = p->p_nr; } \
	ਨਹੀਂਤਾਂ { s = KERNELIPC; } \
	ਪ੍ਰਤਿਜ੍ਞਾ(s >= 0 && s < IPCPROCS); \
}

ਸਥਿਰ ਖਾਲੀ statmsg(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *srcp, ਬਣਤਰ proc *dstp)
{
	ਪੂਰਨ src, dst, now, secs, dt;
	ਸਥਿਰ ਪੂਰਨ lastprint;

	/* Stat message. */
	ਪ੍ਰਤਿਜ੍ਞਾ(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	ਜੇ(secs >= 30) {
		memset(winners, 0, ਆਕਾਰ(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, ਆਕਾਰ(messages));
		lastprint = now;
	}
}
#ਜੇ_ਅੰਤ

#ਜੇ DEBUG_IPC_HOOK
ਖਾਲੀ hook_ipc_msgkcall(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *proc)
{
#ਜੇ DEBUG_DUMPIPC
	printmsg(ਸੁਨੇਹਾ_2, proc, ਨਿਰਾ, 'k', 1);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ hook_ipc_msgkresult(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *proc)
{
#ਜੇ DEBUG_DUMPIPC
	printmsg(ਸੁਨੇਹਾ_2, ਨਿਰਾ, proc, 'k', 0);
#ਜੇ_ਅੰਤ
#ਜੇ DEBUG_IPCSTATS
	statmsg(ਸੁਨੇਹਾ_2, proc, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ hook_ipc_msgrecv(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *src, ਬਣਤਰ proc *dst)
{
#ਜੇ DEBUG_DUMPIPC
	printmsg(ਸੁਨੇਹਾ_2, src, dst, 'r', 0);
#ਜੇ_ਅੰਤ
#ਜੇ DEBUG_IPCSTATS
	statmsg(ਸੁਨੇਹਾ_2, src, dst);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ hook_ipc_msgsend(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *src, ਬਣਤਰ proc *dst)
{
#ਜੇ DEBUG_DUMPIPC
	printmsg(ਸੁਨੇਹਾ_2, src, dst, 's', 1);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ hook_ipc_clear(ਬਣਤਰ proc *p)
{
#ਜੇ DEBUG_IPCSTATS
	ਪੂਰਨ ਥਾਂ, i;
	ਪ੍ਰਤਿਜ੍ਞਾ(p);
	proc2slot(p, ਥਾਂ);
	ਲਈ(i = 0; i < IPCPROCS; i++)
		messages[ਥਾਂ][i] = messages[i][ਥਾਂ] = 0;
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ
