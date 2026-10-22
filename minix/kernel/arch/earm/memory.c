
#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "kernel/proc.h"
#অন্তর্ভাব "kernel/vm.h"

#অন্তর্ভাব <machine/vm.h>

#অন্তর্ভাব <minix/type.h>
#অন্তর্ভাব <minix/board.h>
#অন্তর্ভাব <minix/syslib.h>
#অন্তর্ভাব <minix/cpufeature.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব <machine/vm.h>

#অন্তর্ভাব "arch_proto.h"
#অন্তর্ভাব "kernel/proto.h"
#অন্তর্ভাব "kernel/debug.h"
#অন্তর্ভাব "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
স্থির পূর্ণসংখ্যা nfreepdes = 0;
#define MAXFREEPDES	2
স্থির পূর্ণসংখ্যা freepdes[MAXFREEPDES];

স্থির u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
স্থির kern_phys_map *kern_phys_map_শীর্ষ;

শূন্য mem_clear_mapcache(শূন্য)
{
	পূর্ণসংখ্যা i;
	জন্য(i = 0; i < nfreepdes; i++) {
		কাঠামো proc *ptproc = get_cpulocal_var(ptproc);
		পূর্ণসংখ্যা pde = freepdes[i];
		u32_t *ptv;
		প্রতিজ্ঞা(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		প্রতিজ্ঞা(ptv);
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
স্থির phys_bytes createpde(
	ধ্রুবক কাঠামো proc *pr,	/* Requested process, NULL for physical. */
	ধ্রুবক phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	পূর্ণসংখ্যা free_pde_idx,	/* index of the free slot to use */
	পূর্ণসংখ্যা *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	পূর্ণসংখ্যা pde;

	প্রতিজ্ঞা(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	প্রতিজ্ঞা(pde >= 0 && pde < 4096);

	যদি(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ফেরত linaddr;
	}

	যদি(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		প্রতিজ্ঞা(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} অন্যথা {
		/* Requested address is physical. Make up the PDE entry. */
		প্রতিজ্ঞা (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

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
	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	যদি(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & ARM_VM_OFFSET_MASK_1MB; /* Offset in 1MB window. */
	*bytes = MIN(*bytes, ARM_SECTION_আকার - offset);

	/* Return the linear address of the start of the new mapping. */
	ফেরত ARM_SECTION_আকার*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
স্থির পূর্ণসংখ্যা check_resumed_caller(কাঠামো proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	যদি (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		প্রতিজ্ঞা(caller->p_vmrequest.vmresult != VMSUSPEND);
		ফেরত caller->p_vmrequest.vmresult;
	}

	ফেরত OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
স্থির পূর্ণসংখ্যা lin_lin_copy(কাঠামো proc *srcproc, vir_bytes srclinaddr, 
	কাঠামো proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	প্রতিজ্ঞা(get_cpulocal_var(ptproc));
	প্রতিজ্ঞা(get_cpulocal_var(proc_ptr));
	প্রতিজ্ঞা(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	প্রতিজ্ঞা(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	যদি(srcproc) প্রতিজ্ঞা(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	যদি(dstproc) প্রতিজ্ঞা(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	প্রতিজ্ঞা(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	যদি(srcproc) প্রতিজ্ঞা(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	যদি(dstproc) প্রতিজ্ঞা(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	যখন(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		পূর্ণসংখ্যা changed = 0;

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

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		যদি(changed)
			reload_ttbr0();

		/* Check for overflow. */
		যদি (srcptr + chunk < srcptr) ফেরত EFAULT_SRC;
		যদি (dstptr + chunk < dstptr) ফেরত EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		যদি(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			যদি(addr >= src_aligned && addr < (srcptr + chunk)) {
				ফেরত EFAULT_SRC;
			}
			যদি(addr >= dst_aligned && addr < (dstptr + chunk)) {
				ফেরত EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ফেরত EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	যদি(srcproc) প্রতিজ্ঞা(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	যদি(dstproc) প্রতিজ্ঞা(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	প্রতিজ্ঞা(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	ফেরত OK;
}

স্থির u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	পূর্ণসংখ্যা r;

	যদি((r=lin_lin_copy(ফাঁকা, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, আকার(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ফেরত v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  নিবন্ধ কাঠামো proc *rp,		/* pointer to proc table entry for process */
  পূর্ণসংখ্যা seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	যদি(vm_lookup(rp, vir_addr, &phys, ফাঁকা) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} অন্যথা {
		যদি(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	যদি(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		ফেরত 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	যদি(bytes > 0 && vm_lookup_range(rp, vir_addr, ফাঁকা, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		ফেরত 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	প্রতিজ্ঞা(phys);

	ফেরত phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
পূর্ণসংখ্যা vm_lookup(ধ্রুবক কাঠামো proc *proc, ধ্রুবক vir_bytes আভাসী,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *মূল, *pt;
	পূর্ণসংখ্যা pde, pte;
	u32_t pde_v, pte_v;

	প্রতিজ্ঞা(proc);
	প্রতিজ্ঞা(physical);
	প্রতিজ্ঞা(!isemptyp(proc));
	প্রতিজ্ঞা(HASPT(proc));

	/* Retrieve page directory entry. */
	মূল = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	প্রতিজ্ঞা(!((u32_t) মূল % ARM_PAGEDIR_আকার));
	pde = ARM_VM_PDE(আভাসী);
	প্রতিজ্ঞা(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (মূল + pde));

	যদি(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		ফেরত EFAULT;
	}

	যদি(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		যদি(ptent) *ptent = pde_v;
		*physical += আভাসী & ARM_VM_OFFSET_MASK_1MB;
	} অন্যথা  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		প্রতিজ্ঞা(!((u32_t) pt % ARM_PAGETABLE_আকার));
		pte = ARM_VM_PTE(আভাসী);
		প্রতিজ্ঞা(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		যদি(!(pte_v & ARM_VM_PTE_PRESENT)) {
			ফেরত EFAULT;
		}

		যদি(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += আভাসী % ARM_PAGE_আকার;
	}

	ফেরত OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ধ্রুবক কাঠামো proc *proc, vir_bytes vir_addr,
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
	phys_bytes phys, পরবর্তী_phys;
	size_t len;

	প্রতিজ্ঞা(proc);
	প্রতিজ্ঞা(bytes > 0);
	প্রতিজ্ঞা(HASPT(proc));

	/* Look up the first page. */
	যদি (vm_lookup(proc, vir_addr, &phys, ফাঁকা) != OK)
		ফেরত 0;

	যদি (phys_addr != ফাঁকা)
		*phys_addr = phys;

	len = ARM_PAGE_আকার - (vir_addr % ARM_PAGE_আকার);
	vir_addr += len;
	পরবর্তী_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	যখন (len < bytes) {
		যদি (vm_lookup(proc, vir_addr, &phys, ফাঁকা) != OK)
			বিরতি;

		যদি (পরবর্তী_phys != phys)
			বিরতি;

		len += ARM_PAGE_আকার;
		vir_addr += ARM_PAGE_আকার;
		পরবর্তী_phys += ARM_PAGE_আকার;
	}

	/* We might now have overshot the requested length somewhat. */
	ফেরত MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
পূর্ণসংখ্যা vm_check_range(কাঠামো proc *caller, কাঠামো proc *target,
	vir_bytes vir_addr, size_t bytes, পূর্ণসংখ্যা writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	পূর্ণসংখ্যা r;

	যদি ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ফেরত r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	ফেরত VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
পূর্ণসংখ্যা vm_memset(কাঠামো proc* caller, endpoint_t who, phys_bytes ph, পূর্ণসংখ্যা c,
	phys_bytes গুনতি)
{
	u32_t pattern;
	কাঠামো proc *whoptr = ফাঁকা;
	phys_bytes cur_ph = ph;
	phys_bytes বাম = গুনতি;
	phys_bytes ptr, chunk, pfa = 0;
	পূর্ণসংখ্যা new_ttbr, r = OK;

	যদি ((r = check_resumed_caller(caller)) != OK)
		ফেরত r;

	/* NONE for physical, otherwise virtual */
	যদি (who != NONE && !(whoptr = endpoint_lookup(who)))
		ফেরত ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	প্রতিজ্ঞা(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	যখন (বাম > 0) {
		new_ttbr = 0;
		chunk = বাম;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		যদি (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		যদি ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			যদি (whoptr) {
				vm_suspend(caller, whoptr, ph, গুনতি,
						   VMSTYPE_KERNELCALL, 1);
				প্রতিজ্ঞা(catch_pagefaults);
				catch_pagefaults = 0;
				ফেরত VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		বাম -= chunk;
	}

	প্রতিজ্ঞা(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	প্রতিজ্ঞা(catch_pagefaults);
	catch_pagefaults = 0;

	ফেরত OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
পূর্ণসংখ্যা virtual_copy_f(
  কাঠামো proc * caller,
  কাঠামো vir_addr *src_addr,		/* source virtual address */
  কাঠামো vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  পূর্ণসংখ্যা vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  কাঠামো vir_addr *vir_addr[2];	/* virtual source and destination address */
  পূর্ণসংখ্যা i, r;
  কাঠামো proc *procs[2];

  প্রতিজ্ঞা((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  যদি (bytes <= 0) ফেরত(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  জন্য (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	পূর্ণসংখ্যা proc_nr;
	কাঠামো proc *p;

	যদি(proc_e == NONE) {
		p = ফাঁকা;
	} অন্যথা {
		যদি(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			ফেরত ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  যদি ((r = check_resumed_caller(caller)) != OK)
	ফেরত r;

  যদি((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	পূর্ণসংখ্যা writeflag;
  	কাঠামো proc *target = ফাঁকা;
  	phys_bytes lin;
  	যদি(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	যদি(!vmcheck || !caller) {
    		ফেরত r;
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
	ফেরত VMSUSPEND;
  }

  ফেরত OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
পূর্ণসংখ্যা তথ্য_copy(ধ্রুবক endpoint_t from_proc, ধ্রুবক vir_bytes from_addr,
	ধ্রুবক endpoint_t to_proc, ধ্রুবক vir_bytes to_addr,
	size_t bytes)
{
  কাঠামো vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  প্রতিজ্ঞা(src.proc_nr_e != NONE);
  প্রতিজ্ঞা(dst.proc_nr_e != NONE);

  ফেরত virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
পূর্ণসংখ্যা তথ্য_copy_vmcheck(কাঠামো proc * caller,
	ধ্রুবক endpoint_t from_proc, ধ্রুবক vir_bytes from_addr,
	ধ্রুবক endpoint_t to_proc, ধ্রুবক vir_bytes to_addr,
	size_t bytes)
{
  কাঠামো vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  প্রতিজ্ঞা(src.proc_nr_e != NONE);
  প্রতিজ্ঞা(dst.proc_nr_e != NONE);

  ফেরত virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

শূন্য memory_init(শূন্য)
{
	প্রতিজ্ঞা(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	প্রতিজ্ঞা(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	প্রতিজ্ঞা(nfreepdes == 2);
	প্রতিজ্ঞা(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
শূন্য arch_proc_init(কাঠামো proc *pr, ধ্রুবক u32_t ip, ধ্রুবক u32_t sp,
	ধ্রুবক u32_t ps_str, অক্ষর *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

স্থির পূর্ণসংখ্যা usermapped_glo_সূচক = -1,
	usermapped_সূচক = -1, first_um_idx = -1;


/* defined in kernel.lds */
বাহ্যিক অক্ষর usermapped_start, usermapped_end, usermapped_nonglo_start;

পূর্ণসংখ্যা arch_phys_map(ধ্রুবক পূর্ণসংখ্যা সূচক,
			phys_bytes *addr,
			phys_bytes *len,
			পূর্ণসংখ্যা *flags)
{
	স্থির পূর্ণসংখ্যা first = 1;
	kern_phys_map *phys_maps;

	পূর্ণসংখ্যা freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	যদি(first) {
		memset(&minix_kerninfo, 0, আকার(minix_kerninfo));
		যদি(glo_len > 0) {
			usermapped_glo_সূচক = freeidx++;
		}

		usermapped_সূচক = freeidx++;
		first_um_idx = usermapped_সূচক;
		যদি(usermapped_glo_সূচক != -1)
			first_um_idx = usermapped_glo_সূচক;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_শীর্ষ;
		যখন(phys_maps != ফাঁকা){
			phys_maps->সূচক = freeidx++;
			phys_maps = phys_maps->পরবর্তী;
		}

	}

	যদি(সূচক == usermapped_glo_সূচক) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ফেরত OK;
	}
	অন্যথা যদি(সূচক == usermapped_সূচক) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		ফেরত OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_শীর্ষ;
	যখন(phys_maps != ফাঁকা){
		যদি(phys_maps->সূচক == সূচক){
			*addr = phys_maps->addr;
			*len =  phys_maps->আকার_2;
			*flags = phys_maps->vm_flags;
			ফেরত OK;
		}
		phys_maps = phys_maps->পরবর্তী;
	}

	ফেরত EINVAL;
}

পূর্ণসংখ্যা arch_phys_map_reply(ধ্রুবক পূর্ণসংখ্যা সূচক, ধ্রুবক vir_bytes addr)
{
	kern_phys_map *phys_maps;

	যদি(সূচক == first_um_idx) {
		u32_t usermapped_offset;
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
		ASSIGN(arm_frclock);
		ASSIGN(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ফেরত OK;
	}

	যদি (সূচক == usermapped_সূচক) {
		ফেরত OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_শীর্ষ;
	যখন(phys_maps != ফাঁকা){
		যদি(phys_maps->সূচক == সূচক){
			প্রতিজ্ঞা(phys_maps->cb != ফাঁকা);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			ফেরত OK;
		}
		phys_maps = phys_maps->পরবর্তী;
	}

	ফেরত EINVAL;
}

পূর্ণসংখ্যা arch_enable_paging(কাঠামো proc * caller)
{
	kern_phys_map *phys_maps;
	প্রতিজ্ঞা(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_শীর্ষ;
	যখন(phys_maps != ফাঁকা){
		প্রতিজ্ঞা(phys_maps->cb != ফাঁকা);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->পরবর্তী;
	}

	ফেরত OK;
}

শূন্য release_address_space(কাঠামো proc *pr)
{
	pr->p_seg.p_ttbr_v = ফাঁকা;
	barrier();
}



/*
 * Request a physical mapping
 */
পূর্ণসংখ্যা kern_req_phys_map( phys_bytes base_address, vir_bytes io_আকার, 
		       পূর্ণসংখ্যা vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	প্রতিজ্ঞা(base_address != 0);
	প্রতিজ্ঞা(io_আকার % ARM_PAGE_আকার == 0);
	প্রতিজ্ঞা(cb != ফাঁকা);

	priv->addr  = base_address;
	priv->আকার_2  = io_আকার;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->সূচক = -1;
	priv->পরবর্তী = ফাঁকা;
	

	যদি (kern_phys_map_শীর্ষ == ফাঁকা){
		/* keep a list of items this is the first one */
		kern_phys_map_শীর্ষ = priv;
		kern_phys_map_শীর্ষ->পরবর্তী = ফাঁকা;
	} অন্যথা {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->পরবর্তী = kern_phys_map_শীর্ষ;
		/* replace the head */
		kern_phys_map_শীর্ষ = priv;
	}
	ফেরত 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
পূর্ণসংখ্যা kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	ফেরত 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
পূর্ণসংখ্যা kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_আকার, 
	পূর্ণসংখ্যা vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	ফেরত kern_req_phys_map(base_address,io_আকার,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

