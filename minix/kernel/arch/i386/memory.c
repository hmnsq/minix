
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ಅನ್ತರ್ಭಾವ <minix/syslib.h>
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ಅನ್ತರ್ಭಾವ "oxpcie.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ifdef USE_APIC
#ಅನ್ತರ್ಭಾವ "apic.h"
#ifdef USE_WATCHDOG
#ಅನ್ತರ್ಭಾವ "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nfreepdes = 0;
#define MAXFREEPDES	2
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ freepdes[MAXFREEPDES];

ಸ್ಥಿರ u32_t phys_get32(phys_bytes v);

ಶೂನ್ಯ mem_clear_mapcache(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪ್ರತಿ(i = 0; i < nfreepdes; i++) {
		ಸಂರಚನಾ proc *ptproc = get_cpulocal_var(ptproc);
		ಪೂರ್ಣಾಙ್ಕ pde = freepdes[i];
		u32_t *ptv;
		ಪ್ರತಿಜ್ಞಾ(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		ಪ್ರತಿಜ್ಞಾ(ptv);
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
	ಪ್ರತಿಜ್ಞಾ(pde >= 0 && pde < 1024);

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
		ಪ್ರತಿಜ್ಞಾ(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} ಅನ್ಯಥಾ {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ಯದಿ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
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
	ಪ್ರತ್ಯಾಗಮ I386_BIG_PAGE_SIZE*pde + offset;
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
	ಪ್ರತಿಜ್ಞಾ(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ಪ್ರತಿಜ್ಞಾ(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	ಯದಿ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	ಯದಿ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
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

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		ಯದಿ(changed)
			reload_cr3(); 

		/* Check for overflow. */
		ಯದಿ (srcptr + chunk < srcptr) ಪ್ರತ್ಯಾಗಮ EFAULT_SRC;
		ಯದಿ (dstptr + chunk < dstptr) ಪ್ರತ್ಯಾಗಮ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ಯದಿ(addr) {
			/* If addr is nonzero, a page fault was caught. */

			ಯದಿ(addr >= srcptr && addr < (srcptr + chunk)) {
				ಪ್ರತ್ಯಾಗಮ EFAULT_SRC;
			}
			ಯದಿ(addr >= dstptr && addr < (dstptr + chunk)) {
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
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

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

#ಯದಿ 0
ಸ್ಥಿರ ವರ್ಣ *cr0_str(u32_t e)
{
	ಸ್ಥಿರ ವರ್ಣ str[80];
	strcpy(str, "");
#define FLAG(v) ಕುರು { ಯದಿ(e & (v)) { strcat(str, #v " "); e &= ~v; } } ಯಾವತ್(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	ಯದಿ(e) { strcat(str, " (++)"); }
	ಪ್ರತ್ಯಾಗಮ str;
}

ಸ್ಥಿರ ವರ್ಣ *cr4_str(u32_t e)
{
	ಸ್ಥಿರ ವರ್ಣ str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	ಯದಿ(e) { strcat(str, " (++)"); }
	ಪ್ರತ್ಯಾಗಮ str;
}
#endif

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
	ಬೇರು = (u32_t *) proc->p_seg.p_cr3;
	ಪ್ರತಿಜ್ಞಾ(!((u32_t) ಬೇರು % I386_PAGE_SIZE));
	pde = I386_VM_PDE(ಆಭಾಸ);
	ಪ್ರತಿಜ್ಞಾ(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ಬೇರು + pde));

	ಯದಿ(!(pde_v & I386_VM_PRESENT)) {
		ಪ್ರತ್ಯಾಗಮ EFAULT;
	}

	/* We don't expect to ever see this. */
	ಯದಿ(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		ಯದಿ(ptent) *ptent = pde_v;
		*physical += ಆಭಾಸ & I386_VM_OFFSET_MASK_4MB;
	} ಅನ್ಯಥಾ {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		ಪ್ರತಿಜ್ಞಾ(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(ಆಭಾಸ);
		ಪ್ರತಿಜ್ಞಾ(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ಯದಿ(!(pte_v & I386_VM_PRESENT)) {
			ಪ್ರತ್ಯಾಗಮ EFAULT;
		}

		ಯದಿ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += ಆಭಾಸ % I386_PAGE_SIZE;
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

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ಯಾವತ್ (len < bytes) {
		ಯದಿ (vm_lookup(proc, vir_addr, &phys, ರಿಕ್ತ) != OK)
			ಭಙ್ಗ;

		ಯದಿ (next_phys != phys)
			ಭಙ್ಗ;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
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

#ಯದಿ 0
ಸ್ಥಿರ ವರ್ಣ *flagstr(u32_t e, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ dir)
{
	ಸ್ಥಿರ ವರ್ಣ str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	ಯದಿ(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	ಅನ್ಯಥಾ
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	ಪ್ರತ್ಯಾಗಮ str;
}

ಸ್ಥಿರ ಶೂನ್ಯ vm_pt_print(u32_t *pagetable, ಧ್ರುವ u32_t v)
{
	ಪೂರ್ಣಾಙ್ಕ pte;
	ಪೂರ್ಣಾಙ್ಕ col = 0;

	ಪ್ರತಿಜ್ಞಾ(!((u32_t) pagetable % I386_PAGE_SIZE));

	ಪ್ರತಿ(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		ಯದಿ(!(pte_v & I386_VM_PRESENT))
			ಅನುವರ್ತ;
		pfa = I386_VM_PFA(pte_v);
		printf("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		ಯದಿ(col == 3) { printf("\n"); col = 0; }
	}
	ಯದಿ(col > 0) printf("\n");

	ಪ್ರತ್ಯಾಗಮ;
}

ಸ್ಥಿರ ಶೂನ್ಯ vm_print(u32_t *ಬೇರು)
{
	ಪೂರ್ಣಾಙ್ಕ pde;

	ಪ್ರತಿಜ್ಞಾ(!((u32_t) ಬೇರು % I386_PAGE_SIZE));

	printf("page table 0x%lx:\n", ಬೇರು);

	ಪ್ರತಿ(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (ಬೇರು + pde));
		ಯದಿ(!(pde_v & I386_VM_PRESENT))
			ಅನುವರ್ತ;
		ಯದಿ(pde_v & I386_VM_BIGPAGE) {
			printf("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} ಅನ್ಯಥಾ {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			printf("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			printf("\n");
		}
	}


	ಪ್ರತ್ಯಾಗಮ;
}
#endif

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
	ಪೂರ್ಣಾಙ್ಕ new_cr3, r = OK;

	ಯದಿ ((r = check_resumed_caller(caller)) != OK)
		ಪ್ರತ್ಯಾಗಮ r;

	/* NONE for physical, otherwise virtual */
	ಯದಿ (who != NONE && !(whoptr = endpoint_lookup(who)))
		ಪ್ರತ್ಯಾಗಮ ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ಪ್ರತಿಜ್ಞಾ(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	ಯಾವತ್ (ಎಡ > 0) {
		new_cr3 = 0;
		chunk = ಎಡ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		ಯದಿ (new_cr3)
			reload_cr3();

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

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ಪ್ರತಿಜ್ಞಾ(catch_pagefaults);
	catch_pagefaults = 0;

	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ virtual_copy_f(
  ಸಂರಚನಾ proc * caller,
  ಸಂರಚನಾ vir_addr *src_addr,	/* source virtual address */
  ಸಂರಚನಾ vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  ಪೂರ್ಣಾಙ್ಕ vmcheck			/* if nonzero, can return VMSUSPEND */
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

	ಪ್ರತಿಜ್ಞಾ(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
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
	strlcpy(pr->p_name, name, ಪರಿಮಾಣ(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

ಬಾಹ್ಯ ವರ್ಣ *video_mem;

ಬಾಹ್ಯ ವರ್ಣ usermapped_start, usermapped_end, usermapped_nonglo_start;

ಪೂರ್ಣಾಙ್ಕ arch_phys_map(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ index,
			phys_bytes *addr,
			phys_bytes *len,
			ಪೂರ್ಣಾಙ್ಕ *flags)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ first = 1;
	ಪೂರ್ಣಾಙ್ಕ freeidx = 0;
	ಸ್ಥಿರ ವರ್ಣ *ser_var = ರಿಕ್ತ;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	ಯದಿ(first) {
		memset(&minix_kerninfo, 0, ಪರಿಮಾಣ(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		ಯದಿ(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		ಯದಿ(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		ಯದಿ(lapic_addr)
			lapic_mapping_index = freeidx++;
		ಯದಿ (ioapic_enabled) {
			ioapic_first_index = freeidx;
			ಪ್ರತಿಜ್ಞಾ(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		ಯದಿ((ser_var = env_get("oxpcie"))) {
			ಯದಿ(ser_var[0] != '0' || ser_var[1] != 'x') {
				printf("oxpcie address in hex please\n");
			} ಅನ್ಯಥಾ {
				printf("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
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
	ಅನ್ಯಥಾ ಯದಿ (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		ಪ್ರತ್ಯಾಗಮ OK;
	}
#ifdef USE_APIC
	ಅನ್ಯಥಾ ಯದಿ (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		ಯದಿ (!lapic_addr)
			ಪ್ರತ್ಯಾಗಮ EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ಪ್ರತ್ಯಾಗಮ OK;
	}
	ಅನ್ಯಥಾ ಯದಿ (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		ಪೂರ್ಣಾಙ್ಕ ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		ಪ್ರತಿಜ್ಞಾ(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		printf("ioapic map: addr 0x%lx\n", *addr);
		ಪ್ರತ್ಯಾಗಮ OK;
	}
#endif

#ಯದಿ CONFIG_OXPCIE
	ಯದಿ(index == oxpcie_mapping_index) {
		*addr = strtoul(ser_var+2, ರಿಕ್ತ, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ಪ್ರತ್ಯಾಗಮ OK;
	}
#endif

	ಪ್ರತ್ಯಾಗಮ EINVAL;
}

ಪೂರ್ಣಾಙ್ಕ arch_phys_map_reply(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ index, ಧ್ರುವ vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	ಯದಿ (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		ಪ್ರತ್ಯಾಗಮ OK;
	}
	ಅನ್ಯಥಾ ಯದಿ (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		ಪೂರ್ಣಾಙ್ಕ i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		ಪ್ರತ್ಯಾಗಮ OK;
	}
#endif

#ಯದಿ CONFIG_OXPCIE
	ಯದಿ (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned ವರ್ಣ *) addr);
		ಪ್ರತ್ಯಾಗಮ OK;
	}
#endif
	ಯದಿ(index == first_um_idx) {
		ಬಾಹ್ಯ ಸಂರಚನಾ minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		ಬಾಹ್ಯ u32_t usermapped_offset;
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
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		ಯದಿ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} ಅನ್ಯಥಾ  ಯದಿ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} ಅನ್ಯಥಾ	{
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
		ಯದಿ(env_get("libc_ipc")) {
			printf("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = ರಿಕ್ತ;
		} ಅನ್ಯಥಾ {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ಪ್ರತ್ಯಾಗಮ OK;
	}

	ಯದಿ(index == usermapped_index) ಪ್ರತ್ಯಾಗಮ OK;

	ಯದಿ (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		ಪ್ರತ್ಯಾಗಮ OK;
	}

	ಪ್ರತ್ಯಾಗಮ EINVAL;
}

ಪೂರ್ಣಾಙ್ಕ arch_enable_paging(ಸಂರಚನಾ proc * caller)
{
	ಪ್ರತಿಜ್ಞಾ(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (ವರ್ಣ *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	ಯದಿ (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	ಯದಿ (ioapic_enabled) {
		ಪೂರ್ಣಾಙ್ಕ i;

		ಪ್ರತಿ (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#ಯದಿ CONFIG_SMP
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
	ಯದಿ (watchdog_enabled)
		i386_watchdog_start();
#endif

	ಪ್ರತ್ಯಾಗಮ OK;
}

ಶೂನ್ಯ release_address_space(ಸಂರಚನಾ proc *pr)
{
	pr->p_seg.p_cr3_v = ರಿಕ್ತ;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
ಪೂರ್ಣಾಙ್ಕ platform_tbl_checksum_ok(ಶೂನ್ಯ *ptr, unsigned ಪೂರ್ಣಾಙ್ಕ length)
{
	u8_t total = 0;
	unsigned ಪೂರ್ಣಾಙ್ಕ i;
	ಪ್ರತಿ (i = 0; i < length; i++)
		total += ((unsigned ವರ್ಣ *)ptr)[i];
	ಪ್ರತ್ಯಾಗಮ !total;
}

ಪೂರ್ಣಾಙ್ಕ platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					ಶೂನ್ಯ * buff,
					unsigned size,
					phys_bytes * phys_addr,
					ಪೂರ್ಣಾಙ್ಕ ((* cmp_f)(ಶೂನ್ಯ *)))
{
	phys_bytes addr;

	ಪ್ರತಿ (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		ಯದಿ (cmp_f(buff)) {
			ಯದಿ (phys_addr)
				*phys_addr = addr;
			ಪ್ರತ್ಯಾಗಮ 1;
		}
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}
