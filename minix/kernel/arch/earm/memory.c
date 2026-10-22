
#समावेश "kernel/kernel.h"
#समावेश "kernel/proc.h"
#समावेश "kernel/vm.h"

#समावेश <machine/vm.h>

#समावेश <minix/type.h>
#समावेश <minix/board.h>
#समावेश <minix/syslib.h>
#समावेश <minix/cpufeature.h>
#समावेश <string.h>
#समावेश <assert.h>
#समावेश <signal.h>
#समावेश <stdlib.h>

#समावेश <machine/vm.h>

#समावेश "arch_proto.h"
#समावेश "kernel/proto.h"
#समावेश "kernel/debug.h"
#समावेश "bsp_timer.h"


#घोषणा HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
अटल पूर्णांक nfreepdes = 0;
#घोषणा MAXFREEPDES	2
अटल पूर्णांक freepdes[MAXFREEPDES];

अटल u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
अटल kern_phys_map *kern_phys_map_शीर्ष;

व्योम mem_clear_mapcache(व्योम)
{
	पूर्णांक i;
	क्रम(i = 0; i < nfreepdes; i++) {
		काष्ठा proc *ptproc = get_cpulocal_var(ptproc);
		पूर्णांक pde = freepdes[i];
		u32_t *ptv;
		निश्चित(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		निश्चित(ptv);
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
अटल phys_bytes createpde(
	स्थिर काष्ठा proc *pr,	/* Requested process, NULL for physical. */
	स्थिर phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	पूर्णांक free_pde_idx,	/* index of the free slot to use */
	पूर्णांक *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	पूर्णांक pde;

	निश्चित(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	निश्चित(pde >= 0 && pde < 4096);

	अगर(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		वापस linaddr;
	}

	अगर(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		निश्चित(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} अन्यथा {
		/* Requested address is physical. Make up the PDE entry. */
		निश्चित (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

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
	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	अगर(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & ARM_VM_OFFSET_MASK_1MB; /* Offset in 1MB window. */
	*bytes = MIN(*bytes, ARM_SECTION_आकार - offset);

	/* Return the linear address of the start of the new mapping. */
	वापस ARM_SECTION_आकार*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
अटल पूर्णांक check_resumed_caller(काष्ठा proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	अगर (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		निश्चित(caller->p_vmrequest.vmresult != VMSUSPEND);
		वापस caller->p_vmrequest.vmresult;
	}

	वापस OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
अटल पूर्णांक lin_lin_copy(काष्ठा proc *srcproc, vir_bytes srclinaddr, 
	काष्ठा proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	निश्चित(get_cpulocal_var(ptproc));
	निश्चित(get_cpulocal_var(proc_ptr));
	निश्चित(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	निश्चित(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	अगर(srcproc) निश्चित(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	अगर(dstproc) निश्चित(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	निश्चित(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	अगर(srcproc) निश्चित(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	अगर(dstproc) निश्चित(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	जबतक(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		पूर्णांक changed = 0;

#अगर_घोषित CONFIG_SMP
		अचिन्हित cpu = cpuid;

		अगर (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		अगर (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#पूर्ण_अगर

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		अगर(changed)
			reload_ttbr0();

		/* Check for overflow. */
		अगर (srcptr + chunk < srcptr) वापस EFAULT_SRC;
		अगर (dstptr + chunk < dstptr) वापस EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		अगर(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			अगर(addr >= src_aligned && addr < (srcptr + chunk)) {
				वापस EFAULT_SRC;
			}
			अगर(addr >= dst_aligned && addr < (dstptr + chunk)) {
				वापस EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			वापस EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	अगर(srcproc) निश्चित(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	अगर(dstproc) निश्चित(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	निश्चित(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	वापस OK;
}

अटल u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	पूर्णांक r;

	अगर((r=lin_lin_copy(शून्य, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, माप(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	वापस v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  रेजिस्टर काष्ठा proc *rp,		/* pointer to proc table entry for process */
  पूर्णांक seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	अगर(vm_lookup(rp, vir_addr, &phys, शून्य) != OK) {
		म_लिखो("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} अन्यथा {
		अगर(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	अगर(phys == 0) {
		म_लिखो("SYSTEM:umap_virtual: lookup failed\n");
		वापस 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	अगर(bytes > 0 && vm_lookup_range(rp, vir_addr, शून्य, bytes) != bytes) {
		म_लिखो("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		वापस 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	निश्चित(phys);

	वापस phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
पूर्णांक vm_lookup(स्थिर काष्ठा proc *proc, स्थिर vir_bytes भव,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *मूल, *pt;
	पूर्णांक pde, pte;
	u32_t pde_v, pte_v;

	निश्चित(proc);
	निश्चित(physical);
	निश्चित(!isemptyp(proc));
	निश्चित(HASPT(proc));

	/* Retrieve page directory entry. */
	मूल = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	निश्चित(!((u32_t) मूल % ARM_PAGEDIR_आकार));
	pde = ARM_VM_PDE(भव);
	निश्चित(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (मूल + pde));

	अगर(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		वापस EFAULT;
	}

	अगर(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		अगर(ptent) *ptent = pde_v;
		*physical += भव & ARM_VM_OFFSET_MASK_1MB;
	} अन्यथा  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		निश्चित(!((u32_t) pt % ARM_PAGETABLE_आकार));
		pte = ARM_VM_PTE(भव);
		निश्चित(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		अगर(!(pte_v & ARM_VM_PTE_PRESENT)) {
			वापस EFAULT;
		}

		अगर(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += भव % ARM_PAGE_आकार;
	}

	वापस OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
माप_प्रकार vm_lookup_range(स्थिर काष्ठा proc *proc, vir_bytes vir_addr,
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
	phys_bytes phys, अग्र_phys;
	माप_प्रकार len;

	निश्चित(proc);
	निश्चित(bytes > 0);
	निश्चित(HASPT(proc));

	/* Look up the first page. */
	अगर (vm_lookup(proc, vir_addr, &phys, शून्य) != OK)
		वापस 0;

	अगर (phys_addr != शून्य)
		*phys_addr = phys;

	len = ARM_PAGE_आकार - (vir_addr % ARM_PAGE_आकार);
	vir_addr += len;
	अग्र_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	जबतक (len < bytes) {
		अगर (vm_lookup(proc, vir_addr, &phys, शून्य) != OK)
			अवरोध;

		अगर (अग्र_phys != phys)
			अवरोध;

		len += ARM_PAGE_आकार;
		vir_addr += ARM_PAGE_आकार;
		अग्र_phys += ARM_PAGE_आकार;
	}

	/* We might now have overshot the requested length somewhat. */
	वापस MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
पूर्णांक vm_check_range(काष्ठा proc *caller, काष्ठा proc *target,
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

	अगर ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		वापस r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	वापस VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
पूर्णांक vm_memset(काष्ठा proc* caller, endpoint_t who, phys_bytes ph, पूर्णांक c,
	phys_bytes गिनती)
{
	u32_t pattern;
	काष्ठा proc *whoptr = शून्य;
	phys_bytes cur_ph = ph;
	phys_bytes बायाँ = गिनती;
	phys_bytes ptr, chunk, pfa = 0;
	पूर्णांक new_ttbr, r = OK;

	अगर ((r = check_resumed_caller(caller)) != OK)
		वापस r;

	/* NONE for physical, otherwise virtual */
	अगर (who != NONE && !(whoptr = endpoint_lookup(who)))
		वापस ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	निश्चित(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	जबतक (बायाँ > 0) {
		new_ttbr = 0;
		chunk = बायाँ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		अगर (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		अगर ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			अगर (whoptr) {
				vm_suspend(caller, whoptr, ph, गिनती,
						   VMSTYPE_KERNELCALL, 1);
				निश्चित(catch_pagefaults);
				catch_pagefaults = 0;
				वापस VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		बायाँ -= chunk;
	}

	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	निश्चित(catch_pagefaults);
	catch_pagefaults = 0;

	वापस OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
पूर्णांक virtual_copy_f(
  काष्ठा proc * caller,
  काष्ठा vir_addr *src_addr,		/* source virtual address */
  काष्ठा vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  पूर्णांक vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  काष्ठा vir_addr *vir_addr[2];	/* virtual source and destination address */
  पूर्णांक i, r;
  काष्ठा proc *procs[2];

  निश्चित((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  अगर (bytes <= 0) वापस(गलत_तर्क);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  क्रम (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	पूर्णांक proc_nr;
	काष्ठा proc *p;

	अगर(proc_e == NONE) {
		p = शून्य;
	} अन्यथा {
		अगर(!isokendpt(proc_e, &proc_nr)) {
			म_लिखो("virtual_copy: no reasonable endpoint\n");
			वापस ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  अगर ((r = check_resumed_caller(caller)) != OK)
	वापस r;

  अगर((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	पूर्णांक writeflag;
  	काष्ठा proc *target = शून्य;
  	phys_bytes lin;
  	अगर(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	अगर(!vmcheck || !caller) {
    		वापस r;
  	}

  	अगर(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} अन्यथा अगर(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} अन्यथा {
  		panic("r strange: %d",  r);
  	}

	निश्चित(caller);
	निश्चित(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	वापस VMSUSPEND;
  }

  वापस OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
पूर्णांक तथ्य_copy(स्थिर endpoint_t from_proc, स्थिर vir_bytes from_addr,
	स्थिर endpoint_t to_proc, स्थिर vir_bytes to_addr,
	माप_प्रकार bytes)
{
  काष्ठा vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  निश्चित(src.proc_nr_e != NONE);
  निश्चित(dst.proc_nr_e != NONE);

  वापस virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
पूर्णांक तथ्य_copy_vmcheck(काष्ठा proc * caller,
	स्थिर endpoint_t from_proc, स्थिर vir_bytes from_addr,
	स्थिर endpoint_t to_proc, स्थिर vir_bytes to_addr,
	माप_प्रकार bytes)
{
  काष्ठा vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  निश्चित(src.proc_nr_e != NONE);
  निश्चित(dst.proc_nr_e != NONE);

  वापस virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

व्योम memory_init(व्योम)
{
	निश्चित(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	निश्चित(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	निश्चित(nfreepdes == 2);
	निश्चित(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
व्योम arch_proc_init(काष्ठा proc *pr, स्थिर u32_t ip, स्थिर u32_t sp,
	स्थिर u32_t ps_str, अक्षर *name)
{
	arch_proc_reset(pr);
	म_नकल(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

अटल पूर्णांक usermapped_glo_सूचकांक = -1,
	usermapped_सूचकांक = -1, first_um_idx = -1;


/* defined in kernel.lds */
बाह्य अक्षर usermapped_start, usermapped_end, usermapped_nonglo_start;

पूर्णांक arch_phys_map(स्थिर पूर्णांक सूचकांक,
			phys_bytes *addr,
			phys_bytes *len,
			पूर्णांक *flags)
{
	अटल पूर्णांक first = 1;
	kern_phys_map *phys_maps;

	पूर्णांक freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	अगर(first) {
		स_रखो(&minix_kerninfo, 0, माप(minix_kerninfo));
		अगर(glo_len > 0) {
			usermapped_glo_सूचकांक = freeidx++;
		}

		usermapped_सूचकांक = freeidx++;
		first_um_idx = usermapped_सूचकांक;
		अगर(usermapped_glo_सूचकांक != -1)
			first_um_idx = usermapped_glo_सूचकांक;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_शीर्ष;
		जबतक(phys_maps != शून्य){
			phys_maps->सूचकांक = freeidx++;
			phys_maps = phys_maps->अग्र;
		}

	}

	अगर(सूचकांक == usermapped_glo_सूचकांक) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		वापस OK;
	}
	अन्यथा अगर(सूचकांक == usermapped_सूचकांक) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		वापस OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_शीर्ष;
	जबतक(phys_maps != शून्य){
		अगर(phys_maps->सूचकांक == सूचकांक){
			*addr = phys_maps->addr;
			*len =  phys_maps->आकार;
			*flags = phys_maps->vm_flags;
			वापस OK;
		}
		phys_maps = phys_maps->अग्र;
	}

	वापस EINVAL;
}

पूर्णांक arch_phys_map_reply(स्थिर पूर्णांक सूचकांक, स्थिर vir_bytes addr)
{
	kern_phys_map *phys_maps;

	अगर(सूचकांक == first_um_idx) {
		u32_t usermapped_offset;
		निश्चित(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#घोषणा FIXEDPTR(ptr) (व्योम *) ((u32_t)ptr + usermapped_offset)
#घोषणा FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#घोषणा ASSIGN(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
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

		वापस OK;
	}

	अगर (सूचकांक == usermapped_सूचकांक) {
		वापस OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_शीर्ष;
	जबतक(phys_maps != शून्य){
		अगर(phys_maps->सूचकांक == सूचकांक){
			निश्चित(phys_maps->cb != शून्य);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			वापस OK;
		}
		phys_maps = phys_maps->अग्र;
	}

	वापस EINVAL;
}

पूर्णांक arch_enable_paging(काष्ठा proc * caller)
{
	kern_phys_map *phys_maps;
	निश्चित(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_शीर्ष;
	जबतक(phys_maps != शून्य){
		निश्चित(phys_maps->cb != शून्य);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->अग्र;
	}

	वापस OK;
}

व्योम release_address_space(काष्ठा proc *pr)
{
	pr->p_seg.p_ttbr_v = शून्य;
	barrier();
}



/*
 * Request a physical mapping
 */
पूर्णांक kern_req_phys_map( phys_bytes base_address, vir_bytes io_आकार, 
		       पूर्णांक vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	निश्चित(base_address != 0);
	निश्चित(io_आकार % ARM_PAGE_आकार == 0);
	निश्चित(cb != शून्य);

	priv->addr  = base_address;
	priv->आकार  = io_आकार;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->सूचकांक = -1;
	priv->अग्र = शून्य;
	

	अगर (kern_phys_map_शीर्ष == शून्य){
		/* keep a list of items this is the first one */
		kern_phys_map_शीर्ष = priv;
		kern_phys_map_शीर्ष->अग्र = शून्य;
	} अन्यथा {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->अग्र = kern_phys_map_शीर्ष;
		/* replace the head */
		kern_phys_map_शीर्ष = priv;
	}
	वापस 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
पूर्णांक kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	वापस 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
पूर्णांक kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_आकार, 
	पूर्णांक vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	वापस kern_req_phys_map(base_address,io_आकार,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

