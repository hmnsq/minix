
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
स्थिर पूर्णाङ्क nfreepdes = 0;
#define MAXFREEPDES	2
स्थिर पूर्णाङ्क freepdes[MAXFREEPDES];

स्थिर u32_t phys_get32(phys_bytes v);

शून्य mem_clear_mapcache(शून्य)
{
	पूर्णाङ्क i;
	लागि(i = 0; i < nfreepdes; i++) {
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
स्थिर phys_bytes createpde(
	अचर संरचना proc *pr,	/* Requested process, NULL for physical. */
	अचर phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	पूर्णाङ्क free_pde_idx,	/* index of the free slot to use */
	पूर्णाङ्क *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
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
		फर्काऊ linaddr;
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
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
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
	फर्काऊ I386_BIG_PAGE_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
स्थिर पूर्णाङ्क check_resumed_caller(संरचना proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	यदि (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		प्रतिज्ञा(caller->p_vmrequest.vmresult != VMSUSPEND);
		फर्काऊ caller->p_vmrequest.vmresult;
	}

	फर्काऊ OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
स्थिर पूर्णाङ्क lin_lin_copy(संरचना proc *srcproc, vir_bytes srclinaddr, 
	संरचना proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
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

	जबसम्म(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		पूर्णाङ्क changed = 0;

#ifdef CONFIG_SMP
		अचिन्हित cpu = cpuid;

		यदि (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		यदि (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		यदि(changed)
			reload_cr3(); 

		/* Check for overflow. */
		यदि (srcptr + chunk < srcptr) फर्काऊ EFAULT_SRC;
		यदि (dstptr + chunk < dstptr) फर्काऊ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		यदि(addr) {
			/* If addr is nonzero, a page fault was caught. */

			यदि(addr >= srcptr && addr < (srcptr + chunk)) {
				फर्काऊ EFAULT_SRC;
			}
			यदि(addr >= dstptr && addr < (dstptr + chunk)) {
				फर्काऊ EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			फर्काऊ EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	यदि(srcproc) प्रतिज्ञा(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	यदि(dstproc) प्रतिज्ञा(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	प्रतिज्ञा(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	फर्काऊ OK;
}


स्थिर u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	पूर्णाङ्क r;

	यदि((r=lin_lin_copy(रिक्त, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, परिमाण(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	फर्काऊ v;
}

#यदि 0
स्थिर वर्ण *cr0_str(u32_t e)
{
	स्थिर वर्ण str[80];
	म_नकल(str, "");
#define FLAG(v) गर { यदि(e & (v)) { म_जोड़ो(str, #v " "); e &= ~v; } } जबसम्म(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	यदि(e) { म_जोड़ो(str, " (++)"); }
	फर्काऊ str;
}

स्थिर वर्ण *cr4_str(u32_t e)
{
	स्थिर वर्ण str[80];
	म_नकल(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	यदि(e) { म_जोड़ो(str, " (++)"); }
	फर्काऊ str;
}
#endif

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  पञ्जिका संरचना proc *rp,		/* pointer to proc table entry for process */
  पूर्णाङ्क seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	यदि(vm_lookup(rp, vir_addr, &phys, रिक्त) != OK) {
		म_लिखो("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} अन्यथा {
		यदि(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	यदि(phys == 0) {
		म_लिखो("SYSTEM:umap_virtual: lookup failed\n");
		फर्काऊ 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	यदि(bytes > 0 && vm_lookup_range(rp, vir_addr, रिक्त, bytes) != bytes) {
		म_लिखो("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		फर्काऊ 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	प्रतिज्ञा(phys);

	फर्काऊ phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
पूर्णाङ्क vm_lookup(अचर संरचना proc *proc, अचर vir_bytes आभास,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *जरा, *pt;
	पूर्णाङ्क pde, pte;
	u32_t pde_v, pte_v;

	प्रतिज्ञा(proc);
	प्रतिज्ञा(physical);
	प्रतिज्ञा(!isemptyp(proc));
	प्रतिज्ञा(HASPT(proc));

	/* Retrieve page directory entry. */
	जरा = (u32_t *) proc->p_seg.p_cr3;
	प्रतिज्ञा(!((u32_t) जरा % I386_PAGE_SIZE));
	pde = I386_VM_PDE(आभास);
	प्रतिज्ञा(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (जरा + pde));

	यदि(!(pde_v & I386_VM_PRESENT)) {
		फर्काऊ EFAULT;
	}

	/* We don't expect to ever see this. */
	यदि(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		यदि(ptent) *ptent = pde_v;
		*physical += आभास & I386_VM_OFFSET_MASK_4MB;
	} अन्यथा {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		प्रतिज्ञा(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(आभास);
		प्रतिज्ञा(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		यदि(!(pte_v & I386_VM_PRESENT)) {
			फर्काऊ EFAULT;
		}

		यदि(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += आभास % I386_PAGE_SIZE;
	}

	फर्काऊ OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
माप_प्रकार vm_lookup_range(अचर संरचना proc *proc, vir_bytes vir_addr,
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
	यदि (vm_lookup(proc, vir_addr, &phys, रिक्त) != OK)
		फर्काऊ 0;

	यदि (phys_addr != रिक्त)
		*phys_addr = phys;

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	जबसम्म (len < bytes) {
		यदि (vm_lookup(proc, vir_addr, &phys, रिक्त) != OK)
			रोक;

		यदि (next_phys != phys)
			रोक;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	फर्काऊ MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
पूर्णाङ्क vm_check_range(संरचना proc *caller, संरचना proc *target,
	vir_bytes vir_addr, माप_प्रकार bytes, पूर्णाङ्क writeflag)
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
		फर्काऊ r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	फर्काऊ VMSUSPEND;
}

#यदि 0
स्थिर वर्ण *flagstr(u32_t e, अचर पूर्णाङ्क dir)
{
	स्थिर वर्ण str[80];
	म_नकल(str, "");
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
	फर्काऊ str;
}

स्थिर शून्य vm_pt_print(u32_t *pagetable, अचर u32_t v)
{
	पूर्णाङ्क pte;
	पूर्णाङ्क col = 0;

	प्रतिज्ञा(!((u32_t) pagetable % I386_PAGE_SIZE));

	लागि(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		यदि(!(pte_v & I386_VM_PRESENT))
			जारी;
		pfa = I386_VM_PFA(pte_v);
		म_लिखो("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		यदि(col == 3) { म_लिखो("\n"); col = 0; }
	}
	यदि(col > 0) म_लिखो("\n");

	फर्काऊ;
}

स्थिर शून्य vm_print(u32_t *जरा)
{
	पूर्णाङ्क pde;

	प्रतिज्ञा(!((u32_t) जरा % I386_PAGE_SIZE));

	म_लिखो("page table 0x%lx:\n", जरा);

	लागि(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (जरा + pde));
		यदि(!(pde_v & I386_VM_PRESENT))
			जारी;
		यदि(pde_v & I386_VM_BIGPAGE) {
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


	फर्काऊ;
}
#endif

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
पूर्णाङ्क vm_memset(संरचना proc* caller, endpoint_t who, phys_bytes ph, पूर्णाङ्क c,
	phys_bytes गन्ती)
{
	u32_t pattern;
	संरचना proc *whoptr = रिक्त;
	phys_bytes cur_ph = ph;
	phys_bytes बायाँ = गन्ती;
	phys_bytes ptr, chunk, pfa = 0;
	पूर्णाङ्क new_cr3, r = OK;

	यदि ((r = check_resumed_caller(caller)) != OK)
		फर्काऊ r;

	/* NONE for physical, otherwise virtual */
	यदि (who != NONE && !(whoptr = endpoint_lookup(who)))
		फर्काऊ ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	प्रतिज्ञा(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	जबसम्म (बायाँ > 0) {
		new_cr3 = 0;
		chunk = बायाँ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		यदि (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		यदि ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			यदि (whoptr) {
				vm_suspend(caller, whoptr, ph, गन्ती,
						   VMSTYPE_KERNELCALL, 1);
				प्रतिज्ञा(catch_pagefaults);
				catch_pagefaults = 0;
				फर्काऊ VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		बायाँ -= chunk;
	}

	प्रतिज्ञा(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	प्रतिज्ञा(catch_pagefaults);
	catch_pagefaults = 0;

	फर्काऊ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
पूर्णाङ्क virtual_copy_f(
  संरचना proc * caller,
  संरचना vir_addr *src_addr,	/* source virtual address */
  संरचना vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  पूर्णाङ्क vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  संरचना vir_addr *vir_addr[2];	/* virtual source and destination address */
  पूर्णाङ्क i, r;
  संरचना proc *procs[2];

  प्रतिज्ञा((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  यदि (bytes <= 0) फर्काऊ(गलत_तर्क);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  लागि (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	पूर्णाङ्क proc_nr;
	संरचना proc *p;

	यदि(proc_e == NONE) {
		p = रिक्त;
	} अन्यथा {
		यदि(!isokendpt(proc_e, &proc_nr)) {
			म_लिखो("virtual_copy: no reasonable endpoint\n");
			फर्काऊ ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  यदि ((r = check_resumed_caller(caller)) != OK)
	फर्काऊ r;

  यदि((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	पूर्णाङ्क writeflag;
  	संरचना proc *target = रिक्त;
  	phys_bytes lin;
  	यदि(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	यदि(!vmcheck || !caller) {
    		फर्काऊ r;
  	}

  	यदि(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} अन्यथा यदि(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} अन्यथा {
  		panic("r strange: %d",  r);
  	}

	प्रतिज्ञा(caller);
	प्रतिज्ञा(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	फर्काऊ VMSUSPEND;
  }

  फर्काऊ OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
पूर्णाङ्क data_copy(अचर endpoint_t from_proc, अचर vir_bytes from_addr,
	अचर endpoint_t to_proc, अचर vir_bytes to_addr,
	माप_प्रकार bytes)
{
  संरचना vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  प्रतिज्ञा(src.proc_nr_e != NONE);
  प्रतिज्ञा(dst.proc_nr_e != NONE);

  फर्काऊ virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
पूर्णाङ्क data_copy_vmcheck(संरचना proc * caller,
	अचर endpoint_t from_proc, अचर vir_bytes from_addr,
	अचर endpoint_t to_proc, अचर vir_bytes to_addr,
	माप_प्रकार bytes)
{
  संरचना vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  प्रतिज्ञा(src.proc_nr_e != NONE);
  प्रतिज्ञा(dst.proc_nr_e != NONE);

  फर्काऊ virtual_copy_vmcheck(caller, &src, &dst, bytes);
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
शून्य arch_proc_init(संरचना proc *pr, अचर u32_t ip, अचर u32_t sp,
	अचर u32_t ps_str, वर्ण *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, परिमाण(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

स्थिर पूर्णाङ्क oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

बाह्य वर्ण *video_mem;

बाह्य वर्ण usermapped_start, usermapped_end, usermapped_nonglo_start;

पूर्णाङ्क arch_phys_map(अचर पूर्णाङ्क index,
			phys_bytes *addr,
			phys_bytes *len,
			पूर्णाङ्क *flags)
{
	स्थिर पूर्णाङ्क first = 1;
	पूर्णाङ्क freeidx = 0;
	स्थिर वर्ण *ser_var = रिक्त;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	यदि(first) {
		स_रखो(&minix_kerninfo, 0, परिमाण(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		यदि(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		यदि(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		यदि(lapic_addr)
			lapic_mapping_index = freeidx++;
		यदि (ioapic_enabled) {
			ioapic_first_index = freeidx;
			प्रतिज्ञा(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		यदि((ser_var = env_get("oxpcie"))) {
			यदि(ser_var[0] != '0' || ser_var[1] != 'x') {
				म_लिखो("oxpcie address in hex please\n");
			} अन्यथा {
				म_लिखो("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
	}

	यदि(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		फर्काऊ OK;
	}
	अन्यथा यदि(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		फर्काऊ OK;
	}
	अन्यथा यदि (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		फर्काऊ OK;
	}
#ifdef USE_APIC
	अन्यथा यदि (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		यदि (!lapic_addr)
			फर्काऊ EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		फर्काऊ OK;
	}
	अन्यथा यदि (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		पूर्णाङ्क ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		प्रतिज्ञा(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		म_लिखो("ioapic map: addr 0x%lx\n", *addr);
		फर्काऊ OK;
	}
#endif

#यदि CONFIG_OXPCIE
	यदि(index == oxpcie_mapping_index) {
		*addr = म_से_अदीर्घ(ser_var+2, रिक्त, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		फर्काऊ OK;
	}
#endif

	फर्काऊ EINVAL;
}

पूर्णाङ्क arch_phys_map_reply(अचर पूर्णाङ्क index, अचर vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	यदि (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		फर्काऊ OK;
	}
	अन्यथा यदि (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		पूर्णाङ्क i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		फर्काऊ OK;
	}
#endif

#यदि CONFIG_OXPCIE
	यदि (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((अचिन्हित वर्ण *) addr);
		फर्काऊ OK;
	}
#endif
	यदि(index == first_um_idx) {
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
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		यदि(env_get("libc_ipc")) {
			म_लिखो("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = रिक्त;
		} अन्यथा {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		फर्काऊ OK;
	}

	यदि(index == usermapped_index) फर्काऊ OK;

	यदि (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		फर्काऊ OK;
	}

	फर्काऊ EINVAL;
}

पूर्णाङ्क arch_enable_paging(संरचना proc * caller)
{
	प्रतिज्ञा(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (वर्ण *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	यदि (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	यदि (ioapic_enabled) {
		पूर्णाङ्क i;

		लागि (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#यदि CONFIG_SMP
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
	यदि (watchdog_enabled)
		i386_watchdog_start();
#endif

	फर्काऊ OK;
}

शून्य release_address_space(संरचना proc *pr)
{
	pr->p_seg.p_cr3_v = रिक्त;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
पूर्णाङ्क platform_tbl_checksum_ok(शून्य *ptr, अचिन्हित पूर्णाङ्क length)
{
	u8_t total = 0;
	अचिन्हित पूर्णाङ्क i;
	लागि (i = 0; i < length; i++)
		total += ((अचिन्हित वर्ण *)ptr)[i];
	फर्काऊ !total;
}

पूर्णाङ्क platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					अचिन्हित increment,
					शून्य * buff,
					अचिन्हित size,
					phys_bytes * phys_addr,
					पूर्णाङ्क ((* cmp_f)(शून्य *)))
{
	phys_bytes addr;

	लागि (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		यदि (cmp_f(buff)) {
			यदि (phys_addr)
				*phys_addr = addr;
			फर्काऊ 1;
		}
	}
	फर्काऊ 0;
}
