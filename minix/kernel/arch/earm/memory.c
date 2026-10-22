
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


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
स्थिर पूर्णांक nfreepdes = 0;
#define MAXFREEPDES	2
स्थिर पूर्णांक freepdes[MAXFREEPDES];

स्थिर u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
स्थिर kern_phys_map *kern_phys_map_head;

शून्य mem_clear_mapcache(शून्य)
{
	पूर्णांक i;
	साठी(i = 0; i < nfreepdes; i++) {
		संरचना proc *ptproc = get_cpulocal_var(ptproc);
		पूर्णांक pde = freepdes[i];
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
स्थिर phys_bytes createpde(
	स्थिरांक संरचना proc *pr,	/* Requested process, NULL for physical. */
	स्थिरांक phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	पूर्णांक free_pde_idx,	/* index of the free slot to use */
	पूर्णांक *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	पूर्णांक pde;

	प्रतिज्ञा(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	प्रतिज्ञा(pde >= 0 && pde < 4096);

	जर(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		परत linaddr;
	}

	जर(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		प्रतिज्ञा(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} अन्यथा {
		/* Requested address is physical. Make up the PDE entry. */
		प्रतिज्ञा (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

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
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	जर(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
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
	परत ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
स्थिर पूर्णांक check_resumed_caller(संरचना proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	जर (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		प्रतिज्ञा(caller->p_vmrequest.vmresult != VMSUSPEND);
		परत caller->p_vmrequest.vmresult;
	}

	परत OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
स्थिर पूर्णांक lin_lin_copy(संरचना proc *srcproc, vir_bytes srclinaddr, 
	संरचना proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	प्रतिज्ञा(get_cpulocal_var(ptproc));
	प्रतिज्ञा(get_cpulocal_var(proc_ptr));
	प्रतिज्ञा(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	प्रतिज्ञा(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	जर(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	जर(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	प्रतिज्ञा(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	जर(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	जर(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	पर्यंत(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		पूर्णांक changed = 0;

#ifdef CONFIG_SMP
		अचिन्हित cpu = cpuid;

		जर (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		जर (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		जर(changed)
			reload_ttbr0();

		/* Check for overflow. */
		जर (srcptr + chunk < srcptr) परत EFAULT_SRC;
		जर (dstptr + chunk < dstptr) परत EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		जर(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			जर(addr >= src_aligned && addr < (srcptr + chunk)) {
				परत EFAULT_SRC;
			}
			जर(addr >= dst_aligned && addr < (dstptr + chunk)) {
				परत EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			परत EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	जर(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	जर(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	प्रतिज्ञा(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	परत OK;
}

स्थिर u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	पूर्णांक r;

	जर((r=lin_lin_copy(रिक्त, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, आकार(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	परत v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  नोंदणी संरचना proc *rp,		/* pointer to proc table entry for process */
  पूर्णांक seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	जर(vm_lookup(rp, vir_addr, &phys, रिक्त) != OK) {
		म_लिखो("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} अन्यथा {
		जर(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	जर(phys == 0) {
		म_लिखो("SYSTEM:umap_virtual: lookup failed\n");
		परत 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	जर(bytes > 0 && vm_lookup_range(rp, vir_addr, रिक्त, bytes) != bytes) {
		म_लिखो("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		परत 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	प्रतिज्ञा(phys);

	परत phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
पूर्णांक vm_lookup(स्थिरांक संरचना proc *proc, स्थिरांक vir_bytes आभासी,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *मूळ, *pt;
	पूर्णांक pde, pte;
	u32_t pde_v, pte_v;

	प्रतिज्ञा(proc);
	प्रतिज्ञा(physical);
	प्रतिज्ञा(!isemptyp(proc));
	प्रतिज्ञा(HASPT(proc));

	/* Retrieve page directory entry. */
	मूळ = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	प्रतिज्ञा(!((u32_t) मूळ % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(आभासी);
	प्रतिज्ञा(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (मूळ + pde));

	जर(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		परत EFAULT;
	}

	जर(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		जर(ptent) *ptent = pde_v;
		*physical += आभासी & ARM_VM_OFFSET_MASK_1MB;
	} अन्यथा  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		प्रतिज्ञा(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(आभासी);
		प्रतिज्ञा(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		जर(!(pte_v & ARM_VM_PTE_PRESENT)) {
			परत EFAULT;
		}

		जर(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += आभासी % ARM_PAGE_SIZE;
	}

	परत OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
माप_प्रकार vm_lookup_range(स्थिरांक संरचना proc *proc, vir_bytes vir_addr,
	phys_bytes *phys_addr, माप_प्रकार bytes)
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
	माप_प्रकार len;

	प्रतिज्ञा(proc);
	प्रतिज्ञा(bytes > 0);
	प्रतिज्ञा(HASPT(proc));

	/* Look up the first page. */
	जर (vm_lookup(proc, vir_addr, &phys, रिक्त) != OK)
		परत 0;

	जर (phys_addr != रिक्त)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	पर्यंत (len < bytes) {
		जर (vm_lookup(proc, vir_addr, &phys, रिक्त) != OK)
			थांबा;

		जर (next_phys != phys)
			थांबा;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	परत MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
पूर्णांक vm_check_range(संरचना proc *caller, संरचना proc *target,
	vir_bytes vir_addr, माप_प्रकार bytes, पूर्णांक writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	पूर्णांक r;

	जर ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		परत r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	परत VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
पूर्णांक vm_memset(संरचना proc* caller, endpoint_t who, phys_bytes ph, पूर्णांक c,
	phys_bytes मोजणी)
{
	u32_t pattern;
	संरचना proc *whoptr = रिक्त;
	phys_bytes cur_ph = ph;
	phys_bytes डावा = मोजणी;
	phys_bytes ptr, chunk, pfa = 0;
	पूर्णांक new_ttbr, r = OK;

	जर ((r = check_resumed_caller(caller)) != OK)
		परत r;

	/* NONE for physical, otherwise virtual */
	जर (who != NONE && !(whoptr = endpoint_lookup(who)))
		परत ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	प्रतिज्ञा(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	पर्यंत (डावा > 0) {
		new_ttbr = 0;
		chunk = डावा;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		जर (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		जर ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			जर (whoptr) {
				vm_suspend(caller, whoptr, ph, मोजणी,
						   VMSTYPE_KERNELCALL, 1);
				प्रतिज्ञा(catch_pagefaults);
				catch_pagefaults = 0;
				परत VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		डावा -= chunk;
	}

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	प्रतिज्ञा(catch_pagefaults);
	catch_pagefaults = 0;

	परत OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
पूर्णांक virtual_copy_f(
  संरचना proc * caller,
  संरचना vir_addr *src_addr,		/* source virtual address */
  संरचना vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  पूर्णांक vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  संरचना vir_addr *vir_addr[2];	/* virtual source and destination address */
  पूर्णांक i, r;
  संरचना proc *procs[2];

  प्रतिज्ञा((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  जर (bytes <= 0) परत(गलत_तर्क);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  साठी (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	पूर्णांक proc_nr;
	संरचना proc *p;

	जर(proc_e == NONE) {
		p = रिक्त;
	} अन्यथा {
		जर(!isokendpt(proc_e, &proc_nr)) {
			म_लिखो("virtual_copy: no reasonable endpoint\n");
			परत ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  जर ((r = check_resumed_caller(caller)) != OK)
	परत r;

  जर((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	पूर्णांक writeflag;
  	संरचना proc *target = रिक्त;
  	phys_bytes lin;
  	जर(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	जर(!vmcheck || !caller) {
    		परत r;
  	}

  	जर(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} अन्यथा जर(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} अन्यथा {
  		panic("r strange: %d",  r);
  	}

	प्रतिज्ञा(caller);
	प्रतिज्ञा(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	परत VMSUSPEND;
  }

  परत OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
पूर्णांक data_copy(स्थिरांक endpoint_t from_proc, स्थिरांक vir_bytes from_addr,
	स्थिरांक endpoint_t to_proc, स्थिरांक vir_bytes to_addr,
	माप_प्रकार bytes)
{
  संरचना vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  प्रतिज्ञा(src.proc_nr_e != NONE);
  प्रतिज्ञा(dst.proc_nr_e != NONE);

  परत virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
पूर्णांक data_copy_vmcheck(संरचना proc * caller,
	स्थिरांक endpoint_t from_proc, स्थिरांक vir_bytes from_addr,
	स्थिरांक endpoint_t to_proc, स्थिरांक vir_bytes to_addr,
	माप_प्रकार bytes)
{
  संरचना vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  प्रतिज्ञा(src.proc_nr_e != NONE);
  प्रतिज्ञा(dst.proc_nr_e != NONE);

  परत virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

शून्य memory_init(शून्य)
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
शून्य arch_proc_init(संरचना proc *pr, स्थिरांक u32_t ip, स्थिरांक u32_t sp,
	स्थिरांक u32_t ps_str, अक्षर *name)
{
	arch_proc_reset(pr);
	म_नकल(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

स्थिर पूर्णांक usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
बाह्य अक्षर usermapped_start, usermapped_end, usermapped_nonglo_start;

पूर्णांक arch_phys_map(स्थिरांक पूर्णांक index,
			phys_bytes *addr,
			phys_bytes *len,
			पूर्णांक *flags)
{
	स्थिर पूर्णांक first = 1;
	kern_phys_map *phys_maps;

	पूर्णांक freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	जर(first) {
		स_रखो(&minix_kerninfo, 0, आकार(minix_kerninfo));
		जर(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		जर(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		पर्यंत(phys_maps != रिक्त){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	जर(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		परत OK;
	}
	अन्यथा जर(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		परत OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	पर्यंत(phys_maps != रिक्त){
		जर(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			परत OK;
		}
		phys_maps = phys_maps->next;
	}

	परत EINVAL;
}

पूर्णांक arch_phys_map_reply(स्थिरांक पूर्णांक index, स्थिरांक vir_bytes addr)
{
	kern_phys_map *phys_maps;

	जर(index == first_um_idx) {
		u32_t usermapped_offset;
		प्रतिज्ञा(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (शून्य *) ((u32_t)ptr + usermapped_offset)
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

		परत OK;
	}

	जर (index == usermapped_index) {
		परत OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	पर्यंत(phys_maps != रिक्त){
		जर(phys_maps->index == index){
			प्रतिज्ञा(phys_maps->cb != रिक्त);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			परत OK;
		}
		phys_maps = phys_maps->next;
	}

	परत EINVAL;
}

पूर्णांक arch_enable_paging(संरचना proc * caller)
{
	kern_phys_map *phys_maps;
	प्रतिज्ञा(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	पर्यंत(phys_maps != रिक्त){
		प्रतिज्ञा(phys_maps->cb != रिक्त);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	परत OK;
}

शून्य release_address_space(संरचना proc *pr)
{
	pr->p_seg.p_ttbr_v = रिक्त;
	barrier();
}



/*
 * Request a physical mapping
 */
पूर्णांक kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       पूर्णांक vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	प्रतिज्ञा(base_address != 0);
	प्रतिज्ञा(io_size % ARM_PAGE_SIZE == 0);
	प्रतिज्ञा(cb != रिक्त);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = रिक्त;
	

	जर (kern_phys_map_head == रिक्त){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = रिक्त;
	} अन्यथा {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	परत 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
पूर्णांक kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	परत 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
पूर्णांक kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	पूर्णांक vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	परत kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

