
#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "kernel/proc.h"
#ଅନ୍ତର୍ଭାଵ "kernel/vm.h"

#ଅନ୍ତର୍ଭାଵ <machine/vm.h>

#ଅନ୍ତର୍ଭାଵ <minix/type.h>
#ଅନ୍ତର୍ଭାଵ <minix/board.h>
#ଅନ୍ତର୍ଭାଵ <minix/syslib.h>
#ଅନ୍ତର୍ଭାଵ <minix/cpufeature.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ <machine/vm.h>

#ଅନ୍ତର୍ଭାଵ "arch_proto.h"
#ଅନ୍ତର୍ଭାଵ "kernel/proto.h"
#ଅନ୍ତର୍ଭାଵ "kernel/debug.h"
#ଅନ୍ତର୍ଭାଵ "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfreepdes = 0;
#define MAXFREEPDES	2
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା freepdes[MAXFREEPDES];

ସ୍ଥିର u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
ସ୍ଥିର kern_phys_map *kern_phys_map_head;

ଶୂନ୍ୟ mem_clear_mapcache(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପାଇଁ(i = 0; i < nfreepdes; i++) {
		ସଂରଚନା proc *ptproc = get_cpulocal_var(ptproc);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pde = freepdes[i];
		u32_t *ptv;
		ପ୍ରତିଜ୍ଞା(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		ପ୍ରତିଜ୍ଞା(ptv);
		ptv[pde] = 0;
	}
}

/* This function sets up a mapping from within the kernel's address
 * space to any other area of memory, either straight physical
 * memory (pr == NULL) or a process view of memory, in 1MB windows.
 * I.e., it maps in 1MB chunks of virtual (or physical) address space
 * to 1MB chunks of kernel virtual address space.
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
ସ୍ଥିର phys_bytes createpde(
	ଅଚଳ ସଂରଚନା proc *pr,	/* Requested process, NULL for physical. */
	ଅଚଳ phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_pde_idx,	/* index of the free slot to use */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pde;

	ପ୍ରତିଜ୍ଞା(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	ପ୍ରତିଜ୍ଞା(pde >= 0 && pde < 4096);

	ଯଦି(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ଫେରନ୍ତୁ linaddr;
	}

	ଯଦି(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		ପ୍ରତିଜ୍ଞା(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} ଅନ୍ୟଥା {
		/* Requested address is physical. Make up the PDE entry. */
		ପ୍ରତିଜ୍ଞା (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

		/* memory */
		pdeval = (linaddr & ARM_VM_SECTION_MASK)
			| ARM_VM_SECTION
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED
			| ARM_VM_SECTION_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ଯଦି(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & ARM_VM_OFFSET_MASK_1MB; /* Offset in 1MB window. */
	*bytes = MIN(*bytes, ARM_SECTION_SIZE - offset);

	/* Return the linear address of the start of the new mapping. */
	ଫେରନ୍ତୁ ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_resumed_caller(ସଂରଚନା proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	ଯଦି (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		ପ୍ରତିଜ୍ଞା(caller->p_vmrequest.vmresult != VMSUSPEND);
		ଫେରନ୍ତୁ caller->p_vmrequest.vmresult;
	}

	ଫେରନ୍ତୁ OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା lin_lin_copy(ସଂରଚନା proc *srcproc, vir_bytes srclinaddr, 
	ସଂରଚନା proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc));
	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(proc_ptr));
	ପ୍ରତିଜ୍ଞା(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ପ୍ରତିଜ୍ଞା(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	ଯଦି(srcproc) ପ୍ରତିଜ୍ଞା(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	ଯଦି(dstproc) ପ୍ରତିଜ୍ଞା(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ପ୍ରତିଜ୍ଞା(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ଯଦି(srcproc) ପ୍ରତିଜ୍ଞା(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	ଯଦି(dstproc) ପ୍ରତିଜ୍ଞା(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	ଯେତେବେଳେ(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		ଯଦି (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		ଯଦି (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		ଯଦି(changed)
			reload_ttbr0();

		/* Check for overflow. */
		ଯଦି (srcptr + chunk < srcptr) ଫେରନ୍ତୁ EFAULT_SRC;
		ଯଦି (dstptr + chunk < dstptr) ଫେରନ୍ତୁ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ଯଦି(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			ଯଦି(addr >= src_aligned && addr < (srcptr + chunk)) {
				ଫେରନ୍ତୁ EFAULT_SRC;
			}
			ଯଦି(addr >= dst_aligned && addr < (dstptr + chunk)) {
				ଫେରନ୍ତୁ EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ଫେରନ୍ତୁ EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	ଯଦି(srcproc) ପ୍ରତିଜ୍ଞା(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	ଯଦି(dstproc) ପ୍ରତିଜ୍ଞା(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ପ୍ରତିଜ୍ଞା(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

	ଯଦି((r=lin_lin_copy(ଖାଲି, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, ଆକାର(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ଫେରନ୍ତୁ v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  ପଞ୍ଜିକରଣ ସଂରଚନା proc *rp,		/* pointer to proc table entry for process */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	ଯଦି(vm_lookup(rp, vir_addr, &phys, ଖାଲି) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} ଅନ୍ୟଥା {
		ଯଦି(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	ଯଦି(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		ଫେରନ୍ତୁ 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	ଯଦି(bytes > 0 && vm_lookup_range(rp, vir_addr, ଖାଲି, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		ଫେରନ୍ତୁ 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	ପ୍ରତିଜ୍ଞା(phys);

	ଫେରନ୍ତୁ phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_lookup(ଅଚଳ ସଂରଚନା proc *proc, ଅଚଳ vir_bytes ଆଭାସୀ,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *ମୂଳ, *pt;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pde, pte;
	u32_t pde_v, pte_v;

	ପ୍ରତିଜ୍ଞା(proc);
	ପ୍ରତିଜ୍ଞା(physical);
	ପ୍ରତିଜ୍ଞା(!isemptyp(proc));
	ପ୍ରତିଜ୍ଞା(HASPT(proc));

	/* Retrieve page directory entry. */
	ମୂଳ = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	ପ୍ରତିଜ୍ଞା(!((u32_t) ମୂଳ % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(ଆଭାସୀ);
	ପ୍ରତିଜ୍ଞା(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ମୂଳ + pde));

	ଯଦି(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		ଫେରନ୍ତୁ EFAULT;
	}

	ଯଦି(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		ଯଦି(ptent) *ptent = pde_v;
		*physical += ଆଭାସୀ & ARM_VM_OFFSET_MASK_1MB;
	} ଅନ୍ୟଥା  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		ପ୍ରତିଜ୍ଞା(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(ଆଭାସୀ);
		ପ୍ରତିଜ୍ଞା(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ଯଦି(!(pte_v & ARM_VM_PTE_PRESENT)) {
			ଫେରନ୍ତୁ EFAULT;
		}

		ଯଦି(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += ଆଭାସୀ % ARM_PAGE_SIZE;
	}

	ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ଅଚଳ ସଂରଚନା proc *proc, vir_bytes vir_addr,
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

	ପ୍ରତିଜ୍ଞା(proc);
	ପ୍ରତିଜ୍ଞା(bytes > 0);
	ପ୍ରତିଜ୍ଞା(HASPT(proc));

	/* Look up the first page. */
	ଯଦି (vm_lookup(proc, vir_addr, &phys, ଖାଲି) != OK)
		ଫେରନ୍ତୁ 0;

	ଯଦି (phys_addr != ଖାଲି)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ଯେତେବେଳେ (len < bytes) {
		ଯଦି (vm_lookup(proc, vir_addr, &phys, ଖାଲି) != OK)
			ଭାଙ୍ଗ;

		ଯଦି (next_phys != phys)
			ଭାଙ୍ଗ;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	ଫେରନ୍ତୁ MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_check_range(ସଂରଚନା proc *caller, ସଂରଚନା proc *target,
	vir_bytes vir_addr, size_t bytes, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

	ଯଦି ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ଫେରନ୍ତୁ r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	ଫେରନ୍ତୁ VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_memset(ସଂରଚନା proc* caller, endpoint_t who, phys_bytes ph, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c,
	phys_bytes ଗଣନା_2)
{
	u32_t pattern;
	ସଂରଚନା proc *whoptr = ଖାଲି;
	phys_bytes cur_ph = ph;
	phys_bytes ବାମ = ଗଣନା_2;
	phys_bytes ptr, chunk, pfa = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_ttbr, r = OK;

	ଯଦି ((r = check_resumed_caller(caller)) != OK)
		ଫେରନ୍ତୁ r;

	/* NONE for physical, otherwise virtual */
	ଯଦି (who != NONE && !(whoptr = endpoint_lookup(who)))
		ଫେରନ୍ତୁ ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ପ୍ରତିଜ୍ଞା(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	ଯେତେବେଳେ (ବାମ > 0) {
		new_ttbr = 0;
		chunk = ବାମ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		ଯଦି (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		ଯଦି ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			ଯଦି (whoptr) {
				vm_suspend(caller, whoptr, ph, ଗଣନା_2,
						   VMSTYPE_KERNELCALL, 1);
				ପ୍ରତିଜ୍ଞା(catch_pagefaults);
				catch_pagefaults = 0;
				ଫେରନ୍ତୁ VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		ବାମ -= chunk;
	}

	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ପ୍ରତିଜ୍ଞା(catch_pagefaults);
	catch_pagefaults = 0;

	ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା virtual_copy_f(
  ସଂରଚନା proc * caller,
  ସଂରଚନା vir_addr *src_addr,		/* source virtual address */
  ସଂରଚନା vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ସଂରଚନା vir_addr *vir_addr[2];	/* virtual source and destination address */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, r;
  ସଂରଚନା proc *procs[2];

  ପ୍ରତିଜ୍ଞା((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  ଯଦି (bytes <= 0) ଫେରନ୍ତୁ(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  ପାଇଁ (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;
	ସଂରଚନା proc *p;

	ଯଦି(proc_e == NONE) {
		p = ଖାଲି;
	} ଅନ୍ୟଥା {
		ଯଦି(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			ଫେରନ୍ତୁ ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  ଯଦି ((r = check_resumed_caller(caller)) != OK)
	ଫେରନ୍ତୁ r;

  ଯଦି((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା writeflag;
  	ସଂରଚନା proc *target = ଖାଲି;
  	phys_bytes lin;
  	ଯଦି(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	ଯଦି(!vmcheck || !caller) {
    		ଫେରନ୍ତୁ r;
  	}

  	ଯଦି(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} ଅନ୍ୟଥା ଯଦି(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} ଅନ୍ୟଥା {
  		panic("r strange: %d",  r);
  	}

	ପ୍ରତିଜ୍ଞା(caller);
	ପ୍ରତିଜ୍ଞା(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	ଫେରନ୍ତୁ VMSUSPEND;
  }

  ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା data_copy(ଅଚଳ endpoint_t from_proc, ଅଚଳ vir_bytes from_addr,
	ଅଚଳ endpoint_t to_proc, ଅଚଳ vir_bytes to_addr,
	size_t bytes)
{
  ସଂରଚନା vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  ପ୍ରତିଜ୍ଞା(src.proc_nr_e != NONE);
  ପ୍ରତିଜ୍ଞା(dst.proc_nr_e != NONE);

  ଫେରନ୍ତୁ virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା data_copy_vmcheck(ସଂରଚନା proc * caller,
	ଅଚଳ endpoint_t from_proc, ଅଚଳ vir_bytes from_addr,
	ଅଚଳ endpoint_t to_proc, ଅଚଳ vir_bytes to_addr,
	size_t bytes)
{
  ସଂରଚନା vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  ପ୍ରତିଜ୍ଞା(src.proc_nr_e != NONE);
  ପ୍ରତିଜ୍ଞା(dst.proc_nr_e != NONE);

  ଫେରନ୍ତୁ virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

ଶୂନ୍ୟ memory_init(ଶୂନ୍ୟ)
{
	ପ୍ରତିଜ୍ଞା(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	ପ୍ରତିଜ୍ଞା(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	ପ୍ରତିଜ୍ଞା(nfreepdes == 2);
	ପ୍ରତିଜ୍ଞା(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
ଶୂନ୍ୟ arch_proc_init(ସଂରଚନା proc *pr, ଅଚଳ u32_t ip, ଅଚଳ u32_t sp,
	ଅଚଳ u32_t ps_str, ଅକ୍ଷର *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
ବାହ୍ୟ ଅକ୍ଷର usermapped_start, usermapped_end, usermapped_nonglo_start;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_phys_map(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
			phys_bytes *addr,
			phys_bytes *len,
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flags)
{
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା first = 1;
	kern_phys_map *phys_maps;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	ଯଦି(first) {
		memset(&minix_kerninfo, 0, ଆକାର(minix_kerninfo));
		ଯଦି(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		ଯଦି(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		ଯେତେବେଳେ(phys_maps != ଖାଲି){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	ଯଦି(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ଫେରନ୍ତୁ OK;
	}
	ଅନ୍ୟଥା ଯଦି(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		ଫେରନ୍ତୁ OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	ଯେତେବେଳେ(phys_maps != ଖାଲି){
		ଯଦି(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			ଫେରନ୍ତୁ OK;
		}
		phys_maps = phys_maps->next;
	}

	ଫେରନ୍ତୁ EINVAL;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_phys_map_reply(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, ଅଚଳ vir_bytes addr)
{
	kern_phys_map *phys_maps;

	ଯଦି(index == first_um_idx) {
		u32_t usermapped_offset;
		ପ୍ରତିଜ୍ଞା(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (ଶୂନ୍ୟ *) ((u32_t)ptr + usermapped_offset)
#define FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#define ASSIGN(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ASSIGN(kinfo);
		ASSIGN(machine);
		ASSIGN(kmessages);
		ASSIGN(loadinfo);
		ASSIGN(kuserinfo);
		ASSIGN(arm_frclock);
		ASSIGN(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ଫେରନ୍ତୁ OK;
	}

	ଯଦି (index == usermapped_index) {
		ଫେରନ୍ତୁ OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	ଯେତେବେଳେ(phys_maps != ଖାଲି){
		ଯଦି(phys_maps->index == index){
			ପ୍ରତିଜ୍ଞା(phys_maps->cb != ଖାଲି);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			ଫେରନ୍ତୁ OK;
		}
		phys_maps = phys_maps->next;
	}

	ଫେରନ୍ତୁ EINVAL;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_enable_paging(ସଂରଚନା proc * caller)
{
	kern_phys_map *phys_maps;
	ପ୍ରତିଜ୍ଞା(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	ଯେତେବେଳେ(phys_maps != ଖାଲି){
		ପ୍ରତିଜ୍ଞା(phys_maps->cb != ଖାଲି);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	ଫେରନ୍ତୁ OK;
}

ଶୂନ୍ୟ release_address_space(ସଂରଚନା proc *pr)
{
	pr->p_seg.p_ttbr_v = ଖାଲି;
	barrier();
}



/*
 * Request a physical mapping
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	ପ୍ରତିଜ୍ଞା(base_address != 0);
	ପ୍ରତିଜ୍ଞା(io_size % ARM_PAGE_SIZE == 0);
	ପ୍ରତିଜ୍ଞା(cb != ଖାଲି);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = ଖାଲି;
	

	ଯଦି (kern_phys_map_head == ଖାଲି){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = ଖାଲି;
	} ଅନ୍ୟଥା {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	ଫେରନ୍ତୁ 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	ଫେରନ୍ତୁ 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	ଫେରନ୍ତୁ kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

