
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "kernel/proc.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ಅನ್ತರ್ಭಾವ <minix/type.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <minix/syslib.h>
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"
#ಅನ್ತರ್ಭಾವ "kernel/debug.h"
#ಅನ್ತರ್ಭಾವ "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nfreepdes = 0;
#define MAXFREEPDES	2
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ freepdes[MAXFREEPDES];

ಸ್ಥಿರ u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
ಸ್ಥಿರ kern_phys_map *kern_phys_map_head;

ಶೂನ್ಯ mem_clear_mapcache(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪ್ರತಿ(i = 0; i < nfreepdes; i++) {
		ಸಂರಚನಾ proc *ptproc = get_cpulocal_var(ptproc);
		ಪೂರ್ಣಾಙ್ಕ pde = freepdes[i];
		u32_t *ptv;
		ಪ್ರತಿಜ್ಞಾ(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		ಪ್ರತಿಜ್ಞಾ(ptv);
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
ಸ್ಥಿರ phys_bytes createpde(
	ಧ್ರುವ ಸಂರಚನಾ proc *pr,	/* Requested process, NULL for physical. */
	ಧ್ರುವ phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	ಪೂರ್ಣಾಙ್ಕ free_pde_idx,	/* index of the free slot to use */
	ಪೂರ್ಣಾಙ್ಕ *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	ಪೂರ್ಣಾಙ್ಕ pde;

	ಪ್ರತಿಜ್ಞಾ(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	ಪ್ರತಿಜ್ಞಾ(pde >= 0 && pde < 4096);

	ಯದಿ(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ಪ್ರತ್ಯಾಗಮ linaddr;
	}

	ಯದಿ(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		ಪ್ರತಿಜ್ಞಾ(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} ಅನ್ಯಥಾ {
		/* Requested address is physical. Make up the PDE entry. */
		ಪ್ರತಿಜ್ಞಾ (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

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
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ಯದಿ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
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
	ಪ್ರತ್ಯಾಗಮ ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_resumed_caller(ಸಂರಚನಾ proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	ಯದಿ (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		ಪ್ರತಿಜ್ಞಾ(caller->p_vmrequest.vmresult != VMSUSPEND);
		ಪ್ರತ್ಯಾಗಮ caller->p_vmrequest.vmresult;
	}

	ಪ್ರತ್ಯಾಗಮ OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lin_lin_copy(ಸಂರಚನಾ proc *srcproc, vir_bytes srclinaddr, 
	ಸಂರಚನಾ proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(proc_ptr));
	ಪ್ರತಿಜ್ಞಾ(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ಪ್ರತಿಜ್ಞಾ(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	ಯದಿ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	ಯದಿ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ಯದಿ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	ಯದಿ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	ಯಾವತ್(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		ಪೂರ್ಣಾಙ್ಕ changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		ಯದಿ (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		ಯದಿ (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		ಯದಿ(changed)
			reload_ttbr0();

		/* Check for overflow. */
		ಯದಿ (srcptr + chunk < srcptr) ಪ್ರತ್ಯಾಗಮ EFAULT_SRC;
		ಯದಿ (dstptr + chunk < dstptr) ಪ್ರತ್ಯಾಗಮ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ಯದಿ(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			ಯದಿ(addr >= src_aligned && addr < (srcptr + chunk)) {
				ಪ್ರತ್ಯಾಗಮ EFAULT_SRC;
			}
			ಯದಿ(addr >= dst_aligned && addr < (dstptr + chunk)) {
				ಪ್ರತ್ಯಾಗಮ EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ಪ್ರತ್ಯಾಗಮ EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	ಯದಿ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	ಯದಿ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	ಪ್ರತ್ಯಾಗಮ OK;
}

ಸ್ಥಿರ u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	ಪೂರ್ಣಾಙ್ಕ r;

	ಯದಿ((r=lin_lin_copy(ರಿಕ್ತ, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, ಪರಿಮಾಣ(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ಪ್ರತ್ಯಾಗಮ v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp,		/* pointer to proc table entry for process */
  ಪೂರ್ಣಾಙ್ಕ seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	ಯದಿ(vm_lookup(rp, vir_addr, &phys, ರಿಕ್ತ) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} ಅನ್ಯಥಾ {
		ಯದಿ(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	ಯದಿ(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	ಯದಿ(bytes > 0 && vm_lookup_range(rp, vir_addr, ರಿಕ್ತ, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	ಪ್ರತಿಜ್ಞಾ(phys);

	ಪ್ರತ್ಯಾಗಮ phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ vm_lookup(ಧ್ರುವ ಸಂರಚನಾ proc *proc, ಧ್ರುವ vir_bytes ಆಭಾಸ,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *ಬೇರು, *pt;
	ಪೂರ್ಣಾಙ್ಕ pde, pte;
	u32_t pde_v, pte_v;

	ಪ್ರತಿಜ್ಞಾ(proc);
	ಪ್ರತಿಜ್ಞಾ(physical);
	ಪ್ರತಿಜ್ಞಾ(!isemptyp(proc));
	ಪ್ರತಿಜ್ಞಾ(HASPT(proc));

	/* Retrieve page directory entry. */
	ಬೇರು = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	ಪ್ರತಿಜ್ಞಾ(!((u32_t) ಬೇರು % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(ಆಭಾಸ);
	ಪ್ರತಿಜ್ಞಾ(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ಬೇರು + pde));

	ಯದಿ(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		ಪ್ರತ್ಯಾಗಮ EFAULT;
	}

	ಯದಿ(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		ಯದಿ(ptent) *ptent = pde_v;
		*physical += ಆಭಾಸ & ARM_VM_OFFSET_MASK_1MB;
	} ಅನ್ಯಥಾ  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		ಪ್ರತಿಜ್ಞಾ(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(ಆಭಾಸ);
		ಪ್ರತಿಜ್ಞಾ(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ಯದಿ(!(pte_v & ARM_VM_PTE_PRESENT)) {
			ಪ್ರತ್ಯಾಗಮ EFAULT;
		}

		ಯದಿ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += ಆಭಾಸ % ARM_PAGE_SIZE;
	}

	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ಧ್ರುವ ಸಂರಚನಾ proc *proc, vir_bytes vir_addr,
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

	ಪ್ರತಿಜ್ಞಾ(proc);
	ಪ್ರತಿಜ್ಞಾ(bytes > 0);
	ಪ್ರತಿಜ್ಞಾ(HASPT(proc));

	/* Look up the first page. */
	ಯದಿ (vm_lookup(proc, vir_addr, &phys, ರಿಕ್ತ) != OK)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ (phys_addr != ರಿಕ್ತ)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ಯಾವತ್ (len < bytes) {
		ಯದಿ (vm_lookup(proc, vir_addr, &phys, ರಿಕ್ತ) != OK)
			ಭಙ್ಗ;

		ಯದಿ (next_phys != phys)
			ಭಙ್ಗ;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	ಪ್ರತ್ಯಾಗಮ MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ vm_check_range(ಸಂರಚನಾ proc *caller, ಸಂರಚನಾ proc *target,
	vir_bytes vir_addr, size_t bytes, ಪೂರ್ಣಾಙ್ಕ writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	ಪೂರ್ಣಾಙ್ಕ r;

	ಯದಿ ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ಪ್ರತ್ಯಾಗಮ r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	ಪ್ರತ್ಯಾಗಮ VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ vm_memset(ಸಂರಚನಾ proc* caller, endpoint_t who, phys_bytes ph, ಪೂರ್ಣಾಙ್ಕ c,
	phys_bytes ಎಣಿಕೆ)
{
	u32_t pattern;
	ಸಂರಚನಾ proc *whoptr = ರಿಕ್ತ;
	phys_bytes cur_ph = ph;
	phys_bytes ಎಡ = ಎಣಿಕೆ;
	phys_bytes ptr, chunk, pfa = 0;
	ಪೂರ್ಣಾಙ್ಕ new_ttbr, r = OK;

	ಯದಿ ((r = check_resumed_caller(caller)) != OK)
		ಪ್ರತ್ಯಾಗಮ r;

	/* NONE for physical, otherwise virtual */
	ಯದಿ (who != NONE && !(whoptr = endpoint_lookup(who)))
		ಪ್ರತ್ಯಾಗಮ ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ಪ್ರತಿಜ್ಞಾ(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	ಯಾವತ್ (ಎಡ > 0) {
		new_ttbr = 0;
		chunk = ಎಡ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		ಯದಿ (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		ಯದಿ ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			ಯದಿ (whoptr) {
				vm_suspend(caller, whoptr, ph, ಎಣಿಕೆ,
						   VMSTYPE_KERNELCALL, 1);
				ಪ್ರತಿಜ್ಞಾ(catch_pagefaults);
				catch_pagefaults = 0;
				ಪ್ರತ್ಯಾಗಮ VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		ಎಡ -= chunk;
	}

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ಪ್ರತಿಜ್ಞಾ(catch_pagefaults);
	catch_pagefaults = 0;

	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ virtual_copy_f(
  ಸಂರಚನಾ proc * caller,
  ಸಂರಚನಾ vir_addr *src_addr,		/* source virtual address */
  ಸಂರಚನಾ vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  ಪೂರ್ಣಾಙ್ಕ vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ಸಂರಚನಾ vir_addr *vir_addr[2];	/* virtual source and destination address */
  ಪೂರ್ಣಾಙ್ಕ i, r;
  ಸಂರಚನಾ proc *procs[2];

  ಪ್ರತಿಜ್ಞಾ((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  ಯದಿ (bytes <= 0) ಪ್ರತ್ಯಾಗಮ(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  ಪ್ರತಿ (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	ಪೂರ್ಣಾಙ್ಕ proc_nr;
	ಸಂರಚನಾ proc *p;

	ಯದಿ(proc_e == NONE) {
		p = ರಿಕ್ತ;
	} ಅನ್ಯಥಾ {
		ಯದಿ(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			ಪ್ರತ್ಯಾಗಮ ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  ಯದಿ ((r = check_resumed_caller(caller)) != OK)
	ಪ್ರತ್ಯಾಗಮ r;

  ಯದಿ((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	ಪೂರ್ಣಾಙ್ಕ writeflag;
  	ಸಂರಚನಾ proc *target = ರಿಕ್ತ;
  	phys_bytes lin;
  	ಯದಿ(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	ಯದಿ(!vmcheck || !caller) {
    		ಪ್ರತ್ಯಾಗಮ r;
  	}

  	ಯದಿ(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} ಅನ್ಯಥಾ ಯದಿ(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} ಅನ್ಯಥಾ {
  		panic("r strange: %d",  r);
  	}

	ಪ್ರತಿಜ್ಞಾ(caller);
	ಪ್ರತಿಜ್ಞಾ(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	ಪ್ರತ್ಯಾಗಮ VMSUSPEND;
  }

  ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ data_copy(ಧ್ರುವ endpoint_t from_proc, ಧ್ರುವ vir_bytes from_addr,
	ಧ್ರುವ endpoint_t to_proc, ಧ್ರುವ vir_bytes to_addr,
	size_t bytes)
{
  ಸಂರಚನಾ vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  ಪ್ರತಿಜ್ಞಾ(src.proc_nr_e != NONE);
  ಪ್ರತಿಜ್ಞಾ(dst.proc_nr_e != NONE);

  ಪ್ರತ್ಯಾಗಮ virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ data_copy_vmcheck(ಸಂರಚನಾ proc * caller,
	ಧ್ರುವ endpoint_t from_proc, ಧ್ರುವ vir_bytes from_addr,
	ಧ್ರುವ endpoint_t to_proc, ಧ್ರುವ vir_bytes to_addr,
	size_t bytes)
{
  ಸಂರಚನಾ vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  ಪ್ರತಿಜ್ಞಾ(src.proc_nr_e != NONE);
  ಪ್ರತಿಜ್ಞಾ(dst.proc_nr_e != NONE);

  ಪ್ರತ್ಯಾಗಮ virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

ಶೂನ್ಯ memory_init(ಶೂನ್ಯ)
{
	ಪ್ರತಿಜ್ಞಾ(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	ಪ್ರತಿಜ್ಞಾ(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	ಪ್ರತಿಜ್ಞಾ(nfreepdes == 2);
	ಪ್ರತಿಜ್ಞಾ(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
ಶೂನ್ಯ arch_proc_init(ಸಂರಚನಾ proc *pr, ಧ್ರುವ u32_t ip, ಧ್ರುವ u32_t sp,
	ಧ್ರುವ u32_t ps_str, ವರ್ಣ *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
ಬಾಹ್ಯ ವರ್ಣ usermapped_start, usermapped_end, usermapped_nonglo_start;

ಪೂರ್ಣಾಙ್ಕ arch_phys_map(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ index,
			phys_bytes *addr,
			phys_bytes *len,
			ಪೂರ್ಣಾಙ್ಕ *flags)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ first = 1;
	kern_phys_map *phys_maps;

	ಪೂರ್ಣಾಙ್ಕ freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	ಯದಿ(first) {
		memset(&minix_kerninfo, 0, ಪರಿಮಾಣ(minix_kerninfo));
		ಯದಿ(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		ಯದಿ(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		ಯಾವತ್(phys_maps != ರಿಕ್ತ){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	ಯದಿ(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ಪ್ರತ್ಯಾಗಮ OK;
	}
	ಅನ್ಯಥಾ ಯದಿ(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		ಪ್ರತ್ಯಾಗಮ OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	ಯಾವತ್(phys_maps != ರಿಕ್ತ){
		ಯದಿ(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			ಪ್ರತ್ಯಾಗಮ OK;
		}
		phys_maps = phys_maps->next;
	}

	ಪ್ರತ್ಯಾಗಮ EINVAL;
}

ಪೂರ್ಣಾಙ್ಕ arch_phys_map_reply(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ index, ಧ್ರುವ vir_bytes addr)
{
	kern_phys_map *phys_maps;

	ಯದಿ(index == first_um_idx) {
		u32_t usermapped_offset;
		ಪ್ರತಿಜ್ಞಾ(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (ಶೂನ್ಯ *) ((u32_t)ptr + usermapped_offset)
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

		ಪ್ರತ್ಯಾಗಮ OK;
	}

	ಯದಿ (index == usermapped_index) {
		ಪ್ರತ್ಯಾಗಮ OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	ಯಾವತ್(phys_maps != ರಿಕ್ತ){
		ಯದಿ(phys_maps->index == index){
			ಪ್ರತಿಜ್ಞಾ(phys_maps->cb != ರಿಕ್ತ);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			ಪ್ರತ್ಯಾಗಮ OK;
		}
		phys_maps = phys_maps->next;
	}

	ಪ್ರತ್ಯಾಗಮ EINVAL;
}

ಪೂರ್ಣಾಙ್ಕ arch_enable_paging(ಸಂರಚನಾ proc * caller)
{
	kern_phys_map *phys_maps;
	ಪ್ರತಿಜ್ಞಾ(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	ಯಾವತ್(phys_maps != ರಿಕ್ತ){
		ಪ್ರತಿಜ್ಞಾ(phys_maps->cb != ರಿಕ್ತ);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	ಪ್ರತ್ಯಾಗಮ OK;
}

ಶೂನ್ಯ release_address_space(ಸಂರಚನಾ proc *pr)
{
	pr->p_seg.p_ttbr_v = ರಿಕ್ತ;
	barrier();
}



/*
 * Request a physical mapping
 */
ಪೂರ್ಣಾಙ್ಕ kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       ಪೂರ್ಣಾಙ್ಕ vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	ಪ್ರತಿಜ್ಞಾ(base_address != 0);
	ಪ್ರತಿಜ್ಞಾ(io_size % ARM_PAGE_SIZE == 0);
	ಪ್ರತಿಜ್ಞಾ(cb != ರಿಕ್ತ);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = ರಿಕ್ತ;
	

	ಯದಿ (kern_phys_map_head == ರಿಕ್ತ){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = ರಿಕ್ತ;
	} ಅನ್ಯಥಾ {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
ಪೂರ್ಣಾಙ್ಕ kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
ಪೂರ್ಣಾಙ್ಕ kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	ಪೂರ್ಣಾಙ್ಕ vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	ಪ್ರತ್ಯಾಗಮ kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

