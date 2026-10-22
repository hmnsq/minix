
#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "kernel/proc.h"
#अन्तर्भाव "kernel/vm.h"

#अन्तर्भाव <machine/vm.h>

#अन्तर्भाव <minix/type.h>
#अन्तर्भाव <minix/board.h>
#अन्तर्भाव <minix/syslib.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdlib.h>

#अन्तर्भाव <machine/vm.h>

#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव "kernel/proto.h"
#अन्तर्भाव "kernel/debug.h"
#अन्तर्भाव "bsp_timer.h"


#परिभाषय HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
स्थिर पूर्णाङ्क nfreepdes = 0;
#परिभाषय MAXFREEPDES	2
स्थिर पूर्णाङ्क freepdes[MAXFREEPDES];

स्थिर u32_t phys_get32(phys_अष्टकानि v);

/* list of requested physical mapping */
स्थिर kern_phys_map *kern_phys_map_शीर्ष;

शून्य mem_clear_mapcache(शून्य)
{
	पूर्णाङ्क i;
	प्रति(i = 0; i < nfreepdes; i++) {
		संरचना proc *ptproc = get_cpulocal_var(ptproc);
		पूर्णाङ्क pde = freepdes[i];
		u32_t *ptv;
		प्रतिज्ञा(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		प्रतिज्ञा(ptv);
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
स्थिर phys_अष्टकानि createpde(
	ध्रुव संरचना proc *pr,	/* Requested process, NULL for physical. */
	ध्रुव phys_अष्टकानि linaddr,/* Address after segment translation. */
	phys_अष्टकानि *अष्टकानि,	/* Size of chunk, function may truncate it. */
	पूर्णाङ्क free_pde_idx,	/* index of the free slot to use */
	पूर्णाङ्क *परिवर्तितम्		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_अष्टकानि विस्थापनम्;
	पूर्णाङ्क pde;

	प्रतिज्ञा(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	प्रतिज्ञा(pde >= 0 && pde < 4096);

	यदि(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		प्रत्यागम linaddr;
	}

	यदि(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		प्रतिज्ञा(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} अन्यथा {
		/* Requested address is physical. Make up the PDE entry. */
		प्रतिज्ञा (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_अन्तः);

		/*स्मृतिः*/
		pdeval = (linaddr & ARM_VM_SECTION_आवरणम्)
			| ARM_VM_SECTION
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED
			| ARM_VM_SECTION_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	यदि(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*परिवर्तितम् = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	विस्थापनम् = linaddr & ARM_VM_विस्थापनम्_आवरणम्_1_MB; /* Offset in 1MB window. */
	*अष्टकानि = MIN(*अष्टकानि, ARM_SECTION_आकार - विस्थापनम्);

	/* Return the linear address of the start of the new mapping. */
	प्रत्यागम ARM_SECTION_आकार*pde + विस्थापनम्;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
स्थिर पूर्णाङ्क check_resumed_caller(संरचना proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	यदि (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		प्रतिज्ञा(caller->p_vmrequest.vmresult != VMSUSPEND);
		प्रत्यागम caller->p_vmrequest.vmresult;
	}

	प्रत्यागम OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
स्थिर पूर्णाङ्क lin_lin_copy(संरचना proc *srcproc, vir_अष्टकानि srclinaddr, 
	संरचना proc *dstproc, vir_अष्टकानि dstlinaddr, vir_अष्टकानि अष्टकानि)
{
	u32_t addr;
	proc_nr_t procslot;

	प्रतिज्ञा(get_cpulocal_var(ptproc));
	प्रतिज्ञा(get_cpulocal_var(proc_ptr));
	प्रतिज्ञा(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	प्रतिज्ञा(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	यदि(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	यदि(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	प्रतिज्ञा(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	यदि(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	यदि(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	यावत्(अष्टकानि > 0) {
		phys_अष्टकानि srcptr, dstptr;
		vir_अष्टकानि chunk = अष्टकानि;
		पूर्णाङ्क परिवर्तितम् = 0;

#यदि_परिभाषितम् CONFIG_SMP
		अचिह्नित cpu = cpuid;

		यदि (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			परिवर्तितम् = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		यदि (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			परिवर्तितम् = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#यदि_अन्तः

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &परिवर्तितम्);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &परिवर्तितम्);
		यदि(परिवर्तितम्)
			reload_ttbr0();

		/* Check for overflow. */
		यदि (srcptr + chunk < srcptr) प्रत्यागम EFAULT_SRC;
		यदि (dstptr + chunk < dstptr) प्रत्यागम EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		यदि(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_अष्टकानि src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			यदि(addr >= src_aligned && addr < (srcptr + chunk)) {
				प्रत्यागम EFAULT_SRC;
			}
			यदि(addr >= dst_aligned && addr < (dstptr + chunk)) {
				प्रत्यागम EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			प्रत्यागम EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		अष्टकानि -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	यदि(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	यदि(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	प्रतिज्ञा(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	प्रत्यागम OK;
}

स्थिर u32_t phys_get32(phys_अष्टकानि addr)
{
	u32_t v;
	पूर्णाङ्क r;

	यदि((r=lin_lin_copy(रिक्त, addr, 
		proc_addr(SYSTEM), (phys_अष्टकानि) &v, परिमाण(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	प्रत्यागम v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_अष्टकानि umap_virtual(
  पञ्जिका संरचना proc *rp,		/* pointer to proc table entry for process */
  पूर्णाङ्क seg,				/* T, D, or S segment */
  vir_अष्टकानि vir_addr,			/* virtual address in bytes within the seg */
  vir_अष्टकानि अष्टकानि			/* # of bytes to be copied */
)
{
	phys_अष्टकानि phys = 0;

	यदि(vm_lookup(rp, vir_addr, &phys, रिक्त) != OK) {
		प्रारूप_लिख("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} अन्यथा {
		यदि(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	यदि(phys == 0) {
		प्रारूप_लिख("SYSTEM:umap_virtual: lookup failed\n");
		प्रत्यागम 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	यदि(अष्टकानि > 0 && vm_lookup_range(rp, vir_addr, रिक्त, अष्टकानि) != अष्टकानि) {
		प्रारूप_लिख("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, अष्टकानि, vir_addr, vir_addr);
		प्रत्यागम 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	प्रतिज्ञा(phys);

	प्रत्यागम phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
पूर्णाङ्क vm_lookup(ध्रुव संरचना proc *proc, ध्रुव vir_अष्टकानि आभास,
 phys_अष्टकानि *physical, u32_t *ptent)
{
	u32_t *मूलम्, *pt;
	पूर्णाङ्क pde, pte;
	u32_t pde_v, pte_v;

	प्रतिज्ञा(proc);
	प्रतिज्ञा(physical);
	प्रतिज्ञा(!isemptyp(proc));
	प्रतिज्ञा(HASPT(proc));

	/* Retrieve page directory entry. */
	मूलम् = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_आवरणम्);
	प्रतिज्ञा(!((u32_t) मूलम् % ARM_PAGEDIR_आकार));
	pde = ARM_VM_PDE(आभास);
	प्रतिज्ञा(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (मूलम् + pde));

	यदि(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		प्रत्यागम EFAULT;
	}

	यदि(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_आवरणम्;
		यदि(ptent) *ptent = pde_v;
		*physical += आभास & ARM_VM_विस्थापनम्_आवरणम्_1_MB;
	} अन्यथा  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_आवरणम्);
		प्रतिज्ञा(!((u32_t) pt % ARM_PAGETABLE_आकार));
		pte = ARM_VM_PTE(आभास);
		प्रतिज्ञा(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		यदि(!(pte_v & ARM_VM_PTE_PRESENT)) {
			प्रत्यागम EFAULT;
		}

		यदि(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_आवरणम्;
		*physical += आभास % ARM_PAGE_आकार;
	}

	प्रत्यागम OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ध्रुव संरचना proc *proc, vir_अष्टकानि vir_addr,
	phys_अष्टकानि *phys_addr, size_t अष्टकानि)
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
	phys_अष्टकानि phys, अग्र_phys;
	size_t len;

	प्रतिज्ञा(proc);
	प्रतिज्ञा(अष्टकानि > 0);
	प्रतिज्ञा(HASPT(proc));

	/* Look up the first page. */
	यदि (vm_lookup(proc, vir_addr, &phys, रिक्त) != OK)
		प्रत्यागम 0;

	यदि (phys_addr != रिक्त)
		*phys_addr = phys;

	len = ARM_PAGE_आकार - (vir_addr % ARM_PAGE_आकार);
	vir_addr += len;
	अग्र_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	यावत् (len < अष्टकानि) {
		यदि (vm_lookup(proc, vir_addr, &phys, रिक्त) != OK)
			भङ्ग;

		यदि (अग्र_phys != phys)
			भङ्ग;

		len += ARM_PAGE_आकार;
		vir_addr += ARM_PAGE_आकार;
		अग्र_phys += ARM_PAGE_आकार;
	}

	/* We might now have overshot the requested length somewhat. */
	प्रत्यागम MIN(अष्टकानि, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
पूर्णाङ्क vm_check_range(संरचना proc *caller, संरचना proc *target,
	vir_अष्टकानि vir_addr, size_t अष्टकानि, पूर्णाङ्क writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	पूर्णाङ्क r;

	यदि ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		प्रत्यागम r;

	vm_suspend(caller, target, vir_addr, अष्टकानि, VMSTYPE_KERNELCALL,
		writeflag);

	प्रत्यागम VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
पूर्णाङ्क vm_memset(संरचना proc* caller, endpoint_t who, phys_अष्टकानि ph, पूर्णाङ्क c,
	phys_अष्टकानि गणनम्)
{
	u32_t आदर्शः;
	संरचना proc *whoptr = रिक्त;
	phys_अष्टकानि cur_ph = ph;
	phys_अष्टकानि वामः = गणनम्;
	phys_अष्टकानि ptr, chunk, pfa = 0;
	पूर्णाङ्क new_ttbr, r = OK;

	यदि ((r = check_resumed_caller(caller)) != OK)
		प्रत्यागम r;

	/* NONE for physical, otherwise virtual */
	यदि (who != NONE && !(whoptr = endpoint_lookup(who)))
		प्रत्यागम ESRCH;

	c &= 0xFF;
	आदर्शः = c | (c << 8) | (c << 16) | (c << 24);

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	प्रतिज्ञा(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	यावत् (वामः > 0) {
		new_ttbr = 0;
		chunk = वामः;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		यदि (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		यदि ((pfa = phys_memset(ptr, आदर्शः, chunk))) {

			/* If a process pagefaults, VM may help out */
			यदि (whoptr) {
				vm_suspend(caller, whoptr, ph, गणनम्,
						   VMSTYPE_KERNELCALL, 1);
				प्रतिज्ञा(catch_pagefaults);
				catch_pagefaults = 0;
				प्रत्यागम VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		वामः -= chunk;
	}

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	प्रतिज्ञा(catch_pagefaults);
	catch_pagefaults = 0;

	प्रत्यागम OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
पूर्णाङ्क virtual_copy_f(
  संरचना proc * caller,
  संरचना vir_addr *src_addr,		/* source virtual address */
  संरचना vir_addr *dst_addr,		/* destination virtual address */
  vir_अष्टकानि अष्टकानि,			/* # of bytes to copy  */
  पूर्णाङ्क vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  संरचना vir_addr *vir_addr[2];	/* virtual source and destination address */
  पूर्णाङ्क i, r;
  संरचना proc *procs[2];

  प्रतिज्ञा((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  यदि (अष्टकानि <= 0) प्रत्यागम(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  प्रति (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	पूर्णाङ्क proc_nr;
	संरचना proc *p;

	यदि(proc_e == NONE) {
		p = रिक्त;
	} अन्यथा {
		यदि(!isokendpt(proc_e, &proc_nr)) {
			प्रारूप_लिख("virtual_copy: no reasonable endpoint\n");
			प्रत्यागम ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  यदि ((r = check_resumed_caller(caller)) != OK)
	प्रत्यागम r;

  यदि((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->विस्थापनम्,
  	procs[_DST_], vir_addr[_DST_]->विस्थापनम्, अष्टकानि)) != OK) {
	पूर्णाङ्क writeflag;
  	संरचना proc *target = रिक्त;
  	phys_अष्टकानि lin;
  	यदि(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	यदि(!vmcheck || !caller) {
    		प्रत्यागम r;
  	}

  	यदि(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->विस्थापनम्;
  		target = procs[_SRC_];
		writeflag = 0;
  	} अन्यथा यदि(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->विस्थापनम्;
  		target = procs[_DST_];
		writeflag = 1;
  	} अन्यथा {
  		panic("r strange: %d",  r);
  	}

	प्रतिज्ञा(caller);
	प्रतिज्ञा(target);

	vm_suspend(caller, target, lin, अष्टकानि, VMSTYPE_KERNELCALL, writeflag);
	प्रत्यागम VMSUSPEND;
  }

  प्रत्यागम OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
पूर्णाङ्क दत्त_copy(ध्रुव endpoint_t from_proc, ध्रुव vir_अष्टकानि from_addr,
	ध्रुव endpoint_t गन्तव्यम्_proc, ध्रुव vir_अष्टकानि गन्तव्यम्_addr,
	size_t अष्टकानि)
{
  संरचना vir_addr src, dst;

  src.विस्थापनम् = from_addr;
  dst.विस्थापनम् = गन्तव्यम्_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = गन्तव्यम्_proc;
  प्रतिज्ञा(src.proc_nr_e != NONE);
  प्रतिज्ञा(dst.proc_nr_e != NONE);

  प्रत्यागम virtual_copy(&src, &dst, अष्टकानि);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
पूर्णाङ्क दत्त_copy_vmcheck(संरचना proc * caller,
	ध्रुव endpoint_t from_proc, ध्रुव vir_अष्टकानि from_addr,
	ध्रुव endpoint_t गन्तव्यम्_proc, ध्रुव vir_अष्टकानि गन्तव्यम्_addr,
	size_t अष्टकानि)
{
  संरचना vir_addr src, dst;

  src.विस्थापनम् = from_addr;
  dst.विस्थापनम् = गन्तव्यम्_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = गन्तव्यम्_proc;
  प्रतिज्ञा(src.proc_nr_e != NONE);
  प्रतिज्ञा(dst.proc_nr_e != NONE);

  प्रत्यागम virtual_copy_vmcheck(caller, &src, &dst, अष्टकानि);
}

शून्य स्मृतिः_init(शून्य)
{
	प्रतिज्ञा(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	प्रतिज्ञा(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	प्रतिज्ञा(nfreepdes == 2);
	प्रतिज्ञा(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
शून्य arch_proc_init(संरचना proc *pr, ध्रुव u32_t ip, ध्रुव u32_t sp,
	ध्रुव u32_t ps_str, वर्ण *name)
{
	arch_proc_reset(pr);
	वाक्यं_प्रतिलिख(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

स्थिर पूर्णाङ्क usermapped_glo_सूचकांक = -1,
	usermapped_सूचकांक = -1, first_um_idx = -1;


/* defined in kernel.lds */
बाह्य वर्ण usermapped_start, usermapped_अन्तः, usermapped_nonglo_start;

पूर्णाङ्क arch_phys_map(ध्रुव पूर्णाङ्क सूचकांक,
			phys_अष्टकानि *addr,
			phys_अष्टकानि *len,
			पूर्णाङ्क *flags)
{
	स्थिर पूर्णाङ्क first = 1;
	kern_phys_map *phys_maps;

	पूर्णाङ्क freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	यदि(first) {
		स्मृतिं_पूरय(&minix_kerninfo, 0, परिमाण(minix_kerninfo));
		यदि(glo_len > 0) {
			usermapped_glo_सूचकांक = freeidx++;
		}

		usermapped_सूचकांक = freeidx++;
		first_um_idx = usermapped_सूचकांक;
		यदि(usermapped_glo_सूचकांक != -1)
			first_um_idx = usermapped_glo_सूचकांक;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_शीर्ष;
		यावत्(phys_maps != रिक्त){
			phys_maps->सूचकांक = freeidx++;
			phys_maps = phys_maps->अग्र;
		}

	}

	यदि(सूचकांक == usermapped_glo_सूचकांक) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		प्रत्यागम OK;
	}
	अन्यथा यदि(सूचकांक == usermapped_सूचकांक) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_अन्तः -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		प्रत्यागम OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_शीर्ष;
	यावत्(phys_maps != रिक्त){
		यदि(phys_maps->सूचकांक == सूचकांक){
			*addr = phys_maps->addr;
			*len =  phys_maps->आकार;
			*flags = phys_maps->vm_flags;
			प्रत्यागम OK;
		}
		phys_maps = phys_maps->अग्र;
	}

	प्रत्यागम EINVAL;
}

पूर्णाङ्क arch_phys_map_प्रत्युत्तरम्(ध्रुव पूर्णाङ्क सूचकांक, ध्रुव vir_अष्टकानि addr)
{
	kern_phys_map *phys_maps;

	यदि(सूचकांक == first_um_idx) {
		u32_t usermapped_विस्थापनम्;
		प्रतिज्ञा(addr > (u32_t) &usermapped_start);
		usermapped_विस्थापनम् = addr - (u32_t) &usermapped_start;
#परिभाषय FIXEDPTR(ptr) (शून्य *) ((u32_t)ptr + usermapped_विस्थापनम्)
#परिभाषय FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#परिभाषय नियोजय(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		नियोजय(kinfo);
		नियोजय(machine);
		नियोजय(kmessages);
		नियोजय(loadinfo);
		नियोजय(kuserinfo);
		नियोजय(arm_frclock);
		नियोजय(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_अष्टकानि) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		प्रत्यागम OK;
	}

	यदि (सूचकांक == usermapped_सूचकांक) {
		प्रत्यागम OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_शीर्ष;
	यावत्(phys_maps != रिक्त){
		यदि(phys_maps->सूचकांक == सूचकांक){
			प्रतिज्ञा(phys_maps->cb != रिक्त);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			प्रत्यागम OK;
		}
		phys_maps = phys_maps->अग्र;
	}

	प्रत्यागम EINVAL;
}

पूर्णाङ्क arch_enable_paging(संरचना proc * caller)
{
	kern_phys_map *phys_maps;
	प्रतिज्ञा(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_स्थानाङ्कः_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_शीर्ष;
	यावत्(phys_maps != रिक्त){
		प्रतिज्ञा(phys_maps->cb != रिक्त);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->अग्र;
	}

	प्रत्यागम OK;
}

शून्य release_स्थानाङ्कः_space(संरचना proc *pr)
{
	pr->p_seg.p_ttbr_v = रिक्त;
	barrier();
}



/*
 * Request a physical mapping
 */
पूर्णाङ्क kern_req_phys_map( phys_अष्टकानि आधारः_स्थानाङ्कः, vir_अष्टकानि io_आकार, 
		       पूर्णाङ्क vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_अष्टकानि id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	प्रतिज्ञा(आधारः_स्थानाङ्कः != 0);
	प्रतिज्ञा(io_आकार % ARM_PAGE_आकार == 0);
	प्रतिज्ञा(cb != रिक्त);

	priv->addr  = आधारः_स्थानाङ्कः;
	priv->आकार  = io_आकार;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->सूचकांक = -1;
	priv->अग्र = रिक्त;
	

	यदि (kern_phys_map_शीर्ष == रिक्त){
		/* keep a list of items this is the first one */
		kern_phys_map_शीर्ष = priv;
		kern_phys_map_शीर्ष->अग्र = रिक्त;
	} अन्यथा {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->अग्र = kern_phys_map_शीर्ष;
		/* replace the head */
		kern_phys_map_शीर्ष = priv;
	}
	प्रत्यागम 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
पूर्णाङ्क kern_phys_map_mapped_ptr(vir_अष्टकानि id, phys_अष्टकानि स्थानाङ्कः){
	*((vir_अष्टकानि*)id) = स्थानाङ्कः;
	प्रत्यागम 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
पूर्णाङ्क kern_phys_map_ptr(
	phys_अष्टकानि आधारः_स्थानाङ्कः, 
	vir_अष्टकानि io_आकार, 
	पूर्णाङ्क vm_flags, 
	kern_phys_map * priv,
	vir_अष्टकानि ptr)
{
	प्रत्यागम kern_req_phys_map(आधारः_स्थानाङ्कः,io_आकार,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

