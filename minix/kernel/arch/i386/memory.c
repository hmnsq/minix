
#অন্তর্ভাৱ "kernel/kernel.h"
#অন্তর্ভাৱ "kernel/vm.h"

#অন্তর্ভাৱ <machine/vm.h>

#অন্তর্ভাৱ <minix/syslib.h>
#অন্তর্ভাৱ <minix/cpufeature.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <stdlib.h>

#অন্তর্ভাৱ <machine/vm.h>

#অন্তর্ভাৱ "oxpcie.h"
#অন্তর্ভাৱ "arch_proto.h"

#ifdef USE_APIC
#অন্তর্ভাৱ "apic.h"
#ifdef USE_WATCHDOG
#অন্তর্ভাৱ "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
স্থির পূর্ণাঙ্ক nfreepdes = 0;
#define MAXFREEPDES	2
স্থির পূর্ণাঙ্ক freepdes[MAXFREEPDES];

স্থির u32_t phys_get32(phys_bytes v);

শূন্য mem_clear_mapcache(শূন্য)
{
	পূর্ণাঙ্ক i;
	বাবে(i = 0; i < nfreepdes; i++) {
		সংরচনা proc *ptproc = get_cpulocal_var(ptproc);
		পূর্ণাঙ্ক pde = freepdes[i];
		u32_t *ptv;
		প্রতিজ্ঞা(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		প্রতিজ্ঞা(ptv);
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
স্থির phys_bytes createpde(
	ধ্রুৱ সংরচনা proc *pr,	/* Requested process, NULL for physical. */
	ধ্রুৱ phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	পূর্ণাঙ্ক free_pde_idx,	/* index of the free slot to use */
	পূর্ণাঙ্ক *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	পূর্ণাঙ্ক pde;

	প্রতিজ্ঞা(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	প্রতিজ্ঞা(pde >= 0 && pde < 1024);

	যদি(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ঘূৰাই linaddr;
	}

	যদি(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		প্রতিজ্ঞা(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} অন্যথা {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	যদি(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
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
	ঘূৰাই I386_BIG_PAGE_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
স্থির পূর্ণাঙ্ক check_resumed_caller(সংরচনা proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	যদি (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		প্রতিজ্ঞা(caller->p_vmrequest.vmresult != VMSUSPEND);
		ঘূৰাই caller->p_vmrequest.vmresult;
	}

	ঘূৰাই OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
স্থির পূর্ণাঙ্ক lin_lin_copy(সংরচনা proc *srcproc, vir_bytes srclinaddr, 
	সংরচনা proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	প্রতিজ্ঞা(get_cpulocal_var(ptproc));
	প্রতিজ্ঞা(get_cpulocal_var(proc_ptr));
	প্রতিজ্ঞা(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	প্রতিজ্ঞা(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	যদি(srcproc) প্রতিজ্ঞা(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	যদি(dstproc) প্রতিজ্ঞা(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	প্রতিজ্ঞা(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	যদি(srcproc) প্রতিজ্ঞা(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	যদি(dstproc) প্রতিজ্ঞা(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	যেতিয়ালৈ(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		পূর্ণাঙ্ক changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		যদি (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		যদি (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		যদি(changed)
			reload_cr3(); 

		/* Check for overflow. */
		যদি (srcptr + chunk < srcptr) ঘূৰাই EFAULT_SRC;
		যদি (dstptr + chunk < dstptr) ঘূৰাই EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		যদি(addr) {
			/* If addr is nonzero, a page fault was caught. */

			যদি(addr >= srcptr && addr < (srcptr + chunk)) {
				ঘূৰাই EFAULT_SRC;
			}
			যদি(addr >= dstptr && addr < (dstptr + chunk)) {
				ঘূৰাই EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ঘূৰাই EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	যদি(srcproc) প্রতিজ্ঞা(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	যদি(dstproc) প্রতিজ্ঞা(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	প্রতিজ্ঞা(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	ঘূৰাই OK;
}


স্থির u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	পূর্ণাঙ্ক r;

	যদি((r=lin_lin_copy(রিক্ত, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, পরিমাণ(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ঘূৰাই v;
}

#যদি 0
স্থির ৱর্ণ *cr0_str(u32_t e)
{
	স্থির ৱর্ণ str[80];
	strcpy(str, "");
#define FLAG(v) কৰা { যদি(e & (v)) { strcat(str, #v " "); e &= ~v; } } যেতিয়ালৈ(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	যদি(e) { strcat(str, " (++)"); }
	ঘূৰাই str;
}

স্থির ৱর্ণ *cr4_str(u32_t e)
{
	স্থির ৱর্ণ str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	যদি(e) { strcat(str, " (++)"); }
	ঘূৰাই str;
}
#endif

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  পঞ্জিকা সংরচনা proc *rp,		/* pointer to proc table entry for process */
  পূর্ণাঙ্ক seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	যদি(vm_lookup(rp, vir_addr, &phys, রিক্ত) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} অন্যথা {
		যদি(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	যদি(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		ঘূৰাই 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	যদি(bytes > 0 && vm_lookup_range(rp, vir_addr, রিক্ত, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		ঘূৰাই 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	প্রতিজ্ঞা(phys);

	ঘূৰাই phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
পূর্ণাঙ্ক vm_lookup(ধ্রুৱ সংরচনা proc *proc, ধ্রুৱ vir_bytes আভাস,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *মূল, *pt;
	পূর্ণাঙ্ক pde, pte;
	u32_t pde_v, pte_v;

	প্রতিজ্ঞা(proc);
	প্রতিজ্ঞা(physical);
	প্রতিজ্ঞা(!isemptyp(proc));
	প্রতিজ্ঞা(HASPT(proc));

	/* Retrieve page directory entry. */
	মূল = (u32_t *) proc->p_seg.p_cr3;
	প্রতিজ্ঞা(!((u32_t) মূল % I386_PAGE_SIZE));
	pde = I386_VM_PDE(আভাস);
	প্রতিজ্ঞা(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (মূল + pde));

	যদি(!(pde_v & I386_VM_PRESENT)) {
		ঘূৰাই EFAULT;
	}

	/* We don't expect to ever see this. */
	যদি(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		যদি(ptent) *ptent = pde_v;
		*physical += আভাস & I386_VM_OFFSET_MASK_4MB;
	} অন্যথা {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		প্রতিজ্ঞা(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(আভাস);
		প্রতিজ্ঞা(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		যদি(!(pte_v & I386_VM_PRESENT)) {
			ঘূৰাই EFAULT;
		}

		যদি(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += আভাস % I386_PAGE_SIZE;
	}

	ঘূৰাই OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ধ্রুৱ সংরচনা proc *proc, vir_bytes vir_addr,
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

	প্রতিজ্ঞা(proc);
	প্রতিজ্ঞা(bytes > 0);
	প্রতিজ্ঞা(HASPT(proc));

	/* Look up the first page. */
	যদি (vm_lookup(proc, vir_addr, &phys, রিক্ত) != OK)
		ঘূৰাই 0;

	যদি (phys_addr != রিক্ত)
		*phys_addr = phys;

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	যেতিয়ালৈ (len < bytes) {
		যদি (vm_lookup(proc, vir_addr, &phys, রিক্ত) != OK)
			ভঙ্গ;

		যদি (next_phys != phys)
			ভঙ্গ;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	ঘূৰাই MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
পূর্ণাঙ্ক vm_check_range(সংরচনা proc *caller, সংরচনা proc *target,
	vir_bytes vir_addr, size_t bytes, পূর্ণাঙ্ক writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	পূর্ণাঙ্ক r;

	যদি ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ঘূৰাই r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	ঘূৰাই VMSUSPEND;
}

#যদি 0
স্থির ৱর্ণ *flagstr(u32_t e, ধ্রুৱ পূর্ণাঙ্ক dir)
{
	স্থির ৱর্ণ str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	যদি(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	অন্যথা
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	ঘূৰাই str;
}

স্থির শূন্য vm_pt_print(u32_t *pagetable, ধ্রুৱ u32_t v)
{
	পূর্ণাঙ্ক pte;
	পূর্ণাঙ্ক col = 0;

	প্রতিজ্ঞা(!((u32_t) pagetable % I386_PAGE_SIZE));

	বাবে(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		যদি(!(pte_v & I386_VM_PRESENT))
			অনুৱর্ত;
		pfa = I386_VM_PFA(pte_v);
		printf("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		যদি(col == 3) { printf("\n"); col = 0; }
	}
	যদি(col > 0) printf("\n");

	ঘূৰাই;
}

স্থির শূন্য vm_print(u32_t *মূল)
{
	পূর্ণাঙ্ক pde;

	প্রতিজ্ঞা(!((u32_t) মূল % I386_PAGE_SIZE));

	printf("page table 0x%lx:\n", মূল);

	বাবে(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (মূল + pde));
		যদি(!(pde_v & I386_VM_PRESENT))
			অনুৱর্ত;
		যদি(pde_v & I386_VM_BIGPAGE) {
			printf("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} অন্যথা {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			printf("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			printf("\n");
		}
	}


	ঘূৰাই;
}
#endif

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
পূর্ণাঙ্ক vm_memset(সংরচনা proc* caller, endpoint_t who, phys_bytes ph, পূর্ণাঙ্ক c,
	phys_bytes গুনতি)
{
	u32_t pattern;
	সংরচনা proc *whoptr = রিক্ত;
	phys_bytes cur_ph = ph;
	phys_bytes বাম = গুনতি;
	phys_bytes ptr, chunk, pfa = 0;
	পূর্ণাঙ্ক new_cr3, r = OK;

	যদি ((r = check_resumed_caller(caller)) != OK)
		ঘূৰাই r;

	/* NONE for physical, otherwise virtual */
	যদি (who != NONE && !(whoptr = endpoint_lookup(who)))
		ঘূৰাই ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	প্রতিজ্ঞা(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	যেতিয়ালৈ (বাম > 0) {
		new_cr3 = 0;
		chunk = বাম;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		যদি (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		যদি ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			যদি (whoptr) {
				vm_suspend(caller, whoptr, ph, গুনতি,
						   VMSTYPE_KERNELCALL, 1);
				প্রতিজ্ঞা(catch_pagefaults);
				catch_pagefaults = 0;
				ঘূৰাই VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		বাম -= chunk;
	}

	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	প্রতিজ্ঞা(catch_pagefaults);
	catch_pagefaults = 0;

	ঘূৰাই OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
পূর্ণাঙ্ক virtual_copy_f(
  সংরচনা proc * caller,
  সংরচনা vir_addr *src_addr,	/* source virtual address */
  সংরচনা vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  পূর্ণাঙ্ক vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  সংরচনা vir_addr *vir_addr[2];	/* virtual source and destination address */
  পূর্ণাঙ্ক i, r;
  সংরচনা proc *procs[2];

  প্রতিজ্ঞা((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  যদি (bytes <= 0) ঘূৰাই(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  বাবে (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	পূর্ণাঙ্ক proc_nr;
	সংরচনা proc *p;

	যদি(proc_e == NONE) {
		p = রিক্ত;
	} অন্যথা {
		যদি(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			ঘূৰাই ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  যদি ((r = check_resumed_caller(caller)) != OK)
	ঘূৰাই r;

  যদি((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	পূর্ণাঙ্ক writeflag;
  	সংরচনা proc *target = রিক্ত;
  	phys_bytes lin;
  	যদি(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	যদি(!vmcheck || !caller) {
    		ঘূৰাই r;
  	}

  	যদি(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} অন্যথা যদি(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} অন্যথা {
  		panic("r strange: %d",  r);
  	}

	প্রতিজ্ঞা(caller);
	প্রতিজ্ঞা(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	ঘূৰাই VMSUSPEND;
  }

  ঘূৰাই OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
পূর্ণাঙ্ক data_copy(ধ্রুৱ endpoint_t from_proc, ধ্রুৱ vir_bytes from_addr,
	ধ্রুৱ endpoint_t to_proc, ধ্রুৱ vir_bytes to_addr,
	size_t bytes)
{
  সংরচনা vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  প্রতিজ্ঞা(src.proc_nr_e != NONE);
  প্রতিজ্ঞা(dst.proc_nr_e != NONE);

  ঘূৰাই virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
পূর্ণাঙ্ক data_copy_vmcheck(সংরচনা proc * caller,
	ধ্রুৱ endpoint_t from_proc, ধ্রুৱ vir_bytes from_addr,
	ধ্রুৱ endpoint_t to_proc, ধ্রুৱ vir_bytes to_addr,
	size_t bytes)
{
  সংরচনা vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  প্রতিজ্ঞা(src.proc_nr_e != NONE);
  প্রতিজ্ঞা(dst.proc_nr_e != NONE);

  ঘূৰাই virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

শূন্য memory_init(শূন্য)
{
	প্রতিজ্ঞা(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	প্রতিজ্ঞা(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
	প্রতিজ্ঞা(nfreepdes == 2);
	প্রতিজ্ঞা(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
শূন্য arch_proc_init(সংরচনা proc *pr, ধ্রুৱ u32_t ip, ধ্রুৱ u32_t sp,
	ধ্রুৱ u32_t ps_str, ৱর্ণ *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, পরিমাণ(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

স্থির পূর্ণাঙ্ক oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

বাহ্য ৱর্ণ *video_mem;

বাহ্য ৱর্ণ usermapped_start, usermapped_end, usermapped_nonglo_start;

পূর্ণাঙ্ক arch_phys_map(ধ্রুৱ পূর্ণাঙ্ক index,
			phys_bytes *addr,
			phys_bytes *len,
			পূর্ণাঙ্ক *flags)
{
	স্থির পূর্ণাঙ্ক first = 1;
	পূর্ণাঙ্ক freeidx = 0;
	স্থির ৱর্ণ *ser_var = রিক্ত;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	যদি(first) {
		memset(&minix_kerninfo, 0, পরিমাণ(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		যদি(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		যদি(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		যদি(lapic_addr)
			lapic_mapping_index = freeidx++;
		যদি (ioapic_enabled) {
			ioapic_first_index = freeidx;
			প্রতিজ্ঞা(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		যদি((ser_var = env_get("oxpcie"))) {
			যদি(ser_var[0] != '0' || ser_var[1] != 'x') {
				printf("oxpcie address in hex please\n");
			} অন্যথা {
				printf("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
	}

	যদি(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ঘূৰাই OK;
	}
	অন্যথা যদি(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		ঘূৰাই OK;
	}
	অন্যথা যদি (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		ঘূৰাই OK;
	}
#ifdef USE_APIC
	অন্যথা যদি (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		যদি (!lapic_addr)
			ঘূৰাই EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ঘূৰাই OK;
	}
	অন্যথা যদি (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		পূর্ণাঙ্ক ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		প্রতিজ্ঞা(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		printf("ioapic map: addr 0x%lx\n", *addr);
		ঘূৰাই OK;
	}
#endif

#যদি CONFIG_OXPCIE
	যদি(index == oxpcie_mapping_index) {
		*addr = strtoul(ser_var+2, রিক্ত, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ঘূৰাই OK;
	}
#endif

	ঘূৰাই EINVAL;
}

পূর্ণাঙ্ক arch_phys_map_reply(ধ্রুৱ পূর্ণাঙ্ক index, ধ্রুৱ vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	যদি (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		ঘূৰাই OK;
	}
	অন্যথা যদি (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		পূর্ণাঙ্ক i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		ঘূৰাই OK;
	}
#endif

#যদি CONFIG_OXPCIE
	যদি (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned ৱর্ণ *) addr);
		ঘূৰাই OK;
	}
#endif
	যদি(index == first_um_idx) {
		বাহ্য সংরচনা minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		বাহ্য u32_t usermapped_offset;
		প্রতিজ্ঞা(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (শূন্য *) ((u32_t)ptr + usermapped_offset)
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
		যদি(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} অন্যথা  যদি(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} অন্যথা	{
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
		যদি(env_get("libc_ipc")) {
			printf("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = রিক্ত;
		} অন্যথা {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ঘূৰাই OK;
	}

	যদি(index == usermapped_index) ঘূৰাই OK;

	যদি (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		ঘূৰাই OK;
	}

	ঘূৰাই EINVAL;
}

পূর্ণাঙ্ক arch_enable_paging(সংরচনা proc * caller)
{
	প্রতিজ্ঞা(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (ৱর্ণ *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	যদি (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	যদি (ioapic_enabled) {
		পূর্ণাঙ্ক i;

		বাবে (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#যদি CONFIG_SMP
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
	যদি (watchdog_enabled)
		i386_watchdog_start();
#endif

	ঘূৰাই OK;
}

শূন্য release_address_space(সংরচনা proc *pr)
{
	pr->p_seg.p_cr3_v = রিক্ত;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
পূর্ণাঙ্ক platform_tbl_checksum_ok(শূন্য *ptr, unsigned পূর্ণাঙ্ক length)
{
	u8_t total = 0;
	unsigned পূর্ণাঙ্ক i;
	বাবে (i = 0; i < length; i++)
		total += ((unsigned ৱর্ণ *)ptr)[i];
	ঘূৰাই !total;
}

পূর্ণাঙ্ক platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					শূন্য * buff,
					unsigned size,
					phys_bytes * phys_addr,
					পূর্ণাঙ্ক ((* cmp_f)(শূন্য *)))
{
	phys_bytes addr;

	বাবে (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		যদি (cmp_f(buff)) {
			যদি (phys_addr)
				*phys_addr = addr;
			ঘূৰাই 1;
		}
	}
	ঘূৰাই 0;
}
