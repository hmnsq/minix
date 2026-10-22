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

целое runqueues_ok_cpu(unsigned cpu)
{
  целое q, l = 0;
  register struct proc *xp;
  struct proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  для (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	если (l++ > MAX_LOOP) panic("check error");
  }

  для (q=l=0; q < NR_SCHED_QUEUES; q++) {
    если (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	вернуть 0;
    }
    если (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	вернуть 0;
    }
    если (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	вернуть 0;
    }
    для(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	const vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	если(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		вернуть 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	если(dxp % sizeof(struct proc)) {
  		printf("xp not a real pointer");
		вернуть 0;
	}
	если(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		вернуть 0;
	}
	если (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		вернуть 0;
	}
        если (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		вернуть 0;
        }
        если (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		вернуть 0;
	}
	если (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		вернуть 0;
	}
	xp->p_found = 1;
	если (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		вернуть 0;
	}
	если (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		вернуть 0;
	}
    }
  }	

  для (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	если(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		вернуть 0;
	}
	если (isemptyp(xp))
		continue;
	если(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		вернуть 0;
	}
  }

  /* All is ok. */
  вернуть 1;
}

#ifdef CONFIG_SMP
static целое runqueues_ok_all(void)
{
	unsigned c;

	для (c = 0 ; c < ncpus; c++) {
		если (!runqueues_ok_cpu(c))
			вернуть 0;
	}
	вернуть 1;	
}

целое runqueues_ok(void)
{
	вернуть runqueues_ok_all();
}

#иначе

целое runqueues_ok(void)
{
	вернуть runqueues_ok_cpu(0);
}


#endif

char *
rtsflagstr(const u32_t flags)
{
	static char str[100];
	str[0] = '\0';

#define FLAG(n) если(flags & n) { strlcat(str, #n " ", sizeof(str)); }

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

	вернуть str;
}

char *
miscflagstr(const u32_t flags)
{
	static char str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	вернуть str;
}

char *
schedulerstr(struct proc *scheduler)
{
	если (scheduler != NULL)
	{
		вернуть scheduler->p_name;
	}

	вернуть "KERNEL";
}

static void
print_proc_name(struct proc *pp)
{
	char *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	если(name) {
		printf("%s(%d)", name, ep);
	}
	иначе {
		printf("%d", ep);
	}
}

static void
print_endpoint(endpoint_t ep)
{
	целое proc_nr;
	struct proc *pp = NULL;

	switch(ep) {
	case ANY:
		printf("ANY");
	break;
	case SELF:
		printf("SELF");
	break;
	case NONE:
		printf("NONE");
	break;
	default:
		если(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		иначе {
			pp = proc_addr(proc_nr);
			если(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			иначе {
				print_proc_name(pp);
			}
		}
	break;
	}
}

static void
print_sigmgr(struct proc *pp)
{
	endpoint_t sig_mgr, bak_sig_mgr;
	sig_mgr = priv(pp) ? priv(pp)->s_sig_mgr : NONE;
	bak_sig_mgr = priv(pp) ? priv(pp)->s_bak_sig_mgr : NONE;
	если(sig_mgr == NONE) { printf("no sigmgr"); вернуть; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	если(bak_sig_mgr != NONE) {
		printf(" / ");
		print_endpoint(bak_sig_mgr);
	}
}

void print_proc(struct proc *pp)
{
	endpoint_t dep;

	printf("%d: %s %d prio %d time %d/%d cycles 0x%x%08x cpu %2d "
			"pdbr 0x%lx rts %s misc %s sched %s ",
		proc_nr(pp), pp->p_name, pp->p_endpoint, 
		pp->p_priority, pp->p_user_time,
		pp->p_sys_time, ex64hi(pp->p_cycles),
		ex64lo(pp->p_cycles), pp->p_cpu,
#если defined(__i386__)
		pp->p_seg.p_cr3,
#elif defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	если(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

static void print_proc_depends(struct proc *pp, const целое level)
{
	struct proc *depproc = NULL;
	endpoint_t dep;
#define COL { целое i; для(i = 0; i < level; i++) printf("> "); }

	если(level >= NR_PROCS) {
		printf("loop??\n");
		вернуть;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	если(dep != NONE && dep != ANY) {
		целое procno;
		если(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			если(isemptyp(depproc))
				depproc = NULL;
		}
		если (depproc)
			print_proc_depends(depproc, level+1);
	}
}

void print_proc_recursive(struct proc *pp)
{
	print_proc_depends(pp, 0);
}

#если DEBUG_DUMPIPC || DEBUG_DUMPIPCF
static const char *mtypename(целое mtype, целое *possible_callname)
{
	char *callname = NULL, *errname = NULL;
	/* use generated file to recognize message types
	 *
	 * we try to match both error numbers and call numbers, as the
	 * reader can probably decide from context what's going on.
	 *
	 * whenever it might be a call number we tell the caller so the
	 * call message fields can be decoded if known.
	 */
	switch(mtype) {
#define IDENT(x) case x: callname = #x; *possible_callname = 1; break;
#include "kernel/extracted-mtype.h"
#undef IDENT
	}
	switch(mtype) {
#define IDENT(x) case x: errname = #x; break;
#include "kernel/extracted-errno.h"
#undef IDENT
	}

	/* no match */
	если(!errname && !callname)
		вернуть NULL;

	/* 2 matches */
	если(errname && callname) {
		static char typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		вернуть typename;
	}

	если(errname) вернуть errname;

	assert(callname);
	вернуть callname;
}

static void printproc(struct proc *rp)
{
	если (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	иначе
		printf(" kernel");
}

static void printparam(const char *name, const void *data, size_t size)
{
	printf(" %s=", name);
	switch (size) {
		case sizeof(char):	printf("%d", *(char *) data);	break;
		case sizeof(short):	printf("%d", *(short *) data);	break;
		case sizeof(целое):	printf("%d", *(целое *) data);	break;
		default:		printf("(%u bytes)", size);	break;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
static целое namematch(char **names, целое nnames, char *name)
{
	целое i;
	для(i = 0; i < nnames; i++)
		если(!strcmp(names[i], name))
			вернуть 1;
	вернуть 0;
}
#endif

void printmsg(message *msg, struct proc *src, struct proc *dst,
	char operation, целое printparams)
{
	const char *name;
	целое mtype = msg->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	char *names[] = DEBUG_DUMPIPC_NAMES;
	целое nnames = sizeof(names)/sizeof(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	если(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		вернуть;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	если (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} иначе {
		printf(" %d/0x%x", mtype, mtype);
	}

	если (mightbecall && printparams) {
#define IDENT(x, y) если (mtype == x) printparam(#y, &msg->y, sizeof(msg->y));
#include "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#если DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
static целое messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
static struct {
	целое src, dst, messages;
} winners[PRINTSLOTS];
static целое total, goodslots;

static void printstats(целое ticks)
{
	целое i;
	для(i = 0; i < goodslots; i++) {
#define name(s) (s == KERNELIPC ? "kernel" : proc_addr(s)->p_name)
#define persec(n) (system_hz*(n)/ticks)
		char	*n1 = name(winners[i].src),
			*n2 = name(winners[i].dst);
		printf("%2d.  %8s -> %8s  %9d/s\n",
			i, n1, n2, persec(winners[i].messages));
	}
	printf("total %d/s\n", persec(total));
}

static void sortstats(void)
{
	/* Print top message senders/receivers. */
	целое src_slot, dst_slot;
	total = goodslots = 0;
	для(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		для(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			целое w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			пока(w > 0 && n > winners[w-1].messages)
				w--;
			если(w >= PRINTSLOTS) continue;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			assert(rem >= 0);
			assert(rem < PRINTSLOTS);
			если(rem > 0) {
				assert(w+1 <= PRINTSLOTS-1);
				assert(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*sizeof(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			если(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	если(p) { s = p->p_nr; } \
	иначе { s = KERNELIPC; } \
	assert(s >= 0 && s < IPCPROCS); \
}

static void statmsg(message *msg, struct proc *srcp, struct proc *dstp)
{
	целое src, dst, now, secs, dt;
	static целое lastprint;

	/* Stat message. */
	assert(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	если(secs >= 30) {
		memset(winners, 0, sizeof(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, sizeof(messages));
		lastprint = now;
	}
}
#endif

#если DEBUG_IPC_HOOK
void hook_ipc_msgkcall(message *msg, struct proc *proc)
{
#если DEBUG_DUMPIPC
	printmsg(msg, proc, NULL, 'k', 1);
#endif
}

void hook_ipc_msgkresult(message *msg, struct proc *proc)
{
#если DEBUG_DUMPIPC
	printmsg(msg, NULL, proc, 'k', 0);
#endif
#если DEBUG_IPCSTATS
	statmsg(msg, proc, NULL);
#endif
}

void hook_ipc_msgrecv(message *msg, struct proc *src, struct proc *dst)
{
#если DEBUG_DUMPIPC
	printmsg(msg, src, dst, 'r', 0);
#endif
#если DEBUG_IPCSTATS
	statmsg(msg, src, dst);
#endif
}

void hook_ipc_msgsend(message *msg, struct proc *src, struct proc *dst)
{
#если DEBUG_DUMPIPC
	printmsg(msg, src, dst, 's', 1);
#endif
}

void hook_ipc_clear(struct proc *p)
{
#если DEBUG_IPCSTATS
	целое slot, i;
	assert(p);
	proc2slot(p, slot);
	для(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
