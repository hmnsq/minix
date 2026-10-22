
#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "kernel/vm.h"

#ଅନ୍ତର୍ଭାଵ <machine/vm.h>

#ଅନ୍ତର୍ଭାଵ <minix/syslib.h>
#ଅନ୍ତର୍ଭାଵ <minix/cpufeature.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ <machine/vm.h>

#ଅନ୍ତର୍ଭାଵ "oxpcie.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

#ifdef USE_APIC
#ଅନ୍ତର୍ଭାଵ "apic.h"
#ifdef USE_WATCHDOG
#ଅନ୍ତର୍ଭାଵ "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfreepdes = 0;
#define MAXFREEPDES	2
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା freepdes[MAXFREEPDES];

ସ୍ଥିର u32_t phys_get32(phys_bytes v);

ଶୂନ୍ୟ mem_clear_mapcache(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପାଇଁ(i = 0; i < nfreepdes; i++) {
		ସଂରଚନା proc *ptproc = get_cpulocal_var(ptproc);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pde = freepdes[i];
		u32_t *ptv;
		ପ୍ରତିଜ୍ଞା(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		ପ୍ରତିଜ୍ଞା(ptv);
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
	ପ୍ରତିଜ୍ଞା(pde >= 0 && pde < 1024);

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
		ପ୍ରତିଜ୍ଞା(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} ଅନ୍ୟଥା {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ଯଦି(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
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
	ଫେରନ୍ତୁ I386_BIG_PAGE_SIZE*pde + offset;
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
	ପ୍ରତିଜ୍ଞା(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ପ୍ରତିଜ୍ଞା(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	ଯଦି(srcproc) ପ୍ରତିଜ୍ଞା(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	ଯଦି(dstproc) ପ୍ରତିଜ୍ଞା(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ପ୍ରତିଜ୍ଞା(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
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

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		ଯଦି(changed)
			reload_cr3(); 

		/* Check for overflow. */
		ଯଦି (srcptr + chunk < srcptr) ଫେରନ୍ତୁ EFAULT_SRC;
		ଯଦି (dstptr + chunk < dstptr) ଫେରନ୍ତୁ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ଯଦି(addr) {
			/* If addr is nonzero, a page fault was caught. */

			ଯଦି(addr >= srcptr && addr < (srcptr + chunk)) {
				ଫେରନ୍ତୁ EFAULT_SRC;
			}
			ଯଦି(addr >= dstptr && addr < (dstptr + chunk)) {
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
	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

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

#ଯଦି 0
ସ୍ଥିର ଅକ୍ଷର *cr0_str(u32_t e)
{
	ସ୍ଥିର ଅକ୍ଷର str[80];
	strcpy(str, "");
#define FLAG(v) କର { ଯଦି(e & (v)) { strcat(str, #v " "); e &= ~v; } } ଯେତେବେଳେ(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	ଯଦି(e) { strcat(str, " (++)"); }
	ଫେରନ୍ତୁ str;
}

ସ୍ଥିର ଅକ୍ଷର *cr4_str(u32_t e)
{
	ସ୍ଥିର ଅକ୍ଷର str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	ଯଦି(e) { strcat(str, " (++)"); }
	ଫେରନ୍ତୁ str;
}
#endif

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
	ମୂଳ = (u32_t *) proc->p_seg.p_cr3;
	ପ୍ରତିଜ୍ଞା(!((u32_t) ମୂଳ % I386_PAGE_SIZE));
	pde = I386_VM_PDE(ଆଭାସୀ);
	ପ୍ରତିଜ୍ଞା(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ମୂଳ + pde));

	ଯଦି(!(pde_v & I386_VM_PRESENT)) {
		ଫେରନ୍ତୁ EFAULT;
	}

	/* We don't expect to ever see this. */
	ଯଦି(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		ଯଦି(ptent) *ptent = pde_v;
		*physical += ଆଭାସୀ & I386_VM_OFFSET_MASK_4MB;
	} ଅନ୍ୟଥା {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		ପ୍ରତିଜ୍ଞା(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(ଆଭାସୀ);
		ପ୍ରତିଜ୍ଞା(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ଯଦି(!(pte_v & I386_VM_PRESENT)) {
			ଫେରନ୍ତୁ EFAULT;
		}

		ଯଦି(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += ଆଭାସୀ % I386_PAGE_SIZE;
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

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ଯେତେବେଳେ (len < bytes) {
		ଯଦି (vm_lookup(proc, vir_addr, &phys, ଖାଲି) != OK)
			ଭାଙ୍ଗ;

		ଯଦି (next_phys != phys)
			ଭାଙ୍ଗ;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
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

#ଯଦି 0
ସ୍ଥିର ଅକ୍ଷର *flagstr(u32_t e, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir)
{
	ସ୍ଥିର ଅକ୍ଷର str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	ଯଦି(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	ଅନ୍ୟଥା
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	ଫେରନ୍ତୁ str;
}

ସ୍ଥିର ଶୂନ୍ୟ vm_pt_print(u32_t *pagetable, ଅଚଳ u32_t v)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pte;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 0;

	ପ୍ରତିଜ୍ଞା(!((u32_t) pagetable % I386_PAGE_SIZE));

	ପାଇଁ(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		ଯଦି(!(pte_v & I386_VM_PRESENT))
			ଜାରି;
		pfa = I386_VM_PFA(pte_v);
		printf("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		ଯଦି(col == 3) { printf("\n"); col = 0; }
	}
	ଯଦି(col > 0) printf("\n");

	ଫେରନ୍ତୁ;
}

ସ୍ଥିର ଶୂନ୍ୟ vm_print(u32_t *ମୂଳ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pde;

	ପ୍ରତିଜ୍ଞା(!((u32_t) ମୂଳ % I386_PAGE_SIZE));

	printf("page table 0x%lx:\n", ମୂଳ);

	ପାଇଁ(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (ମୂଳ + pde));
		ଯଦି(!(pde_v & I386_VM_PRESENT))
			ଜାରି;
		ଯଦି(pde_v & I386_VM_BIGPAGE) {
			printf("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} ଅନ୍ୟଥା {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			printf("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			printf("\n");
		}
	}


	ଫେରନ୍ତୁ;
}
#endif

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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_cr3, r = OK;

	ଯଦି ((r = check_resumed_caller(caller)) != OK)
		ଫେରନ୍ତୁ r;

	/* NONE for physical, otherwise virtual */
	ଯଦି (who != NONE && !(whoptr = endpoint_lookup(who)))
		ଫେରନ୍ତୁ ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ପ୍ରତିଜ୍ଞା(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	ଯେତେବେଳେ (ବାମ > 0) {
		new_cr3 = 0;
		chunk = ବାମ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		ଯଦି (new_cr3)
			reload_cr3();

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

	ପ୍ରତିଜ୍ଞା(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ପ୍ରତିଜ୍ଞା(catch_pagefaults);
	catch_pagefaults = 0;

	ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା virtual_copy_f(
  ସଂରଚନା proc * caller,
  ସଂରଚନା vir_addr *src_addr,	/* source virtual address */
  ସଂରଚନା vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା vmcheck			/* if nonzero, can return VMSUSPEND */
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

	ପ୍ରତିଜ୍ଞା(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
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
	strlcpy(pr->p_name, name, ଆକାର(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

ବାହ୍ୟ ଅକ୍ଷର *video_mem;

ବାହ୍ୟ ଅକ୍ଷର usermapped_start, usermapped_end, usermapped_nonglo_start;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_phys_map(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
			phys_bytes *addr,
			phys_bytes *len,
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flags)
{
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା first = 1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା freeidx = 0;
	ସ୍ଥିର ଅକ୍ଷର *ser_var = ଖାଲି;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	ଯଦି(first) {
		memset(&minix_kerninfo, 0, ଆକାର(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		ଯଦି(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		ଯଦି(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		ଯଦି(lapic_addr)
			lapic_mapping_index = freeidx++;
		ଯଦି (ioapic_enabled) {
			ioapic_first_index = freeidx;
			ପ୍ରତିଜ୍ଞା(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		ଯଦି((ser_var = env_get("oxpcie"))) {
			ଯଦି(ser_var[0] != '0' || ser_var[1] != 'x') {
				printf("oxpcie address in hex please\n");
			} ଅନ୍ୟଥା {
				printf("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
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
	ଅନ୍ୟଥା ଯଦି (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		ଫେରନ୍ତୁ OK;
	}
#ifdef USE_APIC
	ଅନ୍ୟଥା ଯଦି (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		ଯଦି (!lapic_addr)
			ଫେରନ୍ତୁ EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ଫେରନ୍ତୁ OK;
	}
	ଅନ୍ୟଥା ଯଦି (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		ପ୍ରତିଜ୍ଞା(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		printf("ioapic map: addr 0x%lx\n", *addr);
		ଫେରନ୍ତୁ OK;
	}
#endif

#ଯଦି CONFIG_OXPCIE
	ଯଦି(index == oxpcie_mapping_index) {
		*addr = strtoul(ser_var+2, ଖାଲି, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ଫେରନ୍ତୁ OK;
	}
#endif

	ଫେରନ୍ତୁ EINVAL;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_phys_map_reply(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, ଅଚଳ vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	ଯଦି (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		ଫେରନ୍ତୁ OK;
	}
	ଅନ୍ୟଥା ଯଦି (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		ଫେରନ୍ତୁ OK;
	}
#endif

#ଯଦି CONFIG_OXPCIE
	ଯଦି (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned ଅକ୍ଷର *) addr);
		ଫେରନ୍ତୁ OK;
	}
#endif
	ଯଦି(index == first_um_idx) {
		ବାହ୍ୟ ସଂରଚନା minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		ବାହ୍ୟ u32_t usermapped_offset;
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
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		ଯଦି(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} ଅନ୍ୟଥା  ଯଦି(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} ଅନ୍ୟଥା	{
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
		ଯଦି(env_get("libc_ipc")) {
			printf("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = ଖାଲି;
		} ଅନ୍ୟଥା {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ଫେରନ୍ତୁ OK;
	}

	ଯଦି(index == usermapped_index) ଫେରନ୍ତୁ OK;

	ଯଦି (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		ଫେରନ୍ତୁ OK;
	}

	ଫେରନ୍ତୁ EINVAL;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_enable_paging(ସଂରଚନା proc * caller)
{
	ପ୍ରତିଜ୍ଞା(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (ଅକ୍ଷର *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	ଯଦି (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	ଯଦି (ioapic_enabled) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		ପାଇଁ (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#ଯଦି CONFIG_SMP
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
	ଯଦି (watchdog_enabled)
		i386_watchdog_start();
#endif

	ଫେରନ୍ତୁ OK;
}

ଶୂନ୍ୟ release_address_space(ସଂରଚନା proc *pr)
{
	pr->p_seg.p_cr3_v = ଖାଲି;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା platform_tbl_checksum_ok(ଶୂନ୍ୟ *ptr, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
	u8_t total = 0;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପାଇଁ (i = 0; i < length; i++)
		total += ((unsigned ଅକ୍ଷର *)ptr)[i];
	ଫେରନ୍ତୁ !total;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					ଶୂନ୍ୟ * buff,
					unsigned size,
					phys_bytes * phys_addr,
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା ((* cmp_f)(ଶୂନ୍ୟ *)))
{
	phys_bytes addr;

	ପାଇଁ (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		ଯଦି (cmp_f(buff)) {
			ଯଦି (phys_addr)
				*phys_addr = addr;
			ଫେରନ୍ତୁ 1;
		}
	}
	ଫେରନ୍ତୁ 0;
}
