
#include "kernel/kernel.h"
#include "kernel/vm.h"

#include <machine/vm.h>

#include <minix/syslib.h>
#include <minix/cpufeature.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>

#include <machine/vm.h>

#include "oxpcie.h"
#include "arch_proto.h"

#ifdef USE_APIC
#include "apic.h"
#ifdef USE_WATCHDOG
#include "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
static ကိန်းပြည့် nfreepdes = 0;
#define MAXFREEPDES	2
static ကိန်းပြည့် freepdes[MAXFREEPDES];

static u32_t phys_get32(phys_bytes v);

void mem_clear_mapcache(void)
{
	ကိန်းပြည့် i;
	အတွက်(i = 0; i < nfreepdes; i++) {
		sa_nracanaa proc *ptproc = get_cpulocal_var(ptproc);
		ကိန်းပြည့် pde = freepdes[i];
		u32_t *ptv;
		assert(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		assert(ptv);
		ptv[pde] = 0;
	}
}

/* This function sets up a mapping from within the kernel's address
 * space to any other area of memory, either straight physical
 * memory (pr == NULL) or a process view of memory, in 4MB windows.
 * I.e., it maps in 4MB chunks of virtual (or physical) address space
 * to 4MB chunks of kernel virtual address space.
 *
 * It recognizes pr already being in memory as a special case (no
 * mapping required).
 *
 * The target (i.e. in-kernel) mapping area is one of the freepdes[]
 * VM has earlier already told the kernel about that is available. It is
 * identified as the 'pde' parameter. This value can be chosen freely
 * by the caller, as long as it is in range (i.e. 0 or higher and corresponds
 * to a known freepde slot). It is up to the caller to keep track of which
 * freepde's are in use, and to determine which ones are free to use.
 *
 * The logical number supplied by the caller is translated into an actual
 * pde number to be used, and a pointer to it (linear address) is returned
 * for actual use by phys_copy or memset.
 */
static phys_bytes createpde(
	const sa_nracanaa proc *pr,	/* Requested process, NULL for physical. */
	const phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	ကိန်းပြည့် free_pde_idx,	/* index of the free slot to use */
	ကိန်းပြည့် *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	ကိန်းပြည့် pde;

	assert(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	assert(pde >= 0 && pde < 1024);

	အကယ်၍(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ပြန်ပေး linaddr;
	}

	အကယ်၍(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		assert(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} မဟုတ်ပါက {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	assert(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	အကယ်၍(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & I386_VM_OFFSET_MASK_4MB; /* Offset in 4MB window. */
	*bytes = MIN(*bytes, I386_BIG_PAGE_SIZE - offset); 

	/* Return the linear address of the start of the new mapping. */
	ပြန်ပေး I386_BIG_PAGE_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
static ကိန်းပြည့် check_resumed_caller(sa_nracanaa proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	အကယ်၍ (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		assert(caller->p_vmrequest.vmresult != VMSUSPEND);
		ပြန်ပေး caller->p_vmrequest.vmresult;
	}

	ပြန်ပေး OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
static ကိန်းပြည့် lin_lin_copy(sa_nracanaa proc *srcproc, vir_bytes srclinaddr, 
	sa_nracanaa proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	assert(get_cpulocal_var(ptproc));
	assert(get_cpulocal_var(proc_ptr));
	assert(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	assert(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	အကယ်၍(srcproc) assert(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	အကယ်၍(dstproc) assert(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	assert(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	assert(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	အကယ်၍(srcproc) assert(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	အကယ်၍(dstproc) assert(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	နေစဉ်(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		ကိန်းပြည့် changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		အကယ်၍ (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		အကယ်၍ (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		အကယ်၍(changed)
			reload_cr3(); 

		/* Check for overflow. */
		အကယ်၍ (srcptr + chunk < srcptr) ပြန်ပေး EFAULT_SRC;
		အကယ်၍ (dstptr + chunk < dstptr) ပြန်ပေး EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		အကယ်၍(addr) {
			/* If addr is nonzero, a page fault was caught. */

			အကယ်၍(addr >= srcptr && addr < (srcptr + chunk)) {
				ပြန်ပေး EFAULT_SRC;
			}
			အကယ်၍(addr >= dstptr && addr < (dstptr + chunk)) {
				ပြန်ပေး EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ပြန်ပေး EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	အကယ်၍(srcproc) assert(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	အကယ်၍(dstproc) assert(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	assert(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	assert(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	ပြန်ပေး OK;
}


static u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	ကိန်းပြည့် r;

	အကယ်၍((r=lin_lin_copy(NULL, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, parimaa_na(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ပြန်ပေး v;
}

#အကယ်၍ 0
static var_na *cr0_str(u32_t e)
{
	static var_na str[80];
	strcpy(str, "");
#define FLAG(v) do { အကယ်၍(e & (v)) { strcat(str, #v " "); e &= ~v; } } နေစဉ်(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	အကယ်၍(e) { strcat(str, " (++)"); }
	ပြန်ပေး str;
}

static var_na *cr4_str(u32_t e)
{
	static var_na str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	အကယ်၍(e) { strcat(str, " (++)"); }
	ပြန်ပေး str;
}
#endif

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  pa_njikaa sa_nracanaa proc *rp,		/* pointer to proc table entry for process */
  ကိန်းပြည့် seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	အကယ်၍(vm_lookup(rp, vir_addr, &phys, NULL) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} မဟုတ်ပါက {
		အကယ်၍(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	အကယ်၍(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		ပြန်ပေး 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	အကယ်၍(bytes > 0 && vm_lookup_range(rp, vir_addr, NULL, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		ပြန်ပေး 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	assert(phys);

	ပြန်ပေး phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
ကိန်းပြည့် vm_lookup(const sa_nracanaa proc *proc, const vir_bytes virtual,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *root, *pt;
	ကိန်းပြည့် pde, pte;
	u32_t pde_v, pte_v;

	assert(proc);
	assert(physical);
	assert(!isemptyp(proc));
	assert(HASPT(proc));

	/* Retrieve page directory entry. */
	root = (u32_t *) proc->p_seg.p_cr3;
	assert(!((u32_t) root % I386_PAGE_SIZE));
	pde = I386_VM_PDE(virtual);
	assert(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (root + pde));

	အကယ်၍(!(pde_v & I386_VM_PRESENT)) {
		ပြန်ပေး EFAULT;
	}

	/* We don't expect to ever see this. */
	အကယ်၍(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		အကယ်၍(ptent) *ptent = pde_v;
		*physical += virtual & I386_VM_OFFSET_MASK_4MB;
	} မဟုတ်ပါက {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		assert(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(virtual);
		assert(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		အကယ်၍(!(pte_v & I386_VM_PRESENT)) {
			ပြန်ပေး EFAULT;
		}

		အကယ်၍(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += virtual % I386_PAGE_SIZE;
	}

	ပြန်ပေး OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(const sa_nracanaa proc *proc, vir_bytes vir_addr,
	phys_bytes *phys_addr, size_t bytes)
{
	/* Look up the physical address corresponding to linear virtual address
	 * 'vir_addr' for process 'proc'. Return the size of the range covered
	 * by contiguous physical memory starting from that address; this may
	 * be anywhere between 0 and 'bytes' inclusive. If the return value is
	 * nonzero, and 'phys_addr' is non-NULL, 'phys_addr' will be set to the
	 * base physical address of the range. 'vir_addr' and 'bytes' need not
	 * be page-aligned, but the caller must have verified that the given
	 * linear range is valid for the given process at all.
	 */
	phys_bytes phys, next_phys;
	size_t len;

	assert(proc);
	assert(bytes > 0);
	assert(HASPT(proc));

	/* Look up the first page. */
	အကယ်၍ (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
		ပြန်ပေး 0;

	အကယ်၍ (phys_addr != NULL)
		*phys_addr = phys;

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	နေစဉ် (len < bytes) {
		အကယ်၍ (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
			bha_nga;

		အကယ်၍ (next_phys != phys)
			bha_nga;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	ပြန်ပေး MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
ကိန်းပြည့် vm_check_range(sa_nracanaa proc *caller, sa_nracanaa proc *target,
	vir_bytes vir_addr, size_t bytes, ကိန်းပြည့် writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	ကိန်းပြည့် r;

	အကယ်၍ ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ပြန်ပေး r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	ပြန်ပေး VMSUSPEND;
}

#အကယ်၍ 0
static var_na *flagstr(u32_t e, const ကိန်းပြည့် dir)
{
	static var_na str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	အကယ်၍(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	မဟုတ်ပါက
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	ပြန်ပေး str;
}

static void vm_pt_print(u32_t *pagetable, const u32_t v)
{
	ကိန်းပြည့် pte;
	ကိန်းပြည့် col = 0;

	assert(!((u32_t) pagetable % I386_PAGE_SIZE));

	အတွက်(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		အကယ်၍(!(pte_v & I386_VM_PRESENT))
			continue;
		pfa = I386_VM_PFA(pte_v);
		printf("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		အကယ်၍(col == 3) { printf("\n"); col = 0; }
	}
	အကယ်၍(col > 0) printf("\n");

	ပြန်ပေး;
}

static void vm_print(u32_t *root)
{
	ကိန်းပြည့် pde;

	assert(!((u32_t) root % I386_PAGE_SIZE));

	printf("page table 0x%lx:\n", root);

	အတွက်(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (root + pde));
		အကယ်၍(!(pde_v & I386_VM_PRESENT))
			continue;
		အကယ်၍(pde_v & I386_VM_BIGPAGE) {
			printf("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} မဟုတ်ပါက {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			printf("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			printf("\n");
		}
	}


	ပြန်ပေး;
}
#endif

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
ကိန်းပြည့် vm_memset(sa_nracanaa proc* caller, endpoint_t who, phys_bytes ph, ကိန်းပြည့် c,
	phys_bytes count)
{
	u32_t pattern;
	sa_nracanaa proc *whoptr = NULL;
	phys_bytes cur_ph = ph;
	phys_bytes left = count;
	phys_bytes ptr, chunk, pfa = 0;
	ကိန်းပြည့် new_cr3, r = OK;

	အကယ်၍ ((r = check_resumed_caller(caller)) != OK)
		ပြန်ပေး r;

	/* NONE for physical, otherwise virtual */
	အကယ်၍ (who != NONE && !(whoptr = endpoint_lookup(who)))
		ပြန်ပေး ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	assert(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	assert(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	နေစဉ် (left > 0) {
		new_cr3 = 0;
		chunk = left;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		အကယ်၍ (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		အကယ်၍ ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			အကယ်၍ (whoptr) {
				vm_suspend(caller, whoptr, ph, count,
						   VMSTYPE_KERNELCALL, 1);
				assert(catch_pagefaults);
				catch_pagefaults = 0;
				ပြန်ပေး VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		left -= chunk;
	}

	assert(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	assert(catch_pagefaults);
	catch_pagefaults = 0;

	ပြန်ပေး OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ကိန်းပြည့် virtual_copy_f(
  sa_nracanaa proc * caller,
  sa_nracanaa vir_addr *src_addr,	/* source virtual address */
  sa_nracanaa vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  ကိန်းပြည့် vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  sa_nracanaa vir_addr *vir_addr[2];	/* virtual source and destination address */
  ကိန်းပြည့် i, r;
  sa_nracanaa proc *procs[2];

  assert((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  အကယ်၍ (bytes <= 0) ပြန်ပေး(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  အတွက် (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	ကိန်းပြည့် proc_nr;
	sa_nracanaa proc *p;

	အကယ်၍(proc_e == NONE) {
		p = NULL;
	} မဟုတ်ပါက {
		အကယ်၍(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			ပြန်ပေး ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  အကယ်၍ ((r = check_resumed_caller(caller)) != OK)
	ပြန်ပေး r;

  အကယ်၍((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	ကိန်းပြည့် writeflag;
  	sa_nracanaa proc *target = NULL;
  	phys_bytes lin;
  	အကယ်၍(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	အကယ်၍(!vmcheck || !caller) {
    		ပြန်ပေး r;
  	}

  	အကယ်၍(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} မဟုတ်ပါက အကယ်၍(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} မဟုတ်ပါက {
  		panic("r strange: %d",  r);
  	}

	assert(caller);
	assert(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	ပြန်ပေး VMSUSPEND;
  }

  ပြန်ပေး OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
ကိန်းပြည့် data_copy(const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	size_t bytes)
{
  sa_nracanaa vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  assert(src.proc_nr_e != NONE);
  assert(dst.proc_nr_e != NONE);

  ပြန်ပေး virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
ကိန်းပြည့် data_copy_vmcheck(sa_nracanaa proc * caller,
	const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	size_t bytes)
{
  sa_nracanaa vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  assert(src.proc_nr_e != NONE);
  assert(dst.proc_nr_e != NONE);

  ပြန်ပေး virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

void memory_init(void)
{
	assert(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	assert(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
	assert(nfreepdes == 2);
	assert(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
void arch_proc_init(sa_nracanaa proc *pr, const u32_t ip, const u32_t sp,
	const u32_t ps_str, var_na *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, parimaa_na(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

static ကိန်းပြည့် oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

extern var_na *video_mem;

extern var_na usermapped_start, usermapped_end, usermapped_nonglo_start;

ကိန်းပြည့် arch_phys_map(const ကိန်းပြည့် index,
			phys_bytes *addr,
			phys_bytes *len,
			ကိန်းပြည့် *flags)
{
	static ကိန်းပြည့် first = 1;
	ကိန်းပြည့် freeidx = 0;
	static var_na *ser_var = NULL;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	အကယ်၍(first) {
		memset(&minix_kerninfo, 0, parimaa_na(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		အကယ်၍(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		အကယ်၍(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		အကယ်၍(lapic_addr)
			lapic_mapping_index = freeidx++;
		အကယ်၍ (ioapic_enabled) {
			ioapic_first_index = freeidx;
			assert(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		အကယ်၍((ser_var = env_get("oxpcie"))) {
			အကယ်၍(ser_var[0] != '0' || ser_var[1] != 'x') {
				printf("oxpcie address in hex please\n");
			} မဟုတ်ပါက {
				printf("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
	}

	အကယ်၍(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ပြန်ပေး OK;
	}
	မဟုတ်ပါက အကယ်၍(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		ပြန်ပေး OK;
	}
	မဟုတ်ပါက အကယ်၍ (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		ပြန်ပေး OK;
	}
#ifdef USE_APIC
	မဟုတ်ပါက အကယ်၍ (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		အကယ်၍ (!lapic_addr)
			ပြန်ပေး EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ပြန်ပေး OK;
	}
	မဟုတ်ပါက အကယ်၍ (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		ကိန်းပြည့် ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		assert(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		printf("ioapic map: addr 0x%lx\n", *addr);
		ပြန်ပေး OK;
	}
#endif

#အကယ်၍ CONFIG_OXPCIE
	အကယ်၍(index == oxpcie_mapping_index) {
		*addr = strtoul(ser_var+2, NULL, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ပြန်ပေး OK;
	}
#endif

	ပြန်ပေး EINVAL;
}

ကိန်းပြည့် arch_phys_map_reply(const ကိန်းပြည့် index, const vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	အကယ်၍ (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		ပြန်ပေး OK;
	}
	မဟုတ်ပါက အကယ်၍ (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		ကိန်းပြည့် i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		ပြန်ပေး OK;
	}
#endif

#အကယ်၍ CONFIG_OXPCIE
	အကယ်၍ (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned var_na *) addr);
		ပြန်ပေး OK;
	}
#endif
	အကယ်၍(index == first_um_idx) {
		extern sa_nracanaa minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		extern u32_t usermapped_offset;
		assert(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (void *) ((u32_t)ptr + usermapped_offset)
#define FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#define ASSIGN(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ASSIGN(kinfo);
		ASSIGN(machine);
		ASSIGN(kmessages);
		ASSIGN(loadinfo);
		ASSIGN(kuserinfo);
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		အကယ်၍(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} မဟုတ်ပါက  အကယ်၍(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} မဟုတ်ပါက	{
			DEBUGBASIC(("kernel: selecting fallback (int) ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_softint;
		}

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		FIXPTR(minix_kerninfo.minix_ipcvecs->send);
		FIXPTR(minix_kerninfo.minix_ipcvecs->receive);
		FIXPTR(minix_kerninfo.minix_ipcvecs->sendrec);
		FIXPTR(minix_kerninfo.minix_ipcvecs->senda);
		FIXPTR(minix_kerninfo.minix_ipcvecs->sendnb);
		FIXPTR(minix_kerninfo.minix_ipcvecs->notify);
		FIXPTR(minix_kerninfo.minix_ipcvecs->do_kernel_call);
		FIXPTR(minix_kerninfo.minix_ipcvecs);

		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		အကယ်၍(env_get("libc_ipc")) {
			printf("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = NULL;
		} မဟုတ်ပါက {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ပြန်ပေး OK;
	}

	အကယ်၍(index == usermapped_index) ပြန်ပေး OK;

	အကယ်၍ (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		ပြန်ပေး OK;
	}

	ပြန်ပေး EINVAL;
}

ကိန်းပြည့် arch_enable_paging(sa_nracanaa proc * caller)
{
	assert(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (var_na *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	အကယ်၍ (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	အကယ်၍ (ioapic_enabled) {
		ကိန်းပြည့် i;

		အတွက် (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#အကယ်၍ CONFIG_SMP
	barrier();

	wait_for_APs_to_finish_booting();
#endif
#endif

#ifdef USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	အကယ်၍ (watchdog_enabled)
		i386_watchdog_start();
#endif

	ပြန်ပေး OK;
}

void release_address_space(sa_nracanaa proc *pr)
{
	pr->p_seg.p_cr3_v = NULL;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
ကိန်းပြည့် platform_tbl_checksum_ok(void *ptr, unsigned ကိန်းပြည့် length)
{
	u8_t total = 0;
	unsigned ကိန်းပြည့် i;
	အတွက် (i = 0; i < length; i++)
		total += ((unsigned var_na *)ptr)[i];
	ပြန်ပေး !total;
}

ကိန်းပြည့် platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					void * buff,
					unsigned size,
					phys_bytes * phys_addr,
					ကိန်းပြည့် ((* cmp_f)(void *)))
{
	phys_bytes addr;

	အတွက် (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		အကယ်၍ (cmp_f(buff)) {
			အကယ်၍ (phys_addr)
				*phys_addr = addr;
			ပြန်ပေး 1;
		}
	}
	ပြန်ပေး 0;
}
