
#समावेश "kernel/kernel.h"
#समावेश "kernel/vm.h"

#समावेश <machine/vm.h>

#समावेश <minix/syslib.h>
#समावेश <minix/cpufeature.h>
#समावेश <string.h>
#समावेश <assert.h>
#समावेश <signal.h>
#समावेश <stdlib.h>

#समावेश <machine/vm.h>

#समावेश "oxpcie.h"
#समावेश "arch_proto.h"

#अगर_घोषित USE_APIC
#समावेश "apic.h"
#अगर_घोषित USE_WATCHDOG
#समावेश "kernel/watchdog.h"
#पूर्ण_अगर
#पूर्ण_अगर

phys_bytes video_mem_vaddr = 0;

#घोषणा HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
अटल पूर्णांक nfreepdes = 0;
#घोषणा MAXFREEPDES	2
अटल पूर्णांक freepdes[MAXFREEPDES];

अटल u32_t phys_get32(phys_bytes v);

व्योम mem_clear_mapcache(व्योम)
{
	पूर्णांक i;
	क्रम(i = 0; i < nfreepdes; i++) {
		काष्ठा proc *ptproc = get_cpulocal_var(ptproc);
		पूर्णांक pde = freepdes[i];
		u32_t *ptv;
		निश्चित(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		निश्चित(ptv);
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
	निश्चित(pde >= 0 && pde < 1024);

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
		निश्चित(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} अन्यथा {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	अगर(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & I386_VM_OFFSET_MASK_4MB; /* Offset in 4MB window. */
	*bytes = MIN(*bytes, I386_BIG_PAGE_आकार - offset); 

	/* Return the linear address of the start of the new mapping. */
	वापस I386_BIG_PAGE_आकार*pde + offset;
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
	निश्चित(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	निश्चित(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	अगर(srcproc) निश्चित(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	अगर(dstproc) निश्चित(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	निश्चित(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
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

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		अगर(changed)
			reload_cr3(); 

		/* Check for overflow. */
		अगर (srcptr + chunk < srcptr) वापस EFAULT_SRC;
		अगर (dstptr + chunk < dstptr) वापस EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		अगर(addr) {
			/* If addr is nonzero, a page fault was caught. */

			अगर(addr >= srcptr && addr < (srcptr + chunk)) {
				वापस EFAULT_SRC;
			}
			अगर(addr >= dstptr && addr < (dstptr + chunk)) {
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
	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

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

#अगर 0
अटल अक्षर *cr0_str(u32_t e)
{
	अटल अक्षर str[80];
	म_नकल(str, "");
#घोषणा FLAG(v) करो { अगर(e & (v)) { म_जोड़ो(str, #v " "); e &= ~v; } } जबतक(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	अगर(e) { म_जोड़ो(str, " (++)"); }
	वापस str;
}

अटल अक्षर *cr4_str(u32_t e)
{
	अटल अक्षर str[80];
	म_नकल(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	अगर(e) { म_जोड़ो(str, " (++)"); }
	वापस str;
}
#पूर्ण_अगर

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
	मूल = (u32_t *) proc->p_seg.p_cr3;
	निश्चित(!((u32_t) मूल % I386_PAGE_आकार));
	pde = I386_VM_PDE(भव);
	निश्चित(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (मूल + pde));

	अगर(!(pde_v & I386_VM_PRESENT)) {
		वापस EFAULT;
	}

	/* We don't expect to ever see this. */
	अगर(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		अगर(ptent) *ptent = pde_v;
		*physical += भव & I386_VM_OFFSET_MASK_4MB;
	} अन्यथा {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		निश्चित(!((u32_t) pt % I386_PAGE_आकार));
		pte = I386_VM_PTE(भव);
		निश्चित(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		अगर(!(pte_v & I386_VM_PRESENT)) {
			वापस EFAULT;
		}

		अगर(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += भव % I386_PAGE_आकार;
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

	len = I386_PAGE_आकार - (vir_addr % I386_PAGE_आकार);
	vir_addr += len;
	अग्र_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	जबतक (len < bytes) {
		अगर (vm_lookup(proc, vir_addr, &phys, शून्य) != OK)
			अवरोध;

		अगर (अग्र_phys != phys)
			अवरोध;

		len += I386_PAGE_आकार;
		vir_addr += I386_PAGE_आकार;
		अग्र_phys += I386_PAGE_आकार;
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

#अगर 0
अटल अक्षर *flagstr(u32_t e, स्थिर पूर्णांक dir)
{
	अटल अक्षर str[80];
	म_नकल(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	अगर(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	अन्यथा
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	वापस str;
}

अटल व्योम vm_pt_print(u32_t *pagetable, स्थिर u32_t v)
{
	पूर्णांक pte;
	पूर्णांक col = 0;

	निश्चित(!((u32_t) pagetable % I386_PAGE_आकार));

	क्रम(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		अगर(!(pte_v & I386_VM_PRESENT))
			जारी;
		pfa = I386_VM_PFA(pte_v);
		म_लिखो("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_आकार*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		अगर(col == 3) { म_लिखो("\n"); col = 0; }
	}
	अगर(col > 0) म_लिखो("\n");

	वापस;
}

अटल व्योम vm_print(u32_t *मूल)
{
	पूर्णांक pde;

	निश्चित(!((u32_t) मूल % I386_PAGE_आकार));

	म_लिखो("page table 0x%lx:\n", मूल);

	क्रम(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (मूल + pde));
		अगर(!(pde_v & I386_VM_PRESENT))
			जारी;
		अगर(pde_v & I386_VM_BIGPAGE) {
			म_लिखो("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} अन्यथा {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			म_लिखो("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_आकार);
			म_लिखो("\n");
		}
	}


	वापस;
}
#पूर्ण_अगर

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
	पूर्णांक new_cr3, r = OK;

	अगर ((r = check_resumed_caller(caller)) != OK)
		वापस r;

	/* NONE for physical, otherwise virtual */
	अगर (who != NONE && !(whoptr = endpoint_lookup(who)))
		वापस ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	निश्चित(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	जबतक (बायाँ > 0) {
		new_cr3 = 0;
		chunk = बायाँ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		अगर (new_cr3)
			reload_cr3();

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

	निश्चित(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	निश्चित(catch_pagefaults);
	catch_pagefaults = 0;

	वापस OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
पूर्णांक virtual_copy_f(
  काष्ठा proc * caller,
  काष्ठा vir_addr *src_addr,	/* source virtual address */
  काष्ठा vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  पूर्णांक vmcheck			/* if nonzero, can return VMSUSPEND */
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

	निश्चित(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
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
	strlcpy(pr->p_name, name, माप(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

अटल पूर्णांक oxpcie_mapping_सूचकांक = -1,
	lapic_mapping_सूचकांक = -1,
	ioapic_first_सूचकांक = -1,
	ioapic_last_सूचकांक = -1,
	video_mem_mapping_सूचकांक = -1,
	usermapped_glo_सूचकांक = -1,
	usermapped_सूचकांक = -1, first_um_idx = -1;

बाह्य अक्षर *video_mem;

बाह्य अक्षर usermapped_start, usermapped_end, usermapped_nonglo_start;

पूर्णांक arch_phys_map(स्थिर पूर्णांक सूचकांक,
			phys_bytes *addr,
			phys_bytes *len,
			पूर्णांक *flags)
{
	अटल पूर्णांक first = 1;
	पूर्णांक freeidx = 0;
	अटल अक्षर *ser_var = शून्य;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	अगर(first) {
		स_रखो(&minix_kerninfo, 0, माप(minix_kerninfo));
		video_mem_mapping_सूचकांक = freeidx++;
		अगर(glo_len > 0) {
			usermapped_glo_सूचकांक = freeidx++;
		}
		
		usermapped_सूचकांक = freeidx++;
		first_um_idx = usermapped_सूचकांक;
		अगर(usermapped_glo_सूचकांक != -1)
			first_um_idx = usermapped_glo_सूचकांक;

#अगर_घोषित USE_APIC
		अगर(lapic_addr)
			lapic_mapping_सूचकांक = freeidx++;
		अगर (ioapic_enabled) {
			ioapic_first_सूचकांक = freeidx;
			निश्चित(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_सूचकांक = freeidx-1;
		}
#पूर्ण_अगर

#अगर_घोषित CONFIG_OXPCIE
		अगर((ser_var = env_get("oxpcie"))) {
			अगर(ser_var[0] != '0' || ser_var[1] != 'x') {
				म_लिखो("oxpcie address in hex please\n");
			} अन्यथा {
				म_लिखो("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_सूचकांक = freeidx++;
			}
		}
#पूर्ण_अगर

		first = 0;
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
	अन्यथा अगर (सूचकांक == video_mem_mapping_सूचकांक) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_बफर;
		*len = I386_PAGE_आकार;
		*flags = VMMF_WRITE;
		वापस OK;
	}
#अगर_घोषित USE_APIC
	अन्यथा अगर (सूचकांक == lapic_mapping_सूचकांक) {
		/* map the local APIC if enabled */
		अगर (!lapic_addr)
			वापस EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		वापस OK;
	}
	अन्यथा अगर (ioapic_enabled && सूचकांक >= ioapic_first_सूचकांक && सूचकांक <= ioapic_last_सूचकांक) {
		पूर्णांक ioapic_idx = सूचकांक - ioapic_first_सूचकांक;
		*addr = io_apic[ioapic_idx].paddr;
		निश्चित(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		म_लिखो("ioapic map: addr 0x%lx\n", *addr);
		वापस OK;
	}
#पूर्ण_अगर

#अगर CONFIG_OXPCIE
	अगर(सूचकांक == oxpcie_mapping_सूचकांक) {
		*addr = म_से_अदीर्घ(ser_var+2, शून्य, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		वापस OK;
	}
#पूर्ण_अगर

	वापस EINVAL;
}

पूर्णांक arch_phys_map_reply(स्थिर पूर्णांक सूचकांक, स्थिर vir_bytes addr)
{
#अगर_घोषित USE_APIC
	/* if local APIC is enabled */
	अगर (सूचकांक == lapic_mapping_सूचकांक && lapic_addr) {
		lapic_addr_vaddr = addr;
		वापस OK;
	}
	अन्यथा अगर (ioapic_enabled && सूचकांक >= ioapic_first_सूचकांक &&
		सूचकांक <= ioapic_last_सूचकांक) {
		पूर्णांक i = सूचकांक - ioapic_first_सूचकांक;
		io_apic[i].vaddr = addr;
		वापस OK;
	}
#पूर्ण_अगर

#अगर CONFIG_OXPCIE
	अगर (सूचकांक == oxpcie_mapping_सूचकांक) {
		oxpcie_set_vaddr((अचिन्हित अक्षर *) addr);
		वापस OK;
	}
#पूर्ण_अगर
	अगर(सूचकांक == first_um_idx) {
		बाह्य काष्ठा minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		बाह्य u32_t usermapped_offset;
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
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		अगर(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} अन्यथा  अगर(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} अन्यथा	{
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
		अगर(env_get("libc_ipc")) {
			म_लिखो("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = शून्य;
		} अन्यथा {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		वापस OK;
	}

	अगर(सूचकांक == usermapped_सूचकांक) वापस OK;

	अगर (सूचकांक == video_mem_mapping_सूचकांक) {
		video_mem_vaddr =  addr;
		वापस OK;
	}

	वापस EINVAL;
}

पूर्णांक arch_enable_paging(काष्ठा proc * caller)
{
	निश्चित(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (अक्षर *) video_mem_vaddr;

#अगर_घोषित USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	अगर (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	अगर (ioapic_enabled) {
		पूर्णांक i;

		क्रम (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#अगर CONFIG_SMP
	barrier();

	wait_for_APs_to_finish_booting();
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	अगर (watchdog_enabled)
		i386_watchdog_start();
#पूर्ण_अगर

	वापस OK;
}

व्योम release_address_space(काष्ठा proc *pr)
{
	pr->p_seg.p_cr3_v = शून्य;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
पूर्णांक platform_tbl_checksum_ok(व्योम *ptr, अचिन्हित पूर्णांक लंबाई)
{
	u8_t योग = 0;
	अचिन्हित पूर्णांक i;
	क्रम (i = 0; i < लंबाई; i++)
		योग += ((अचिन्हित अक्षर *)ptr)[i];
	वापस !योग;
}

पूर्णांक platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					अचिन्हित increment,
					व्योम * buff,
					अचिन्हित आकार,
					phys_bytes * phys_addr,
					पूर्णांक ((* cmp_f)(व्योम *)))
{
	phys_bytes addr;

	क्रम (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, आकार);
		अगर (cmp_f(buff)) {
			अगर (phys_addr)
				*phys_addr = addr;
			वापस 1;
		}
	}
	वापस 0;
}
