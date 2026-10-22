
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

#यदि_परिभाषितम् USE_APIC
#अन्तर्भाव "apic.h"
#यदि_परिभाषितम् USE_WATCHDOG
#अन्तर्भाव "kernel/watchdog.h"
#यदि_अन्तः
#यदि_अन्तः

phys_अष्टकानि video_mem_vaddr = 0;

#परिभाषय HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
स्थिर पूर्णाङ्क nfreepdes = 0;
#परिभाषय MAXFREEPDES	2
स्थिर पूर्णाङ्क freepdes[MAXFREEPDES];

स्थिर u32_t phys_get32(phys_अष्टकानि v);

शून्य mem_clear_mapcache(शून्य)
{
	पूर्णाङ्क i;
	प्रति(i = 0; i < nfreepdes; i++) {
		संरचना proc *ptproc = get_cpulocal_var(ptproc);
		पूर्णाङ्क pde = freepdes[i];
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
	प्रतिज्ञा(pde >= 0 && pde < 1024);

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
		प्रतिज्ञा(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} अन्यथा {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_आवरणम्_4_MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	यदि(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*परिवर्तितम् = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	विस्थापनम् = linaddr & I386_VM_विस्थापनम्_आवरणम्_4_MB; /* Offset in 4MB window. */
	*अष्टकानि = MIN(*अष्टकानि, I386_BIG_PAGE_आकार - विस्थापनम्); 

	/* Return the linear address of the start of the new mapping. */
	प्रत्यागम I386_BIG_PAGE_आकार*pde + विस्थापनम्;
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
	प्रतिज्ञा(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	प्रतिज्ञा(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	यदि(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	यदि(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	प्रतिज्ञा(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
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

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &परिवर्तितम्);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &परिवर्तितम्);
		यदि(परिवर्तितम्)
			reload_cr3(); 

		/* Check for overflow. */
		यदि (srcptr + chunk < srcptr) प्रत्यागम EFAULT_SRC;
		यदि (dstptr + chunk < dstptr) प्रत्यागम EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		यदि(addr) {
			/* If addr is nonzero, a page fault was caught. */

			यदि(addr >= srcptr && addr < (srcptr + chunk)) {
				प्रत्यागम EFAULT_SRC;
			}
			यदि(addr >= dstptr && addr < (dstptr + chunk)) {
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
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

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

#यदि 0
स्थिर वर्ण *cr0_str(u32_t e)
{
	स्थिर वर्ण str[80];
	वाक्यं_प्रतिलिख(str, "");
#परिभाषय FLAG(v) कुरु { यदि(e & (v)) { वाक्यं_योजय(str, #v " "); e &= ~v; } } यावत्(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	यदि(e) { वाक्यं_योजय(str, " (++)"); }
	प्रत्यागम str;
}

स्थिर वर्ण *cr4_str(u32_t e)
{
	स्थिर वर्ण str[80];
	वाक्यं_प्रतिलिख(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	यदि(e) { वाक्यं_योजय(str, " (++)"); }
	प्रत्यागम str;
}
#यदि_अन्तः

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
	मूलम् = (u32_t *) proc->p_seg.p_cr3;
	प्रतिज्ञा(!((u32_t) मूलम् % I386_PAGE_आकार));
	pde = I386_VM_PDE(आभास);
	प्रतिज्ञा(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (मूलम् + pde));

	यदि(!(pde_v & I386_VM_PRESENT)) {
		प्रत्यागम EFAULT;
	}

	/* We don't expect to ever see this. */
	यदि(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_आवरणम्_4_MB;
		यदि(ptent) *ptent = pde_v;
		*physical += आभास & I386_VM_विस्थापनम्_आवरणम्_4_MB;
	} अन्यथा {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		प्रतिज्ञा(!((u32_t) pt % I386_PAGE_आकार));
		pte = I386_VM_PTE(आभास);
		प्रतिज्ञा(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		यदि(!(pte_v & I386_VM_PRESENT)) {
			प्रत्यागम EFAULT;
		}

		यदि(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += आभास % I386_PAGE_आकार;
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

	len = I386_PAGE_आकार - (vir_addr % I386_PAGE_आकार);
	vir_addr += len;
	अग्र_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	यावत् (len < अष्टकानि) {
		यदि (vm_lookup(proc, vir_addr, &phys, रिक्त) != OK)
			भङ्ग;

		यदि (अग्र_phys != phys)
			भङ्ग;

		len += I386_PAGE_आकार;
		vir_addr += I386_PAGE_आकार;
		अग्र_phys += I386_PAGE_आकार;
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

#यदि 0
स्थिर वर्ण *flagstr(u32_t e, ध्रुव पूर्णाङ्क dir)
{
	स्थिर वर्ण str[80];
	वाक्यं_प्रतिलिख(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	यदि(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	अन्यथा
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	प्रत्यागम str;
}

स्थिर शून्य vm_pt_print(u32_t *pagetable, ध्रुव u32_t v)
{
	पूर्णाङ्क pte;
	पूर्णाङ्क col = 0;

	प्रतिज्ञा(!((u32_t) pagetable % I386_PAGE_आकार));

	प्रति(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		यदि(!(pte_v & I386_VM_PRESENT))
			अनुवर्त;
		pfa = I386_VM_PFA(pte_v);
		प्रारूप_लिख("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_आकार*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		यदि(col == 3) { प्रारूप_लिख("\n"); col = 0; }
	}
	यदि(col > 0) प्रारूप_लिख("\n");

	प्रत्यागम;
}

स्थिर शून्य vm_print(u32_t *मूलम्)
{
	पूर्णाङ्क pde;

	प्रतिज्ञा(!((u32_t) मूलम् % I386_PAGE_आकार));

	प्रारूप_लिख("page table 0x%lx:\n", मूलम्);

	प्रति(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (मूलम् + pde));
		यदि(!(pde_v & I386_VM_PRESENT))
			अनुवर्त;
		यदि(pde_v & I386_VM_BIGPAGE) {
			प्रारूप_लिख("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} अन्यथा {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			प्रारूप_लिख("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_आकार);
			प्रारूप_लिख("\n");
		}
	}


	प्रत्यागम;
}
#यदि_अन्तः

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
	पूर्णाङ्क new_cr3, r = OK;

	यदि ((r = check_resumed_caller(caller)) != OK)
		प्रत्यागम r;

	/* NONE for physical, otherwise virtual */
	यदि (who != NONE && !(whoptr = endpoint_lookup(who)))
		प्रत्यागम ESRCH;

	c &= 0xFF;
	आदर्शः = c | (c << 8) | (c << 16) | (c << 24);

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	प्रतिज्ञा(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	यावत् (वामः > 0) {
		new_cr3 = 0;
		chunk = वामः;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		यदि (new_cr3)
			reload_cr3();

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

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	प्रतिज्ञा(catch_pagefaults);
	catch_pagefaults = 0;

	प्रत्यागम OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
पूर्णाङ्क virtual_copy_f(
  संरचना proc * caller,
  संरचना vir_addr *src_addr,	/* source virtual address */
  संरचना vir_addr *dst_addr,	/* destination virtual address */
  vir_अष्टकानि अष्टकानि,		/* # of bytes to copy  */
  पूर्णाङ्क vmcheck			/* if nonzero, can return VMSUSPEND */
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

	प्रतिज्ञा(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
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
	strlcpy(pr->p_name, name, परिमाण(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

स्थिर पूर्णाङ्क oxpcie_mapping_सूचकांक = -1,
	lapic_mapping_सूचकांक = -1,
	ioapic_first_सूचकांक = -1,
	ioapic_चरमः_सूचकांक = -1,
	video_mem_mapping_सूचकांक = -1,
	usermapped_glo_सूचकांक = -1,
	usermapped_सूचकांक = -1, first_um_idx = -1;

बाह्य वर्ण *video_mem;

बाह्य वर्ण usermapped_start, usermapped_अन्तः, usermapped_nonglo_start;

पूर्णाङ्क arch_phys_map(ध्रुव पूर्णाङ्क सूचकांक,
			phys_अष्टकानि *addr,
			phys_अष्टकानि *len,
			पूर्णाङ्क *flags)
{
	स्थिर पूर्णाङ्क first = 1;
	पूर्णाङ्क freeidx = 0;
	स्थिर वर्ण *ser_var = रिक्त;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	यदि(first) {
		स्मृतिं_पूरय(&minix_kerninfo, 0, परिमाण(minix_kerninfo));
		video_mem_mapping_सूचकांक = freeidx++;
		यदि(glo_len > 0) {
			usermapped_glo_सूचकांक = freeidx++;
		}
		
		usermapped_सूचकांक = freeidx++;
		first_um_idx = usermapped_सूचकांक;
		यदि(usermapped_glo_सूचकांक != -1)
			first_um_idx = usermapped_glo_सूचकांक;

#यदि_परिभाषितम् USE_APIC
		यदि(lapic_addr)
			lapic_mapping_सूचकांक = freeidx++;
		यदि (ioapic_enabled) {
			ioapic_first_सूचकांक = freeidx;
			प्रतिज्ञा(nioapics > 0);
			freeidx += nioapics;
			ioapic_चरमः_सूचकांक = freeidx-1;
		}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_OXPCIE
		यदि((ser_var = env_get("oxpcie"))) {
			यदि(ser_var[0] != '0' || ser_var[1] != 'x') {
				प्रारूप_लिख("oxpcie address in hex please\n");
			} अन्यथा {
				प्रारूप_लिख("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_सूचकांक = freeidx++;
			}
		}
#यदि_अन्तः

		first = 0;
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
	अन्यथा यदि (सूचकांक == video_mem_mapping_सूचकांक) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_बफर;
		*len = I386_PAGE_आकार;
		*flags = VMMF_WRITE;
		प्रत्यागम OK;
	}
#यदि_परिभाषितम् USE_APIC
	अन्यथा यदि (सूचकांक == lapic_mapping_सूचकांक) {
		/* map the local APIC if enabled */
		यदि (!lapic_addr)
			प्रत्यागम EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		प्रत्यागम OK;
	}
	अन्यथा यदि (ioapic_enabled && सूचकांक >= ioapic_first_सूचकांक && सूचकांक <= ioapic_चरमः_सूचकांक) {
		पूर्णाङ्क ioapic_idx = सूचकांक - ioapic_first_सूचकांक;
		*addr = io_apic[ioapic_idx].paddr;
		प्रतिज्ञा(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		प्रारूप_लिख("ioapic map: addr 0x%lx\n", *addr);
		प्रत्यागम OK;
	}
#यदि_अन्तः

#यदि CONFIG_OXPCIE
	यदि(सूचकांक == oxpcie_mapping_सूचकांक) {
		*addr = strtoul(ser_var+2, रिक्त, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		प्रत्यागम OK;
	}
#यदि_अन्तः

	प्रत्यागम EINVAL;
}

पूर्णाङ्क arch_phys_map_प्रत्युत्तरम्(ध्रुव पूर्णाङ्क सूचकांक, ध्रुव vir_अष्टकानि addr)
{
#यदि_परिभाषितम् USE_APIC
	/* if local APIC is enabled */
	यदि (सूचकांक == lapic_mapping_सूचकांक && lapic_addr) {
		lapic_addr_vaddr = addr;
		प्रत्यागम OK;
	}
	अन्यथा यदि (ioapic_enabled && सूचकांक >= ioapic_first_सूचकांक &&
		सूचकांक <= ioapic_चरमः_सूचकांक) {
		पूर्णाङ्क i = सूचकांक - ioapic_first_सूचकांक;
		io_apic[i].vaddr = addr;
		प्रत्यागम OK;
	}
#यदि_अन्तः

#यदि CONFIG_OXPCIE
	यदि (सूचकांक == oxpcie_mapping_सूचकांक) {
		oxpcie_समुच्चयः_vaddr((अचिह्नित वर्ण *) addr);
		प्रत्यागम OK;
	}
#यदि_अन्तः
	यदि(सूचकांक == first_um_idx) {
		बाह्य संरचना minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		बाह्य u32_t usermapped_विस्थापनम्;
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
		नियोजय(arm_frclock); /* eh, why not. */
		नियोजय(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		यदि(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} अन्यथा  यदि(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
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
		minix_kerninfo_user = (vir_अष्टकानि) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		यदि(env_get("libc_ipc")) {
			प्रारूप_लिख("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = रिक्त;
		} अन्यथा {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		प्रत्यागम OK;
	}

	यदि(सूचकांक == usermapped_सूचकांक) प्रत्यागम OK;

	यदि (सूचकांक == video_mem_mapping_सूचकांक) {
		video_mem_vaddr =  addr;
		प्रत्यागम OK;
	}

	प्रत्यागम EINVAL;
}

पूर्णाङ्क arch_enable_paging(संरचना proc * caller)
{
	प्रतिज्ञा(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_स्थानाङ्कः_space(caller);

	video_mem = (वर्ण *) video_mem_vaddr;

#यदि_परिभाषितम् USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	यदि (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	यदि (ioapic_enabled) {
		पूर्णाङ्क i;

		प्रति (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#यदि CONFIG_SMP
	barrier();

	wait_for_APs_गन्तव्यम्_finish_booting();
#यदि_अन्तः
#यदि_अन्तः

#यदि_परिभाषितम् USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	यदि (watchdog_enabled)
		i386_watchdog_start();
#यदि_अन्तः

	प्रत्यागम OK;
}

शून्य release_स्थानाङ्कः_space(संरचना proc *pr)
{
	pr->p_seg.p_cr3_v = रिक्त;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
पूर्णाङ्क platform_tbl_checksum_ok(शून्य *ptr, अचिह्नित पूर्णाङ्क दीर्घता)
{
	u8_t योग = 0;
	अचिह्नित पूर्णाङ्क i;
	प्रति (i = 0; i < दीर्घता; i++)
		योग += ((अचिह्नित वर्ण *)ptr)[i];
	प्रत्यागम !योग;
}

पूर्णाङ्क platform_tbl_ptr(phys_अष्टकानि start,
					phys_अष्टकानि अन्तः_2,
					अचिह्नित increment,
					शून्य * buff,
					अचिह्नित आकार,
					phys_अष्टकानि * phys_addr,
					पूर्णाङ्क ((* cmp_f)(शून्य *)))
{
	phys_अष्टकानि addr;

	प्रति (addr = start; addr < अन्तः_2; addr += increment) {
		phys_copy (addr, (phys_अष्टकानि) buff, आकार);
		यदि (cmp_f(buff)) {
			यदि (phys_addr)
				*phys_addr = addr;
			प्रत्यागम 1;
		}
	}
	प्रत्यागम 0;
}
