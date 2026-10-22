
#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "kernel/vm.h"

#अन्तर्भाव <machine/vm.h>

#अन्तर्भाव <minix/syslib.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdlib.h>

#अन्तर्भाव <machine/vm.h>

#अन्तर्भाव "oxpcie.h"
#अन्तर्भाव "arch_proto.h"

#ifdef USE_APIC
#अन्तर्भाव "apic.h"
#ifdef USE_WATCHDOG
#अन्तर्भाव "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
स्थिर पूर्णांक nfreepdes = 0;
#define MAXFREEPDES	2
स्थिर पूर्णांक freepdes[MAXFREEPDES];

स्थिर u32_t phys_get32(phys_bytes v);

शून्य mem_clear_mapcache(शून्य)
{
	पूर्णांक i;
	साठी(i = 0; i < nfreepdes; i++) {
		संरचना proc *ptproc = get_cpulocal_var(ptproc);
		पूर्णांक pde = freepdes[i];
		u32_t *ptv;
		प्रतिज्ञा(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		प्रतिज्ञा(ptv);
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
	प्रतिज्ञा(pde >= 0 && pde < 1024);

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
		प्रतिज्ञा(pr->p_seg.p_cr3_v);
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
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	जर(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
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
	परत I386_BIG_PAGE_SIZE*pde + offset;
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
	प्रतिज्ञा(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	प्रतिज्ञा(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	जर(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	जर(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	प्रतिज्ञा(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
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

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		जर(changed)
			reload_cr3(); 

		/* Check for overflow. */
		जर (srcptr + chunk < srcptr) परत EFAULT_SRC;
		जर (dstptr + chunk < dstptr) परत EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		जर(addr) {
			/* If addr is nonzero, a page fault was caught. */

			जर(addr >= srcptr && addr < (srcptr + chunk)) {
				परत EFAULT_SRC;
			}
			जर(addr >= dstptr && addr < (dstptr + chunk)) {
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
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

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

#जर 0
स्थिर अक्षर *cr0_str(u32_t e)
{
	स्थिर अक्षर str[80];
	म_नकल(str, "");
#define FLAG(v) करा { जर(e & (v)) { म_जोड़ो(str, #v " "); e &= ~v; } } पर्यंत(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	जर(e) { म_जोड़ो(str, " (++)"); }
	परत str;
}

स्थिर अक्षर *cr4_str(u32_t e)
{
	स्थिर अक्षर str[80];
	म_नकल(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	जर(e) { म_जोड़ो(str, " (++)"); }
	परत str;
}
#endif

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
	मूळ = (u32_t *) proc->p_seg.p_cr3;
	प्रतिज्ञा(!((u32_t) मूळ % I386_PAGE_SIZE));
	pde = I386_VM_PDE(आभासी);
	प्रतिज्ञा(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (मूळ + pde));

	जर(!(pde_v & I386_VM_PRESENT)) {
		परत EFAULT;
	}

	/* We don't expect to ever see this. */
	जर(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		जर(ptent) *ptent = pde_v;
		*physical += आभासी & I386_VM_OFFSET_MASK_4MB;
	} अन्यथा {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		प्रतिज्ञा(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(आभासी);
		प्रतिज्ञा(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		जर(!(pte_v & I386_VM_PRESENT)) {
			परत EFAULT;
		}

		जर(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += आभासी % I386_PAGE_SIZE;
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

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	पर्यंत (len < bytes) {
		जर (vm_lookup(proc, vir_addr, &phys, रिक्त) != OK)
			थांबा;

		जर (next_phys != phys)
			थांबा;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
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

#जर 0
स्थिर अक्षर *flagstr(u32_t e, स्थिरांक पूर्णांक dir)
{
	स्थिर अक्षर str[80];
	म_नकल(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	जर(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	अन्यथा
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	परत str;
}

स्थिर शून्य vm_pt_print(u32_t *pagetable, स्थिरांक u32_t v)
{
	पूर्णांक pte;
	पूर्णांक col = 0;

	प्रतिज्ञा(!((u32_t) pagetable % I386_PAGE_SIZE));

	साठी(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		जर(!(pte_v & I386_VM_PRESENT))
			सुरूठेवा;
		pfa = I386_VM_PFA(pte_v);
		म_लिखो("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		जर(col == 3) { म_लिखो("\n"); col = 0; }
	}
	जर(col > 0) म_लिखो("\n");

	परत;
}

स्थिर शून्य vm_print(u32_t *मूळ)
{
	पूर्णांक pde;

	प्रतिज्ञा(!((u32_t) मूळ % I386_PAGE_SIZE));

	म_लिखो("page table 0x%lx:\n", मूळ);

	साठी(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (मूळ + pde));
		जर(!(pde_v & I386_VM_PRESENT))
			सुरूठेवा;
		जर(pde_v & I386_VM_BIGPAGE) {
			म_लिखो("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} अन्यथा {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			म_लिखो("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			म_लिखो("\n");
		}
	}


	परत;
}
#endif

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
	पूर्णांक new_cr3, r = OK;

	जर ((r = check_resumed_caller(caller)) != OK)
		परत r;

	/* NONE for physical, otherwise virtual */
	जर (who != NONE && !(whoptr = endpoint_lookup(who)))
		परत ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	प्रतिज्ञा(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	पर्यंत (डावा > 0) {
		new_cr3 = 0;
		chunk = डावा;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		जर (new_cr3)
			reload_cr3();

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

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	प्रतिज्ञा(catch_pagefaults);
	catch_pagefaults = 0;

	परत OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
पूर्णांक virtual_copy_f(
  संरचना proc * caller,
  संरचना vir_addr *src_addr,	/* source virtual address */
  संरचना vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  पूर्णांक vmcheck			/* if nonzero, can return VMSUSPEND */
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

	प्रतिज्ञा(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
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
	strlcpy(pr->p_name, name, आकार(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

स्थिर पूर्णांक oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

बाह्य अक्षर *video_mem;

बाह्य अक्षर usermapped_start, usermapped_end, usermapped_nonglo_start;

पूर्णांक arch_phys_map(स्थिरांक पूर्णांक index,
			phys_bytes *addr,
			phys_bytes *len,
			पूर्णांक *flags)
{
	स्थिर पूर्णांक first = 1;
	पूर्णांक freeidx = 0;
	स्थिर अक्षर *ser_var = रिक्त;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	जर(first) {
		स_रखो(&minix_kerninfo, 0, आकार(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		जर(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		जर(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		जर(lapic_addr)
			lapic_mapping_index = freeidx++;
		जर (ioapic_enabled) {
			ioapic_first_index = freeidx;
			प्रतिज्ञा(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		जर((ser_var = env_get("oxpcie"))) {
			जर(ser_var[0] != '0' || ser_var[1] != 'x') {
				म_लिखो("oxpcie address in hex please\n");
			} अन्यथा {
				म_लिखो("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
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
	अन्यथा जर (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		परत OK;
	}
#ifdef USE_APIC
	अन्यथा जर (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		जर (!lapic_addr)
			परत EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		परत OK;
	}
	अन्यथा जर (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		पूर्णांक ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		प्रतिज्ञा(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		म_लिखो("ioapic map: addr 0x%lx\n", *addr);
		परत OK;
	}
#endif

#जर CONFIG_OXPCIE
	जर(index == oxpcie_mapping_index) {
		*addr = म_से_अदीर्घ(ser_var+2, रिक्त, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		परत OK;
	}
#endif

	परत EINVAL;
}

पूर्णांक arch_phys_map_reply(स्थिरांक पूर्णांक index, स्थिरांक vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	जर (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		परत OK;
	}
	अन्यथा जर (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		पूर्णांक i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		परत OK;
	}
#endif

#जर CONFIG_OXPCIE
	जर (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((अचिन्हित अक्षर *) addr);
		परत OK;
	}
#endif
	जर(index == first_um_idx) {
		बाह्य संरचना minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		बाह्य u32_t usermapped_offset;
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
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		जर(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} अन्यथा  जर(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
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
		जर(env_get("libc_ipc")) {
			म_लिखो("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = रिक्त;
		} अन्यथा {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		परत OK;
	}

	जर(index == usermapped_index) परत OK;

	जर (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		परत OK;
	}

	परत EINVAL;
}

पूर्णांक arch_enable_paging(संरचना proc * caller)
{
	प्रतिज्ञा(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (अक्षर *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	जर (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	जर (ioapic_enabled) {
		पूर्णांक i;

		साठी (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#जर CONFIG_SMP
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
	जर (watchdog_enabled)
		i386_watchdog_start();
#endif

	परत OK;
}

शून्य release_address_space(संरचना proc *pr)
{
	pr->p_seg.p_cr3_v = रिक्त;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
पूर्णांक platform_tbl_checksum_ok(शून्य *ptr, अचिन्हित पूर्णांक length)
{
	u8_t total = 0;
	अचिन्हित पूर्णांक i;
	साठी (i = 0; i < length; i++)
		total += ((अचिन्हित अक्षर *)ptr)[i];
	परत !total;
}

पूर्णांक platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					अचिन्हित increment,
					शून्य * buff,
					अचिन्हित size,
					phys_bytes * phys_addr,
					पूर्णांक ((* cmp_f)(शून्य *)))
{
	phys_bytes addr;

	साठी (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		जर (cmp_f(buff)) {
			जर (phys_addr)
				*phys_addr = addr;
			परत 1;
		}
	}
	परत 0;
}
