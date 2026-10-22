
#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "kernel/vm.h"

#അന്തര്ഭാവ <machine/vm.h>

#അന്തര്ഭാവ <minix/syslib.h>
#അന്തര്ഭാവ <minix/cpufeature.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ <machine/vm.h>

#അന്തര്ഭാവ "oxpcie.h"
#അന്തര്ഭാവ "arch_proto.h"

#ifdef USE_APIC
#അന്തര്ഭാവ "apic.h"
#ifdef USE_WATCHDOG
#അന്തര്ഭാവ "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
സ്ഥിതം പൂർണ്ണസംഖ്യ nfreepdes = 0;
#define MAXFREEPDES	2
സ്ഥിതം പൂർണ്ണസംഖ്യ freepdes[MAXFREEPDES];

സ്ഥിതം u32_t phys_get32(phys_bytes v);

ശൂന്യം mem_clear_mapcache(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ i;
	വേണ്ടി(i = 0; i < nfreepdes; i++) {
		ഘടന proc *ptproc = get_cpulocal_var(ptproc);
		പൂർണ്ണസംഖ്യ pde = freepdes[i];
		u32_t *ptv;
		പ്രതിജ്ഞാ(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		പ്രതിജ്ഞാ(ptv);
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
	പ്രതിജ്ഞാ(pde >= 0 && pde < 1024);

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
		പ്രതിജ്ഞാ(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} അല്ലെങ്കിൽ {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	എങ്കിൽ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
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
	തിരികെ I386_BIG_PAGE_SIZE*pde + offset;
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
	പ്രതിജ്ഞാ(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	പ്രതിജ്ഞാ(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	എങ്കിൽ(srcproc) പ്രതിജ്ഞാ(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	എങ്കിൽ(dstproc) പ്രതിജ്ഞാ(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	പ്രതിജ്ഞാ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
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

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		എങ്കിൽ(changed)
			reload_cr3(); 

		/* Check for overflow. */
		എങ്കിൽ (srcptr + chunk < srcptr) തിരികെ EFAULT_SRC;
		എങ്കിൽ (dstptr + chunk < dstptr) തിരികെ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		എങ്കിൽ(addr) {
			/* If addr is nonzero, a page fault was caught. */

			എങ്കിൽ(addr >= srcptr && addr < (srcptr + chunk)) {
				തിരികെ EFAULT_SRC;
			}
			എങ്കിൽ(addr >= dstptr && addr < (dstptr + chunk)) {
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
	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

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

#എങ്കിൽ 0
സ്ഥിതം അക്ഷരം *cr0_str(u32_t e)
{
	സ്ഥിതം അക്ഷരം str[80];
	strcpy(str, "");
#define FLAG(v) ചെയ്യുക { എങ്കിൽ(e & (v)) { strcat(str, #v " "); e &= ~v; } } ആകുമ്പോൾ(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	എങ്കിൽ(e) { strcat(str, " (++)"); }
	തിരികെ str;
}

സ്ഥിതം അക്ഷരം *cr4_str(u32_t e)
{
	സ്ഥിതം അക്ഷരം str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	എങ്കിൽ(e) { strcat(str, " (++)"); }
	തിരികെ str;
}
#endif

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
	വേര് = (u32_t *) proc->p_seg.p_cr3;
	പ്രതിജ്ഞാ(!((u32_t) വേര് % I386_PAGE_SIZE));
	pde = I386_VM_PDE(മായാ);
	പ്രതിജ്ഞാ(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (വേര് + pde));

	എങ്കിൽ(!(pde_v & I386_VM_PRESENT)) {
		തിരികെ EFAULT;
	}

	/* We don't expect to ever see this. */
	എങ്കിൽ(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		എങ്കിൽ(ptent) *ptent = pde_v;
		*physical += മായാ & I386_VM_OFFSET_MASK_4MB;
	} അല്ലെങ്കിൽ {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		പ്രതിജ്ഞാ(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(മായാ);
		പ്രതിജ്ഞാ(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		എങ്കിൽ(!(pte_v & I386_VM_PRESENT)) {
			തിരികെ EFAULT;
		}

		എങ്കിൽ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += മായാ % I386_PAGE_SIZE;
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

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ആകുമ്പോൾ (len < bytes) {
		എങ്കിൽ (vm_lookup(proc, vir_addr, &phys, ശൂന്യ) != OK)
			നിർത്തുക;

		എങ്കിൽ (next_phys != phys)
			നിർത്തുക;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
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

#എങ്കിൽ 0
സ്ഥിതം അക്ഷരം *flagstr(u32_t e, സ്ഥിരം പൂർണ്ണസംഖ്യ dir)
{
	സ്ഥിതം അക്ഷരം str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	എങ്കിൽ(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	അല്ലെങ്കിൽ
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	തിരികെ str;
}

സ്ഥിതം ശൂന്യം vm_pt_print(u32_t *pagetable, സ്ഥിരം u32_t v)
{
	പൂർണ്ണസംഖ്യ pte;
	പൂർണ്ണസംഖ്യ col = 0;

	പ്രതിജ്ഞാ(!((u32_t) pagetable % I386_PAGE_SIZE));

	വേണ്ടി(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		എങ്കിൽ(!(pte_v & I386_VM_PRESENT))
			തുടരുക;
		pfa = I386_VM_PFA(pte_v);
		printf("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		എങ്കിൽ(col == 3) { printf("\n"); col = 0; }
	}
	എങ്കിൽ(col > 0) printf("\n");

	തിരികെ;
}

സ്ഥിതം ശൂന്യം vm_print(u32_t *വേര്)
{
	പൂർണ്ണസംഖ്യ pde;

	പ്രതിജ്ഞാ(!((u32_t) വേര് % I386_PAGE_SIZE));

	printf("page table 0x%lx:\n", വേര്);

	വേണ്ടി(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (വേര് + pde));
		എങ്കിൽ(!(pde_v & I386_VM_PRESENT))
			തുടരുക;
		എങ്കിൽ(pde_v & I386_VM_BIGPAGE) {
			printf("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} അല്ലെങ്കിൽ {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			printf("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			printf("\n");
		}
	}


	തിരികെ;
}
#endif

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
	പൂർണ്ണസംഖ്യ new_cr3, r = OK;

	എങ്കിൽ ((r = check_resumed_caller(caller)) != OK)
		തിരികെ r;

	/* NONE for physical, otherwise virtual */
	എങ്കിൽ (who != NONE && !(whoptr = endpoint_lookup(who)))
		തിരികെ ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	പ്രതിജ്ഞാ(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	ആകുമ്പോൾ (ഇടത് > 0) {
		new_cr3 = 0;
		chunk = ഇടത്;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		എങ്കിൽ (new_cr3)
			reload_cr3();

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

	പ്രതിജ്ഞാ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	പ്രതിജ്ഞാ(catch_pagefaults);
	catch_pagefaults = 0;

	തിരികെ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ virtual_copy_f(
  ഘടന proc * caller,
  ഘടന vir_addr *src_addr,	/* source virtual address */
  ഘടന vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  പൂർണ്ണസംഖ്യ vmcheck			/* if nonzero, can return VMSUSPEND */
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

	പ്രതിജ്ഞാ(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
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
	strlcpy(pr->p_name, name, വലുപ്പം(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

ബാഹ്യ അക്ഷരം *video_mem;

ബാഹ്യ അക്ഷരം usermapped_start, usermapped_end, usermapped_nonglo_start;

പൂർണ്ണസംഖ്യ arch_phys_map(സ്ഥിരം പൂർണ്ണസംഖ്യ index,
			phys_bytes *addr,
			phys_bytes *len,
			പൂർണ്ണസംഖ്യ *flags)
{
	സ്ഥിതം പൂർണ്ണസംഖ്യ first = 1;
	പൂർണ്ണസംഖ്യ freeidx = 0;
	സ്ഥിതം അക്ഷരം *ser_var = ശൂന്യ;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	എങ്കിൽ(first) {
		memset(&minix_kerninfo, 0, വലുപ്പം(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		എങ്കിൽ(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		എങ്കിൽ(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		എങ്കിൽ(lapic_addr)
			lapic_mapping_index = freeidx++;
		എങ്കിൽ (ioapic_enabled) {
			ioapic_first_index = freeidx;
			പ്രതിജ്ഞാ(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		എങ്കിൽ((ser_var = env_get("oxpcie"))) {
			എങ്കിൽ(ser_var[0] != '0' || ser_var[1] != 'x') {
				printf("oxpcie address in hex please\n");
			} അല്ലെങ്കിൽ {
				printf("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
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
	അല്ലെങ്കിൽ എങ്കിൽ (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		തിരികെ OK;
	}
#ifdef USE_APIC
	അല്ലെങ്കിൽ എങ്കിൽ (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		എങ്കിൽ (!lapic_addr)
			തിരികെ EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		തിരികെ OK;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		പൂർണ്ണസംഖ്യ ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		പ്രതിജ്ഞാ(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		printf("ioapic map: addr 0x%lx\n", *addr);
		തിരികെ OK;
	}
#endif

#എങ്കിൽ CONFIG_OXPCIE
	എങ്കിൽ(index == oxpcie_mapping_index) {
		*addr = strtoul(ser_var+2, ശൂന്യ, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		തിരികെ OK;
	}
#endif

	തിരികെ EINVAL;
}

പൂർണ്ണസംഖ്യ arch_phys_map_reply(സ്ഥിരം പൂർണ്ണസംഖ്യ index, സ്ഥിരം vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	എങ്കിൽ (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		തിരികെ OK;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		പൂർണ്ണസംഖ്യ i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		തിരികെ OK;
	}
#endif

#എങ്കിൽ CONFIG_OXPCIE
	എങ്കിൽ (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned അക്ഷരം *) addr);
		തിരികെ OK;
	}
#endif
	എങ്കിൽ(index == first_um_idx) {
		ബാഹ്യ ഘടന minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		ബാഹ്യ u32_t usermapped_offset;
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
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		എങ്കിൽ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} അല്ലെങ്കിൽ  എങ്കിൽ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} അല്ലെങ്കിൽ	{
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
		എങ്കിൽ(env_get("libc_ipc")) {
			printf("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = ശൂന്യ;
		} അല്ലെങ്കിൽ {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		തിരികെ OK;
	}

	എങ്കിൽ(index == usermapped_index) തിരികെ OK;

	എങ്കിൽ (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		തിരികെ OK;
	}

	തിരികെ EINVAL;
}

പൂർണ്ണസംഖ്യ arch_enable_paging(ഘടന proc * caller)
{
	പ്രതിജ്ഞാ(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (അക്ഷരം *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	എങ്കിൽ (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	എങ്കിൽ (ioapic_enabled) {
		പൂർണ്ണസംഖ്യ i;

		വേണ്ടി (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#എങ്കിൽ CONFIG_SMP
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
	എങ്കിൽ (watchdog_enabled)
		i386_watchdog_start();
#endif

	തിരികെ OK;
}

ശൂന്യം release_address_space(ഘടന proc *pr)
{
	pr->p_seg.p_cr3_v = ശൂന്യ;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
പൂർണ്ണസംഖ്യ platform_tbl_checksum_ok(ശൂന്യം *ptr, unsigned പൂർണ്ണസംഖ്യ length)
{
	u8_t total = 0;
	unsigned പൂർണ്ണസംഖ്യ i;
	വേണ്ടി (i = 0; i < length; i++)
		total += ((unsigned അക്ഷരം *)ptr)[i];
	തിരികെ !total;
}

പൂർണ്ണസംഖ്യ platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					ശൂന്യം * buff,
					unsigned size,
					phys_bytes * phys_addr,
					പൂർണ്ണസംഖ്യ ((* cmp_f)(ശൂന്യം *)))
{
	phys_bytes addr;

	വേണ്ടി (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		എങ്കിൽ (cmp_f(buff)) {
			എങ്കിൽ (phys_addr)
				*phys_addr = addr;
			തിരികെ 1;
		}
	}
	തിരികെ 0;
}
