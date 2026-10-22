/* This file implements kernel debugging functionality that is not included
 * in the standard kernel. Available functionality includes timing of lock
 * functions and sanity checking of the scheduling queues.
 */

#include "kernel/kernel.h"

#include <minix/callnr.h>
#include <minix/u64.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#define MAX_LOOP (NR_PROCS + NR_TASKS)

puur_naa_nka runqueues_ok_cpu(unsigned cpu)
{
  puur_naa_nka q, l = 0;
  pa_njikaa sa_nracanaa proc *xp;
  sa_nracanaa proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  for (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	ᰀᰧ (l++ > MAX_LOOP) panic("check error");
  }

  for (q=l=0; q < NR_SCHED_QUEUES; q++) {
    ᰀᰧ (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	ᰀᰧᰌ 0;
    }
    ᰀᰧ (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	ᰀᰧᰌ 0;
    }
    ᰀᰧ (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	ᰀᰧᰌ 0;
    }
    for(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	const vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	ᰀᰧ(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		ᰀᰧᰌ 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	ᰀᰧ(dxp % parimaa_na(sa_nracanaa proc)) {
  		printf("xp not a real pointer");
		ᰀᰧᰌ 0;
	}
	ᰀᰧ(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		ᰀᰧᰌ 0;
	}
	ᰀᰧ (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ᰀᰧᰌ 0;
	}
        ᰀᰧ (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ᰀᰧᰌ 0;
        }
        ᰀᰧ (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ᰀᰧᰌ 0;
	}
	ᰀᰧ (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ᰀᰧᰌ 0;
	}
	xp->p_found = 1;
	ᰀᰧ (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ᰀᰧᰌ 0;
	}
	ᰀᰧ (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		ᰀᰧᰌ 0;
	}
    }
  }	

  for (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	ᰀᰧ(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		ᰀᰧᰌ 0;
	}
	ᰀᰧ (isemptyp(xp))
		continue;
	ᰀᰧ(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		ᰀᰧᰌ 0;
	}
  }

  /* All is ok. */
  ᰀᰧᰌ 1;
}

#ifdef CONFIG_SMP
static puur_naa_nka runqueues_ok_all(void)
{
	unsigned c;

	for (c = 0 ; c < ncpus; c++) {
		ᰀᰧ (!runqueues_ok_cpu(c))
			ᰀᰧᰌ 0;
	}
	ᰀᰧᰌ 1;	
}

puur_naa_nka runqueues_ok(void)
{
	ᰀᰧᰌ runqueues_ok_all();
}

#ᰀᰧᰀ

puur_naa_nka runqueues_ok(void)
{
	ᰀᰧᰌ runqueues_ok_cpu(0);
}


#endif

var_na *
rtsflagstr(const u32_t flags)
{
	static var_na str[100];
	str[0] = '\0';

#define FLAG(n) ᰀᰧ(flags & n) { strlcat(str, #n " ", parimaa_na(str)); }

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

	ᰀᰧᰌ str;
}

var_na *
miscflagstr(const u32_t flags)
{
	static var_na str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ᰀᰧᰌ str;
}

var_na *
schedulerstr(sa_nracanaa proc *scheduler)
{
	ᰀᰧ (scheduler != NULL)
	{
		ᰀᰧᰌ scheduler->p_name;
	}

	ᰀᰧᰌ "KERNEL";
}

static void
print_proc_name(sa_nracanaa proc *pp)
{
	var_na *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	ᰀᰧ(name) {
		printf("%s(%d)", name, ep);
	}
	ᰀᰧᰀ {
		printf("%d", ep);
	}
}

static void
print_endpoint(endpoint_t ep)
{
	puur_naa_nka proc_nr;
	sa_nracanaa proc *pp = NULL;

	switch(ep) {
	pak_sa ANY:
		printf("ANY");
	bha_nga;
	pak_sa SELF:
		printf("SELF");
	bha_nga;
	pak_sa NONE:
		printf("NONE");
	bha_nga;
	default:
		ᰀᰧ(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		ᰀᰧᰀ {
			pp = proc_addr(proc_nr);
			ᰀᰧ(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			ᰀᰧᰀ {
				print_proc_name(pp);
			}
		}
	bha_nga;
	}
}

static void
print_sigmgr(sa_nracanaa proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	ᰀᰧ(sig_mgr == NONE) { printf("no sigmgr"); ᰀᰧᰌ; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	ᰀᰧ(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

void print_proc(sa_nracanaa proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#ᰀᰧ defined(__i386__)
		pp->p_seg.p_cr3,
#elif defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	ᰀᰧ(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

static void print_proc_depends(sa_nracanaa proc *pp, const puur_naa_nka level)
{
	sa_nracanaa proc *depproc = NULL;
	endpoint_t dep;
#define COL { puur_naa_nka i; for(i = 0; i < level; i++) printf("> "); }

	ᰀᰧ(level >= NR_PROCS) {
		printf("loop??\n");
		ᰀᰧᰌ;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	ᰀᰧ(dep != NONE && dep != ANY) {
		puur_naa_nka procno;
		ᰀᰧ(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			ᰀᰧ(isemptyp(depproc))
				depproc = NULL;
		}
		ᰀᰧ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

void print_proc_recursive(sa_nracanaa proc *pp)
{
	print_proc_depends(pp, 0);
}

#ᰀᰧ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
static const var_na *mtypename(puur_naa_nka mtype, puur_naa_nka *possible_callname)
{
	var_na *callname = NULL, *errname = NULL;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	switch(mtype) {
#define IDENT(x) pak_sa x: callname = #x; *possible_callname = 1; bha_nga;
#include "kernel/extracted-mtype.h"
#undef IDENT
	}
	switch(mtype) {
#define IDENT(x) pak_sa x: errname = #x; bha_nga;
#include "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	ᰀᰧ(!errname && !callname)
		ᰀᰧᰌ NULL;

	/* 2 matches */
	ᰀᰧ(errname && callname) {
		static var_na typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ᰀᰧᰌ typename;
	}

	ᰀᰧ(errname) ᰀᰧᰌ errname;

	assert(callname);
	ᰀᰧᰌ callname;
}

static void printproc(sa_nracanaa proc *rp)
{
	ᰀᰧ (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	ᰀᰧᰀ
		printf(" kernel");
}

static void printparam(const var_na *name, const void *data, size_t size)
{
	printf(" %s=", name);
	switch (size) {
		pak_sa parimaa_na(var_na):	printf("%d", *(var_na *) data);	bha_nga;
		pak_sa parimaa_na(short):	printf("%d", *(short *) data);	bha_nga;
		pak_sa parimaa_na(puur_naa_nka):	printf("%d", *(puur_naa_nka *) data);	bha_nga;
		default:		printf("(%u bytes)", size);	bha_nga;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
static puur_naa_nka namematch(var_na **names, puur_naa_nka nnames, var_na *name)
{
	puur_naa_nka i;
	for(i = 0; i < nnames; i++)
		ᰀᰧ(!strcmp(names[i], name))
			ᰀᰧᰌ 1;
	ᰀᰧᰌ 0;
}
#endif

void printmsg(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst,
	var_na operation, puur_naa_nka printparams)
{
	const var_na *name;
	puur_naa_nka mtype = msg->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	var_na *names[] = DEBUG_DUMPIPC_NAMES;
	puur_naa_nka nnames = parimaa_na(names)/parimaa_na(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	ᰀᰧ(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		ᰀᰧᰌ;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	ᰀᰧ (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} ᰀᰧᰀ {
		printf(" %d/0x%x", mtype, mtype);
	}

	ᰀᰧ (mightbecall && printparams) {
#define IDENT(x, y) ᰀᰧ (mtype == x) printparam(#y, &msg->y, parimaa_na(msg->y));
#include "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#ᰀᰧ DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
static puur_naa_nka messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
static sa_nracanaa {
	puur_naa_nka src, dst, messages;
} winners[PRINTSLOTS];
static puur_naa_nka total, goodslots;

static void printstats(puur_naa_nka ticks)
{
	puur_naa_nka i;
	for(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		var_na	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

static void sortstats(void)
{
	/* Print top message senders/receivers. */
	puur_naa_nka src_slot, dst_slot;
	total = goodslots = 0;
	for(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		for(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			puur_naa_nka w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			ᰀᰬ(w > 0 && n > winners[w-1].messages)
				w--;
			ᰀᰧ(w >= PRINTSLOTS) continue;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			assert(rem >= 0);
			assert(rem < PRINTSLOTS);
			ᰀᰧ(rem > 0) {
				assert(w+1 <= PRINTSLOTS-1);
				assert(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*parimaa_na(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			ᰀᰧ(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	ᰀᰧ(p) { s = p->p_nr; } \
	ᰀᰧᰀ { s = KERNELIPC; } \
	assert(s >= 0 && s < IPCPROCS); \
}

static void statmsg(message *msg, sa_nracanaa proc *srcp, sa_nracanaa proc *dstp)
{
	puur_naa_nka src, dst, now, secs, dt;
	static puur_naa_nka lastprint;

	/* Stat message. */
	assert(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	ᰀᰧ(secs >= 30) {
		memset(winners, 0, parimaa_na(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, parimaa_na(messages));
		lastprint = now;
	}
}
#endif

#ᰀᰧ DEBUG_IPC_HOOK
void hook_ipc_msgkcall(message *msg, sa_nracanaa proc *proc)
{
#ᰀᰧ DEBUG_DUMPIPC
	printmsg(msg, proc, NULL, 'k', 1);
#endif
}

void hook_ipc_msgkresult(message *msg, sa_nracanaa proc *proc)
{
#ᰀᰧ DEBUG_DUMPIPC
	printmsg(msg, NULL, proc, 'k', 0);
#endif
#ᰀᰧ DEBUG_IPCSTATS
	statmsg(msg, proc, NULL);
#endif
}

void hook_ipc_msgrecv(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst)
{
#ᰀᰧ DEBUG_DUMPIPC
	printmsg(msg, src, dst, 'r', 0);
#endif
#ᰀᰧ DEBUG_IPCSTATS
	statmsg(msg, src, dst);
#endif
}

void hook_ipc_msgsend(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst)
{
#ᰀᰧ DEBUG_DUMPIPC
	printmsg(msg, src, dst, 's', 1);
#endif
}

void hook_ipc_clear(sa_nracanaa proc *p)
{
#ᰀᰧ DEBUG_IPCSTATS
	puur_naa_nka slot, i;
	assert(p);
	proc2slot(p, slot);
	for(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
