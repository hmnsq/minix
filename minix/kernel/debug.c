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

ကိန်းပြည့် runqueues_ok_cpu(unsigned cpu)
{
  ကိန်းပြည့် q, l = 0;
  pa_njikaa sa_nracanaa proc *xp;
  sa_nracanaa proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  အတွက် (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	အကယ်၍ (l++ > MAX_LOOP) panic("check error");
  }

  အတွက် (q=l=0; q < NR_SCHED_QUEUES; q++) {
    အကယ်၍ (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	ပြန်ပေး 0;
    }
    အကယ်၍ (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	ပြန်ပေး 0;
    }
    အကယ်၍ (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	ပြန်ပေး 0;
    }
    အတွက်(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	const vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	အကယ်၍(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		ပြန်ပေး 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	အကယ်၍(dxp % parimaa_na(sa_nracanaa proc)) {
  		printf("xp not a real pointer");
		ပြန်ပေး 0;
	}
	အကယ်၍(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		ပြန်ပေး 0;
	}
	အကယ်၍ (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		ပြန်ပေး 0;
	}
        အကယ်၍ (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		ပြန်ပေး 0;
        }
        အကယ်၍ (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		ပြန်ပေး 0;
	}
	အကယ်၍ (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		ပြန်ပေး 0;
	}
	xp->p_found = 1;
	အကယ်၍ (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		ပြန်ပေး 0;
	}
	အကယ်၍ (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		ပြန်ပေး 0;
	}
    }
  }	

  အတွက် (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	အကယ်၍(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		ပြန်ပေး 0;
	}
	အကယ်၍ (isemptyp(xp))
		continue;
	အကယ်၍(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		ပြန်ပေး 0;
	}
  }

  /* All is ok. */
  ပြန်ပေး 1;
}

#ifdef CONFIG_SMP
static ကိန်းပြည့် runqueues_ok_all(void)
{
	unsigned c;

	အတွက် (c = 0 ; c < ncpus; c++) {
		အကယ်၍ (!runqueues_ok_cpu(c))
			ပြန်ပေး 0;
	}
	ပြန်ပေး 1;	
}

ကိန်းပြည့် runqueues_ok(void)
{
	ပြန်ပေး runqueues_ok_all();
}

#မဟုတ်ပါက

ကိန်းပြည့် runqueues_ok(void)
{
	ပြန်ပေး runqueues_ok_cpu(0);
}


#endif

var_na *
rtsflagstr(const u32_t flags)
{
	static var_na str[100];
	str[0] = '\0';

#define FLAG(n) အကယ်၍(flags & n) { strlcat(str, #n " ", parimaa_na(str)); }

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

	ပြန်ပေး str;
}

var_na *
miscflagstr(const u32_t flags)
{
	static var_na str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	ပြန်ပေး str;
}

var_na *
schedulerstr(sa_nracanaa proc *scheduler)
{
	အကယ်၍ (scheduler != NULL)
	{
		ပြန်ပေး scheduler->p_name;
	}

	ပြန်ပေး "KERNEL";
}

static void
print_proc_name(sa_nracanaa proc *pp)
{
	var_na *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	အကယ်၍(name) {
		printf("%s(%d)", name, ep);
	}
	မဟုတ်ပါက {
		printf("%d", ep);
	}
}

static void
print_endpoint(endpoint_t ep)
{
	ကိန်းပြည့် proc_nr;
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
		အကယ်၍(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		မဟုတ်ပါက {
			pp = proc_addr(proc_nr);
			အကယ်၍(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			မဟုတ်ပါက {
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
	အကယ်၍(sig_mgr == NONE) { printf("no sigmgr"); ပြန်ပေး; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	အကယ်၍(bak_sig_mgr != NONE) {
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
#အကယ်၍ defined(__i386__)
		pp->p_seg.p_cr3,
#elif defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	အကယ်၍(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

static void print_proc_depends(sa_nracanaa proc *pp, const ကိန်းပြည့် level)
{
	sa_nracanaa proc *depproc = NULL;
	endpoint_t dep;
#define COL { ကိန်းပြည့် i; အတွက်(i = 0; i < level; i++) printf("> "); }

	အကယ်၍(level >= NR_PROCS) {
		printf("loop??\n");
		ပြန်ပေး;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	အကယ်၍(dep != NONE && dep != ANY) {
		ကိန်းပြည့် procno;
		အကယ်၍(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			အကယ်၍(isemptyp(depproc))
				depproc = NULL;
		}
		အကယ်၍ (depproc)
			print_proc_depends(depproc, level+1);
	}
}

void print_proc_recursive(sa_nracanaa proc *pp)
{
	print_proc_depends(pp, 0);
}

#အကယ်၍ DEBUG_DUMPIPC || DEBUG_DUMPIPCF
static const var_na *mtypename(ကိန်းပြည့် mtype, ကိန်းပြည့် *possible_callname)
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
	အကယ်၍(!errname && !callname)
		ပြန်ပေး NULL;

	/* 2 matches */
	အကယ်၍(errname && callname) {
		static var_na typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		ပြန်ပေး typename;
	}

	အကယ်၍(errname) ပြန်ပေး errname;

	assert(callname);
	ပြန်ပေး callname;
}

static void printproc(sa_nracanaa proc *rp)
{
	အကယ်၍ (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	မဟုတ်ပါက
		printf(" kernel");
}

static void printparam(const var_na *name, const void *data, size_t size)
{
	printf(" %s=", name);
	switch (size) {
		pak_sa parimaa_na(var_na):	printf("%d", *(var_na *) data);	bha_nga;
		pak_sa parimaa_na(short):	printf("%d", *(short *) data);	bha_nga;
		pak_sa parimaa_na(ကိန်းပြည့်):	printf("%d", *(ကိန်းပြည့် *) data);	bha_nga;
		default:		printf("(%u bytes)", size);	bha_nga;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
static ကိန်းပြည့် namematch(var_na **names, ကိန်းပြည့် nnames, var_na *name)
{
	ကိန်းပြည့် i;
	အတွက်(i = 0; i < nnames; i++)
		အကယ်၍(!strcmp(names[i], name))
			ပြန်ပေး 1;
	ပြန်ပေး 0;
}
#endif

void printmsg(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst,
	var_na operation, ကိန်းပြည့် printparams)
{
	const var_na *name;
	ကိန်းပြည့် mtype = msg->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	var_na *names[] = DEBUG_DUMPIPC_NAMES;
	ကိန်းပြည့် nnames = parimaa_na(names)/parimaa_na(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	အကယ်၍(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		ပြန်ပေး;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	အကယ်၍ (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} မဟုတ်ပါက {
		printf(" %d/0x%x", mtype, mtype);
	}

	အကယ်၍ (mightbecall && printparams) {
#define IDENT(x, y) အကယ်၍ (mtype == x) printparam(#y, &msg->y, parimaa_na(msg->y));
#include "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#အကယ်၍ DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
static ကိန်းပြည့် messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
static sa_nracanaa {
	ကိန်းပြည့် src, dst, messages;
} winners[PRINTSLOTS];
static ကိန်းပြည့် total, goodslots;

static void printstats(ကိန်းပြည့် ticks)
{
	ကိန်းပြည့် i;
	အတွက်(i = 0; i < goodslots; i++) {
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
	ကိန်းပြည့် src_slot, dst_slot;
	total = goodslots = 0;
	အတွက်(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		အတွက်(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			ကိန်းပြည့် w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			နေစဉ်(w > 0 && n > winners[w-1].messages)
				w--;
			အကယ်၍(w >= PRINTSLOTS) continue;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			assert(rem >= 0);
			assert(rem < PRINTSLOTS);
			အကယ်၍(rem > 0) {
				assert(w+1 <= PRINTSLOTS-1);
				assert(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*parimaa_na(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			အကယ်၍(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	အကယ်၍(p) { s = p->p_nr; } \
	မဟုတ်ပါက { s = KERNELIPC; } \
	assert(s >= 0 && s < IPCPROCS); \
}

static void statmsg(message *msg, sa_nracanaa proc *srcp, sa_nracanaa proc *dstp)
{
	ကိန်းပြည့် src, dst, now, secs, dt;
	static ကိန်းပြည့် lastprint;

	/* Stat message. */
	assert(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	အကယ်၍(secs >= 30) {
		memset(winners, 0, parimaa_na(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, parimaa_na(messages));
		lastprint = now;
	}
}
#endif

#အကယ်၍ DEBUG_IPC_HOOK
void hook_ipc_msgkcall(message *msg, sa_nracanaa proc *proc)
{
#အကယ်၍ DEBUG_DUMPIPC
	printmsg(msg, proc, NULL, 'k', 1);
#endif
}

void hook_ipc_msgkresult(message *msg, sa_nracanaa proc *proc)
{
#အကယ်၍ DEBUG_DUMPIPC
	printmsg(msg, NULL, proc, 'k', 0);
#endif
#အကယ်၍ DEBUG_IPCSTATS
	statmsg(msg, proc, NULL);
#endif
}

void hook_ipc_msgrecv(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst)
{
#အကယ်၍ DEBUG_DUMPIPC
	printmsg(msg, src, dst, 'r', 0);
#endif
#အကယ်၍ DEBUG_IPCSTATS
	statmsg(msg, src, dst);
#endif
}

void hook_ipc_msgsend(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst)
{
#အကယ်၍ DEBUG_DUMPIPC
	printmsg(msg, src, dst, 's', 1);
#endif
}

void hook_ipc_clear(sa_nracanaa proc *p)
{
#အကယ်၍ DEBUG_IPCSTATS
	ကိန်းပြည့် slot, i;
	assert(p);
	proc2slot(p, slot);
	အတွက်(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
