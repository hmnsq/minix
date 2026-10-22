
#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "kernel/proc.h"
#અન્તર્ભાવ "kernel/vm.h"

#અન્તર્ભાવ <machine/vm.h>

#અન્તર્ભાવ <minix/type.h>
#અન્તર્ભાવ <minix/board.h>
#અન્તર્ભાવ <minix/syslib.h>
#અન્તર્ભાવ <minix/cpufeature.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ <machine/vm.h>

#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ "kernel/proto.h"
#અન્તર્ભાવ "kernel/debug.h"
#અન્તર્ભાવ "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
સ્થિર પૂર્ણાંક nfreepdes = 0;
#define MAXFREEPDES	2
સ્થિર પૂર્ણાંક freepdes[MAXFREEPDES];

સ્થિર u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
સ્થિર kern_phys_map *kern_phys_map_head;

શૂન્ય mem_clear_mapcache(શૂન્ય)
{
	પૂર્ણાંક i;
	માટે(i = 0; i < nfreepdes; i++) {
		સંરચના proc *ptproc = get_cpulocal_var(ptproc);
		પૂર્ણાંક pde = freepdes[i];
		u32_t *ptv;
		પ્રતિજ્ઞા(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		પ્રતિજ્ઞા(ptv);
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
સ્થિર phys_bytes createpde(
	અચલ સંરચના proc *pr,	/* Requested process, NULL for physical. */
	અચલ phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	પૂર્ણાંક free_pde_idx,	/* index of the free slot to use */
	પૂર્ણાંક *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	પૂર્ણાંક pde;

	પ્રતિજ્ઞા(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	પ્રતિજ્ઞા(pde >= 0 && pde < 4096);

	જો(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		પાછા linaddr;
	}

	જો(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		પ્રતિજ્ઞા(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} નહીંતર {
		/* Requested address is physical. Make up the PDE entry. */
		પ્રતિજ્ઞા (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

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
	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	જો(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
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
	પાછા ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
સ્થિર પૂર્ણાંક check_resumed_caller(સંરચના proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	જો (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		પ્રતિજ્ઞા(caller->p_vmrequest.vmresult != VMSUSPEND);
		પાછા caller->p_vmrequest.vmresult;
	}

	પાછા OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
સ્થિર પૂર્ણાંક lin_lin_copy(સંરચના proc *srcproc, vir_bytes srclinaddr, 
	સંરચના proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	પ્રતિજ્ઞા(get_cpulocal_var(ptproc));
	પ્રતિજ્ઞા(get_cpulocal_var(proc_ptr));
	પ્રતિજ્ઞા(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	પ્રતિજ્ઞા(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	જો(srcproc) પ્રતિજ્ઞા(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	જો(dstproc) પ્રતિજ્ઞા(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	પ્રતિજ્ઞા(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	જો(srcproc) પ્રતિજ્ઞા(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	જો(dstproc) પ્રતિજ્ઞા(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	જ્યારે(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		પૂર્ણાંક changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		જો (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		જો (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		જો(changed)
			reload_ttbr0();

		/* Check for overflow. */
		જો (srcptr + chunk < srcptr) પાછા EFAULT_SRC;
		જો (dstptr + chunk < dstptr) પાછા EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		જો(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			જો(addr >= src_aligned && addr < (srcptr + chunk)) {
				પાછા EFAULT_SRC;
			}
			જો(addr >= dst_aligned && addr < (dstptr + chunk)) {
				પાછા EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			પાછા EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	જો(srcproc) પ્રતિજ્ઞા(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	જો(dstproc) પ્રતિજ્ઞા(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	પ્રતિજ્ઞા(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	પાછા OK;
}

સ્થિર u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	પૂર્ણાંક r;

	જો((r=lin_lin_copy(ખાલી, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, કદ(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	પાછા v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  નોંધણી સંરચના proc *rp,		/* pointer to proc table entry for process */
  પૂર્ણાંક seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	જો(vm_lookup(rp, vir_addr, &phys, ખાલી) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} નહીંતર {
		જો(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	જો(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		પાછા 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	જો(bytes > 0 && vm_lookup_range(rp, vir_addr, ખાલી, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		પાછા 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	પ્રતિજ્ઞા(phys);

	પાછા phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
પૂર્ણાંક vm_lookup(અચલ સંરચના proc *proc, અચલ vir_bytes આભાસી,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *મૂળ, *pt;
	પૂર્ણાંક pde, pte;
	u32_t pde_v, pte_v;

	પ્રતિજ્ઞા(proc);
	પ્રતિજ્ઞા(physical);
	પ્રતિજ્ઞા(!isemptyp(proc));
	પ્રતિજ્ઞા(HASPT(proc));

	/* Retrieve page directory entry. */
	મૂળ = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	પ્રતિજ્ઞા(!((u32_t) મૂળ % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(આભાસી);
	પ્રતિજ્ઞા(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (મૂળ + pde));

	જો(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		પાછા EFAULT;
	}

	જો(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		જો(ptent) *ptent = pde_v;
		*physical += આભાસી & ARM_VM_OFFSET_MASK_1MB;
	} નહીંતર  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		પ્રતિજ્ઞા(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(આભાસી);
		પ્રતિજ્ઞા(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		જો(!(pte_v & ARM_VM_PTE_PRESENT)) {
			પાછા EFAULT;
		}

		જો(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += આભાસી % ARM_PAGE_SIZE;
	}

	પાછા OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(અચલ સંરચના proc *proc, vir_bytes vir_addr,
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

	પ્રતિજ્ઞા(proc);
	પ્રતિજ્ઞા(bytes > 0);
	પ્રતિજ્ઞા(HASPT(proc));

	/* Look up the first page. */
	જો (vm_lookup(proc, vir_addr, &phys, ખાલી) != OK)
		પાછા 0;

	જો (phys_addr != ખાલી)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	જ્યારે (len < bytes) {
		જો (vm_lookup(proc, vir_addr, &phys, ખાલી) != OK)
			તોડો;

		જો (next_phys != phys)
			તોડો;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	પાછા MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
પૂર્ણાંક vm_check_range(સંરચના proc *caller, સંરચના proc *target,
	vir_bytes vir_addr, size_t bytes, પૂર્ણાંક writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	પૂર્ણાંક r;

	જો ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		પાછા r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	પાછા VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
પૂર્ણાંક vm_memset(સંરચના proc* caller, endpoint_t who, phys_bytes ph, પૂર્ણાંક c,
	phys_bytes ગણતરી)
{
	u32_t pattern;
	સંરચના proc *whoptr = ખાલી;
	phys_bytes cur_ph = ph;
	phys_bytes ડાબું = ગણતરી;
	phys_bytes ptr, chunk, pfa = 0;
	પૂર્ણાંક new_ttbr, r = OK;

	જો ((r = check_resumed_caller(caller)) != OK)
		પાછા r;

	/* NONE for physical, otherwise virtual */
	જો (who != NONE && !(whoptr = endpoint_lookup(who)))
		પાછા ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	પ્રતિજ્ઞા(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	જ્યારે (ડાબું > 0) {
		new_ttbr = 0;
		chunk = ડાબું;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		જો (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		જો ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			જો (whoptr) {
				vm_suspend(caller, whoptr, ph, ગણતરી,
						   VMSTYPE_KERNELCALL, 1);
				પ્રતિજ્ઞા(catch_pagefaults);
				catch_pagefaults = 0;
				પાછા VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		ડાબું -= chunk;
	}

	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	પ્રતિજ્ઞા(catch_pagefaults);
	catch_pagefaults = 0;

	પાછા OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
પૂર્ણાંક virtual_copy_f(
  સંરચના proc * caller,
  સંરચના vir_addr *src_addr,		/* source virtual address */
  સંરચના vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  પૂર્ણાંક vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  સંરચના vir_addr *vir_addr[2];	/* virtual source and destination address */
  પૂર્ણાંક i, r;
  સંરચના proc *procs[2];

  પ્રતિજ્ઞા((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  જો (bytes <= 0) પાછા(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  માટે (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	પૂર્ણાંક proc_nr;
	સંરચના proc *p;

	જો(proc_e == NONE) {
		p = ખાલી;
	} નહીંતર {
		જો(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			પાછા ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  જો ((r = check_resumed_caller(caller)) != OK)
	પાછા r;

  જો((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	પૂર્ણાંક writeflag;
  	સંરચના proc *target = ખાલી;
  	phys_bytes lin;
  	જો(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	જો(!vmcheck || !caller) {
    		પાછા r;
  	}

  	જો(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} નહીંતર જો(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} નહીંતર {
  		panic("r strange: %d",  r);
  	}

	પ્રતિજ્ઞા(caller);
	પ્રતિજ્ઞા(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	પાછા VMSUSPEND;
  }

  પાછા OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
પૂર્ણાંક data_copy(અચલ endpoint_t from_proc, અચલ vir_bytes from_addr,
	અચલ endpoint_t to_proc, અચલ vir_bytes to_addr,
	size_t bytes)
{
  સંરચના vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  પ્રતિજ્ઞા(src.proc_nr_e != NONE);
  પ્રતિજ્ઞા(dst.proc_nr_e != NONE);

  પાછા virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
પૂર્ણાંક data_copy_vmcheck(સંરચના proc * caller,
	અચલ endpoint_t from_proc, અચલ vir_bytes from_addr,
	અચલ endpoint_t to_proc, અચલ vir_bytes to_addr,
	size_t bytes)
{
  સંરચના vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  પ્રતિજ્ઞા(src.proc_nr_e != NONE);
  પ્રતિજ્ઞા(dst.proc_nr_e != NONE);

  પાછા virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

શૂન્ય memory_init(શૂન્ય)
{
	પ્રતિજ્ઞા(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	પ્રતિજ્ઞા(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	પ્રતિજ્ઞા(nfreepdes == 2);
	પ્રતિજ્ઞા(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
શૂન્ય arch_proc_init(સંરચના proc *pr, અચલ u32_t ip, અચલ u32_t sp,
	અચલ u32_t ps_str, અક્ષર *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

સ્થિર પૂર્ણાંક usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
બાહ્ય અક્ષર usermapped_start, usermapped_end, usermapped_nonglo_start;

પૂર્ણાંક arch_phys_map(અચલ પૂર્ણાંક index,
			phys_bytes *addr,
			phys_bytes *len,
			પૂર્ણાંક *flags)
{
	સ્થિર પૂર્ણાંક first = 1;
	kern_phys_map *phys_maps;

	પૂર્ણાંક freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	જો(first) {
		memset(&minix_kerninfo, 0, કદ(minix_kerninfo));
		જો(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		જો(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		જ્યારે(phys_maps != ખાલી){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	જો(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		પાછા OK;
	}
	નહીંતર જો(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		પાછા OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	જ્યારે(phys_maps != ખાલી){
		જો(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			પાછા OK;
		}
		phys_maps = phys_maps->next;
	}

	પાછા EINVAL;
}

પૂર્ણાંક arch_phys_map_reply(અચલ પૂર્ણાંક index, અચલ vir_bytes addr)
{
	kern_phys_map *phys_maps;

	જો(index == first_um_idx) {
		u32_t usermapped_offset;
		પ્રતિજ્ઞા(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (શૂન્ય *) ((u32_t)ptr + usermapped_offset)
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

		પાછા OK;
	}

	જો (index == usermapped_index) {
		પાછા OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	જ્યારે(phys_maps != ખાલી){
		જો(phys_maps->index == index){
			પ્રતિજ્ઞા(phys_maps->cb != ખાલી);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			પાછા OK;
		}
		phys_maps = phys_maps->next;
	}

	પાછા EINVAL;
}

પૂર્ણાંક arch_enable_paging(સંરચના proc * caller)
{
	kern_phys_map *phys_maps;
	પ્રતિજ્ઞા(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	જ્યારે(phys_maps != ખાલી){
		પ્રતિજ્ઞા(phys_maps->cb != ખાલી);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	પાછા OK;
}

શૂન્ય release_address_space(સંરચના proc *pr)
{
	pr->p_seg.p_ttbr_v = ખાલી;
	barrier();
}



/*
 * Request a physical mapping
 */
પૂર્ણાંક kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       પૂર્ણાંક vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	પ્રતિજ્ઞા(base_address != 0);
	પ્રતિજ્ઞા(io_size % ARM_PAGE_SIZE == 0);
	પ્રતિજ્ઞા(cb != ખાલી);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = ખાલી;
	

	જો (kern_phys_map_head == ખાલી){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = ખાલી;
	} નહીંતર {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	પાછા 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
પૂર્ણાંક kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	પાછા 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
પૂર્ણાંક kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	પૂર્ણાંક vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	પાછા kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

