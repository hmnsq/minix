/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಅನ್ತರ್ಭಾವ <minix/callnr.h>
#ಅನ್ತರ್ಭಾವ <minix/u64.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_LOOP (NR_PROCS + NR_TASKS)

ಪೂರ್ಣಾಂಕ runqueues_ok_cpu(ಚಿಹ್ನೆರಹಿತ cpu)
{
  ಪೂರ್ಣಾಂಕ q, l = 0;
  ನೋಂದಣಿ ರಚನೆ proc *xp;
  ರಚನೆ proc **rdy_ತಲೆ, **rdy_ಬಾಲ;

  rdy_ತಲೆ = get_cpu_var(cpu, ಸತತ_q_ತಲೆ);
  rdy_ಬಾಲ = get_cpu_var(cpu, ಸತತ_q_ಬಾಲ);

  ಗಾಗಿ (xp = BEG_PROC_ADDR; xp < ಕೊನೆ_PROC_ADDR; ++xp) {
	xp->p_ಸಿಕ್ಕಿತು = 0;
	ಆದರೆ (l++ > MAX_LOOP) panic("check error");
  }

  ಗಾಗಿ (q=l=0; q < NR_SCHED_QUEUES; q++) {
    ಆದರೆ (rdy_ತಲೆ[q] && !rdy_ಬಾಲ[q]) {
	ಬರೆ("head but no tail in %d\n", q);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (!rdy_ತಲೆ[q] && rdy_ಬಾಲ[q]) {
	ಬರೆ("tail but no head in %d\n", q);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (rdy_ಬಾಲ[q] && rdy_ಬಾಲ[q]->p_nextready) {
	ಬರೆ("tail and tail->next not null in %d\n", q);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಗಾಗಿ(xp = rdy_ತಲೆ[q]; xp; xp = xp->p_nextready) {
	ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು vxp = (vir_ಬೈಟುಗಳು) xp;
	vir_ಬೈಟುಗಳು dxp;
	ಆದರೆ(vxp < (vir_ಬೈಟುಗಳು) BEG_PROC_ADDR || vxp >= (vir_ಬೈಟುಗಳು) ಕೊನೆ_PROC_ADDR) {
  		ಬರೆ("xp out of range\n");
		ಹಿಂತಿರುಗಿ 0;
	}
	dxp = vxp - (vir_ಬೈಟುಗಳು) BEG_PROC_ADDR;
	ಆದರೆ(dxp % ಗಾತ್ರ(ರಚನೆ proc)) {
  		ಬರೆ("xp not a real pointer");
		ಹಿಂತಿರುಗಿ 0;
	}
	ಆದರೆ(!proc_ptr_ok(xp)) {
  		ಬರೆ("xp bogus pointer");
		ಹಿಂತಿರುಗಿ 0;
	}
	ಆದರೆ (RTS_ISSET(xp, RTS_ಸ್ಥಾನ_FREE)) {
		ಬರೆ("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ಹಿಂತಿರುಗಿ 0;
	}
        ಆದರೆ (!proc_is_runnable(xp)) {
		ಬರೆ("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (xp->p_ಆದ್ಯತೆ != q) {
		ಬರೆ("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ಹಿಂತಿರುಗಿ 0;
	}
	ಆದರೆ (xp->p_ಸಿಕ್ಕಿತು) {
		ಬರೆ("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ಹಿಂತಿರುಗಿ 0;
	}
	xp->p_ಸಿಕ್ಕಿತು = 1;
	ಆದರೆ (!xp->p_nextready && rdy_ಬಾಲ[q] != xp) {
		ಬರೆ("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ಹಿಂತಿರುಗಿ 0;
	}
	ಆದರೆ (l++ > MAX_LOOP) {
		ಬರೆ("loop in schedule queue?");
		ಹಿಂತಿರುಗಿ 0;
	}
    }
  }	

  ಗಾಗಿ (xp = BEG_PROC_ADDR; xp < ಕೊನೆ_PROC_ADDR; ++xp) {
	ಆದರೆ(!proc_ptr_ok(xp)) {
		ಬರೆ("xp bogus pointer in proc table\n");
		ಹಿಂತಿರುಗಿ 0;
	}
	ಆದರೆ (isemptyp(xp))
		ಮುಂದುವರಿ;
	ಆದರೆ(proc_is_runnable(xp) && !xp->p_ಸಿಕ್ಕಿತು) {
		ಬರೆ("sched error: ready proc %d not on queue\n", xp->p_nr);
		ಹಿಂತಿರುಗಿ 0;
	}
  }

  /* All is ok. */
  ಹಿಂತಿರುಗಿ 1;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ runqueues_ok_all(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ c;

	ಗಾಗಿ (c = 0 ; c < ncpus; c++) {
		ಆದರೆ (!runqueues_ok_cpu(c))
			ಹಿಂತಿರುಗಿ 0;
	}
	ಹಿಂತಿರುಗಿ 1;	
}

ಪೂರ್ಣಾಂಕ runqueues_ok(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ runqueues_ok_all();
}

#ಇಲ್ಲದಿದ್ದರೆ

ಪೂರ್ಣಾಂಕ runqueues_ok(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ runqueues_ok_cpu(0);
}


#ಆದರೆ_ಅಂತ್ಯ

ಅಕ್ಷರ *
rtsflagstr(ಸ್ಥಿರಾಂಕ u32_t flags)
{
	ಸ್ಥಿರ ಅಕ್ಷರ str[100];
	str[0] = '\0';

#ವ್ಯಾಖ್ಯಾನಿಸು FLAG(n) ಆದರೆ(flags & n) { strlcat(str, #n " ", ಗಾತ್ರ(str)); }

	FLAG(RTS_ಸ್ಥಾನ_FREE);
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
	FLAG(RTS_NO_ಕಾಲಾಂಶ);

	ಹಿಂತಿರುಗಿ str;
}

ಅಕ್ಷರ *
miscflagstr(ಸ್ಥಿರಾಂಕ u32_t flags)
{
	ಸ್ಥಿರ ಅಕ್ಷರ str[100];
	str[0] = '\0';

	FLAG(MF_ಉತ್ತರ_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ಹಿಂತಿರುಗಿ str;
}

ಅಕ್ಷರ *
schedulerstr(ರಚನೆ proc *scheduler)
{
	ಆದರೆ (scheduler != ಖಾಲಿ)
	{
		ಹಿಂತಿರುಗಿ scheduler->p_name;
	}

	ಹಿಂತಿರುಗಿ "KERNEL";
}

ಸ್ಥಿರ ಶೂನ್ಯ
print_proc_name(ರಚನೆ proc *pp)
{
	ಅಕ್ಷರ *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	ಆದರೆ(name) {
		ಬರೆ("%s(%d)", name, ep);
	}
	ಇಲ್ಲದಿದ್ದರೆ {
		ಬರೆ("%d", ep);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ
print_endpoint(endpoint_t ep)
{
	ಪೂರ್ಣಾಂಕ proc_nr;
	ರಚನೆ proc *pp = ಖಾಲಿ;

	ಆಯ್ಕೆ(ep) {
	ಸ್ಥಿತಿ ANY:
		ಬರೆ("ANY");
	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SELF:
		ಬರೆ("SELF");
	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NONE:
		ಬರೆ("NONE");
	ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ(!isokendpt(ep, &proc_nr)) {
			ಬರೆ("??? %d\n", ep);
		}
		ಇಲ್ಲದಿದ್ದರೆ {
			pp = proc_addr(proc_nr);
			ಆದರೆ(isemptyp(pp)) {
				ಬರೆ("??? empty slot %d\n", proc_nr);
			}
			ಇಲ್ಲದಿದ್ದರೆ {
				print_proc_name(pp);
			}
		}
	ನಿಲ್ಲಿಸು;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ
print_sigmgr(ರಚನೆ proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	ಆದರೆ(sig_mgr == NONE) { ಬರೆ("no sigmgr"); ಹಿಂತಿರುಗಿ; }
	ಬರೆ("sigmgr ");
	print_endpoint(sig_mgr);
	ಆದರೆ(bak_sig_mgr != NONE) {
		ಬರೆ(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

ಶೂನ್ಯ print_proc(ರಚನೆ proc *pp)
{
	endpoint_t dep;

	ಬರೆ("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_ಆದ್ಯತೆ, pp->p_user_ಸಮಯ,
		pp->p_sys_ಸಮಯ, ex64hi(pp->p_ಚಕ್ರಗಳು),
		ex64lo(pp->p_ಚಕ್ರಗಳು), pp->p_cpu,
#ಆದರೆ defined(__i386__)
		pp->p_seg.p_cr3,
#ಅಥವಾಆದರೆ defined(__arm__)
		pp->p_seg.p_ttbr,
#ಆದರೆ_ಅಂತ್ಯ
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	ಆದರೆ(dep != NONE) {
		ಬರೆ(" blocked on: ");
		print_endpoint(dep);
	}
	ಬರೆ("\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ print_proc_depends(ರಚನೆ proc *pp, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ level)
{
	ರಚನೆ proc *depproc = ಖಾಲಿ;
	endpoint_t dep;
#ವ್ಯಾಖ್ಯಾನಿಸು ಲಂಬಸಾಲು_2 { ಪೂರ್ಣಾಂಕ i; ಗಾಗಿ(i = 0; i < level; i++) ಬರೆ("> "); }

	ಆದರೆ(level >= NR_PROCS) {
		ಬರೆ("loop??\n");
		ಹಿಂತಿರುಗಿ;
	}

	ಲಂಬಸಾಲು_2

	print_proc(pp);

	ಲಂಬಸಾಲು_2
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	ಆದರೆ(dep != NONE && dep != ANY) {
		ಪೂರ್ಣಾಂಕ procno;
		ಆದರೆ(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			ಆದರೆ(isemptyp(depproc))
				depproc = ಖಾಲಿ;
		}
		ಆದರೆ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

ಶೂನ್ಯ print_proc_recursive(ರಚನೆ proc *pp)
{
	print_proc_depends(pp, 0);
}

#ಆದರೆ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mtypename(ಪೂರ್ಣಾಂಕ mtype, ಪೂರ್ಣಾಂಕ *possible_callname)
{
	ಅಕ್ಷರ *callname = ಖಾಲಿ, *errname = ಖಾಲಿ;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	ಆಯ್ಕೆ(mtype) {
#ವ್ಯಾಖ್ಯಾನಿಸು IDENT(x) ಸ್ಥಿತಿ x: callname = #x; *possible_callname = 1; ನಿಲ್ಲಿಸು;
#ಅನ್ತರ್ಭಾವ "kernel/extracted-mtype.h"
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು IDENT
	}
	ಆಯ್ಕೆ(mtype) {
#ವ್ಯಾಖ್ಯಾನಿಸು IDENT(x) ಸ್ಥಿತಿ x: errname = #x; ನಿಲ್ಲಿಸು;
#ಅನ್ತರ್ಭಾವ "kernel/extracted-errno.h"
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು IDENT
	}

	/* no match */
	ಆದರೆ(!errname && !callname)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	/* 2 matches */
	ಆದರೆ(errname && callname) {
		ಸ್ಥಿರ ಅಕ್ಷರ typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ಹಿಂತಿರುಗಿ typename;
	}

	ಆದರೆ(errname) ಹಿಂತಿರುಗಿ errname;

	ಪ್ರತಿಜ್ಞಾ(callname);
	ಹಿಂತಿರುಗಿ callname;
}

ಸ್ಥಿರ ಶೂನ್ಯ printproc(ರಚನೆ proc *rp)
{
	ಆದರೆ (rp)
		ಬರೆ(" %s(%d)", rp->p_name, rp - proc);
	ಇಲ್ಲದಿದ್ದರೆ
		ಬರೆ(" kernel");
}

ಸ್ಥಿರ ಶೂನ್ಯ printparam(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2)
{
	ಬರೆ(" %s=", name);
	ಆಯ್ಕೆ (ಗಾತ್ರ_2) {
		ಸ್ಥಿತಿ ಗಾತ್ರ(ಅಕ್ಷರ):	ಬರೆ("%d", *(ಅಕ್ಷರ *) ದತ್ತಾಂಶ);	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ಗಾತ್ರ(ಗಿಡ್ಡ):	ಬರೆ("%d", *(ಗಿಡ್ಡ *) ದತ್ತಾಂಶ);	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ):	ಬರೆ("%d", *(ಪೂರ್ಣಾಂಕ *) ದತ್ತಾಂಶ);	ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:		ಬರೆ("(%u bytes)", ಗಾತ್ರ_2);	ನಿಲ್ಲಿಸು;
	}
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_DUMPIPC_ಹೆಸರುಗಳು
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ namematch(ಅಕ್ಷರ **ಹೆಸರುಗಳು, ಪೂರ್ಣಾಂಕ nnames, ಅಕ್ಷರ *name)
{
	ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ(i = 0; i < nnames; i++)
		ಆದರೆ(!strcmp(ಹೆಸರುಗಳು[i], name))
			ಹಿಂತಿರುಗಿ 1;
	ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ printmsg(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *src, ರಚನೆ proc *dst,
	ಅಕ್ಷರ operation, ಪೂರ್ಣಾಂಕ printparams)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
	ಪೂರ್ಣಾಂಕ mtype = ಸಂದೇಶ_2->m_type, mightbecall = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_DUMPIPC_ಹೆಸರುಗಳು
  {
	ಅಕ್ಷರ *ಹೆಸರುಗಳು[] = DEBUG_DUMPIPC_ಹೆಸರುಗಳು;
	ಪೂರ್ಣಾಂಕ nnames = ಗಾತ್ರ(ಹೆಸರುಗಳು)/ಗಾತ್ರ(ಹೆಸರುಗಳು[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	ಆದರೆ(!(src && namematch(ಹೆಸರುಗಳು, nnames, src->p_name)) &&
	   !(dst && namematch(ಹೆಸರುಗಳು, nnames, dst->p_name))) {
		ಹಿಂತಿರುಗಿ;
	}
  }
#ಆದರೆ_ಅಂತ್ಯ

	/* source, destination and message type */
	ಬರೆ("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	ಆದರೆ (name) {
		ಬರೆ(" %s(%d/0x%x)", name, mtype, mtype);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಬರೆ(" %d/0x%x", mtype, mtype);
	}

	ಆದರೆ (mightbecall && printparams) {
#ವ್ಯಾಖ್ಯಾನಿಸು IDENT(x, y) ಆದರೆ (mtype == x) printparam(#y, &ಸಂದೇಶ_2->y, ಗಾತ್ರ(ಸಂದೇಶ_2->y));
#ಅನ್ತರ್ಭಾವ "kernel/extracted-mfield.h"
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು IDENT
	}
	ಬರೆ("\n");
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ DEBUG_IPCSTATS
#ವ್ಯಾಖ್ಯಾನಿಸು IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#ವ್ಯಾಖ್ಯಾನಿಸು KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ messages[IPCPROCS][IPCPROCS];

#ವ್ಯಾಖ್ಯಾನಿಸು PRINTSLOTS 20
ಸ್ಥಿರ ರಚನೆ {
	ಪೂರ್ಣಾಂಕ src, dst, messages;
} winners[PRINTSLOTS];
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಒಟ್ಟು, goodslots;

ಸ್ಥಿರ ಶೂನ್ಯ printstats(ಪೂರ್ಣಾಂಕ ticks)
{
	ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ(i = 0; i < goodslots; i++) {
#ವ್ಯಾಖ್ಯಾನಿಸು name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#ವ್ಯಾಖ್ಯಾನಿಸು persec(n) (system_hz*(n)/ticks)
		ಅಕ್ಷರ	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		ಬರೆ("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	ಬರೆ("total %d/s\n", persec(ಒಟ್ಟು));
}

ಸ್ಥಿರ ಶೂನ್ಯ sortstats(ಶೂನ್ಯ)
{
	/* Print top message senders/receivers. */
	ಪೂರ್ಣಾಂಕ src_ಸ್ಥಾನ, dst_ಸ್ಥಾನ;
	ಒಟ್ಟು = goodslots = 0;
	ಗಾಗಿ(src_ಸ್ಥಾನ = 0; src_ಸ್ಥಾನ < IPCPROCS; src_ಸ್ಥಾನ++) {
		ಗಾಗಿ(dst_ಸ್ಥಾನ = 0; dst_ಸ್ಥಾನ < IPCPROCS; dst_ಸ್ಥಾನ++) {
			ಪೂರ್ಣಾಂಕ w = PRINTSLOTS, rem,
				n = messages[src_ಸ್ಥಾನ][dst_ಸ್ಥಾನ];
			ಒಟ್ಟು += n;
			ಆಗ(w > 0 && n > winners[w-1].messages)
				w--;
			ಆದರೆ(w >= PRINTSLOTS) ಮುಂದುವರಿ;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			ಪ್ರತಿಜ್ಞಾ(rem >= 0);
			ಪ್ರತಿಜ್ಞಾ(rem < PRINTSLOTS);
			ಆದರೆ(rem > 0) {
				ಪ್ರತಿಜ್ಞಾ(w+1 <= PRINTSLOTS-1);
				ಪ್ರತಿಜ್ಞಾ(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*ಗಾತ್ರ(winners[0]));
			}
			winners[w].src = src_ಸ್ಥಾನ;
			winners[w].dst = dst_ಸ್ಥಾನ;
			winners[w].messages = n;
			ಆದರೆ(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#ವ್ಯಾಖ್ಯಾನಿಸು proc2slot(p, s) { \
	ಆದರೆ(p) { s = p->p_nr; } \
	ಇಲ್ಲದಿದ್ದರೆ { s = KERNELIPC; } \
	ಪ್ರತಿಜ್ಞಾ(s >= 0 && s < IPCPROCS); \
}

ಸ್ಥಿರ ಶೂನ್ಯ statmsg(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *srcp, ರಚನೆ proc *dstp)
{
	ಪೂರ್ಣಾಂಕ src, dst, now, secs, dt;
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lastprint;

	/* Stat message. */
	ಪ್ರತಿಜ್ಞಾ(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	ಆದರೆ(secs >= 30) {
		memset(winners, 0, ಗಾತ್ರ(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, ಗಾತ್ರ(messages));
		lastprint = now;
	}
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ DEBUG_IPC_HOOK
ಶೂನ್ಯ hook_ipc_msgkcall(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *proc)
{
#ಆದರೆ DEBUG_DUMPIPC
	printmsg(ಸಂದೇಶ_2, proc, ಖಾಲಿ, 'k', 1);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ hook_ipc_msgkresult(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *proc)
{
#ಆದರೆ DEBUG_DUMPIPC
	printmsg(ಸಂದೇಶ_2, ಖಾಲಿ, proc, 'k', 0);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ DEBUG_IPCSTATS
	statmsg(ಸಂದೇಶ_2, proc, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ hook_ipc_msgrecv(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *src, ರಚನೆ proc *dst)
{
#ಆದರೆ DEBUG_DUMPIPC
	printmsg(ಸಂದೇಶ_2, src, dst, 'r', 0);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ DEBUG_IPCSTATS
	statmsg(ಸಂದೇಶ_2, src, dst);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ hook_ipc_msgsend(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *src, ರಚನೆ proc *dst)
{
#ಆದರೆ DEBUG_DUMPIPC
	printmsg(ಸಂದೇಶ_2, src, dst, 's', 1);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ hook_ipc_clear(ರಚನೆ proc *p)
{
#ಆದರೆ DEBUG_IPCSTATS
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನ, i;
	ಪ್ರತಿಜ್ಞಾ(p);
	proc2slot(p, ಸ್ಥಾನ);
	ಗಾಗಿ(i = 0; i < IPCPROCS; i++)
		messages[ಸ್ಥಾನ][i] = messages[i][ಸ್ಥಾನ] = 0;
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ
