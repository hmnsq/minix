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

ཧྲིལ runqueues_ok_cpu(unsigned cpu)
{
  ཧྲིལ q, l = 0;
  pa_njikaa sa_nracanaa proc *xp;
  sa_nracanaa proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  ཆེད་དུ (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	གལ་ཏེ (l++ > MAX_LOOP) panic("check error");
  }

  ཆེད་དུ (q=l=0; q < NR_SCHED_QUEUES; q++) {
    གལ་ཏེ (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	ཕྱིར 0;
    }
    གལ་ཏེ (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	ཕྱིར 0;
    }
    གལ་ཏེ (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	ཕྱིར 0;
    }
    ཆེད་དུ(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	const vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	གལ་ཏེ(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		ཕྱིར 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	གལ་ཏེ(dxp % parimaa_na(sa_nracanaa proc)) {
  		printf("xp not a real pointer");
		ཕྱིར 0;
	}
	གལ་ཏེ(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		ཕྱིར 0;
	}
	གལ་ཏེ (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ཕྱིར 0;
	}
        གལ་ཏེ (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ཕྱིར 0;
        }
        གལ་ཏེ (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ཕྱིར 0;
	}
	གལ་ཏེ (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ཕྱིར 0;
	}
	xp->p_found = 1;
	གལ་ཏེ (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ཕྱིར 0;
	}
	གལ་ཏེ (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		ཕྱིར 0;
	}
    }
  }	

  ཆེད་དུ (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	གལ་ཏེ(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		ཕྱིར 0;
	}
	གལ་ཏེ (isemptyp(xp))
		continue;
	གལ་ཏེ(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		ཕྱིར 0;
	}
  }

  /* All is ok. */
  ཕྱིར 1;
}

#ifdef CONFIG_SMP
static ཧྲིལ runqueues_ok_all(void)
{
	unsigned c;

	ཆེད་དུ (c = 0 ; c < ncpus; c++) {
		གལ་ཏེ (!runqueues_ok_cpu(c))
			ཕྱིར 0;
	}
	ཕྱིར 1;	
}

ཧྲིལ runqueues_ok(void)
{
	ཕྱིར runqueues_ok_all();
}

#དེ་མིན

ཧྲིལ runqueues_ok(void)
{
	ཕྱིར runqueues_ok_cpu(0);
}


#endif

var_na *
rtsflagstr(const u32_t flags)
{
	static var_na str[100];
	str[0] = '\0';

#define FLAG(n) གལ་ཏེ(flags & n) { strlcat(str, #n " ", parimaa_na(str)); }

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

	ཕྱིར str;
}

var_na *
miscflagstr(const u32_t flags)
{
	static var_na str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ཕྱིར str;
}

var_na *
schedulerstr(sa_nracanaa proc *scheduler)
{
	གལ་ཏེ (scheduler != NULL)
	{
		ཕྱིར scheduler->p_name;
	}

	ཕྱིར "KERNEL";
}

static void
print_proc_name(sa_nracanaa proc *pp)
{
	var_na *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	གལ་ཏེ(name) {
		printf("%s(%d)", name, ep);
	}
	དེ་མིན {
		printf("%d", ep);
	}
}

static void
print_endpoint(endpoint_t ep)
{
	ཧྲིལ proc_nr;
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
		གལ་ཏེ(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		དེ་མིན {
			pp = proc_addr(proc_nr);
			གལ་ཏེ(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			དེ་མིན {
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
	གལ་ཏེ(sig_mgr == NONE) { printf("no sigmgr"); ཕྱིར; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	གལ་ཏེ(bak_sig_mgr != NONE) {
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
#གལ་ཏེ defined(__i386__)
		pp->p_seg.p_cr3,
#elif defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	གལ་ཏེ(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

static void print_proc_depends(sa_nracanaa proc *pp, const ཧྲིལ level)
{
	sa_nracanaa proc *depproc = NULL;
	endpoint_t dep;
#define COL { ཧྲིལ i; ཆེད་དུ(i = 0; i < level; i++) printf("> "); }

	གལ་ཏེ(level >= NR_PROCS) {
		printf("loop??\n");
		ཕྱིར;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	གལ་ཏེ(dep != NONE && dep != ANY) {
		ཧྲིལ procno;
		གལ་ཏེ(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			གལ་ཏེ(isemptyp(depproc))
				depproc = NULL;
		}
		གལ་ཏེ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

void print_proc_recursive(sa_nracanaa proc *pp)
{
	print_proc_depends(pp, 0);
}

#གལ་ཏེ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
static const var_na *mtypename(ཧྲིལ mtype, ཧྲིལ *possible_callname)
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
	གལ་ཏེ(!errname && !callname)
		ཕྱིར NULL;

	/* 2 matches */
	གལ་ཏེ(errname && callname) {
		static var_na typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ཕྱིར typename;
	}

	གལ་ཏེ(errname) ཕྱིར errname;

	assert(callname);
	ཕྱིར callname;
}

static void printproc(sa_nracanaa proc *rp)
{
	གལ་ཏེ (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	དེ་མིན
		printf(" kernel");
}

static void printparam(const var_na *name, const void *data, size_t size)
{
	printf(" %s=", name);
	switch (size) {
		pak_sa parimaa_na(var_na):	printf("%d", *(var_na *) data);	bha_nga;
		pak_sa parimaa_na(short):	printf("%d", *(short *) data);	bha_nga;
		pak_sa parimaa_na(ཧྲིལ):	printf("%d", *(ཧྲིལ *) data);	bha_nga;
		default:		printf("(%u bytes)", size);	bha_nga;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
static ཧྲིལ namematch(var_na **names, ཧྲིལ nnames, var_na *name)
{
	ཧྲིལ i;
	ཆེད་དུ(i = 0; i < nnames; i++)
		གལ་ཏེ(!strcmp(names[i], name))
			ཕྱིར 1;
	ཕྱིར 0;
}
#endif

void printmsg(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst,
	var_na operation, ཧྲིལ printparams)
{
	const var_na *name;
	ཧྲིལ mtype = msg->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	var_na *names[] = DEBUG_DUMPIPC_NAMES;
	ཧྲིལ nnames = parimaa_na(names)/parimaa_na(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	གལ་ཏེ(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		ཕྱིར;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	གལ་ཏེ (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} དེ་མིན {
		printf(" %d/0x%x", mtype, mtype);
	}

	གལ་ཏེ (mightbecall && printparams) {
#define IDENT(x, y) གལ་ཏེ (mtype == x) printparam(#y, &msg->y, parimaa_na(msg->y));
#include "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#གལ་ཏེ DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
static ཧྲིལ messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
static sa_nracanaa {
	ཧྲིལ src, dst, messages;
} winners[PRINTSLOTS];
static ཧྲིལ total, goodslots;

static void printstats(ཧྲིལ ticks)
{
	ཧྲིལ i;
	ཆེད་དུ(i = 0; i < goodslots; i++) {
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
	ཧྲིལ src_slot, dst_slot;
	total = goodslots = 0;
	ཆེད་དུ(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		ཆེད་དུ(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			ཧྲིལ w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			སྐབས(w > 0 && n > winners[w-1].messages)
				w--;
			གལ་ཏེ(w >= PRINTSLOTS) continue;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			assert(rem >= 0);
			assert(rem < PRINTSLOTS);
			གལ་ཏེ(rem > 0) {
				assert(w+1 <= PRINTSLOTS-1);
				assert(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*parimaa_na(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			གལ་ཏེ(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	གལ་ཏེ(p) { s = p->p_nr; } \
	དེ་མིན { s = KERNELIPC; } \
	assert(s >= 0 && s < IPCPROCS); \
}

static void statmsg(message *msg, sa_nracanaa proc *srcp, sa_nracanaa proc *dstp)
{
	ཧྲིལ src, dst, now, secs, dt;
	static ཧྲིལ lastprint;

	/* Stat message. */
	assert(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	གལ་ཏེ(secs >= 30) {
		memset(winners, 0, parimaa_na(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, parimaa_na(messages));
		lastprint = now;
	}
}
#endif

#གལ་ཏེ DEBUG_IPC_HOOK
void hook_ipc_msgkcall(message *msg, sa_nracanaa proc *proc)
{
#གལ་ཏེ DEBUG_DUMPIPC
	printmsg(msg, proc, NULL, 'k', 1);
#endif
}

void hook_ipc_msgkresult(message *msg, sa_nracanaa proc *proc)
{
#གལ་ཏེ DEBUG_DUMPIPC
	printmsg(msg, NULL, proc, 'k', 0);
#endif
#གལ་ཏེ DEBUG_IPCSTATS
	statmsg(msg, proc, NULL);
#endif
}

void hook_ipc_msgrecv(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst)
{
#གལ་ཏེ DEBUG_DUMPIPC
	printmsg(msg, src, dst, 'r', 0);
#endif
#གལ་ཏེ DEBUG_IPCSTATS
	statmsg(msg, src, dst);
#endif
}

void hook_ipc_msgsend(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst)
{
#གལ་ཏེ DEBUG_DUMPIPC
	printmsg(msg, src, dst, 's', 1);
#endif
}

void hook_ipc_clear(sa_nracanaa proc *p)
{
#གལ་ཏེ DEBUG_IPCSTATS
	ཧྲིལ slot, i;
	assert(p);
	proc2slot(p, slot);
	ཆེད་དུ(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
