
#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "kernel/proc.h"
#അന്തര്ഭാവ "kernel/vm.h"

#അന്തര്ഭാവ <machine/vm.h>

#അന്തര്ഭാവ <minix/type.h>
#അന്തര്ഭാവ <minix/board.h>
#അന്തര്ഭാവ <minix/syslib.h>
#അന്തര്ഭാവ <minix/cpufeature.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ <machine/vm.h>

#അന്തര്ഭാവ "arch_proto.h"
#അന്തര്ഭാവ "kernel/proto.h"
#അന്തര്ഭാവ "kernel/debug.h"
#അന്തര്ഭാവ "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
സ്ഥിതം പൂർണ്ണസംഖ്യ nfreepdes = 0;
#define MAXFREEPDES	2
സ്ഥിതം പൂർണ്ണസംഖ്യ freepdes[MAXFREEPDES];

സ്ഥിതം u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
സ്ഥിതം kern_phys_map *kern_phys_map_head;

ശൂന്യം mem_clear_mapcache(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ i;
	വേണ്ടി(i = 0; i < nfreepdes; i++) {
		ഘടന proc *ptproc = get_cpulocal_var(ptproc);
		പൂർണ്ണസംഖ്യ pde = freepdes[i];
		u32_t *ptv;
		പ്രതിജ്ഞാ(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		പ്രതിജ്ഞാ(ptv);
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
സ്ഥിതം phys_bytes createpde(
	സ്ഥിരം ഘടന proc *pr,	/* Requested process, NULL for physical. */
	സ്ഥിരം phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	പൂർണ്ണസംഖ്യ free_pde_idx,	/* index of the free slot to use */
	പൂർണ്ണസംഖ്യ *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	പൂർണ്ണസംഖ്യ pde;

	പ്രതിജ്ഞാ(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	പ്രതിജ്ഞാ(pde >= 0 && pde < 4096);

	എങ്കിൽ(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		തിരികെ linaddr;
	}

	എങ്കിൽ(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		പ്രതിജ്ഞാ(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} അല്ലെങ്കിൽ {
		/* Requested address is physical. Make up the PDE entry. */
		പ്രതിജ്ഞാ (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

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
	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	എങ്കിൽ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
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
	തിരികെ ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
സ്ഥിതം പൂർണ്ണസംഖ്യ check_resumed_caller(ഘടന proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	എങ്കിൽ (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		പ്രതിജ്ഞാ(caller->p_vmrequest.vmresult != VMSUSPEND);
		തിരികെ caller->p_vmrequest.vmresult;
	}

	തിരികെ OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
സ്ഥിതം പൂർണ്ണസംഖ്യ lin_lin_copy(ഘടന proc *srcproc, vir_bytes srclinaddr, 
	ഘടന proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc));
	പ്രതിജ്ഞാ(get_cpulocal_var(proc_ptr));
	പ്രതിജ്ഞാ(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	പ്രതിജ്ഞാ(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	എങ്കിൽ(srcproc) പ്രതിജ്ഞാ(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	എങ്കിൽ(dstproc) പ്രതിജ്ഞാ(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	പ്രതിജ്ഞാ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	എങ്കിൽ(srcproc) പ്രതിജ്ഞാ(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	എങ്കിൽ(dstproc) പ്രതിജ്ഞാ(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	ആകുമ്പോൾ(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		പൂർണ്ണസംഖ്യ changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		എങ്കിൽ (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		എങ്കിൽ (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		എങ്കിൽ(changed)
			reload_ttbr0();

		/* Check for overflow. */
		എങ്കിൽ (srcptr + chunk < srcptr) തിരികെ EFAULT_SRC;
		എങ്കിൽ (dstptr + chunk < dstptr) തിരികെ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		എങ്കിൽ(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			എങ്കിൽ(addr >= src_aligned && addr < (srcptr + chunk)) {
				തിരികെ EFAULT_SRC;
			}
			എങ്കിൽ(addr >= dst_aligned && addr < (dstptr + chunk)) {
				തിരികെ EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			തിരികെ EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	എങ്കിൽ(srcproc) പ്രതിജ്ഞാ(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	എങ്കിൽ(dstproc) പ്രതിജ്ഞാ(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	പ്രതിജ്ഞാ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	തിരികെ OK;
}

സ്ഥിതം u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	പൂർണ്ണസംഖ്യ r;

	എങ്കിൽ((r=lin_lin_copy(ശൂന്യ, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, വലുപ്പം(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	തിരികെ v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  രജിസ്റ്റർ ഘടന proc *rp,		/* pointer to proc table entry for process */
  പൂർണ്ണസംഖ്യ seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	എങ്കിൽ(vm_lookup(rp, vir_addr, &phys, ശൂന്യ) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} അല്ലെങ്കിൽ {
		എങ്കിൽ(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	എങ്കിൽ(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		തിരികെ 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	എങ്കിൽ(bytes > 0 && vm_lookup_range(rp, vir_addr, ശൂന്യ, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		തിരികെ 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	പ്രതിജ്ഞാ(phys);

	തിരികെ phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ vm_lookup(സ്ഥിരം ഘടന proc *proc, സ്ഥിരം vir_bytes മായാ,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *വേര്, *pt;
	പൂർണ്ണസംഖ്യ pde, pte;
	u32_t pde_v, pte_v;

	പ്രതിജ്ഞാ(proc);
	പ്രതിജ്ഞാ(physical);
	പ്രതിജ്ഞാ(!isemptyp(proc));
	പ്രതിജ്ഞാ(HASPT(proc));

	/* Retrieve page directory entry. */
	വേര് = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	പ്രതിജ്ഞാ(!((u32_t) വേര് % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(മായാ);
	പ്രതിജ്ഞാ(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (വേര് + pde));

	എങ്കിൽ(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		തിരികെ EFAULT;
	}

	എങ്കിൽ(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		എങ്കിൽ(ptent) *ptent = pde_v;
		*physical += മായാ & ARM_VM_OFFSET_MASK_1MB;
	} അല്ലെങ്കിൽ  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		പ്രതിജ്ഞാ(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(മായാ);
		പ്രതിജ്ഞാ(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		എങ്കിൽ(!(pte_v & ARM_VM_PTE_PRESENT)) {
			തിരികെ EFAULT;
		}

		എങ്കിൽ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += മായാ % ARM_PAGE_SIZE;
	}

	തിരികെ OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(സ്ഥിരം ഘടന proc *proc, vir_bytes vir_addr,
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

	പ്രതിജ്ഞാ(proc);
	പ്രതിജ്ഞാ(bytes > 0);
	പ്രതിജ്ഞാ(HASPT(proc));

	/* Look up the first page. */
	എങ്കിൽ (vm_lookup(proc, vir_addr, &phys, ശൂന്യ) != OK)
		തിരികെ 0;

	എങ്കിൽ (phys_addr != ശൂന്യ)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ആകുമ്പോൾ (len < bytes) {
		എങ്കിൽ (vm_lookup(proc, vir_addr, &phys, ശൂന്യ) != OK)
			നിർത്തുക;

		എങ്കിൽ (next_phys != phys)
			നിർത്തുക;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	തിരികെ MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ vm_check_range(ഘടന proc *caller, ഘടന proc *target,
	vir_bytes vir_addr, size_t bytes, പൂർണ്ണസംഖ്യ writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	പൂർണ്ണസംഖ്യ r;

	എങ്കിൽ ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		തിരികെ r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	തിരികെ VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ vm_memset(ഘടന proc* caller, endpoint_t who, phys_bytes ph, പൂർണ്ണസംഖ്യ c,
	phys_bytes എണ്ണം)
{
	u32_t pattern;
	ഘടന proc *whoptr = ശൂന്യ;
	phys_bytes cur_ph = ph;
	phys_bytes ഇടത് = എണ്ണം;
	phys_bytes ptr, chunk, pfa = 0;
	പൂർണ്ണസംഖ്യ new_ttbr, r = OK;

	എങ്കിൽ ((r = check_resumed_caller(caller)) != OK)
		തിരികെ r;

	/* NONE for physical, otherwise virtual */
	എങ്കിൽ (who != NONE && !(whoptr = endpoint_lookup(who)))
		തിരികെ ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	പ്രതിജ്ഞാ(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	ആകുമ്പോൾ (ഇടത് > 0) {
		new_ttbr = 0;
		chunk = ഇടത്;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		എങ്കിൽ (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		എങ്കിൽ ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			എങ്കിൽ (whoptr) {
				vm_suspend(caller, whoptr, ph, എണ്ണം,
						   VMSTYPE_KERNELCALL, 1);
				പ്രതിജ്ഞാ(catch_pagefaults);
				catch_pagefaults = 0;
				തിരികെ VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		ഇടത് -= chunk;
	}

	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	പ്രതിജ്ഞാ(catch_pagefaults);
	catch_pagefaults = 0;

	തിരികെ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ virtual_copy_f(
  ഘടന proc * caller,
  ഘടന vir_addr *src_addr,		/* source virtual address */
  ഘടന vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  പൂർണ്ണസംഖ്യ vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ഘടന vir_addr *vir_addr[2];	/* virtual source and destination address */
  പൂർണ്ണസംഖ്യ i, r;
  ഘടന proc *procs[2];

  പ്രതിജ്ഞാ((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  എങ്കിൽ (bytes <= 0) തിരികെ(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  വേണ്ടി (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	പൂർണ്ണസംഖ്യ proc_nr;
	ഘടന proc *p;

	എങ്കിൽ(proc_e == NONE) {
		p = ശൂന്യ;
	} അല്ലെങ്കിൽ {
		എങ്കിൽ(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			തിരികെ ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  എങ്കിൽ ((r = check_resumed_caller(caller)) != OK)
	തിരികെ r;

  എങ്കിൽ((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	പൂർണ്ണസംഖ്യ writeflag;
  	ഘടന proc *target = ശൂന്യ;
  	phys_bytes lin;
  	എങ്കിൽ(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	എങ്കിൽ(!vmcheck || !caller) {
    		തിരികെ r;
  	}

  	എങ്കിൽ(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} അല്ലെങ്കിൽ എങ്കിൽ(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} അല്ലെങ്കിൽ {
  		panic("r strange: %d",  r);
  	}

	പ്രതിജ്ഞാ(caller);
	പ്രതിജ്ഞാ(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	തിരികെ VMSUSPEND;
  }

  തിരികെ OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ data_copy(സ്ഥിരം endpoint_t from_proc, സ്ഥിരം vir_bytes from_addr,
	സ്ഥിരം endpoint_t to_proc, സ്ഥിരം vir_bytes to_addr,
	size_t bytes)
{
  ഘടന vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  പ്രതിജ്ഞാ(src.proc_nr_e != NONE);
  പ്രതിജ്ഞാ(dst.proc_nr_e != NONE);

  തിരികെ virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ data_copy_vmcheck(ഘടന proc * caller,
	സ്ഥിരം endpoint_t from_proc, സ്ഥിരം vir_bytes from_addr,
	സ്ഥിരം endpoint_t to_proc, സ്ഥിരം vir_bytes to_addr,
	size_t bytes)
{
  ഘടന vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  പ്രതിജ്ഞാ(src.proc_nr_e != NONE);
  പ്രതിജ്ഞാ(dst.proc_nr_e != NONE);

  തിരികെ virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

ശൂന്യം memory_init(ശൂന്യം)
{
	പ്രതിജ്ഞാ(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	പ്രതിജ്ഞാ(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	പ്രതിജ്ഞാ(nfreepdes == 2);
	പ്രതിജ്ഞാ(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
ശൂന്യം arch_proc_init(ഘടന proc *pr, സ്ഥിരം u32_t ip, സ്ഥിരം u32_t sp,
	സ്ഥിരം u32_t ps_str, അക്ഷരം *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

സ്ഥിതം പൂർണ്ണസംഖ്യ usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
ബാഹ്യ അക്ഷരം usermapped_start, usermapped_end, usermapped_nonglo_start;

പൂർണ്ണസംഖ്യ arch_phys_map(സ്ഥിരം പൂർണ്ണസംഖ്യ index,
			phys_bytes *addr,
			phys_bytes *len,
			പൂർണ്ണസംഖ്യ *flags)
{
	സ്ഥിതം പൂർണ്ണസംഖ്യ first = 1;
	kern_phys_map *phys_maps;

	പൂർണ്ണസംഖ്യ freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	എങ്കിൽ(first) {
		memset(&minix_kerninfo, 0, വലുപ്പം(minix_kerninfo));
		എങ്കിൽ(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		എങ്കിൽ(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		ആകുമ്പോൾ(phys_maps != ശൂന്യ){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	എങ്കിൽ(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		തിരികെ OK;
	}
	അല്ലെങ്കിൽ എങ്കിൽ(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		തിരികെ OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	ആകുമ്പോൾ(phys_maps != ശൂന്യ){
		എങ്കിൽ(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			തിരികെ OK;
		}
		phys_maps = phys_maps->next;
	}

	തിരികെ EINVAL;
}

പൂർണ്ണസംഖ്യ arch_phys_map_reply(സ്ഥിരം പൂർണ്ണസംഖ്യ index, സ്ഥിരം vir_bytes addr)
{
	kern_phys_map *phys_maps;

	എങ്കിൽ(index == first_um_idx) {
		u32_t usermapped_offset;
		പ്രതിജ്ഞാ(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (ശൂന്യം *) ((u32_t)ptr + usermapped_offset)
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

		തിരികെ OK;
	}

	എങ്കിൽ (index == usermapped_index) {
		തിരികെ OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	ആകുമ്പോൾ(phys_maps != ശൂന്യ){
		എങ്കിൽ(phys_maps->index == index){
			പ്രതിജ്ഞാ(phys_maps->cb != ശൂന്യ);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			തിരികെ OK;
		}
		phys_maps = phys_maps->next;
	}

	തിരികെ EINVAL;
}

പൂർണ്ണസംഖ്യ arch_enable_paging(ഘടന proc * caller)
{
	kern_phys_map *phys_maps;
	പ്രതിജ്ഞാ(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	ആകുമ്പോൾ(phys_maps != ശൂന്യ){
		പ്രതിജ്ഞാ(phys_maps->cb != ശൂന്യ);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	തിരികെ OK;
}

ശൂന്യം release_address_space(ഘടന proc *pr)
{
	pr->p_seg.p_ttbr_v = ശൂന്യ;
	barrier();
}



/*
 * Request a physical mapping
 */
പൂർണ്ണസംഖ്യ kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       പൂർണ്ണസംഖ്യ vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	പ്രതിജ്ഞാ(base_address != 0);
	പ്രതിജ്ഞാ(io_size % ARM_PAGE_SIZE == 0);
	പ്രതിജ്ഞാ(cb != ശൂന്യ);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = ശൂന്യ;
	

	എങ്കിൽ (kern_phys_map_head == ശൂന്യ){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = ശൂന്യ;
	} അല്ലെങ്കിൽ {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	തിരികെ 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
പൂർണ്ണസംഖ്യ kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	തിരികെ 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
പൂർണ്ണസംഖ്യ kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	പൂർണ്ണസംഖ്യ vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	തിരികെ kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

