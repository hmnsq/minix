
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


#ವ್ಯಾಖ್ಯಾನಿಸು HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nfreepdes = 0;
#ವ್ಯಾಖ್ಯಾನಿಸು MAXFREEPDES	2
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ freepdes[MAXFREEPDES];

ಸ್ಥಿರ u32_t phys_get32(phys_ಬೈಟುಗಳು v);

/* list of requested physical mapping */
ಸ್ಥಿರ kern_phys_map *kern_phys_map_ತಲೆ;

ಶೂನ್ಯ mem_clear_mapcache(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ(i = 0; i < nfreepdes; i++) {
		ರಚನೆ proc *ptproc = get_cpulocal_var(ptproc);
		ಪೂರ್ಣಾಂಕ pde = freepdes[i];
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
ಸ್ಥಿರ phys_ಬೈಟುಗಳು createpde(
	ಸ್ಥಿರಾಂಕ ರಚನೆ proc *pr,	/* Requested process, NULL for physical. */
	ಸ್ಥಿರಾಂಕ phys_ಬೈಟುಗಳು linaddr,/* Address after segment translation. */
	phys_ಬೈಟುಗಳು *ಬೈಟುಗಳು,	/* Size of chunk, function may truncate it. */
	ಪೂರ್ಣಾಂಕ free_pde_idx,	/* index of the free slot to use */
	ಪೂರ್ಣಾಂಕ *ಬದಲಾಯಿತು		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_ಬೈಟುಗಳು ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ pde;

	ಪ್ರತಿಜ್ಞಾ(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	ಪ್ರತಿಜ್ಞಾ(pde >= 0 && pde < 4096);

	ಆದರೆ(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ಹಿಂತಿರುಗಿ linaddr;
	}

	ಆದರೆ(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		ಪ್ರತಿಜ್ಞಾ(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* Requested address is physical. Make up the PDE entry. */
		ಪ್ರತಿಜ್ಞಾ (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_ಕೊನೆ);

		/*ಸ್ಮರಣೆ*/
		pdeval = (linaddr & ARM_VM_SECTION_ಮುಖವಾಡ)
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
	ಆದರೆ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*ಬದಲಾಯಿತು = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	ಸ್ಥಾನಾಂತರ = linaddr & ARM_VM_ಸ್ಥಾನಾಂತರ_ಮುಖವಾಡ_1_MB; /* Offset in 1MB window. */
	*ಬೈಟುಗಳು = MIN(*ಬೈಟುಗಳು, ARM_SECTION_ಗಾತ್ರ - ಸ್ಥಾನಾಂತರ);

	/* Return the linear address of the start of the new mapping. */
	ಹಿಂತಿರುಗಿ ARM_SECTION_ಗಾತ್ರ*pde + ಸ್ಥಾನಾಂತರ;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_resumed_caller(ರಚನೆ proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	ಆದರೆ (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		ಪ್ರತಿಜ್ಞಾ(caller->p_vmrequest.vmresult != VMSUSPEND);
		ಹಿಂತಿರುಗಿ caller->p_vmrequest.vmresult;
	}

	ಹಿಂತಿರುಗಿ OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lin_lin_copy(ರಚನೆ proc *srcproc, vir_ಬೈಟುಗಳು srclinaddr, 
	ರಚನೆ proc *dstproc, vir_ಬೈಟುಗಳು dstlinaddr, vir_ಬೈಟುಗಳು ಬೈಟುಗಳು)
{
	u32_t addr;
	proc_nr_t procslot;

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(proc_ptr));
	ಪ್ರತಿಜ್ಞಾ(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ಪ್ರತಿಜ್ಞಾ(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	ಆದರೆ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_ಸ್ಥಾನ_FREE));
	ಆದರೆ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ಆದರೆ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	ಆದರೆ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	ಆಗ(ಬೈಟುಗಳು > 0) {
		phys_ಬೈಟುಗಳು srcptr, dstptr;
		vir_ಬೈಟುಗಳು chunk = ಬೈಟುಗಳು;
		ಪೂರ್ಣಾಂಕ ಬದಲಾಯಿತು = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
		ಚಿಹ್ನೆರಹಿತ cpu = cpuid;

		ಆದರೆ (srcproc && GET_ಬಿಟ್(srcproc->p_stale_tlb, cpu)) {
			ಬದಲಾಯಿತು = 1;
			UNSET_ಬಿಟ್(srcproc->p_stale_tlb, cpu);
		}
		ಆದರೆ (dstproc && GET_ಬಿಟ್(dstproc->p_stale_tlb, cpu)) {
			ಬದಲಾಯಿತು = 1;
			UNSET_ಬಿಟ್(dstproc->p_stale_tlb, cpu);
		}
#ಆದರೆ_ಅಂತ್ಯ

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &ಬದಲಾಯಿತು);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &ಬದಲಾಯಿತು);
		ಆದರೆ(ಬದಲಾಯಿತು)
			reload_ttbr0();

		/* Check for overflow. */
		ಆದರೆ (srcptr + chunk < srcptr) ಹಿಂತಿರುಗಿ EFAULT_SRC;
		ಆದರೆ (dstptr + chunk < dstptr) ಹಿಂತಿರುಗಿ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ಆದರೆ(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_ಬೈಟುಗಳು src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			ಆದರೆ(addr >= src_aligned && addr < (srcptr + chunk)) {
				ಹಿಂತಿರುಗಿ EFAULT_SRC;
			}
			ಆದರೆ(addr >= dst_aligned && addr < (dstptr + chunk)) {
				ಹಿಂತಿರುಗಿ EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ಹಿಂತಿರುಗಿ EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		ಬೈಟುಗಳು -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	ಆದರೆ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_ಸ್ಥಾನ_FREE));
	ಆದರೆ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ u32_t phys_get32(phys_ಬೈಟುಗಳು addr)
{
	u32_t v;
	ಪೂರ್ಣಾಂಕ r;

	ಆದರೆ((r=lin_lin_copy(ಖಾಲಿ, addr, 
		proc_addr(SYSTEM), (phys_ಬೈಟುಗಳು) &v, ಗಾತ್ರ(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ಹಿಂತಿರುಗಿ v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_ಬೈಟುಗಳು umap_virtual(
  ನೋಂದಣಿ ರಚನೆ proc *rp,		/* pointer to proc table entry for process */
  ಪೂರ್ಣಾಂಕ seg,				/* T, D, or S segment */
  vir_ಬೈಟುಗಳು vir_addr,			/* virtual address in bytes within the seg */
  vir_ಬೈಟುಗಳು ಬೈಟುಗಳು			/* # of bytes to be copied */
)
{
	phys_ಬೈಟುಗಳು phys = 0;

	ಆದರೆ(vm_lookup(rp, vir_addr, &phys, ಖಾಲಿ) != OK) {
		ಬರೆ("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	ಆದರೆ(phys == 0) {
		ಬರೆ("SYSTEM:umap_virtual: lookup failed\n");
		ಹಿಂತಿರುಗಿ 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	ಆದರೆ(ಬೈಟುಗಳು > 0 && vm_lookup_range(rp, vir_addr, ಖಾಲಿ, ಬೈಟುಗಳು) != ಬೈಟುಗಳು) {
		ಬರೆ("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, ಬೈಟುಗಳು, vir_addr, vir_addr);
		ಹಿಂತಿರುಗಿ 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	ಪ್ರತಿಜ್ಞಾ(phys);

	ಹಿಂತಿರುಗಿ phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ vm_lookup(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು ವಾಸ್ತವಿಕ,
 phys_ಬೈಟುಗಳು *physical, u32_t *ptent)
{
	u32_t *ಬೇರು, *pt;
	ಪೂರ್ಣಾಂಕ pde, pte;
	u32_t pde_v, pte_v;

	ಪ್ರತಿಜ್ಞಾ(proc);
	ಪ್ರತಿಜ್ಞಾ(physical);
	ಪ್ರತಿಜ್ಞಾ(!isemptyp(proc));
	ಪ್ರತಿಜ್ಞಾ(HASPT(proc));

	/* Retrieve page directory entry. */
	ಬೇರು = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_ಮುಖವಾಡ);
	ಪ್ರತಿಜ್ಞಾ(!((u32_t) ಬೇರು % ARM_PAGEDIR_ಗಾತ್ರ));
	pde = ARM_VM_PDE(ವಾಸ್ತವಿಕ);
	ಪ್ರತಿಜ್ಞಾ(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ಬೇರು + pde));

	ಆದರೆ(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		ಹಿಂತಿರುಗಿ EFAULT;
	}

	ಆದರೆ(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_ಮುಖವಾಡ;
		ಆದರೆ(ptent) *ptent = pde_v;
		*physical += ವಾಸ್ತವಿಕ & ARM_VM_ಸ್ಥಾನಾಂತರ_ಮುಖವಾಡ_1_MB;
	} ಇಲ್ಲದಿದ್ದರೆ  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_ಮುಖವಾಡ);
		ಪ್ರತಿಜ್ಞಾ(!((u32_t) pt % ARM_PAGETABLE_ಗಾತ್ರ));
		pte = ARM_VM_PTE(ವಾಸ್ತವಿಕ);
		ಪ್ರತಿಜ್ಞಾ(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ಆದರೆ(!(pte_v & ARM_VM_PTE_PRESENT)) {
			ಹಿಂತಿರುಗಿ EFAULT;
		}

		ಆದರೆ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_ಮುಖವಾಡ;
		*physical += ವಾಸ್ತವಿಕ % ARM_ಪುಟ_ಗಾತ್ರ;
	}

	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *proc, vir_ಬೈಟುಗಳು vir_addr,
	phys_ಬೈಟುಗಳು *phys_addr, size_t ಬೈಟುಗಳು)
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
	phys_ಬೈಟುಗಳು phys, ಮುಂದಿನ_phys;
	size_t len;

	ಪ್ರತಿಜ್ಞಾ(proc);
	ಪ್ರತಿಜ್ಞಾ(ಬೈಟುಗಳು > 0);
	ಪ್ರತಿಜ್ಞಾ(HASPT(proc));

	/* Look up the first page. */
	ಆದರೆ (vm_lookup(proc, vir_addr, &phys, ಖಾಲಿ) != OK)
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (phys_addr != ಖಾಲಿ)
		*phys_addr = phys;

	len = ARM_ಪುಟ_ಗಾತ್ರ - (vir_addr % ARM_ಪುಟ_ಗಾತ್ರ);
	vir_addr += len;
	ಮುಂದಿನ_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ಆಗ (len < ಬೈಟುಗಳು) {
		ಆದರೆ (vm_lookup(proc, vir_addr, &phys, ಖಾಲಿ) != OK)
			ನಿಲ್ಲಿಸು;

		ಆದರೆ (ಮುಂದಿನ_phys != phys)
			ನಿಲ್ಲಿಸು;

		len += ARM_ಪುಟ_ಗಾತ್ರ;
		vir_addr += ARM_ಪುಟ_ಗಾತ್ರ;
		ಮುಂದಿನ_phys += ARM_ಪುಟ_ಗಾತ್ರ;
	}

	/* We might now have overshot the requested length somewhat. */
	ಹಿಂತಿರುಗಿ MIN(ಬೈಟುಗಳು, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ vm_check_range(ರಚನೆ proc *caller, ರಚನೆ proc *ಗುರಿ,
	vir_ಬೈಟುಗಳು vir_addr, size_t ಬೈಟುಗಳು, ಪೂರ್ಣಾಂಕ writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	ಪೂರ್ಣಾಂಕ r;

	ಆದರೆ ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ಹಿಂತಿರುಗಿ r;

	vm_suspend(caller, ಗುರಿ, vir_addr, ಬೈಟುಗಳು, VMSTYPE_KERNELCALL,
		writeflag);

	ಹಿಂತಿರುಗಿ VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ vm_memset(ರಚನೆ proc* caller, endpoint_t who, phys_ಬೈಟುಗಳು ph, ಪೂರ್ಣಾಂಕ c,
	phys_ಬೈಟುಗಳು ಎಣಿಕೆ_2)
{
	u32_t ನಮೂನೆ;
	ರಚನೆ proc *whoptr = ಖಾಲಿ;
	phys_ಬೈಟುಗಳು cur_ph = ph;
	phys_ಬೈಟುಗಳು ಎಡ = ಎಣಿಕೆ_2;
	phys_ಬೈಟುಗಳು ptr, chunk, pfa = 0;
	ಪೂರ್ಣಾಂಕ new_ttbr, r = OK;

	ಆದರೆ ((r = check_resumed_caller(caller)) != OK)
		ಹಿಂತಿರುಗಿ r;

	/* NONE for physical, otherwise virtual */
	ಆದರೆ (who != NONE && !(whoptr = endpoint_lookup(who)))
		ಹಿಂತಿರುಗಿ ESRCH;

	c &= 0xFF;
	ನಮೂನೆ = c | (c << 8) | (c << 16) | (c << 24);

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ಪ್ರತಿಜ್ಞಾ(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	ಆಗ (ಎಡ > 0) {
		new_ttbr = 0;
		chunk = ಎಡ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		ಆದರೆ (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		ಆದರೆ ((pfa = phys_memset(ptr, ನಮೂನೆ, chunk))) {

			/* If a process pagefaults, VM may help out */
			ಆದರೆ (whoptr) {
				vm_suspend(caller, whoptr, ph, ಎಣಿಕೆ_2,
						   VMSTYPE_KERNELCALL, 1);
				ಪ್ರತಿಜ್ಞಾ(catch_pagefaults);
				catch_pagefaults = 0;
				ಹಿಂತಿರುಗಿ VMSUSPEND;
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

	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ virtual_copy_f(
  ರಚನೆ proc * caller,
  ರಚನೆ vir_addr *src_addr,		/* source virtual address */
  ರಚನೆ vir_addr *dst_addr,		/* destination virtual address */
  vir_ಬೈಟುಗಳು ಬೈಟುಗಳು,			/* # of bytes to copy  */
  ಪೂರ್ಣಾಂಕ vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ರಚನೆ vir_addr *vir_addr[2];	/* virtual source and destination address */
  ಪೂರ್ಣಾಂಕ i, r;
  ರಚನೆ proc *procs[2];

  ಪ್ರತಿಜ್ಞಾ((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  ಆದರೆ (ಬೈಟುಗಳು <= 0) ಹಿಂತಿರುಗಿ(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  ಗಾಗಿ (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	ಪೂರ್ಣಾಂಕ proc_nr;
	ರಚನೆ proc *p;

	ಆದರೆ(proc_e == NONE) {
		p = ಖಾಲಿ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ(!isokendpt(proc_e, &proc_nr)) {
			ಬರೆ("virtual_copy: no reasonable endpoint\n");
			ಹಿಂತಿರುಗಿ ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  ಆದರೆ ((r = check_resumed_caller(caller)) != OK)
	ಹಿಂತಿರುಗಿ r;

  ಆದರೆ((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->ಸ್ಥಾನಾಂತರ,
  	procs[_DST_], vir_addr[_DST_]->ಸ್ಥಾನಾಂತರ, ಬೈಟುಗಳು)) != OK) {
	ಪೂರ್ಣಾಂಕ writeflag;
  	ರಚನೆ proc *ಗುರಿ = ಖಾಲಿ;
  	phys_ಬೈಟುಗಳು lin;
  	ಆದರೆ(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	ಆದರೆ(!vmcheck || !caller) {
    		ಹಿಂತಿರುಗಿ r;
  	}

  	ಆದರೆ(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->ಸ್ಥಾನಾಂತರ;
  		ಗುರಿ = procs[_SRC_];
		writeflag = 0;
  	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->ಸ್ಥಾನಾಂತರ;
  		ಗುರಿ = procs[_DST_];
		writeflag = 1;
  	} ಇಲ್ಲದಿದ್ದರೆ {
  		panic("r strange: %d",  r);
  	}

	ಪ್ರತಿಜ್ಞಾ(caller);
	ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);

	vm_suspend(caller, ಗುರಿ, lin, ಬೈಟುಗಳು, VMSTYPE_KERNELCALL, writeflag);
	ಹಿಂತಿರುಗಿ VMSUSPEND;
  }

  ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_copy(ಸ್ಥಿರಾಂಕ endpoint_t from_proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು from_addr,
	ಸ್ಥಿರಾಂಕ endpoint_t ಗಮ್ಯ_proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು ಗಮ್ಯ_addr,
	size_t ಬೈಟುಗಳು)
{
  ರಚನೆ vir_addr src, dst;

  src.ಸ್ಥಾನಾಂತರ = from_addr;
  dst.ಸ್ಥಾನಾಂತರ = ಗಮ್ಯ_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = ಗಮ್ಯ_proc;
  ಪ್ರತಿಜ್ಞಾ(src.proc_nr_e != NONE);
  ಪ್ರತಿಜ್ಞಾ(dst.proc_nr_e != NONE);

  ಹಿಂತಿರುಗಿ virtual_copy(&src, &dst, ಬೈಟುಗಳು);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_copy_vmcheck(ರಚನೆ proc * caller,
	ಸ್ಥಿರಾಂಕ endpoint_t from_proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು from_addr,
	ಸ್ಥಿರಾಂಕ endpoint_t ಗಮ್ಯ_proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು ಗಮ್ಯ_addr,
	size_t ಬೈಟುಗಳು)
{
  ರಚನೆ vir_addr src, dst;

  src.ಸ್ಥಾನಾಂತರ = from_addr;
  dst.ಸ್ಥಾನಾಂತರ = ಗಮ್ಯ_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = ಗಮ್ಯ_proc;
  ಪ್ರತಿಜ್ಞಾ(src.proc_nr_e != NONE);
  ಪ್ರತಿಜ್ಞಾ(dst.proc_nr_e != NONE);

  ಹಿಂತಿರುಗಿ virtual_copy_vmcheck(caller, &src, &dst, ಬೈಟುಗಳು);
}

ಶೂನ್ಯ ಸ್ಮರಣೆ_init(ಶೂನ್ಯ)
{
	ಪ್ರತಿಜ್ಞಾ(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_ಆರಂಭ++;
	freepdes[nfreepdes++] = kinfo.freepde_ಆರಂಭ++;

	ಪ್ರತಿಜ್ಞಾ(kinfo.freepde_ಆರಂಭ < ARM_VM_DIR_ENTRIES);
	ಪ್ರತಿಜ್ಞಾ(nfreepdes == 2);
	ಪ್ರತಿಜ್ಞಾ(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
ಶೂನ್ಯ arch_proc_init(ರಚನೆ proc *pr, ಸ್ಥಿರಾಂಕ u32_t ip, ಸ್ಥಿರಾಂಕ u32_t sp,
	ಸ್ಥಿರಾಂಕ u32_t ps_str, ಅಕ್ಷರ *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ usermapped_glo_ಸೂಚ್ಯಂಕ = -1,
	usermapped_ಸೂಚ್ಯಂಕ = -1, ಮೊದಲ_um_idx = -1;


/* defined in kernel.lds */
ಬಾಹ್ಯ ಅಕ್ಷರ usermapped_ಆರಂಭ, usermapped_ಕೊನೆ, usermapped_nonglo_ಆರಂಭ;

ಪೂರ್ಣಾಂಕ arch_phys_map(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ,
			phys_ಬೈಟುಗಳು *addr,
			phys_ಬೈಟುಗಳು *len,
			ಪೂರ್ಣಾಂಕ *flags)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮೊದಲ = 1;
	kern_phys_map *phys_maps;

	ಪೂರ್ಣಾಂಕ freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_ಆರಂಭ -
			(u32_t) &usermapped_ಆರಂಭ;

	ಆದರೆ(ಮೊದಲ) {
		memset(&minix_kerninfo, 0, ಗಾತ್ರ(minix_kerninfo));
		ಆದರೆ(glo_len > 0) {
			usermapped_glo_ಸೂಚ್ಯಂಕ = freeidx++;
		}

		usermapped_ಸೂಚ್ಯಂಕ = freeidx++;
		ಮೊದಲ_um_idx = usermapped_ಸೂಚ್ಯಂಕ;
		ಆದರೆ(usermapped_glo_ಸೂಚ್ಯಂಕ != -1)
			ಮೊದಲ_um_idx = usermapped_glo_ಸೂಚ್ಯಂಕ;
		ಮೊದಲ = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_ತಲೆ;
		ಆಗ(phys_maps != ಖಾಲಿ){
			phys_maps->ಸೂಚ್ಯಂಕ = freeidx++;
			phys_maps = phys_maps->ಮುಂದಿನ;
		}

	}

	ಆದರೆ(ಸೂಚ್ಯಂಕ == usermapped_glo_ಸೂಚ್ಯಂಕ) {
		*addr = vir2phys(&usermapped_ಆರಂಭ);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ಹಿಂತಿರುಗಿ OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ == usermapped_ಸೂಚ್ಯಂಕ) {
		*addr = vir2phys(&usermapped_nonglo_ಆರಂಭ);
		*len = (u32_t) &usermapped_ಕೊನೆ -
			(u32_t) &usermapped_nonglo_ಆರಂಭ;
		*flags = VMMF_USER;
		ಹಿಂತಿರುಗಿ OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_ತಲೆ;
	ಆಗ(phys_maps != ಖಾಲಿ){
		ಆದರೆ(phys_maps->ಸೂಚ್ಯಂಕ == ಸೂಚ್ಯಂಕ){
			*addr = phys_maps->addr;
			*len =  phys_maps->ಗಾತ್ರ_2;
			*flags = phys_maps->vm_flags;
			ಹಿಂತಿರುಗಿ OK;
		}
		phys_maps = phys_maps->ಮುಂದಿನ;
	}

	ಹಿಂತಿರುಗಿ EINVAL;
}

ಪೂರ್ಣಾಂಕ arch_phys_map_ಉತ್ತರ(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು addr)
{
	kern_phys_map *phys_maps;

	ಆದರೆ(ಸೂಚ್ಯಂಕ == ಮೊದಲ_um_idx) {
		u32_t usermapped_ಸ್ಥಾನಾಂತರ;
		ಪ್ರತಿಜ್ಞಾ(addr > (u32_t) &usermapped_ಆರಂಭ);
		usermapped_ಸ್ಥಾನಾಂತರ = addr - (u32_t) &usermapped_ಆರಂಭ;
#ವ್ಯಾಖ್ಯಾನಿಸು FIXEDPTR(ptr) (ಶೂನ್ಯ *) ((u32_t)ptr + usermapped_ಸ್ಥಾನಾಂತರ)
#ವ್ಯಾಖ್ಯಾನಿಸು FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ನಿಯೋಜಿಸು(kinfo);
		ನಿಯೋಜಿಸು(machine);
		ನಿಯೋಜಿಸು(kmessages);
		ನಿಯೋಜಿಸು(loadinfo);
		ನಿಯೋಜಿಸು(kuserinfo);
		ನಿಯೋಜಿಸು(arm_frclock);
		ನಿಯೋಜಿಸು(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_ಬೈಟುಗಳು) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ಹಿಂತಿರುಗಿ OK;
	}

	ಆದರೆ (ಸೂಚ್ಯಂಕ == usermapped_ಸೂಚ್ಯಂಕ) {
		ಹಿಂತಿರುಗಿ OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_ತಲೆ;
	ಆಗ(phys_maps != ಖಾಲಿ){
		ಆದರೆ(phys_maps->ಸೂಚ್ಯಂಕ == ಸೂಚ್ಯಂಕ){
			ಪ್ರತಿಜ್ಞಾ(phys_maps->cb != ಖಾಲಿ);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			ಹಿಂತಿರುಗಿ OK;
		}
		phys_maps = phys_maps->ಮುಂದಿನ;
	}

	ಹಿಂತಿರುಗಿ EINVAL;
}

ಪೂರ್ಣಾಂಕ arch_enable_paging(ರಚನೆ proc * caller)
{
	kern_phys_map *phys_maps;
	ಪ್ರತಿಜ್ಞಾ(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_ವಿಳಾಸ_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_ತಲೆ;
	ಆಗ(phys_maps != ಖಾಲಿ){
		ಪ್ರತಿಜ್ಞಾ(phys_maps->cb != ಖಾಲಿ);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->ಮುಂದಿನ;
	}

	ಹಿಂತಿರುಗಿ OK;
}

ಶೂನ್ಯ release_ವಿಳಾಸ_space(ರಚನೆ proc *pr)
{
	pr->p_seg.p_ttbr_v = ಖಾಲಿ;
	barrier();
}



/*
 * Request a physical mapping
 */
ಪೂರ್ಣಾಂಕ kern_req_phys_map( phys_ಬೈಟುಗಳು ತಳ_ವಿಳಾಸ, vir_ಬೈಟುಗಳು io_ಗಾತ್ರ, 
		       ಪೂರ್ಣಾಂಕ vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_ಬೈಟುಗಳು id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	ಪ್ರತಿಜ್ಞಾ(ತಳ_ವಿಳಾಸ != 0);
	ಪ್ರತಿಜ್ಞಾ(io_ಗಾತ್ರ % ARM_ಪುಟ_ಗಾತ್ರ == 0);
	ಪ್ರತಿಜ್ಞಾ(cb != ಖಾಲಿ);

	priv->addr  = ತಳ_ವಿಳಾಸ;
	priv->ಗಾತ್ರ_2  = io_ಗಾತ್ರ;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->ಸೂಚ್ಯಂಕ = -1;
	priv->ಮುಂದಿನ = ಖಾಲಿ;
	

	ಆದರೆ (kern_phys_map_ತಲೆ == ಖಾಲಿ){
		/* keep a list of items this is the first one */
		kern_phys_map_ತಲೆ = priv;
		kern_phys_map_ತಲೆ->ಮುಂದಿನ = ಖಾಲಿ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->ಮುಂದಿನ = kern_phys_map_ತಲೆ;
		/* replace the head */
		kern_phys_map_ತಲೆ = priv;
	}
	ಹಿಂತಿರುಗಿ 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
ಪೂರ್ಣಾಂಕ kern_phys_map_mapped_ptr(vir_ಬೈಟುಗಳು id, phys_ಬೈಟುಗಳು ವಿಳಾಸ){
	*((vir_ಬೈಟುಗಳು*)id) = ವಿಳಾಸ;
	ಹಿಂತಿರುಗಿ 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
ಪೂರ್ಣಾಂಕ kern_phys_map_ptr(
	phys_ಬೈಟುಗಳು ತಳ_ವಿಳಾಸ, 
	vir_ಬೈಟುಗಳು io_ಗಾತ್ರ, 
	ಪೂರ್ಣಾಂಕ vm_flags, 
	kern_phys_map * priv,
	vir_ಬೈಟುಗಳು ptr)
{
	ಹಿಂತಿರುಗಿ kern_req_phys_map(ತಳ_ವಿಳಾಸ,io_ಗಾತ್ರ,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

