
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/type.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/syslib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/debug.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_timer.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
ਸਥਿਰ ਪੂਰਨ nfreepdes = 0;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXFREEPDES	2
ਸਥਿਰ ਪੂਰਨ freepdes[MAXFREEPDES];

ਸਥਿਰ u32_t phys_get32(phys_ਬਾਈਟਾਂ v);

/* list of requested physical mapping */
ਸਥਿਰ kern_phys_map *kern_phys_map_ਸਿਰ;

ਖਾਲੀ mem_clear_mapcache(ਖਾਲੀ)
{
	ਪੂਰਨ i;
	ਲਈ(i = 0; i < nfreepdes; i++) {
		ਬਣਤਰ proc *ptproc = get_cpulocal_var(ptproc);
		ਪੂਰਨ pde = freepdes[i];
		u32_t *ptv;
		ਪ੍ਰਤਿਜ੍ਞਾ(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		ਪ੍ਰਤਿਜ੍ਞਾ(ptv);
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
ਸਥਿਰ phys_ਬਾਈਟਾਂ createpde(
	ਅਚਲ ਬਣਤਰ proc *pr,	/* Requested process, NULL for physical. */
	ਅਚਲ phys_ਬਾਈਟਾਂ linaddr,/* Address after segment translation. */
	phys_ਬਾਈਟਾਂ *ਬਾਈਟਾਂ,	/* Size of chunk, function may truncate it. */
	ਪੂਰਨ free_pde_idx,	/* index of the free slot to use */
	ਪੂਰਨ *ਬਦਲਿਆ		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_ਬਾਈਟਾਂ ਵਿੱਥ;
	ਪੂਰਨ pde;

	ਪ੍ਰਤਿਜ੍ਞਾ(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	ਪ੍ਰਤਿਜ੍ਞਾ(pde >= 0 && pde < 4096);

	ਜੇ(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ਵਾਪਸ linaddr;
	}

	ਜੇ(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		ਪ੍ਰਤਿਜ੍ਞਾ(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} ਨਹੀਂਤਾਂ {
		/* Requested address is physical. Make up the PDE entry. */
		ਪ੍ਰਤਿਜ੍ਞਾ (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_ਸਿਰਾ);

		/*ਸਮ੍ਰਿਤੀ*/
		pdeval = (linaddr & ARM_VM_SECTION_ਨਕਾਬ)
			| ARM_VM_SECTION
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED
			| ARM_VM_SECTION_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ਜੇ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*ਬਦਲਿਆ = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	ਵਿੱਥ = linaddr & ARM_VM_ਵਿੱਥ_ਨਕਾਬ_1_MB; /* Offset in 1MB window. */
	*ਬਾਈਟਾਂ = MIN(*ਬਾਈਟਾਂ, ARM_SECTION_ਮਾਪ - ਵਿੱਥ);

	/* Return the linear address of the start of the new mapping. */
	ਵਾਪਸ ARM_SECTION_ਮਾਪ*pde + ਵਿੱਥ;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ check_resumed_caller(ਬਣਤਰ proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	ਜੇ (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		ਪ੍ਰਤਿਜ੍ਞਾ(caller->p_vmrequest.vmresult != VMSUSPEND);
		ਵਾਪਸ caller->p_vmrequest.vmresult;
	}

	ਵਾਪਸ OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ lin_lin_copy(ਬਣਤਰ proc *srcproc, vir_ਬਾਈਟਾਂ srclinaddr, 
	ਬਣਤਰ proc *dstproc, vir_ਬਾਈਟਾਂ dstlinaddr, vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ)
{
	u32_t addr;
	proc_nr_t procslot;

	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc));
	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(proc_ptr));
	ਪ੍ਰਤਿਜ੍ਞਾ(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ਪ੍ਰਤਿਜ੍ਞਾ(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	ਜੇ(srcproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(srcproc, RTS_ਥਾਂ_FREE));
	ਜੇ(dstproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(dstproc, RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ਜੇ(srcproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	ਜੇ(dstproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	ਜਦੋਂ(ਬਾਈਟਾਂ > 0) {
		phys_ਬਾਈਟਾਂ srcptr, dstptr;
		vir_ਬਾਈਟਾਂ chunk = ਬਾਈਟਾਂ;
		ਪੂਰਨ ਬਦਲਿਆ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
		ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;

		ਜੇ (srcproc && GET_ਦੋਆਧਾਰੀ_ਅੰਕ(srcproc->p_stale_tlb, cpu)) {
			ਬਦਲਿਆ = 1;
			UNSET_ਦੋਆਧਾਰੀ_ਅੰਕ(srcproc->p_stale_tlb, cpu);
		}
		ਜੇ (dstproc && GET_ਦੋਆਧਾਰੀ_ਅੰਕ(dstproc->p_stale_tlb, cpu)) {
			ਬਦਲਿਆ = 1;
			UNSET_ਦੋਆਧਾਰੀ_ਅੰਕ(dstproc->p_stale_tlb, cpu);
		}
#ਜੇ_ਅੰਤ

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &ਬਦਲਿਆ);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &ਬਦਲਿਆ);
		ਜੇ(ਬਦਲਿਆ)
			reload_ttbr0();

		/* Check for overflow. */
		ਜੇ (srcptr + chunk < srcptr) ਵਾਪਸ EFAULT_SRC;
		ਜੇ (dstptr + chunk < dstptr) ਵਾਪਸ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ਜੇ(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_ਬਾਈਟਾਂ src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			ਜੇ(addr >= src_aligned && addr < (srcptr + chunk)) {
				ਵਾਪਸ EFAULT_SRC;
			}
			ਜੇ(addr >= dst_aligned && addr < (dstptr + chunk)) {
				ਵਾਪਸ EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ਵਾਪਸ EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		ਬਾਈਟਾਂ -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	ਜੇ(srcproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(srcproc, RTS_ਥਾਂ_FREE));
	ਜੇ(dstproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(dstproc, RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	ਵਾਪਸ OK;
}

ਸਥਿਰ u32_t phys_get32(phys_ਬਾਈਟਾਂ addr)
{
	u32_t v;
	ਪੂਰਨ r;

	ਜੇ((r=lin_lin_copy(ਨਿਰਾ, addr, 
		proc_addr(SYSTEM), (phys_ਬਾਈਟਾਂ) &v, ਆਕਾਰ(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ਵਾਪਸ v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_ਬਾਈਟਾਂ umap_virtual(
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp,		/* pointer to proc table entry for process */
  ਪੂਰਨ seg,				/* T, D, or S segment */
  vir_ਬਾਈਟਾਂ vir_addr,			/* virtual address in bytes within the seg */
  vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ			/* # of bytes to be copied */
)
{
	phys_ਬਾਈਟਾਂ phys = 0;

	ਜੇ(vm_lookup(rp, vir_addr, &phys, ਨਿਰਾ) != OK) {
		ਲਿਖੋ("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} ਨਹੀਂਤਾਂ {
		ਜੇ(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	ਜੇ(phys == 0) {
		ਲਿਖੋ("SYSTEM:umap_virtual: lookup failed\n");
		ਵਾਪਸ 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	ਜੇ(ਬਾਈਟਾਂ > 0 && vm_lookup_range(rp, vir_addr, ਨਿਰਾ, ਬਾਈਟਾਂ) != ਬਾਈਟਾਂ) {
		ਲਿਖੋ("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, ਬਾਈਟਾਂ, vir_addr, vir_addr);
		ਵਾਪਸ 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	ਪ੍ਰਤਿਜ੍ਞਾ(phys);

	ਵਾਪਸ phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
ਪੂਰਨ vm_lookup(ਅਚਲ ਬਣਤਰ proc *proc, ਅਚਲ vir_ਬਾਈਟਾਂ ਆਭਾਸੀ,
 phys_ਬਾਈਟਾਂ *physical, u32_t *ptent)
{
	u32_t *ਜੜ੍ਹ, *pt;
	ਪੂਰਨ pde, pte;
	u32_t pde_v, pte_v;

	ਪ੍ਰਤਿਜ੍ਞਾ(proc);
	ਪ੍ਰਤਿਜ੍ਞਾ(physical);
	ਪ੍ਰਤਿਜ੍ਞਾ(!isemptyp(proc));
	ਪ੍ਰਤਿਜ੍ਞਾ(HASPT(proc));

	/* Retrieve page directory entry. */
	ਜੜ੍ਹ = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_ਨਕਾਬ);
	ਪ੍ਰਤਿਜ੍ਞਾ(!((u32_t) ਜੜ੍ਹ % ARM_PAGEDIR_ਮਾਪ));
	pde = ARM_VM_PDE(ਆਭਾਸੀ);
	ਪ੍ਰਤਿਜ੍ਞਾ(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ਜੜ੍ਹ + pde));

	ਜੇ(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		ਵਾਪਸ EFAULT;
	}

	ਜੇ(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_ਨਕਾਬ;
		ਜੇ(ptent) *ptent = pde_v;
		*physical += ਆਭਾਸੀ & ARM_VM_ਵਿੱਥ_ਨਕਾਬ_1_MB;
	} ਨਹੀਂਤਾਂ  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_ਨਕਾਬ);
		ਪ੍ਰਤਿਜ੍ਞਾ(!((u32_t) pt % ARM_PAGETABLE_ਮਾਪ));
		pte = ARM_VM_PTE(ਆਭਾਸੀ);
		ਪ੍ਰਤਿਜ੍ਞਾ(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ਜੇ(!(pte_v & ARM_VM_PTE_PRESENT)) {
			ਵਾਪਸ EFAULT;
		}

		ਜੇ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_ਨਕਾਬ;
		*physical += ਆਭਾਸੀ % ARM_ਪੰਨਾ_ਮਾਪ;
	}

	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ਅਚਲ ਬਣਤਰ proc *proc, vir_ਬਾਈਟਾਂ vir_addr,
	phys_ਬਾਈਟਾਂ *phys_addr, size_t ਬਾਈਟਾਂ)
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
	phys_ਬਾਈਟਾਂ phys, ਅਗਲਾ_phys;
	size_t len;

	ਪ੍ਰਤਿਜ੍ਞਾ(proc);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਬਾਈਟਾਂ > 0);
	ਪ੍ਰਤਿਜ੍ਞਾ(HASPT(proc));

	/* Look up the first page. */
	ਜੇ (vm_lookup(proc, vir_addr, &phys, ਨਿਰਾ) != OK)
		ਵਾਪਸ 0;

	ਜੇ (phys_addr != ਨਿਰਾ)
		*phys_addr = phys;

	len = ARM_ਪੰਨਾ_ਮਾਪ - (vir_addr % ARM_ਪੰਨਾ_ਮਾਪ);
	vir_addr += len;
	ਅਗਲਾ_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ਜਦੋਂ (len < ਬਾਈਟਾਂ) {
		ਜੇ (vm_lookup(proc, vir_addr, &phys, ਨਿਰਾ) != OK)
			ਰੋਕੋ;

		ਜੇ (ਅਗਲਾ_phys != phys)
			ਰੋਕੋ;

		len += ARM_ਪੰਨਾ_ਮਾਪ;
		vir_addr += ARM_ਪੰਨਾ_ਮਾਪ;
		ਅਗਲਾ_phys += ARM_ਪੰਨਾ_ਮਾਪ;
	}

	/* We might now have overshot the requested length somewhat. */
	ਵਾਪਸ MIN(ਬਾਈਟਾਂ, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
ਪੂਰਨ vm_check_range(ਬਣਤਰ proc *caller, ਬਣਤਰ proc *ਨਿਸ਼ਾਨਾ,
	vir_ਬਾਈਟਾਂ vir_addr, size_t ਬਾਈਟਾਂ, ਪੂਰਨ writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	ਪੂਰਨ r;

	ਜੇ ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ਵਾਪਸ r;

	vm_suspend(caller, ਨਿਸ਼ਾਨਾ, vir_addr, ਬਾਈਟਾਂ, VMSTYPE_KERNELCALL,
		writeflag);

	ਵਾਪਸ VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
ਪੂਰਨ vm_memset(ਬਣਤਰ proc* caller, endpoint_t who, phys_ਬਾਈਟਾਂ ph, ਪੂਰਨ c,
	phys_ਬਾਈਟਾਂ ਗਿਣਤੀ_2)
{
	u32_t ਨਮੂਨਾ;
	ਬਣਤਰ proc *whoptr = ਨਿਰਾ;
	phys_ਬਾਈਟਾਂ cur_ph = ph;
	phys_ਬਾਈਟਾਂ ਖੱਬਾ = ਗਿਣਤੀ_2;
	phys_ਬਾਈਟਾਂ ptr, chunk, pfa = 0;
	ਪੂਰਨ new_ttbr, r = OK;

	ਜੇ ((r = check_resumed_caller(caller)) != OK)
		ਵਾਪਸ r;

	/* NONE for physical, otherwise virtual */
	ਜੇ (who != NONE && !(whoptr = endpoint_lookup(who)))
		ਵਾਪਸ ESRCH;

	c &= 0xFF;
	ਨਮੂਨਾ = c | (c << 8) | (c << 16) | (c << 24);

	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ਪ੍ਰਤਿਜ੍ਞਾ(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	ਜਦੋਂ (ਖੱਬਾ > 0) {
		new_ttbr = 0;
		chunk = ਖੱਬਾ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		ਜੇ (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		ਜੇ ((pfa = phys_memset(ptr, ਨਮੂਨਾ, chunk))) {

			/* If a process pagefaults, VM may help out */
			ਜੇ (whoptr) {
				vm_suspend(caller, whoptr, ph, ਗਿਣਤੀ_2,
						   VMSTYPE_KERNELCALL, 1);
				ਪ੍ਰਤਿਜ੍ਞਾ(catch_pagefaults);
				catch_pagefaults = 0;
				ਵਾਪਸ VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		ਖੱਬਾ -= chunk;
	}

	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ਪ੍ਰਤਿਜ੍ਞਾ(catch_pagefaults);
	catch_pagefaults = 0;

	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ਪੂਰਨ virtual_copy_f(
  ਬਣਤਰ proc * caller,
  ਬਣਤਰ vir_addr *src_addr,		/* source virtual address */
  ਬਣਤਰ vir_addr *dst_addr,		/* destination virtual address */
  vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ,			/* # of bytes to copy  */
  ਪੂਰਨ vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ਬਣਤਰ vir_addr *vir_addr[2];	/* virtual source and destination address */
  ਪੂਰਨ i, r;
  ਬਣਤਰ proc *procs[2];

  ਪ੍ਰਤਿਜ੍ਞਾ((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  ਜੇ (ਬਾਈਟਾਂ <= 0) ਵਾਪਸ(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  ਲਈ (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	ਪੂਰਨ proc_nr;
	ਬਣਤਰ proc *p;

	ਜੇ(proc_e == NONE) {
		p = ਨਿਰਾ;
	} ਨਹੀਂਤਾਂ {
		ਜੇ(!isokendpt(proc_e, &proc_nr)) {
			ਲਿਖੋ("virtual_copy: no reasonable endpoint\n");
			ਵਾਪਸ ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  ਜੇ ((r = check_resumed_caller(caller)) != OK)
	ਵਾਪਸ r;

  ਜੇ((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->ਵਿੱਥ,
  	procs[_DST_], vir_addr[_DST_]->ਵਿੱਥ, ਬਾਈਟਾਂ)) != OK) {
	ਪੂਰਨ writeflag;
  	ਬਣਤਰ proc *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;
  	phys_ਬਾਈਟਾਂ lin;
  	ਜੇ(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	ਜੇ(!vmcheck || !caller) {
    		ਵਾਪਸ r;
  	}

  	ਜੇ(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->ਵਿੱਥ;
  		ਨਿਸ਼ਾਨਾ = procs[_SRC_];
		writeflag = 0;
  	} ਨਹੀਂਤਾਂ ਜੇ(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->ਵਿੱਥ;
  		ਨਿਸ਼ਾਨਾ = procs[_DST_];
		writeflag = 1;
  	} ਨਹੀਂਤਾਂ {
  		panic("r strange: %d",  r);
  	}

	ਪ੍ਰਤਿਜ੍ਞਾ(caller);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);

	vm_suspend(caller, ਨਿਸ਼ਾਨਾ, lin, ਬਾਈਟਾਂ, VMSTYPE_KERNELCALL, writeflag);
	ਵਾਪਸ VMSUSPEND;
  }

  ਵਾਪਸ OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
ਪੂਰਨ ਅੰਕੜੇ_copy(ਅਚਲ endpoint_t from_proc, ਅਚਲ vir_ਬਾਈਟਾਂ from_addr,
	ਅਚਲ endpoint_t ਮੰਜ਼ਿਲ_proc, ਅਚਲ vir_ਬਾਈਟਾਂ ਮੰਜ਼ਿਲ_addr,
	size_t ਬਾਈਟਾਂ)
{
  ਬਣਤਰ vir_addr src, dst;

  src.ਵਿੱਥ = from_addr;
  dst.ਵਿੱਥ = ਮੰਜ਼ਿਲ_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = ਮੰਜ਼ਿਲ_proc;
  ਪ੍ਰਤਿਜ੍ਞਾ(src.proc_nr_e != NONE);
  ਪ੍ਰਤਿਜ੍ਞਾ(dst.proc_nr_e != NONE);

  ਵਾਪਸ virtual_copy(&src, &dst, ਬਾਈਟਾਂ);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
ਪੂਰਨ ਅੰਕੜੇ_copy_vmcheck(ਬਣਤਰ proc * caller,
	ਅਚਲ endpoint_t from_proc, ਅਚਲ vir_ਬਾਈਟਾਂ from_addr,
	ਅਚਲ endpoint_t ਮੰਜ਼ਿਲ_proc, ਅਚਲ vir_ਬਾਈਟਾਂ ਮੰਜ਼ਿਲ_addr,
	size_t ਬਾਈਟਾਂ)
{
  ਬਣਤਰ vir_addr src, dst;

  src.ਵਿੱਥ = from_addr;
  dst.ਵਿੱਥ = ਮੰਜ਼ਿਲ_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = ਮੰਜ਼ਿਲ_proc;
  ਪ੍ਰਤਿਜ੍ਞਾ(src.proc_nr_e != NONE);
  ਪ੍ਰਤਿਜ੍ਞਾ(dst.proc_nr_e != NONE);

  ਵਾਪਸ virtual_copy_vmcheck(caller, &src, &dst, ਬਾਈਟਾਂ);
}

ਖਾਲੀ ਸਮ੍ਰਿਤੀ_init(ਖਾਲੀ)
{
	ਪ੍ਰਤਿਜ੍ਞਾ(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_ਸ਼ੁਰੂ++;
	freepdes[nfreepdes++] = kinfo.freepde_ਸ਼ੁਰੂ++;

	ਪ੍ਰਤਿਜ੍ਞਾ(kinfo.freepde_ਸ਼ੁਰੂ < ARM_VM_DIR_ENTRIES);
	ਪ੍ਰਤਿਜ੍ਞਾ(nfreepdes == 2);
	ਪ੍ਰਤਿਜ੍ਞਾ(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
ਖਾਲੀ arch_proc_init(ਬਣਤਰ proc *pr, ਅਚਲ u32_t ip, ਅਚਲ u32_t sp,
	ਅਚਲ u32_t ps_str, ਅੱਖਰ *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

ਸਥਿਰ ਪੂਰਨ usermapped_glo_ਸੂਚਕਾਂਕ = -1,
	usermapped_ਸੂਚਕਾਂਕ = -1, ਪਹਿਲਾ_um_idx = -1;


/* defined in kernel.lds */
ਬਾਹਰੀ ਅੱਖਰ usermapped_ਸ਼ੁਰੂ, usermapped_ਸਿਰਾ, usermapped_nonglo_ਸ਼ੁਰੂ;

ਪੂਰਨ arch_phys_map(ਅਚਲ ਪੂਰਨ ਸੂਚਕਾਂਕ,
			phys_ਬਾਈਟਾਂ *addr,
			phys_ਬਾਈਟਾਂ *len,
			ਪੂਰਨ *flags)
{
	ਸਥਿਰ ਪੂਰਨ ਪਹਿਲਾ = 1;
	kern_phys_map *phys_maps;

	ਪੂਰਨ freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_ਸ਼ੁਰੂ -
			(u32_t) &usermapped_ਸ਼ੁਰੂ;

	ਜੇ(ਪਹਿਲਾ) {
		memset(&minix_kerninfo, 0, ਆਕਾਰ(minix_kerninfo));
		ਜੇ(glo_len > 0) {
			usermapped_glo_ਸੂਚਕਾਂਕ = freeidx++;
		}

		usermapped_ਸੂਚਕਾਂਕ = freeidx++;
		ਪਹਿਲਾ_um_idx = usermapped_ਸੂਚਕਾਂਕ;
		ਜੇ(usermapped_glo_ਸੂਚਕਾਂਕ != -1)
			ਪਹਿਲਾ_um_idx = usermapped_glo_ਸੂਚਕਾਂਕ;
		ਪਹਿਲਾ = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_ਸਿਰ;
		ਜਦੋਂ(phys_maps != ਨਿਰਾ){
			phys_maps->ਸੂਚਕਾਂਕ = freeidx++;
			phys_maps = phys_maps->ਅਗਲਾ;
		}

	}

	ਜੇ(ਸੂਚਕਾਂਕ == usermapped_glo_ਸੂਚਕਾਂਕ) {
		*addr = vir2phys(&usermapped_ਸ਼ੁਰੂ);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ਵਾਪਸ OK;
	}
	ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ == usermapped_ਸੂਚਕਾਂਕ) {
		*addr = vir2phys(&usermapped_nonglo_ਸ਼ੁਰੂ);
		*len = (u32_t) &usermapped_ਸਿਰਾ -
			(u32_t) &usermapped_nonglo_ਸ਼ੁਰੂ;
		*flags = VMMF_USER;
		ਵਾਪਸ OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_ਸਿਰ;
	ਜਦੋਂ(phys_maps != ਨਿਰਾ){
		ਜੇ(phys_maps->ਸੂਚਕਾਂਕ == ਸੂਚਕਾਂਕ){
			*addr = phys_maps->addr;
			*len =  phys_maps->ਮਾਪ;
			*flags = phys_maps->vm_flags;
			ਵਾਪਸ OK;
		}
		phys_maps = phys_maps->ਅਗਲਾ;
	}

	ਵਾਪਸ EINVAL;
}

ਪੂਰਨ arch_phys_map_ਜਵਾਬ(ਅਚਲ ਪੂਰਨ ਸੂਚਕਾਂਕ, ਅਚਲ vir_ਬਾਈਟਾਂ addr)
{
	kern_phys_map *phys_maps;

	ਜੇ(ਸੂਚਕਾਂਕ == ਪਹਿਲਾ_um_idx) {
		u32_t usermapped_ਵਿੱਥ;
		ਪ੍ਰਤਿਜ੍ਞਾ(addr > (u32_t) &usermapped_ਸ਼ੁਰੂ);
		usermapped_ਵਿੱਥ = addr - (u32_t) &usermapped_ਸ਼ੁਰੂ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIXEDPTR(ptr) (ਖਾਲੀ *) ((u32_t)ptr + usermapped_ਵਿੱਥ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ਸੌਂਪੋ(kinfo);
		ਸੌਂਪੋ(machine);
		ਸੌਂਪੋ(kmessages);
		ਸੌਂਪੋ(loadinfo);
		ਸੌਂਪੋ(kuserinfo);
		ਸੌਂਪੋ(arm_frclock);
		ਸੌਂਪੋ(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_ਬਾਈਟਾਂ) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ਵਾਪਸ OK;
	}

	ਜੇ (ਸੂਚਕਾਂਕ == usermapped_ਸੂਚਕਾਂਕ) {
		ਵਾਪਸ OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_ਸਿਰ;
	ਜਦੋਂ(phys_maps != ਨਿਰਾ){
		ਜੇ(phys_maps->ਸੂਚਕਾਂਕ == ਸੂਚਕਾਂਕ){
			ਪ੍ਰਤਿਜ੍ਞਾ(phys_maps->cb != ਨਿਰਾ);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			ਵਾਪਸ OK;
		}
		phys_maps = phys_maps->ਅਗਲਾ;
	}

	ਵਾਪਸ EINVAL;
}

ਪੂਰਨ arch_enable_paging(ਬਣਤਰ proc * caller)
{
	kern_phys_map *phys_maps;
	ਪ੍ਰਤਿਜ੍ਞਾ(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_ਪਤਾ_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_ਸਿਰ;
	ਜਦੋਂ(phys_maps != ਨਿਰਾ){
		ਪ੍ਰਤਿਜ੍ਞਾ(phys_maps->cb != ਨਿਰਾ);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->ਅਗਲਾ;
	}

	ਵਾਪਸ OK;
}

ਖਾਲੀ release_ਪਤਾ_space(ਬਣਤਰ proc *pr)
{
	pr->p_seg.p_ttbr_v = ਨਿਰਾ;
	barrier();
}



/*
 * Request a physical mapping
 */
ਪੂਰਨ kern_req_phys_map( phys_ਬਾਈਟਾਂ ਅਧਾਰ_ਪਤਾ, vir_ਬਾਈਟਾਂ io_ਮਾਪ, 
		       ਪੂਰਨ vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_ਬਾਈਟਾਂ id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ_ਪਤਾ != 0);
	ਪ੍ਰਤਿਜ੍ਞਾ(io_ਮਾਪ % ARM_ਪੰਨਾ_ਮਾਪ == 0);
	ਪ੍ਰਤਿਜ੍ਞਾ(cb != ਨਿਰਾ);

	priv->addr  = ਅਧਾਰ_ਪਤਾ;
	priv->ਮਾਪ  = io_ਮਾਪ;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->ਸੂਚਕਾਂਕ = -1;
	priv->ਅਗਲਾ = ਨਿਰਾ;
	

	ਜੇ (kern_phys_map_ਸਿਰ == ਨਿਰਾ){
		/* keep a list of items this is the first one */
		kern_phys_map_ਸਿਰ = priv;
		kern_phys_map_ਸਿਰ->ਅਗਲਾ = ਨਿਰਾ;
	} ਨਹੀਂਤਾਂ {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->ਅਗਲਾ = kern_phys_map_ਸਿਰ;
		/* replace the head */
		kern_phys_map_ਸਿਰ = priv;
	}
	ਵਾਪਸ 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
ਪੂਰਨ kern_phys_map_mapped_ptr(vir_ਬਾਈਟਾਂ id, phys_ਬਾਈਟਾਂ ਪਤਾ){
	*((vir_ਬਾਈਟਾਂ*)id) = ਪਤਾ;
	ਵਾਪਸ 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
ਪੂਰਨ kern_phys_map_ptr(
	phys_ਬਾਈਟਾਂ ਅਧਾਰ_ਪਤਾ, 
	vir_ਬਾਈਟਾਂ io_ਮਾਪ, 
	ਪੂਰਨ vm_flags, 
	kern_phys_map * priv,
	vir_ਬਾਈਟਾਂ ptr)
{
	ਵਾਪਸ kern_req_phys_map(ਅਧਾਰ_ਪਤਾ,io_ਮਾਪ,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

