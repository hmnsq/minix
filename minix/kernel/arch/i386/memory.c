
#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "kernel/vm.h"

#அந்தர்பாவ <machine/vm.h>

#அந்தர்பாவ <minix/syslib.h>
#அந்தர்பாவ <minix/cpufeature.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ <machine/vm.h>

#அந்தர்பாவ "oxpcie.h"
#அந்தர்பாவ "arch_proto.h"

#ifdef USE_APIC
#அந்தர்பாவ "apic.h"
#ifdef USE_WATCHDOG
#அந்தர்பாவ "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
நிலையான முழு nfreepdes = 0;
#define MAXFREEPDES	2
நிலையான முழு freepdes[MAXFREEPDES];

நிலையான u32_t phys_get32(phys_bytes v);

வெறுமை mem_clear_mapcache(வெறுமை)
{
	முழு i;
	க்கு(i = 0; i < nfreepdes; i++) {
		கட்டமைப்பு proc *ptproc = get_cpulocal_var(ptproc);
		முழு pde = freepdes[i];
		u32_t *ptv;
		ப்ரதிஜ்ஞா(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		ப்ரதிஜ்ஞா(ptv);
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
நிலையான phys_bytes createpde(
	மாறிலி கட்டமைப்பு proc *pr,	/* Requested process, NULL for physical. */
	மாறிலி phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	முழு free_pde_idx,	/* index of the free slot to use */
	முழு *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	முழு pde;

	ப்ரதிஜ்ஞா(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	ப்ரதிஜ்ஞா(pde >= 0 && pde < 1024);

	எனில்(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		திரும்பு linaddr;
	}

	எனில்(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		ப்ரதிஜ்ஞா(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} இல்லையெனில் {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	எனில்(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
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
	திரும்பு I386_BIG_PAGE_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
நிலையான முழு check_resumed_caller(கட்டமைப்பு proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	எனில் (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		ப்ரதிஜ்ஞா(caller->p_vmrequest.vmresult != VMSUSPEND);
		திரும்பு caller->p_vmrequest.vmresult;
	}

	திரும்பு OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
நிலையான முழு lin_lin_copy(கட்டமைப்பு proc *srcproc, vir_bytes srclinaddr, 
	கட்டமைப்பு proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc));
	ப்ரதிஜ்ஞா(get_cpulocal_var(proc_ptr));
	ப்ரதிஜ்ஞா(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ப்ரதிஜ்ஞா(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	எனில்(srcproc) ப்ரதிஜ்ஞா(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	எனில்(dstproc) ப்ரதிஜ்ஞா(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ப்ரதிஜ்ஞா(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	எனில்(srcproc) ப்ரதிஜ்ஞா(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	எனில்(dstproc) ப்ரதிஜ்ஞா(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	போது(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		முழு changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		எனில் (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		எனில் (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		எனில்(changed)
			reload_cr3(); 

		/* Check for overflow. */
		எனில் (srcptr + chunk < srcptr) திரும்பு EFAULT_SRC;
		எனில் (dstptr + chunk < dstptr) திரும்பு EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		எனில்(addr) {
			/* If addr is nonzero, a page fault was caught. */

			எனில்(addr >= srcptr && addr < (srcptr + chunk)) {
				திரும்பு EFAULT_SRC;
			}
			எனில்(addr >= dstptr && addr < (dstptr + chunk)) {
				திரும்பு EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			திரும்பு EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	எனில்(srcproc) ப்ரதிஜ்ஞா(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	எனில்(dstproc) ப்ரதிஜ்ஞா(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ப்ரதிஜ்ஞா(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	திரும்பு OK;
}


நிலையான u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	முழு r;

	எனில்((r=lin_lin_copy(வெற்று, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, அளவு(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	திரும்பு v;
}

#எனில் 0
நிலையான எழுத்து *cr0_str(u32_t e)
{
	நிலையான எழுத்து str[80];
	strcpy(str, "");
#define FLAG(v) செய் { எனில்(e & (v)) { strcat(str, #v " "); e &= ~v; } } போது(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	எனில்(e) { strcat(str, " (++)"); }
	திரும்பு str;
}

நிலையான எழுத்து *cr4_str(u32_t e)
{
	நிலையான எழுத்து str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	எனில்(e) { strcat(str, " (++)"); }
	திரும்பு str;
}
#endif

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  பதிவு கட்டமைப்பு proc *rp,		/* pointer to proc table entry for process */
  முழு seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	எனில்(vm_lookup(rp, vir_addr, &phys, வெற்று) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} இல்லையெனில் {
		எனில்(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	எனில்(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		திரும்பு 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	எனில்(bytes > 0 && vm_lookup_range(rp, vir_addr, வெற்று, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		திரும்பு 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	ப்ரதிஜ்ஞா(phys);

	திரும்பு phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
முழு vm_lookup(மாறிலி கட்டமைப்பு proc *proc, மாறிலி vir_bytes மெய்நிகர்,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *வேர், *pt;
	முழு pde, pte;
	u32_t pde_v, pte_v;

	ப்ரதிஜ்ஞா(proc);
	ப்ரதிஜ்ஞா(physical);
	ப்ரதிஜ்ஞா(!isemptyp(proc));
	ப்ரதிஜ்ஞா(HASPT(proc));

	/* Retrieve page directory entry. */
	வேர் = (u32_t *) proc->p_seg.p_cr3;
	ப்ரதிஜ்ஞா(!((u32_t) வேர் % I386_PAGE_SIZE));
	pde = I386_VM_PDE(மெய்நிகர்);
	ப்ரதிஜ்ஞா(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (வேர் + pde));

	எனில்(!(pde_v & I386_VM_PRESENT)) {
		திரும்பு EFAULT;
	}

	/* We don't expect to ever see this. */
	எனில்(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		எனில்(ptent) *ptent = pde_v;
		*physical += மெய்நிகர் & I386_VM_OFFSET_MASK_4MB;
	} இல்லையெனில் {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		ப்ரதிஜ்ஞா(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(மெய்நிகர்);
		ப்ரதிஜ்ஞா(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		எனில்(!(pte_v & I386_VM_PRESENT)) {
			திரும்பு EFAULT;
		}

		எனில்(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += மெய்நிகர் % I386_PAGE_SIZE;
	}

	திரும்பு OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(மாறிலி கட்டமைப்பு proc *proc, vir_bytes vir_addr,
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

	ப்ரதிஜ்ஞா(proc);
	ப்ரதிஜ்ஞா(bytes > 0);
	ப்ரதிஜ்ஞா(HASPT(proc));

	/* Look up the first page. */
	எனில் (vm_lookup(proc, vir_addr, &phys, வெற்று) != OK)
		திரும்பு 0;

	எனில் (phys_addr != வெற்று)
		*phys_addr = phys;

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	போது (len < bytes) {
		எனில் (vm_lookup(proc, vir_addr, &phys, வெற்று) != OK)
			நிறுத்து;

		எனில் (next_phys != phys)
			நிறுத்து;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	திரும்பு MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
முழு vm_check_range(கட்டமைப்பு proc *caller, கட்டமைப்பு proc *target,
	vir_bytes vir_addr, size_t bytes, முழு writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	முழு r;

	எனில் ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		திரும்பு r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	திரும்பு VMSUSPEND;
}

#எனில் 0
நிலையான எழுத்து *flagstr(u32_t e, மாறிலி முழு dir)
{
	நிலையான எழுத்து str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	எனில்(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	இல்லையெனில்
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	திரும்பு str;
}

நிலையான வெறுமை vm_pt_print(u32_t *pagetable, மாறிலி u32_t v)
{
	முழு pte;
	முழு col = 0;

	ப்ரதிஜ்ஞா(!((u32_t) pagetable % I386_PAGE_SIZE));

	க்கு(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		எனில்(!(pte_v & I386_VM_PRESENT))
			தொடர்;
		pfa = I386_VM_PFA(pte_v);
		printf("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		எனில்(col == 3) { printf("\n"); col = 0; }
	}
	எனில்(col > 0) printf("\n");

	திரும்பு;
}

நிலையான வெறுமை vm_print(u32_t *வேர்)
{
	முழு pde;

	ப்ரதிஜ்ஞா(!((u32_t) வேர் % I386_PAGE_SIZE));

	printf("page table 0x%lx:\n", வேர்);

	க்கு(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (வேர் + pde));
		எனில்(!(pde_v & I386_VM_PRESENT))
			தொடர்;
		எனில்(pde_v & I386_VM_BIGPAGE) {
			printf("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} இல்லையெனில் {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			printf("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			printf("\n");
		}
	}


	திரும்பு;
}
#endif

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
முழு vm_memset(கட்டமைப்பு proc* caller, endpoint_t who, phys_bytes ph, முழு c,
	phys_bytes மொத்தம்)
{
	u32_t pattern;
	கட்டமைப்பு proc *whoptr = வெற்று;
	phys_bytes cur_ph = ph;
	phys_bytes இடது = மொத்தம்;
	phys_bytes ptr, chunk, pfa = 0;
	முழு new_cr3, r = OK;

	எனில் ((r = check_resumed_caller(caller)) != OK)
		திரும்பு r;

	/* NONE for physical, otherwise virtual */
	எனில் (who != NONE && !(whoptr = endpoint_lookup(who)))
		திரும்பு ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ப்ரதிஜ்ஞா(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	போது (இடது > 0) {
		new_cr3 = 0;
		chunk = இடது;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		எனில் (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		எனில் ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			எனில் (whoptr) {
				vm_suspend(caller, whoptr, ph, மொத்தம்,
						   VMSTYPE_KERNELCALL, 1);
				ப்ரதிஜ்ஞா(catch_pagefaults);
				catch_pagefaults = 0;
				திரும்பு VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		இடது -= chunk;
	}

	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ப்ரதிஜ்ஞா(catch_pagefaults);
	catch_pagefaults = 0;

	திரும்பு OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
முழு virtual_copy_f(
  கட்டமைப்பு proc * caller,
  கட்டமைப்பு vir_addr *src_addr,	/* source virtual address */
  கட்டமைப்பு vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  முழு vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  கட்டமைப்பு vir_addr *vir_addr[2];	/* virtual source and destination address */
  முழு i, r;
  கட்டமைப்பு proc *procs[2];

  ப்ரதிஜ்ஞா((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  எனில் (bytes <= 0) திரும்பு(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  க்கு (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	முழு proc_nr;
	கட்டமைப்பு proc *p;

	எனில்(proc_e == NONE) {
		p = வெற்று;
	} இல்லையெனில் {
		எனில்(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			திரும்பு ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  எனில் ((r = check_resumed_caller(caller)) != OK)
	திரும்பு r;

  எனில்((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	முழு writeflag;
  	கட்டமைப்பு proc *target = வெற்று;
  	phys_bytes lin;
  	எனில்(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	எனில்(!vmcheck || !caller) {
    		திரும்பு r;
  	}

  	எனில்(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} இல்லையெனில் எனில்(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} இல்லையெனில் {
  		panic("r strange: %d",  r);
  	}

	ப்ரதிஜ்ஞா(caller);
	ப்ரதிஜ்ஞா(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	திரும்பு VMSUSPEND;
  }

  திரும்பு OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
முழு data_copy(மாறிலி endpoint_t from_proc, மாறிலி vir_bytes from_addr,
	மாறிலி endpoint_t to_proc, மாறிலி vir_bytes to_addr,
	size_t bytes)
{
  கட்டமைப்பு vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  ப்ரதிஜ்ஞா(src.proc_nr_e != NONE);
  ப்ரதிஜ்ஞா(dst.proc_nr_e != NONE);

  திரும்பு virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
முழு data_copy_vmcheck(கட்டமைப்பு proc * caller,
	மாறிலி endpoint_t from_proc, மாறிலி vir_bytes from_addr,
	மாறிலி endpoint_t to_proc, மாறிலி vir_bytes to_addr,
	size_t bytes)
{
  கட்டமைப்பு vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  ப்ரதிஜ்ஞா(src.proc_nr_e != NONE);
  ப்ரதிஜ்ஞா(dst.proc_nr_e != NONE);

  திரும்பு virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

வெறுமை memory_init(வெறுமை)
{
	ப்ரதிஜ்ஞா(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	ப்ரதிஜ்ஞா(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
	ப்ரதிஜ்ஞா(nfreepdes == 2);
	ப்ரதிஜ்ஞா(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
வெறுமை arch_proc_init(கட்டமைப்பு proc *pr, மாறிலி u32_t ip, மாறிலி u32_t sp,
	மாறிலி u32_t ps_str, எழுத்து *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, அளவு(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

நிலையான முழு oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

வெளி எழுத்து *video_mem;

வெளி எழுத்து usermapped_start, usermapped_end, usermapped_nonglo_start;

முழு arch_phys_map(மாறிலி முழு index,
			phys_bytes *addr,
			phys_bytes *len,
			முழு *flags)
{
	நிலையான முழு first = 1;
	முழு freeidx = 0;
	நிலையான எழுத்து *ser_var = வெற்று;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	எனில்(first) {
		memset(&minix_kerninfo, 0, அளவு(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		எனில்(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		எனில்(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		எனில்(lapic_addr)
			lapic_mapping_index = freeidx++;
		எனில் (ioapic_enabled) {
			ioapic_first_index = freeidx;
			ப்ரதிஜ்ஞா(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		எனில்((ser_var = env_get("oxpcie"))) {
			எனில்(ser_var[0] != '0' || ser_var[1] != 'x') {
				printf("oxpcie address in hex please\n");
			} இல்லையெனில் {
				printf("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
	}

	எனில்(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		திரும்பு OK;
	}
	இல்லையெனில் எனில்(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		திரும்பு OK;
	}
	இல்லையெனில் எனில் (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		திரும்பு OK;
	}
#ifdef USE_APIC
	இல்லையெனில் எனில் (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		எனில் (!lapic_addr)
			திரும்பு EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		திரும்பு OK;
	}
	இல்லையெனில் எனில் (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		முழு ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		ப்ரதிஜ்ஞா(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		printf("ioapic map: addr 0x%lx\n", *addr);
		திரும்பு OK;
	}
#endif

#எனில் CONFIG_OXPCIE
	எனில்(index == oxpcie_mapping_index) {
		*addr = strtoul(ser_var+2, வெற்று, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		திரும்பு OK;
	}
#endif

	திரும்பு EINVAL;
}

முழு arch_phys_map_reply(மாறிலி முழு index, மாறிலி vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	எனில் (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		திரும்பு OK;
	}
	இல்லையெனில் எனில் (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		முழு i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		திரும்பு OK;
	}
#endif

#எனில் CONFIG_OXPCIE
	எனில் (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned எழுத்து *) addr);
		திரும்பு OK;
	}
#endif
	எனில்(index == first_um_idx) {
		வெளி கட்டமைப்பு minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		வெளி u32_t usermapped_offset;
		ப்ரதிஜ்ஞா(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (வெறுமை *) ((u32_t)ptr + usermapped_offset)
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
		எனில்(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} இல்லையெனில்  எனில்(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} இல்லையெனில்	{
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
		எனில்(env_get("libc_ipc")) {
			printf("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = வெற்று;
		} இல்லையெனில் {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		திரும்பு OK;
	}

	எனில்(index == usermapped_index) திரும்பு OK;

	எனில் (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		திரும்பு OK;
	}

	திரும்பு EINVAL;
}

முழு arch_enable_paging(கட்டமைப்பு proc * caller)
{
	ப்ரதிஜ்ஞா(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (எழுத்து *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	எனில் (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	எனில் (ioapic_enabled) {
		முழு i;

		க்கு (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#எனில் CONFIG_SMP
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
	எனில் (watchdog_enabled)
		i386_watchdog_start();
#endif

	திரும்பு OK;
}

வெறுமை release_address_space(கட்டமைப்பு proc *pr)
{
	pr->p_seg.p_cr3_v = வெற்று;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
முழு platform_tbl_checksum_ok(வெறுமை *ptr, unsigned முழு length)
{
	u8_t total = 0;
	unsigned முழு i;
	க்கு (i = 0; i < length; i++)
		total += ((unsigned எழுத்து *)ptr)[i];
	திரும்பு !total;
}

முழு platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					வெறுமை * buff,
					unsigned size,
					phys_bytes * phys_addr,
					முழு ((* cmp_f)(வெறுமை *)))
{
	phys_bytes addr;

	க்கு (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		எனில் (cmp_f(buff)) {
			எனில் (phys_addr)
				*phys_addr = addr;
			திரும்பு 1;
		}
	}
	திரும்பு 0;
}
