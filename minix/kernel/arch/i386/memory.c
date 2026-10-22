
#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "kernel/vm.h"

#అన్తర్భావ <machine/vm.h>

#అన్తర్భావ <minix/syslib.h>
#అన్తర్భావ <minix/cpufeature.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <stdlib.h>

#అన్తర్భావ <machine/vm.h>

#అన్తర్భావ "oxpcie.h"
#అన్తర్భావ "arch_proto.h"

#నిర్వచితమైతే USE_APIC
#అన్తర్భావ "apic.h"
#నిర్వచితమైతే USE_WATCHDOG
#అన్తర్భావ "kernel/watchdog.h"
#అయితే_ముగింపు
#అయితే_ముగింపు

phys_బైట్లు video_mem_vaddr = 0;

#నిర్వచించు HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
స్థిర పూర్ణాంకం nfreepdes = 0;
#నిర్వచించు MAXFREEPDES	2
స్థిర పూర్ణాంకం freepdes[MAXFREEPDES];

స్థిర u32_t phys_get32(phys_బైట్లు v);

శూన్య mem_clear_mapcache(శూన్య)
{
	పూర్ణాంకం i;
	కొరకు(i = 0; i < nfreepdes; i++) {
		నిర్మాణం proc *ptproc = get_cpulocal_var(ptproc);
		పూర్ణాంకం pde = freepdes[i];
		u32_t *ptv;
		ప్రతిజ్ఞా(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		ప్రతిజ్ఞా(ptv);
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
స్థిర phys_బైట్లు createpde(
	స్థిరరాశి నిర్మాణం proc *pr,	/* Requested process, NULL for physical. */
	స్థిరరాశి phys_బైట్లు linaddr,/* Address after segment translation. */
	phys_బైట్లు *బైట్లు,	/* Size of chunk, function may truncate it. */
	పూర్ణాంకం free_pde_idx,	/* index of the free slot to use */
	పూర్ణాంకం *మారింది		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_బైట్లు స్థానభ్రంశం;
	పూర్ణాంకం pde;

	ప్రతిజ్ఞా(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	ప్రతిజ్ఞా(pde >= 0 && pde < 1024);

	ఒకవేళ(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		తిరిగి linaddr;
	}

	ఒకవేళ(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		ప్రతిజ్ఞా(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} లేకపోతే {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_ముసుగు_4_MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	ప్రతిజ్ఞా(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ఒకవేళ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*మారింది = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	స్థానభ్రంశం = linaddr & I386_VM_స్థానభ్రంశం_ముసుగు_4_MB; /* Offset in 4MB window. */
	*బైట్లు = MIN(*బైట్లు, I386_BIG_పుట_కొలత - స్థానభ్రంశం); 

	/* Return the linear address of the start of the new mapping. */
	తిరిగి I386_BIG_పుట_కొలత*pde + స్థానభ్రంశం;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
స్థిర పూర్ణాంకం check_resumed_caller(నిర్మాణం proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	ఒకవేళ (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		ప్రతిజ్ఞా(caller->p_vmrequest.vmresult != VMSUSPEND);
		తిరిగి caller->p_vmrequest.vmresult;
	}

	తిరిగి OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
స్థిర పూర్ణాంకం lin_lin_copy(నిర్మాణం proc *srcproc, vir_బైట్లు srclinaddr, 
	నిర్మాణం proc *dstproc, vir_బైట్లు dstlinaddr, vir_బైట్లు బైట్లు)
{
	u32_t addr;
	proc_nr_t procslot;

	ప్రతిజ్ఞా(get_cpulocal_var(ptproc));
	ప్రతిజ్ఞా(get_cpulocal_var(proc_ptr));
	ప్రతిజ్ఞా(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ప్రతిజ్ఞా(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	ఒకవేళ(srcproc) ప్రతిజ్ఞా(!RTS_ISSET(srcproc, RTS_స్థానం_FREE));
	ఒకవేళ(dstproc) ప్రతిజ్ఞా(!RTS_ISSET(dstproc, RTS_స్థానం_FREE));
	ప్రతిజ్ఞా(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_స్థానం_FREE));
	ప్రతిజ్ఞా(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ఒకవేళ(srcproc) ప్రతిజ్ఞా(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	ఒకవేళ(dstproc) ప్రతిజ్ఞా(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	అయితే(బైట్లు > 0) {
		phys_బైట్లు srcptr, dstptr;
		vir_బైట్లు chunk = బైట్లు;
		పూర్ణాంకం మారింది = 0;

#నిర్వచితమైతే CONFIG_SMP
		గుర్తులేని cpu = cpuid;

		ఒకవేళ (srcproc && GET_బిట్టు(srcproc->p_stale_tlb, cpu)) {
			మారింది = 1;
			UNSET_బిట్టు(srcproc->p_stale_tlb, cpu);
		}
		ఒకవేళ (dstproc && GET_బిట్టు(dstproc->p_stale_tlb, cpu)) {
			మారింది = 1;
			UNSET_బిట్టు(dstproc->p_stale_tlb, cpu);
		}
#అయితే_ముగింపు

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &మారింది);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &మారింది);
		ఒకవేళ(మారింది)
			reload_cr3(); 

		/* Check for overflow. */
		ఒకవేళ (srcptr + chunk < srcptr) తిరిగి EFAULT_SRC;
		ఒకవేళ (dstptr + chunk < dstptr) తిరిగి EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ఒకవేళ(addr) {
			/* If addr is nonzero, a page fault was caught. */

			ఒకవేళ(addr >= srcptr && addr < (srcptr + chunk)) {
				తిరిగి EFAULT_SRC;
			}
			ఒకవేళ(addr >= dstptr && addr < (dstptr + chunk)) {
				తిరిగి EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			తిరిగి EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		బైట్లు -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	ఒకవేళ(srcproc) ప్రతిజ్ఞా(!RTS_ISSET(srcproc, RTS_స్థానం_FREE));
	ఒకవేళ(dstproc) ప్రతిజ్ఞా(!RTS_ISSET(dstproc, RTS_స్థానం_FREE));
	ప్రతిజ్ఞా(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_స్థానం_FREE));
	ప్రతిజ్ఞా(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	తిరిగి OK;
}


స్థిర u32_t phys_get32(phys_బైట్లు addr)
{
	u32_t v;
	పూర్ణాంకం r;

	ఒకవేళ((r=lin_lin_copy(ఖాళీ, addr, 
		proc_addr(SYSTEM), (phys_బైట్లు) &v, పరిమాణం(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	తిరిగి v;
}

#ఒకవేళ 0
స్థిర అక్షరం *cr0_str(u32_t e)
{
	స్థిర అక్షరం str[80];
	strcpy(str, "");
#నిర్వచించు FLAG(v) చేయి { ఒకవేళ(e & (v)) { strcat(str, #v " "); e &= ~v; } } అయితే(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	ఒకవేళ(e) { strcat(str, " (++)"); }
	తిరిగి str;
}

స్థిర అక్షరం *cr4_str(u32_t e)
{
	స్థిర అక్షరం str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	ఒకవేళ(e) { strcat(str, " (++)"); }
	తిరిగి str;
}
#అయితే_ముగింపు

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_బైట్లు umap_virtual(
  నమోదు నిర్మాణం proc *rp,		/* pointer to proc table entry for process */
  పూర్ణాంకం seg,				/* T, D, or S segment */
  vir_బైట్లు vir_addr,			/* virtual address in bytes within the seg */
  vir_బైట్లు బైట్లు			/* # of bytes to be copied */
)
{
	phys_బైట్లు phys = 0;

	ఒకవేళ(vm_lookup(rp, vir_addr, &phys, ఖాళీ) != OK) {
		రాయి("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} లేకపోతే {
		ఒకవేళ(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	ఒకవేళ(phys == 0) {
		రాయి("SYSTEM:umap_virtual: lookup failed\n");
		తిరిగి 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	ఒకవేళ(బైట్లు > 0 && vm_lookup_range(rp, vir_addr, ఖాళీ, బైట్లు) != బైట్లు) {
		రాయి("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, బైట్లు, vir_addr, vir_addr);
		తిరిగి 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	ప్రతిజ్ఞా(phys);

	తిరిగి phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
పూర్ణాంకం vm_lookup(స్థిరరాశి నిర్మాణం proc *proc, స్థిరరాశి vir_బైట్లు మాయా,
 phys_బైట్లు *physical, u32_t *ptent)
{
	u32_t *మూలం, *pt;
	పూర్ణాంకం pde, pte;
	u32_t pde_v, pte_v;

	ప్రతిజ్ఞా(proc);
	ప్రతిజ్ఞా(physical);
	ప్రతిజ్ఞా(!isemptyp(proc));
	ప్రతిజ్ఞా(HASPT(proc));

	/* Retrieve page directory entry. */
	మూలం = (u32_t *) proc->p_seg.p_cr3;
	ప్రతిజ్ఞా(!((u32_t) మూలం % I386_పుట_కొలత));
	pde = I386_VM_PDE(మాయా);
	ప్రతిజ్ఞా(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (మూలం + pde));

	ఒకవేళ(!(pde_v & I386_VM_PRESENT)) {
		తిరిగి EFAULT;
	}

	/* We don't expect to ever see this. */
	ఒకవేళ(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_ముసుగు_4_MB;
		ఒకవేళ(ptent) *ptent = pde_v;
		*physical += మాయా & I386_VM_స్థానభ్రంశం_ముసుగు_4_MB;
	} లేకపోతే {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		ప్రతిజ్ఞా(!((u32_t) pt % I386_పుట_కొలత));
		pte = I386_VM_PTE(మాయా);
		ప్రతిజ్ఞా(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ఒకవేళ(!(pte_v & I386_VM_PRESENT)) {
			తిరిగి EFAULT;
		}

		ఒకవేళ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += మాయా % I386_పుట_కొలత;
	}

	తిరిగి OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(స్థిరరాశి నిర్మాణం proc *proc, vir_బైట్లు vir_addr,
	phys_బైట్లు *phys_addr, size_t బైట్లు)
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
	phys_బైట్లు phys, తదుపరి_phys;
	size_t len;

	ప్రతిజ్ఞా(proc);
	ప్రతిజ్ఞా(బైట్లు > 0);
	ప్రతిజ్ఞా(HASPT(proc));

	/* Look up the first page. */
	ఒకవేళ (vm_lookup(proc, vir_addr, &phys, ఖాళీ) != OK)
		తిరిగి 0;

	ఒకవేళ (phys_addr != ఖాళీ)
		*phys_addr = phys;

	len = I386_పుట_కొలత - (vir_addr % I386_పుట_కొలత);
	vir_addr += len;
	తదుపరి_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	అయితే (len < బైట్లు) {
		ఒకవేళ (vm_lookup(proc, vir_addr, &phys, ఖాళీ) != OK)
			ఆపు;

		ఒకవేళ (తదుపరి_phys != phys)
			ఆపు;

		len += I386_పుట_కొలత;
		vir_addr += I386_పుట_కొలత;
		తదుపరి_phys += I386_పుట_కొలత;
	}

	/* We might now have overshot the requested length somewhat. */
	తిరిగి MIN(బైట్లు, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
పూర్ణాంకం vm_check_range(నిర్మాణం proc *caller, నిర్మాణం proc *లక్ష్యం,
	vir_బైట్లు vir_addr, size_t బైట్లు, పూర్ణాంకం writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	పూర్ణాంకం r;

	ఒకవేళ ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		తిరిగి r;

	vm_suspend(caller, లక్ష్యం, vir_addr, బైట్లు, VMSTYPE_KERNELCALL,
		writeflag);

	తిరిగి VMSUSPEND;
}

#ఒకవేళ 0
స్థిర అక్షరం *flagstr(u32_t e, స్థిరరాశి పూర్ణాంకం dir)
{
	స్థిర అక్షరం str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	ఒకవేళ(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	లేకపోతే
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	తిరిగి str;
}

స్థిర శూన్య vm_pt_print(u32_t *pagetable, స్థిరరాశి u32_t v)
{
	పూర్ణాంకం pte;
	పూర్ణాంకం నిలువు = 0;

	ప్రతిజ్ఞా(!((u32_t) pagetable % I386_పుట_కొలత));

	కొరకు(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		ఒకవేళ(!(pte_v & I386_VM_PRESENT))
			కొనసాగు;
		pfa = I386_VM_PFA(pte_v);
		రాయి("%4d:%08lx:%08lx %2s ",
			pte, v + I386_పుట_కొలత*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		నిలువు++;
		ఒకవేళ(నిలువు == 3) { రాయి("\n"); నిలువు = 0; }
	}
	ఒకవేళ(నిలువు > 0) రాయి("\n");

	తిరిగి;
}

స్థిర శూన్య vm_print(u32_t *మూలం)
{
	పూర్ణాంకం pde;

	ప్రతిజ్ఞా(!((u32_t) మూలం % I386_పుట_కొలత));

	రాయి("page table 0x%lx:\n", మూలం);

	కొరకు(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (మూలం + pde));
		ఒకవేళ(!(pde_v & I386_VM_PRESENT))
			కొనసాగు;
		ఒకవేళ(pde_v & I386_VM_BIGPAGE) {
			రాయి("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} లేకపోతే {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			రాయి("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_పుట_కొలత);
			రాయి("\n");
		}
	}


	తిరిగి;
}
#అయితే_ముగింపు

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
పూర్ణాంకం vm_memset(నిర్మాణం proc* caller, endpoint_t who, phys_బైట్లు ph, పూర్ణాంకం c,
	phys_బైట్లు లెక్క)
{
	u32_t నమూనా;
	నిర్మాణం proc *whoptr = ఖాళీ;
	phys_బైట్లు cur_ph = ph;
	phys_బైట్లు ఎడమ = లెక్క;
	phys_బైట్లు ptr, chunk, pfa = 0;
	పూర్ణాంకం new_cr3, r = OK;

	ఒకవేళ ((r = check_resumed_caller(caller)) != OK)
		తిరిగి r;

	/* NONE for physical, otherwise virtual */
	ఒకవేళ (who != NONE && !(whoptr = endpoint_lookup(who)))
		తిరిగి ESRCH;

	c &= 0xFF;
	నమూనా = c | (c << 8) | (c << 16) | (c << 24);

	ప్రతిజ్ఞా(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ప్రతిజ్ఞా(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	అయితే (ఎడమ > 0) {
		new_cr3 = 0;
		chunk = ఎడమ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		ఒకవేళ (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		ఒకవేళ ((pfa = phys_memset(ptr, నమూనా, chunk))) {

			/* If a process pagefaults, VM may help out */
			ఒకవేళ (whoptr) {
				vm_suspend(caller, whoptr, ph, లెక్క,
						   VMSTYPE_KERNELCALL, 1);
				ప్రతిజ్ఞా(catch_pagefaults);
				catch_pagefaults = 0;
				తిరిగి VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		ఎడమ -= chunk;
	}

	ప్రతిజ్ఞా(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ప్రతిజ్ఞా(catch_pagefaults);
	catch_pagefaults = 0;

	తిరిగి OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
పూర్ణాంకం virtual_copy_f(
  నిర్మాణం proc * caller,
  నిర్మాణం vir_addr *src_addr,	/* source virtual address */
  నిర్మాణం vir_addr *dst_addr,	/* destination virtual address */
  vir_బైట్లు బైట్లు,		/* # of bytes to copy  */
  పూర్ణాంకం vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  నిర్మాణం vir_addr *vir_addr[2];	/* virtual source and destination address */
  పూర్ణాంకం i, r;
  నిర్మాణం proc *procs[2];

  ప్రతిజ్ఞా((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  ఒకవేళ (బైట్లు <= 0) తిరిగి(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  కొరకు (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	పూర్ణాంకం proc_nr;
	నిర్మాణం proc *p;

	ఒకవేళ(proc_e == NONE) {
		p = ఖాళీ;
	} లేకపోతే {
		ఒకవేళ(!isokendpt(proc_e, &proc_nr)) {
			రాయి("virtual_copy: no reasonable endpoint\n");
			తిరిగి ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  ఒకవేళ ((r = check_resumed_caller(caller)) != OK)
	తిరిగి r;

  ఒకవేళ((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->స్థానభ్రంశం,
  	procs[_DST_], vir_addr[_DST_]->స్థానభ్రంశం, బైట్లు)) != OK) {
	పూర్ణాంకం writeflag;
  	నిర్మాణం proc *లక్ష్యం = ఖాళీ;
  	phys_బైట్లు lin;
  	ఒకవేళ(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	ఒకవేళ(!vmcheck || !caller) {
    		తిరిగి r;
  	}

  	ఒకవేళ(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->స్థానభ్రంశం;
  		లక్ష్యం = procs[_SRC_];
		writeflag = 0;
  	} లేకపోతే ఒకవేళ(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->స్థానభ్రంశం;
  		లక్ష్యం = procs[_DST_];
		writeflag = 1;
  	} లేకపోతే {
  		panic("r strange: %d",  r);
  	}

	ప్రతిజ్ఞా(caller);
	ప్రతిజ్ఞా(లక్ష్యం);

	vm_suspend(caller, లక్ష్యం, lin, బైట్లు, VMSTYPE_KERNELCALL, writeflag);
	తిరిగి VMSUSPEND;
  }

  తిరిగి OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
పూర్ణాంకం దత్తాంశం_copy(స్థిరరాశి endpoint_t from_proc, స్థిరరాశి vir_బైట్లు from_addr,
	స్థిరరాశి endpoint_t గమ్యం_proc, స్థిరరాశి vir_బైట్లు గమ్యం_addr,
	size_t బైట్లు)
{
  నిర్మాణం vir_addr src, dst;

  src.స్థానభ్రంశం = from_addr;
  dst.స్థానభ్రంశం = గమ్యం_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = గమ్యం_proc;
  ప్రతిజ్ఞా(src.proc_nr_e != NONE);
  ప్రతిజ్ఞా(dst.proc_nr_e != NONE);

  తిరిగి virtual_copy(&src, &dst, బైట్లు);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
పూర్ణాంకం దత్తాంశం_copy_vmcheck(నిర్మాణం proc * caller,
	స్థిరరాశి endpoint_t from_proc, స్థిరరాశి vir_బైట్లు from_addr,
	స్థిరరాశి endpoint_t గమ్యం_proc, స్థిరరాశి vir_బైట్లు గమ్యం_addr,
	size_t బైట్లు)
{
  నిర్మాణం vir_addr src, dst;

  src.స్థానభ్రంశం = from_addr;
  dst.స్థానభ్రంశం = గమ్యం_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = గమ్యం_proc;
  ప్రతిజ్ఞా(src.proc_nr_e != NONE);
  ప్రతిజ్ఞా(dst.proc_nr_e != NONE);

  తిరిగి virtual_copy_vmcheck(caller, &src, &dst, బైట్లు);
}

శూన్య జ్ఞాపకం_init(శూన్య)
{
	ప్రతిజ్ఞా(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_ప్రారంభం++;
	freepdes[nfreepdes++] = kinfo.freepde_ప్రారంభం++;

	ప్రతిజ్ఞా(kinfo.freepde_ప్రారంభం < I386_VM_DIR_ENTRIES);
	ప్రతిజ్ఞా(nfreepdes == 2);
	ప్రతిజ్ఞా(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
శూన్య arch_proc_init(నిర్మాణం proc *pr, స్థిరరాశి u32_t ip, స్థిరరాశి u32_t sp,
	స్థిరరాశి u32_t ps_str, అక్షరం *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, పరిమాణం(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

స్థిర పూర్ణాంకం oxpcie_mapping_సూచీ = -1,
	lapic_mapping_సూచీ = -1,
	ioapic_మొదటి_సూచీ = -1,
	ioapic_చివరి_సూచీ = -1,
	video_mem_mapping_సూచీ = -1,
	usermapped_glo_సూచీ = -1,
	usermapped_సూచీ = -1, మొదటి_um_idx = -1;

బాహ్య అక్షరం *video_mem;

బాహ్య అక్షరం usermapped_ప్రారంభం, usermapped_చివర, usermapped_nonglo_ప్రారంభం;

పూర్ణాంకం arch_phys_map(స్థిరరాశి పూర్ణాంకం సూచీ,
			phys_బైట్లు *addr,
			phys_బైట్లు *len,
			పూర్ణాంకం *flags)
{
	స్థిర పూర్ణాంకం మొదటి = 1;
	పూర్ణాంకం freeidx = 0;
	స్థిర అక్షరం *ser_var = ఖాళీ;
	u32_t glo_len = (u32_t) &usermapped_nonglo_ప్రారంభం -
			(u32_t) &usermapped_ప్రారంభం;

	ఒకవేళ(మొదటి) {
		memset(&minix_kerninfo, 0, పరిమాణం(minix_kerninfo));
		video_mem_mapping_సూచీ = freeidx++;
		ఒకవేళ(glo_len > 0) {
			usermapped_glo_సూచీ = freeidx++;
		}
		
		usermapped_సూచీ = freeidx++;
		మొదటి_um_idx = usermapped_సూచీ;
		ఒకవేళ(usermapped_glo_సూచీ != -1)
			మొదటి_um_idx = usermapped_glo_సూచీ;

#నిర్వచితమైతే USE_APIC
		ఒకవేళ(lapic_addr)
			lapic_mapping_సూచీ = freeidx++;
		ఒకవేళ (ioapic_enabled) {
			ioapic_మొదటి_సూచీ = freeidx;
			ప్రతిజ్ఞా(nioapics > 0);
			freeidx += nioapics;
			ioapic_చివరి_సూచీ = freeidx-1;
		}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_OXPCIE
		ఒకవేళ((ser_var = env_get("oxpcie"))) {
			ఒకవేళ(ser_var[0] != '0' || ser_var[1] != 'x') {
				రాయి("oxpcie address in hex please\n");
			} లేకపోతే {
				రాయి("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_సూచీ = freeidx++;
			}
		}
#అయితే_ముగింపు

		మొదటి = 0;
	}

	ఒకవేళ(సూచీ == usermapped_glo_సూచీ) {
		*addr = vir2phys(&usermapped_ప్రారంభం);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		తిరిగి OK;
	}
	లేకపోతే ఒకవేళ(సూచీ == usermapped_సూచీ) {
		*addr = vir2phys(&usermapped_nonglo_ప్రారంభం);
		*len = (u32_t) &usermapped_చివర -
			(u32_t) &usermapped_nonglo_ప్రారంభం;
		*flags = VMMF_USER;
		తిరిగి OK;
	}
	లేకపోతే ఒకవేళ (సూచీ == video_mem_mapping_సూచీ) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_బఫరు;
		*len = I386_పుట_కొలత;
		*flags = VMMF_WRITE;
		తిరిగి OK;
	}
#నిర్వచితమైతే USE_APIC
	లేకపోతే ఒకవేళ (సూచీ == lapic_mapping_సూచీ) {
		/* map the local APIC if enabled */
		ఒకవేళ (!lapic_addr)
			తిరిగి EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		తిరిగి OK;
	}
	లేకపోతే ఒకవేళ (ioapic_enabled && సూచీ >= ioapic_మొదటి_సూచీ && సూచీ <= ioapic_చివరి_సూచీ) {
		పూర్ణాంకం ioapic_idx = సూచీ - ioapic_మొదటి_సూచీ;
		*addr = io_apic[ioapic_idx].paddr;
		ప్రతిజ్ఞా(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		రాయి("ioapic map: addr 0x%lx\n", *addr);
		తిరిగి OK;
	}
#అయితే_ముగింపు

#ఒకవేళ CONFIG_OXPCIE
	ఒకవేళ(సూచీ == oxpcie_mapping_సూచీ) {
		*addr = strtoul(ser_var+2, ఖాళీ, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		తిరిగి OK;
	}
#అయితే_ముగింపు

	తిరిగి EINVAL;
}

పూర్ణాంకం arch_phys_map_జవాబు(స్థిరరాశి పూర్ణాంకం సూచీ, స్థిరరాశి vir_బైట్లు addr)
{
#నిర్వచితమైతే USE_APIC
	/* if local APIC is enabled */
	ఒకవేళ (సూచీ == lapic_mapping_సూచీ && lapic_addr) {
		lapic_addr_vaddr = addr;
		తిరిగి OK;
	}
	లేకపోతే ఒకవేళ (ioapic_enabled && సూచీ >= ioapic_మొదటి_సూచీ &&
		సూచీ <= ioapic_చివరి_సూచీ) {
		పూర్ణాంకం i = సూచీ - ioapic_మొదటి_సూచీ;
		io_apic[i].vaddr = addr;
		తిరిగి OK;
	}
#అయితే_ముగింపు

#ఒకవేళ CONFIG_OXPCIE
	ఒకవేళ (సూచీ == oxpcie_mapping_సూచీ) {
		oxpcie_సమితి_vaddr((గుర్తులేని అక్షరం *) addr);
		తిరిగి OK;
	}
#అయితే_ముగింపు
	ఒకవేళ(సూచీ == మొదటి_um_idx) {
		బాహ్య నిర్మాణం minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		బాహ్య u32_t usermapped_స్థానభ్రంశం;
		ప్రతిజ్ఞా(addr > (u32_t) &usermapped_ప్రారంభం);
		usermapped_స్థానభ్రంశం = addr - (u32_t) &usermapped_ప్రారంభం;
#నిర్వచించు FIXEDPTR(ptr) (శూన్య *) ((u32_t)ptr + usermapped_స్థానభ్రంశం)
#నిర్వచించు FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#నిర్వచించు కేటాయించు(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		కేటాయించు(kinfo);
		కేటాయించు(machine);
		కేటాయించు(kmessages);
		కేటాయించు(loadinfo);
		కేటాయించు(kuserinfo);
		కేటాయించు(arm_frclock); /* eh, why not. */
		కేటాయించు(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		ఒకవేళ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} లేకపోతే  ఒకవేళ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} లేకపోతే	{
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
		minix_kerninfo_user = (vir_బైట్లు) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		ఒకవేళ(env_get("libc_ipc")) {
			రాయి("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = ఖాళీ;
		} లేకపోతే {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		తిరిగి OK;
	}

	ఒకవేళ(సూచీ == usermapped_సూచీ) తిరిగి OK;

	ఒకవేళ (సూచీ == video_mem_mapping_సూచీ) {
		video_mem_vaddr =  addr;
		తిరిగి OK;
	}

	తిరిగి EINVAL;
}

పూర్ణాంకం arch_enable_paging(నిర్మాణం proc * caller)
{
	ప్రతిజ్ఞా(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_చిరునామా_space(caller);

	video_mem = (అక్షరం *) video_mem_vaddr;

#నిర్వచితమైతే USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	ఒకవేళ (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	ఒకవేళ (ioapic_enabled) {
		పూర్ణాంకం i;

		కొరకు (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#ఒకవేళ CONFIG_SMP
	barrier();

	wait_for_APs_గమ్యం_ముగింపు_booting();
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	ఒకవేళ (watchdog_enabled)
		i386_watchdog_ప్రారంభం();
#అయితే_ముగింపు

	తిరిగి OK;
}

శూన్య release_చిరునామా_space(నిర్మాణం proc *pr)
{
	pr->p_seg.p_cr3_v = ఖాళీ;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
పూర్ణాంకం platform_tbl_checksum_ok(శూన్య *ptr, గుర్తులేని పూర్ణాంకం నిడివి)
{
	u8_t మొత్తం = 0;
	గుర్తులేని పూర్ణాంకం i;
	కొరకు (i = 0; i < నిడివి; i++)
		మొత్తం += ((గుర్తులేని అక్షరం *)ptr)[i];
	తిరిగి !మొత్తం;
}

పూర్ణాంకం platform_tbl_ptr(phys_బైట్లు ప్రారంభం,
					phys_బైట్లు చివర,
					గుర్తులేని increment,
					శూన్య * buff,
					గుర్తులేని కొలత,
					phys_బైట్లు * phys_addr,
					పూర్ణాంకం ((* cmp_f)(శూన్య *)))
{
	phys_బైట్లు addr;

	కొరకు (addr = ప్రారంభం; addr < చివర; addr += increment) {
		phys_copy (addr, (phys_బైట్లు) buff, కొలత);
		ఒకవేళ (cmp_f(buff)) {
			ఒకవేళ (phys_addr)
				*phys_addr = addr;
			తిరిగి 1;
		}
	}
	తిరిగి 0;
}
