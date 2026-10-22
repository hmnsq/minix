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

整数 runqueues_ok_cpu(unsigned cpu)
{
  整数 q, l = 0;
  register struct proc *xp;
  struct proc **rdy_head, **rdy_tail;

  rdy_head = get_cpu_var(cpu, run_q_head);
  rdy_tail = get_cpu_var(cpu, run_q_tail);

  为 (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	xp->p_found = 0;
	如果 (l++ > MAX_LOOP) panic("check error");
  }

  为 (q=l=0; q < NR_SCHED_QUEUES; q++) {
    如果 (rdy_head[q] && !rdy_tail[q]) {
	printf("head but no tail in %d\n", q);
	返回 0;
    }
    如果 (!rdy_head[q] && rdy_tail[q]) {
	printf("tail but no head in %d\n", q);
	返回 0;
    }
    如果 (rdy_tail[q] && rdy_tail[q]->p_nextready) {
	printf("tail and tail->next not null in %d\n", q);
	返回 0;
    }
    为(xp = rdy_head[q]; xp; xp = xp->p_nextready) {
	const vir_bytes vxp = (vir_bytes) xp;
	vir_bytes dxp;
	如果(vxp < (vir_bytes) BEG_PROC_ADDR || vxp >= (vir_bytes) END_PROC_ADDR) {
  		printf("xp out of range\n");
		返回 0;
	}
	dxp = vxp - (vir_bytes) BEG_PROC_ADDR;
	如果(dxp % sizeof(struct proc)) {
  		printf("xp not a real pointer");
		返回 0;
	}
	如果(!proc_ptr_ok(xp)) {
  		printf("xp bogus pointer");
		返回 0;
	}
	如果 (RTS_ISSET(xp, RTS_SLOT_FREE)) {
		printf("scheduling error: dead proc q %d %d\n",
			q, xp->p_endpoint);
		返回 0;
	}
        如果 (!proc_is_runnable(xp)) {
		printf("scheduling error: unready on runq %d proc %d\n",
			q, xp->p_nr);
		返回 0;
        }
        如果 (xp->p_priority != q) {
		printf("scheduling error: wrong priority q %d proc %d ep %d name %s\n",
			q, xp->p_nr, xp->p_endpoint, xp->p_name);
		返回 0;
	}
	如果 (xp->p_found) {
		printf("scheduling error: double sched q %d proc %d\n",
			q, xp->p_nr);
		返回 0;
	}
	xp->p_found = 1;
	如果 (!xp->p_nextready && rdy_tail[q] != xp) {
		printf("sched err: last element not tail q %d proc %d\n",
			q, xp->p_nr);
		返回 0;
	}
	如果 (l++ > MAX_LOOP) {
		printf("loop in schedule queue?");
		返回 0;
	}
    }
  }	

  为 (xp = BEG_PROC_ADDR; xp < END_PROC_ADDR; ++xp) {
	如果(!proc_ptr_ok(xp)) {
		printf("xp bogus pointer in proc table\n");
		返回 0;
	}
	如果 (isemptyp(xp))
		continue;
	如果(proc_is_runnable(xp) && !xp->p_found) {
		printf("sched error: ready proc %d not on queue\n", xp->p_nr);
		返回 0;
	}
  }

  /* All is ok. */
  返回 1;
}

#ifdef CONFIG_SMP
static 整数 runqueues_ok_all(void)
{
	unsigned c;

	为 (c = 0 ; c < ncpus; c++) {
		如果 (!runqueues_ok_cpu(c))
			返回 0;
	}
	返回 1;	
}

整数 runqueues_ok(void)
{
	返回 runqueues_ok_all();
}

#否则

整数 runqueues_ok(void)
{
	返回 runqueues_ok_cpu(0);
}


#endif

char *
rtsflagstr(const u32_t flags)
{
	static char str[100];
	str[0] = '\0';

#define FLAG(n) 如果(flags & n) { strlcat(str, #n " ", sizeof(str)); }

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

	返回 str;
}

char *
miscflagstr(const u32_t flags)
{
	static char str[100];
	str[0] = '\0';

	FLAG(MF_REPLY_PEND);
	FLAG(MF_DELIVERMSG);
	FLAG(MF_KCALL_RESUME);

	返回 str;
}

char *
schedulerstr(struct proc *scheduler)
{
	如果 (scheduler != NULL)
	{
		返回 scheduler->p_name;
	}

	返回 "KERNEL";
}

static void
print_proc_name(struct proc *pp)
{
	char *name = pp->p_name;
	endpoint_t ep = pp->p_endpoint;

	如果(name) {
		printf("%s(%d)", name, ep);
	}
	否则 {
		printf("%d", ep);
	}
}

static void
print_endpoint(endpoint_t ep)
{
	整数 proc_nr;
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
		如果(!isokendpt(ep, &proc_nr)) {
			printf("??? %d\n", ep);
		}
		否则 {
			pp = proc_addr(proc_nr);
			如果(isemptyp(pp)) {
				printf("??? empty slot %d\n", proc_nr);
			}
			否则 {
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
	如果(sig_mgr == NONE) { printf("no sigmgr"); 返回; }
	printf("sigmgr ");
	print_endpoint(sig_mgr);
	如果(bak_sig_mgr != NONE) {
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
#如果 defined(__i386__)
		pp->p_seg.p_cr3,
#elif defined(__arm__)
		pp->p_seg.p_ttbr,
#endif
		rtsflagstr(pp->p_rts_flags), miscflagstr(pp->p_misc_flags),
		schedulerstr(pp->p_scheduler));

	print_sigmgr(pp);

	dep = P_BLOCKEDON(pp);
	如果(dep != NONE) {
		printf(" blocked on: ");
		print_endpoint(dep);
	}
	printf("\n");
}

static void print_proc_depends(struct proc *pp, const 整数 level)
{
	struct proc *depproc = NULL;
	endpoint_t dep;
#define COL { 整数 i; 为(i = 0; i < level; i++) printf("> "); }

	如果(level >= NR_PROCS) {
		printf("loop??\n");
		返回;
	}

	COL

	print_proc(pp);

	COL
	proc_stacktrace(pp);


	dep = P_BLOCKEDON(pp);
	如果(dep != NONE && dep != ANY) {
		整数 procno;
		如果(isokendpt(dep, &procno)) {
			depproc = proc_addr(procno);
			如果(isemptyp(depproc))
				depproc = NULL;
		}
		如果 (depproc)
			print_proc_depends(depproc, level+1);
	}
}

void print_proc_recursive(struct proc *pp)
{
	print_proc_depends(pp, 0);
}

#如果 DEBUG_DUMPIPC || DEBUG_DUMPIPCF
static const char *mtypename(整数 mtype, 整数 *possible_callname)
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
	如果(!errname && !callname)
		返回 NULL;

	/* 2 matches */
	如果(errname && callname) {
		static char typename[100];
		strcpy(typename, errname);
		strcat(typename, " / ");
		strcat(typename, callname);
		返回 typename;
	}

	如果(errname) 返回 errname;

	assert(callname);
	返回 callname;
}

static void printproc(struct proc *rp)
{
	如果 (rp)
		printf(" %s(%d)", rp->p_name, rp - proc);
	否则
		printf(" kernel");
}

static void printparam(const char *name, const void *data, size_t size)
{
	printf(" %s=", name);
	switch (size) {
		case sizeof(char):	printf("%d", *(char *) data);	break;
		case sizeof(short):	printf("%d", *(short *) data);	break;
		case sizeof(整数):	printf("%d", *(整数 *) data);	break;
		default:		printf("(%u bytes)", size);	break;
	}
}

#ifdef DEBUG_DUMPIPC_NAMES
static 整数 namematch(char **names, 整数 nnames, char *name)
{
	整数 i;
	为(i = 0; i < nnames; i++)
		如果(!strcmp(names[i], name))
			返回 1;
	返回 0;
}
#endif

void printmsg(message *msg, struct proc *src, struct proc *dst,
	char operation, 整数 printparams)
{
	const char *name;
	整数 mtype = msg->m_type, mightbecall = 0;

#ifdef DEBUG_DUMPIPC_NAMES
  {
	char *names[] = DEBUG_DUMPIPC_NAMES;
	整数 nnames = sizeof(names)/sizeof(names[0]);

	/* skip printing messages for messages neither to
	 * or from DEBUG_DUMPIPC_EP if it is defined; either
	 * can be NULL to indicate kernel
	 */
	如果(!(src && namematch(names, nnames, src->p_name)) &&
	   !(dst && namematch(names, nnames, dst->p_name))) {
		返回;
	}
  }
#endif

	/* source, destination and message type */
	printf("%c", operation);
	printproc(src);
	printproc(dst);
	name = mtypename(mtype, &mightbecall);
	如果 (name) {
		printf(" %s(%d/0x%x)", name, mtype, mtype);
	} 否则 {
		printf(" %d/0x%x", mtype, mtype);
	}

	如果 (mightbecall && printparams) {
#define IDENT(x, y) 如果 (mtype == x) printparam(#y, &msg->y, sizeof(msg->y));
#include "kernel/extracted-mfield.h"
#undef IDENT
	}
	printf("\n");
}
#endif

#如果 DEBUG_IPCSTATS
#define IPCPROCS (NR_PROCS+1)	/* number of slots we need */
#define KERNELIPC NR_PROCS	/* slot number to use for kernel calls */
static 整数 messages[IPCPROCS][IPCPROCS];

#define PRINTSLOTS 20
static struct {
	整数 src, dst, messages;
} winners[PRINTSLOTS];
static 整数 total, goodslots;

static void printstats(整数 ticks)
{
	整数 i;
	为(i = 0; i < goodslots; i++) {
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
	整数 src_slot, dst_slot;
	total = goodslots = 0;
	为(src_slot = 0; src_slot < IPCPROCS; src_slot++) {
		为(dst_slot = 0; dst_slot < IPCPROCS; dst_slot++) {
			整数 w = PRINTSLOTS, rem,
				n = messages[src_slot][dst_slot];
			total += n;
			当(w > 0 && n > winners[w-1].messages)
				w--;
			如果(w >= PRINTSLOTS) continue;

			/* This combination has beaten the current winners
			 * and should be inserted at position 'w.'
			 */
			rem = PRINTSLOTS-w-1;
			assert(rem >= 0);
			assert(rem < PRINTSLOTS);
			如果(rem > 0) {
				assert(w+1 <= PRINTSLOTS-1);
				assert(w >= 0);
				memmove(&winners[w+1], &winners[w],
					rem*sizeof(winners[0]));
			}
			winners[w].src = src_slot;
			winners[w].dst = dst_slot;
			winners[w].messages = n;
			如果(goodslots < PRINTSLOTS) goodslots++;
		}
	}
}

#define proc2slot(p, s) { \
	如果(p) { s = p->p_nr; } \
	否则 { s = KERNELIPC; } \
	assert(s >= 0 && s < IPCPROCS); \
}

static void statmsg(message *msg, struct proc *srcp, struct proc *dstp)
{
	整数 src, dst, now, secs, dt;
	static 整数 lastprint;

	/* Stat message. */
	assert(src);
	proc2slot(srcp, src);
	proc2slot(dstp, dst);
	messages[src][dst]++;

	/* Print something? */
	now = get_monotonic();
	dt = now - lastprint;
	secs = dt/system_hz;
	如果(secs >= 30) {
		memset(winners, 0, sizeof(winners));
		sortstats();
		printstats(dt);
		memset(messages, 0, sizeof(messages));
		lastprint = now;
	}
}
#endif

#如果 DEBUG_IPC_HOOK
void hook_ipc_msgkcall(message *msg, struct proc *proc)
{
#如果 DEBUG_DUMPIPC
	printmsg(msg, proc, NULL, 'k', 1);
#endif
}

void hook_ipc_msgkresult(message *msg, struct proc *proc)
{
#如果 DEBUG_DUMPIPC
	printmsg(msg, NULL, proc, 'k', 0);
#endif
#如果 DEBUG_IPCSTATS
	statmsg(msg, proc, NULL);
#endif
}

void hook_ipc_msgrecv(message *msg, struct proc *src, struct proc *dst)
{
#如果 DEBUG_DUMPIPC
	printmsg(msg, src, dst, 'r', 0);
#endif
#如果 DEBUG_IPCSTATS
	statmsg(msg, src, dst);
#endif
}

void hook_ipc_msgsend(message *msg, struct proc *src, struct proc *dst)
{
#如果 DEBUG_DUMPIPC
	printmsg(msg, src, dst, 's', 1);
#endif
}

void hook_ipc_clear(struct proc *p)
{
#如果 DEBUG_IPCSTATS
	整数 slot, i;
	assert(p);
	proc2slot(p, slot);
	为(i = 0; i < IPCPROCS; i++)
		messages[slot][i] = messages[i][slot] = 0;
#endif
}
#endif
