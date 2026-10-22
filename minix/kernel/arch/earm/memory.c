
#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "kernel/proc.h"
#அந்தர்பாவ "kernel/vm.h"

#அந்தர்பாவ <machine/vm.h>

#அந்தர்பாவ <minix/type.h>
#அந்தர்பாவ <minix/board.h>
#அந்தர்பாவ <minix/syslib.h>
#அந்தர்பாவ <minix/cpufeature.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ <machine/vm.h>

#அந்தர்பாவ "arch_proto.h"
#அந்தர்பாவ "kernel/proto.h"
#அந்தர்பாவ "kernel/debug.h"
#அந்தர்பாவ "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
நிலையான முழு nfreepdes = 0;
#define MAXFREEPDES	2
நிலையான முழு freepdes[MAXFREEPDES];

நிலையான u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
நிலையான kern_phys_map *kern_phys_map_head;

வெறுமை mem_clear_mapcache(வெறுமை)
{
	முழு i;
	க்கு(i = 0; i < nfreepdes; i++) {
		கட்டமைப்பு proc *ptproc = get_cpulocal_var(ptproc);
		முழு pde = freepdes[i];
		u32_t *ptv;
		ப்ரதிஜ்ஞா(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		ப்ரதிஜ்ஞா(ptv);
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
	ப்ரதிஜ்ஞா(pde >= 0 && pde < 4096);

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
		ப்ரதிஜ்ஞா(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} இல்லையெனில் {
		/* Requested address is physical. Make up the PDE entry. */
		ப்ரதிஜ்ஞா (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

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
	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	எனில்(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
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
	திரும்பு ARM_SECTION_SIZE*pde + offset;
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
	ப்ரதிஜ்ஞா(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ப்ரதிஜ்ஞா(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	எனில்(srcproc) ப்ரதிஜ்ஞா(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	எனில்(dstproc) ப்ரதிஜ்ஞா(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	ப்ரதிஜ்ஞா(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
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

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		எனில்(changed)
			reload_ttbr0();

		/* Check for overflow. */
		எனில் (srcptr + chunk < srcptr) திரும்பு EFAULT_SRC;
		எனில் (dstptr + chunk < dstptr) திரும்பு EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		எனில்(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			எனில்(addr >= src_aligned && addr < (srcptr + chunk)) {
				திரும்பு EFAULT_SRC;
			}
			எனில்(addr >= dst_aligned && addr < (dstptr + chunk)) {
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
	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

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
	வேர் = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	ப்ரதிஜ்ஞா(!((u32_t) வேர் % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(மெய்நிகர்);
	ப்ரதிஜ்ஞா(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (வேர் + pde));

	எனில்(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		திரும்பு EFAULT;
	}

	எனில்(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		எனில்(ptent) *ptent = pde_v;
		*physical += மெய்நிகர் & ARM_VM_OFFSET_MASK_1MB;
	} இல்லையெனில்  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		ப்ரதிஜ்ஞா(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(மெய்நிகர்);
		ப்ரதிஜ்ஞா(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		எனில்(!(pte_v & ARM_VM_PTE_PRESENT)) {
			திரும்பு EFAULT;
		}

		எனில்(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += மெய்நிகர் % ARM_PAGE_SIZE;
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

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	போது (len < bytes) {
		எனில் (vm_lookup(proc, vir_addr, &phys, வெற்று) != OK)
			நிறுத்து;

		எனில் (next_phys != phys)
			நிறுத்து;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
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
	முழு new_ttbr, r = OK;

	எனில் ((r = check_resumed_caller(caller)) != OK)
		திரும்பு r;

	/* NONE for physical, otherwise virtual */
	எனில் (who != NONE && !(whoptr = endpoint_lookup(who)))
		திரும்பு ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ப்ரதிஜ்ஞா(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	போது (இடது > 0) {
		new_ttbr = 0;
		chunk = இடது;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		எனில் (new_ttbr) {
			reload_ttbr0();
		}
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

	ப்ரதிஜ்ஞா(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	ப்ரதிஜ்ஞா(catch_pagefaults);
	catch_pagefaults = 0;

	திரும்பு OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
முழு virtual_copy_f(
  கட்டமைப்பு proc * caller,
  கட்டமைப்பு vir_addr *src_addr,		/* source virtual address */
  கட்டமைப்பு vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  முழு vmcheck				/* if nonzero, can return VMSUSPEND */
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

	ப்ரதிஜ்ஞா(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
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
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

நிலையான முழு usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
வெளி எழுத்து usermapped_start, usermapped_end, usermapped_nonglo_start;

முழு arch_phys_map(மாறிலி முழு index,
			phys_bytes *addr,
			phys_bytes *len,
			முழு *flags)
{
	நிலையான முழு first = 1;
	kern_phys_map *phys_maps;

	முழு freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	எனில்(first) {
		memset(&minix_kerninfo, 0, அளவு(minix_kerninfo));
		எனில்(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		எனில்(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		போது(phys_maps != வெற்று){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

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

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	போது(phys_maps != வெற்று){
		எனில்(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			திரும்பு OK;
		}
		phys_maps = phys_maps->next;
	}

	திரும்பு EINVAL;
}

முழு arch_phys_map_reply(மாறிலி முழு index, மாறிலி vir_bytes addr)
{
	kern_phys_map *phys_maps;

	எனில்(index == first_um_idx) {
		u32_t usermapped_offset;
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
		ASSIGN(arm_frclock);
		ASSIGN(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		திரும்பு OK;
	}

	எனில் (index == usermapped_index) {
		திரும்பு OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	போது(phys_maps != வெற்று){
		எனில்(phys_maps->index == index){
			ப்ரதிஜ்ஞா(phys_maps->cb != வெற்று);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			திரும்பு OK;
		}
		phys_maps = phys_maps->next;
	}

	திரும்பு EINVAL;
}

முழு arch_enable_paging(கட்டமைப்பு proc * caller)
{
	kern_phys_map *phys_maps;
	ப்ரதிஜ்ஞா(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	போது(phys_maps != வெற்று){
		ப்ரதிஜ்ஞா(phys_maps->cb != வெற்று);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	திரும்பு OK;
}

வெறுமை release_address_space(கட்டமைப்பு proc *pr)
{
	pr->p_seg.p_ttbr_v = வெற்று;
	barrier();
}



/*
 * Request a physical mapping
 */
முழு kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       முழு vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	ப்ரதிஜ்ஞா(base_address != 0);
	ப்ரதிஜ்ஞா(io_size % ARM_PAGE_SIZE == 0);
	ப்ரதிஜ்ஞா(cb != வெற்று);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = வெற்று;
	

	எனில் (kern_phys_map_head == வெற்று){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = வெற்று;
	} இல்லையெனில் {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	திரும்பு 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
முழு kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	திரும்பு 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
முழு kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	முழு vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	திரும்பு kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

