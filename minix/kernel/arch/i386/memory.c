
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/syslib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "oxpcie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
#ਅਨ੍ਤਰ੍ਭਾਵ "apic.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WATCHDOG
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/watchdog.h"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

phys_ਬਾਈਟਾਂ video_mem_vaddr = 0;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
ਸਥਿਰ ਪੂਰਨ nfreepdes = 0;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXFREEPDES	2
ਸਥਿਰ ਪੂਰਨ freepdes[MAXFREEPDES];

ਸਥਿਰ u32_t phys_get32(phys_ਬਾਈਟਾਂ v);

ਖਾਲੀ mem_clear_mapcache(ਖਾਲੀ)
{
	ਪੂਰਨ i;
	ਲਈ(i = 0; i < nfreepdes; i++) {
		ਬਣਤਰ proc *ptproc = get_cpulocal_var(ptproc);
		ਪੂਰਨ pde = freepdes[i];
		u32_t *ptv;
		ਪ੍ਰਤਿਜ੍ਞਾ(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		ਪ੍ਰਤਿਜ੍ਞਾ(ptv);
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
ਸਥਿਰ phys_ਬਾਈਟਾਂ createpde(
	ਅਚਲ ਬਣਤਰ proc *pr,	/* Requested process, NULL for physical. */
	ਅਚਲ phys_ਬਾਈਟਾਂ linaddr,/* Address after segment translation. */
	phys_ਬਾਈਟਾਂ *ਬਾਈਟਾਂ,	/* Size of chunk, function may truncate it. */
	ਪੂਰਨ free_pde_idx,	/* index of the free slot to use */
	ਪੂਰਨ *ਬਦਲਿਆ		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_ਬਾਈਟਾਂ ਵਿੱਥ;
	ਪੂਰਨ pde;

	ਪ੍ਰਤਿਜ੍ਞਾ(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	ਪ੍ਰਤਿਜ੍ਞਾ(pde >= 0 && pde < 1024);

	ਜੇ(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ਵਾਪਸ linaddr;
	}

	ਜੇ(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		ਪ੍ਰਤਿਜ੍ਞਾ(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} ਨਹੀਂਤਾਂ {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_ਨਕਾਬ_4_MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ਜੇ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*ਬਦਲਿਆ = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	ਵਿੱਥ = linaddr & I386_VM_ਵਿੱਥ_ਨਕਾਬ_4_MB; /* Offset in 4MB window. */
	*ਬਾਈਟਾਂ = MIN(*ਬਾਈਟਾਂ, I386_BIG_ਪੰਨਾ_ਮਾਪ - ਵਿੱਥ); 

	/* Return the linear address of the start of the new mapping. */
	ਵਾਪਸ I386_BIG_ਪੰਨਾ_ਮਾਪ*pde + ਵਿੱਥ;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ check_resumed_caller(ਬਣਤਰ proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	ਜੇ (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		ਪ੍ਰਤਿਜ੍ਞਾ(caller->p_vmrequest.vmresult != VMSUSPEND);
		ਵਾਪਸ caller->p_vmrequest.vmresult;
	}

	ਵਾਪਸ OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ lin_lin_copy(ਬਣਤਰ proc *srcproc, vir_ਬਾਈਟਾਂ srclinaddr, 
	ਬਣਤਰ proc *dstproc, vir_ਬਾਈਟਾਂ dstlinaddr, vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ)
{
	u32_t addr;
	proc_nr_t procslot;

	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc));
	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(proc_ptr));
	ਪ੍ਰਤਿਜ੍ਞਾ(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ਪ੍ਰਤਿਜ੍ਞਾ(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	ਜੇ(srcproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(srcproc, RTS_ਥਾਂ_FREE));
	ਜੇ(dstproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(dstproc, RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ਜੇ(srcproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	ਜੇ(dstproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	ਜਦੋਂ(ਬਾਈਟਾਂ > 0) {
		phys_ਬਾਈਟਾਂ srcptr, dstptr;
		vir_ਬਾਈਟਾਂ chunk = ਬਾਈਟਾਂ;
		ਪੂਰਨ ਬਦਲਿਆ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
		ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;

		ਜੇ (srcproc && GET_ਦੋਆਧਾਰੀ_ਅੰਕ(srcproc->p_stale_tlb, cpu)) {
			ਬਦਲਿਆ = 1;
			UNSET_ਦੋਆਧਾਰੀ_ਅੰਕ(srcproc->p_stale_tlb, cpu);
		}
		ਜੇ (dstproc && GET_ਦੋਆਧਾਰੀ_ਅੰਕ(dstproc->p_stale_tlb, cpu)) {
			ਬਦਲਿਆ = 1;
			UNSET_ਦੋਆਧਾਰੀ_ਅੰਕ(dstproc->p_stale_tlb, cpu);
		}
#ਜੇ_ਅੰਤ

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &ਬਦਲਿਆ);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &ਬਦਲਿਆ);
		ਜੇ(ਬਦਲਿਆ)
			reload_cr3(); 

		/* Check for overflow. */
		ਜੇ (srcptr + chunk < srcptr) ਵਾਪਸ EFAULT_SRC;
		ਜੇ (dstptr + chunk < dstptr) ਵਾਪਸ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ਜੇ(addr) {
			/* If addr is nonzero, a page fault was caught. */

			ਜੇ(addr >= srcptr && addr < (srcptr + chunk)) {
				ਵਾਪਸ EFAULT_SRC;
			}
			ਜੇ(addr >= dstptr && addr < (dstptr + chunk)) {
				ਵਾਪਸ EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ਵਾਪਸ EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		ਬਾਈਟਾਂ -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	ਜੇ(srcproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(srcproc, RTS_ਥਾਂ_FREE));
	ਜੇ(dstproc) ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(dstproc, RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	ਵਾਪਸ OK;
}


ਸਥਿਰ u32_t phys_get32(phys_ਬਾਈਟਾਂ addr)
{
	u32_t v;
	ਪੂਰਨ r;

	ਜੇ((r=lin_lin_copy(ਨਿਰਾ, addr, 
		proc_addr(SYSTEM), (phys_ਬਾਈਟਾਂ) &v, ਆਕਾਰ(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ਵਾਪਸ v;
}

#ਜੇ 0
ਸਥਿਰ ਅੱਖਰ *cr0_str(u32_t e)
{
	ਸਥਿਰ ਅੱਖਰ str[80];
	strcpy(str, "");
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLAG(v) ਕਰੋ { ਜੇ(e & (v)) { strcat(str, #v " "); e &= ~v; } } ਜਦੋਂ(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	ਜੇ(e) { strcat(str, " (++)"); }
	ਵਾਪਸ str;
}

ਸਥਿਰ ਅੱਖਰ *cr4_str(u32_t e)
{
	ਸਥਿਰ ਅੱਖਰ str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	ਜੇ(e) { strcat(str, " (++)"); }
	ਵਾਪਸ str;
}
#ਜੇ_ਅੰਤ

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_ਬਾਈਟਾਂ umap_virtual(
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp,		/* pointer to proc table entry for process */
  ਪੂਰਨ seg,				/* T, D, or S segment */
  vir_ਬਾਈਟਾਂ vir_addr,			/* virtual address in bytes within the seg */
  vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ			/* # of bytes to be copied */
)
{
	phys_ਬਾਈਟਾਂ phys = 0;

	ਜੇ(vm_lookup(rp, vir_addr, &phys, ਨਿਰਾ) != OK) {
		ਲਿਖੋ("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} ਨਹੀਂਤਾਂ {
		ਜੇ(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	ਜੇ(phys == 0) {
		ਲਿਖੋ("SYSTEM:umap_virtual: lookup failed\n");
		ਵਾਪਸ 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	ਜੇ(ਬਾਈਟਾਂ > 0 && vm_lookup_range(rp, vir_addr, ਨਿਰਾ, ਬਾਈਟਾਂ) != ਬਾਈਟਾਂ) {
		ਲਿਖੋ("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, ਬਾਈਟਾਂ, vir_addr, vir_addr);
		ਵਾਪਸ 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	ਪ੍ਰਤਿਜ੍ਞਾ(phys);

	ਵਾਪਸ phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
ਪੂਰਨ vm_lookup(ਅਚਲ ਬਣਤਰ proc *proc, ਅਚਲ vir_ਬਾਈਟਾਂ ਆਭਾਸੀ,
 phys_ਬਾਈਟਾਂ *physical, u32_t *ptent)
{
	u32_t *ਜੜ੍ਹ, *pt;
	ਪੂਰਨ pde, pte;
	u32_t pde_v, pte_v;

	ਪ੍ਰਤਿਜ੍ਞਾ(proc);
	ਪ੍ਰਤਿਜ੍ਞਾ(physical);
	ਪ੍ਰਤਿਜ੍ਞਾ(!isemptyp(proc));
	ਪ੍ਰਤਿਜ੍ਞਾ(HASPT(proc));

	/* Retrieve page directory entry. */
	ਜੜ੍ਹ = (u32_t *) proc->p_seg.p_cr3;
	ਪ੍ਰਤਿਜ੍ਞਾ(!((u32_t) ਜੜ੍ਹ % I386_ਪੰਨਾ_ਮਾਪ));
	pde = I386_VM_PDE(ਆਭਾਸੀ);
	ਪ੍ਰਤਿਜ੍ਞਾ(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ਜੜ੍ਹ + pde));

	ਜੇ(!(pde_v & I386_VM_PRESENT)) {
		ਵਾਪਸ EFAULT;
	}

	/* We don't expect to ever see this. */
	ਜੇ(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_ਨਕਾਬ_4_MB;
		ਜੇ(ptent) *ptent = pde_v;
		*physical += ਆਭਾਸੀ & I386_VM_ਵਿੱਥ_ਨਕਾਬ_4_MB;
	} ਨਹੀਂਤਾਂ {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		ਪ੍ਰਤਿਜ੍ਞਾ(!((u32_t) pt % I386_ਪੰਨਾ_ਮਾਪ));
		pte = I386_VM_PTE(ਆਭਾਸੀ);
		ਪ੍ਰਤਿਜ੍ਞਾ(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ਜੇ(!(pte_v & I386_VM_PRESENT)) {
			ਵਾਪਸ EFAULT;
		}

		ਜੇ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += ਆਭਾਸੀ % I386_ਪੰਨਾ_ਮਾਪ;
	}

	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ਅਚਲ ਬਣਤਰ proc *proc, vir_ਬਾਈਟਾਂ vir_addr,
	phys_ਬਾਈਟਾਂ *phys_addr, size_t ਬਾਈਟਾਂ)
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
	phys_ਬਾਈਟਾਂ phys, ਅਗਲਾ_phys;
	size_t len;

	ਪ੍ਰਤਿਜ੍ਞਾ(proc);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਬਾਈਟਾਂ > 0);
	ਪ੍ਰਤਿਜ੍ਞਾ(HASPT(proc));

	/* Look up the first page. */
	ਜੇ (vm_lookup(proc, vir_addr, &phys, ਨਿਰਾ) != OK)
		ਵਾਪਸ 0;

	ਜੇ (phys_addr != ਨਿਰਾ)
		*phys_addr = phys;

	len = I386_ਪੰਨਾ_ਮਾਪ - (vir_addr % I386_ਪੰਨਾ_ਮਾਪ);
	vir_addr += len;
	ਅਗਲਾ_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ਜਦੋਂ (len < ਬਾਈਟਾਂ) {
		ਜੇ (vm_lookup(proc, vir_addr, &phys, ਨਿਰਾ) != OK)
			ਰੋਕੋ;

		ਜੇ (ਅਗਲਾ_phys != phys)
			ਰੋਕੋ;

		len += I386_ਪੰਨਾ_ਮਾਪ;
		vir_addr += I386_ਪੰਨਾ_ਮਾਪ;
		ਅਗਲਾ_phys += I386_ਪੰਨਾ_ਮਾਪ;
	}

	/* We might now have overshot the requested length somewhat. */
	ਵਾਪਸ MIN(ਬਾਈਟਾਂ, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
ਪੂਰਨ vm_check_range(ਬਣਤਰ proc *caller, ਬਣਤਰ proc *ਨਿਸ਼ਾਨਾ,
	vir_ਬਾਈਟਾਂ vir_addr, size_t ਬਾਈਟਾਂ, ਪੂਰਨ writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	ਪੂਰਨ r;

	ਜੇ ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ਵਾਪਸ r;

	vm_suspend(caller, ਨਿਸ਼ਾਨਾ, vir_addr, ਬਾਈਟਾਂ, VMSTYPE_KERNELCALL,
		writeflag);

	ਵਾਪਸ VMSUSPEND;
}

#ਜੇ 0
ਸਥਿਰ ਅੱਖਰ *flagstr(u32_t e, ਅਚਲ ਪੂਰਨ dir)
{
	ਸਥਿਰ ਅੱਖਰ str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	ਜੇ(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	ਨਹੀਂਤਾਂ
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	ਵਾਪਸ str;
}

ਸਥਿਰ ਖਾਲੀ vm_pt_print(u32_t *pagetable, ਅਚਲ u32_t v)
{
	ਪੂਰਨ pte;
	ਪੂਰਨ ਖਾਨਾ = 0;

	ਪ੍ਰਤਿਜ੍ਞਾ(!((u32_t) pagetable % I386_ਪੰਨਾ_ਮਾਪ));

	ਲਈ(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		ਜੇ(!(pte_v & I386_VM_PRESENT))
			ਜਾਰੀ;
		pfa = I386_VM_PFA(pte_v);
		ਲਿਖੋ("%4d:%08lx:%08lx %2s ",
			pte, v + I386_ਪੰਨਾ_ਮਾਪ*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		ਖਾਨਾ++;
		ਜੇ(ਖਾਨਾ == 3) { ਲਿਖੋ("\n"); ਖਾਨਾ = 0; }
	}
	ਜੇ(ਖਾਨਾ > 0) ਲਿਖੋ("\n");

	ਵਾਪਸ;
}

ਸਥਿਰ ਖਾਲੀ vm_print(u32_t *ਜੜ੍ਹ)
{
	ਪੂਰਨ pde;

	ਪ੍ਰਤਿਜ੍ਞਾ(!((u32_t) ਜੜ੍ਹ % I386_ਪੰਨਾ_ਮਾਪ));

	ਲਿਖੋ("page table 0x%lx:\n", ਜੜ੍ਹ);

	ਲਈ(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (ਜੜ੍ਹ + pde));
		ਜੇ(!(pde_v & I386_VM_PRESENT))
			ਜਾਰੀ;
		ਜੇ(pde_v & I386_VM_BIGPAGE) {
			ਲਿਖੋ("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} ਨਹੀਂਤਾਂ {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			ਲਿਖੋ("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_ਪੰਨਾ_ਮਾਪ);
			ਲਿਖੋ("\n");
		}
	}


	ਵਾਪਸ;
}
#ਜੇ_ਅੰਤ

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
ਪੂਰਨ vm_memset(ਬਣਤਰ proc* caller, endpoint_t who, phys_ਬਾਈਟਾਂ ph, ਪੂਰਨ c,
	phys_ਬਾਈਟਾਂ ਗਿਣਤੀ_2)
{
	u32_t ਨਮੂਨਾ;
	ਬਣਤਰ proc *whoptr = ਨਿਰਾ;
	phys_ਬਾਈਟਾਂ cur_ph = ph;
	phys_ਬਾਈਟਾਂ ਖੱਬਾ = ਗਿਣਤੀ_2;
	phys_ਬਾਈਟਾਂ ptr, chunk, pfa = 0;
	ਪੂਰਨ new_cr3, r = OK;

	ਜੇ ((r = check_resumed_caller(caller)) != OK)
		ਵਾਪਸ r;

	/* NONE for physical, otherwise virtual */
	ਜੇ (who != NONE && !(whoptr = endpoint_lookup(who)))
		ਵਾਪਸ ESRCH;

	c &= 0xFF;
	ਨਮੂਨਾ = c | (c << 8) | (c << 16) | (c << 24);

	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ਪ੍ਰਤਿਜ੍ਞਾ(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	ਜਦੋਂ (ਖੱਬਾ > 0) {
		new_cr3 = 0;
		chunk = ਖੱਬਾ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		ਜੇ (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		ਜੇ ((pfa = phys_memset(ptr, ਨਮੂਨਾ, chunk))) {

			/* If a process pagefaults, VM may help out */
			ਜੇ (whoptr) {
				vm_suspend(caller, whoptr, ph, ਗਿਣਤੀ_2,
						   VMSTYPE_KERNELCALL, 1);
				ਪ੍ਰਤਿਜ੍ਞਾ(catch_pagefaults);
				catch_pagefaults = 0;
				ਵਾਪਸ VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		ਖੱਬਾ -= chunk;
	}

	ਪ੍ਰਤਿਜ੍ਞਾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ਪ੍ਰਤਿਜ੍ਞਾ(catch_pagefaults);
	catch_pagefaults = 0;

	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ਪੂਰਨ virtual_copy_f(
  ਬਣਤਰ proc * caller,
  ਬਣਤਰ vir_addr *src_addr,	/* source virtual address */
  ਬਣਤਰ vir_addr *dst_addr,	/* destination virtual address */
  vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ,		/* # of bytes to copy  */
  ਪੂਰਨ vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ਬਣਤਰ vir_addr *vir_addr[2];	/* virtual source and destination address */
  ਪੂਰਨ i, r;
  ਬਣਤਰ proc *procs[2];

  ਪ੍ਰਤਿਜ੍ਞਾ((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  ਜੇ (ਬਾਈਟਾਂ <= 0) ਵਾਪਸ(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  ਲਈ (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	ਪੂਰਨ proc_nr;
	ਬਣਤਰ proc *p;

	ਜੇ(proc_e == NONE) {
		p = ਨਿਰਾ;
	} ਨਹੀਂਤਾਂ {
		ਜੇ(!isokendpt(proc_e, &proc_nr)) {
			ਲਿਖੋ("virtual_copy: no reasonable endpoint\n");
			ਵਾਪਸ ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  ਜੇ ((r = check_resumed_caller(caller)) != OK)
	ਵਾਪਸ r;

  ਜੇ((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->ਵਿੱਥ,
  	procs[_DST_], vir_addr[_DST_]->ਵਿੱਥ, ਬਾਈਟਾਂ)) != OK) {
	ਪੂਰਨ writeflag;
  	ਬਣਤਰ proc *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;
  	phys_ਬਾਈਟਾਂ lin;
  	ਜੇ(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	ਜੇ(!vmcheck || !caller) {
    		ਵਾਪਸ r;
  	}

  	ਜੇ(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->ਵਿੱਥ;
  		ਨਿਸ਼ਾਨਾ = procs[_SRC_];
		writeflag = 0;
  	} ਨਹੀਂਤਾਂ ਜੇ(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->ਵਿੱਥ;
  		ਨਿਸ਼ਾਨਾ = procs[_DST_];
		writeflag = 1;
  	} ਨਹੀਂਤਾਂ {
  		panic("r strange: %d",  r);
  	}

	ਪ੍ਰਤਿਜ੍ਞਾ(caller);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);

	vm_suspend(caller, ਨਿਸ਼ਾਨਾ, lin, ਬਾਈਟਾਂ, VMSTYPE_KERNELCALL, writeflag);
	ਵਾਪਸ VMSUSPEND;
  }

  ਵਾਪਸ OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
ਪੂਰਨ ਅੰਕੜੇ_copy(ਅਚਲ endpoint_t from_proc, ਅਚਲ vir_ਬਾਈਟਾਂ from_addr,
	ਅਚਲ endpoint_t ਮੰਜ਼ਿਲ_proc, ਅਚਲ vir_ਬਾਈਟਾਂ ਮੰਜ਼ਿਲ_addr,
	size_t ਬਾਈਟਾਂ)
{
  ਬਣਤਰ vir_addr src, dst;

  src.ਵਿੱਥ = from_addr;
  dst.ਵਿੱਥ = ਮੰਜ਼ਿਲ_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = ਮੰਜ਼ਿਲ_proc;
  ਪ੍ਰਤਿਜ੍ਞਾ(src.proc_nr_e != NONE);
  ਪ੍ਰਤਿਜ੍ਞਾ(dst.proc_nr_e != NONE);

  ਵਾਪਸ virtual_copy(&src, &dst, ਬਾਈਟਾਂ);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
ਪੂਰਨ ਅੰਕੜੇ_copy_vmcheck(ਬਣਤਰ proc * caller,
	ਅਚਲ endpoint_t from_proc, ਅਚਲ vir_ਬਾਈਟਾਂ from_addr,
	ਅਚਲ endpoint_t ਮੰਜ਼ਿਲ_proc, ਅਚਲ vir_ਬਾਈਟਾਂ ਮੰਜ਼ਿਲ_addr,
	size_t ਬਾਈਟਾਂ)
{
  ਬਣਤਰ vir_addr src, dst;

  src.ਵਿੱਥ = from_addr;
  dst.ਵਿੱਥ = ਮੰਜ਼ਿਲ_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = ਮੰਜ਼ਿਲ_proc;
  ਪ੍ਰਤਿਜ੍ਞਾ(src.proc_nr_e != NONE);
  ਪ੍ਰਤਿਜ੍ਞਾ(dst.proc_nr_e != NONE);

  ਵਾਪਸ virtual_copy_vmcheck(caller, &src, &dst, ਬਾਈਟਾਂ);
}

ਖਾਲੀ ਸਮ੍ਰਿਤੀ_init(ਖਾਲੀ)
{
	ਪ੍ਰਤਿਜ੍ਞਾ(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_ਸ਼ੁਰੂ++;
	freepdes[nfreepdes++] = kinfo.freepde_ਸ਼ੁਰੂ++;

	ਪ੍ਰਤਿਜ੍ਞਾ(kinfo.freepde_ਸ਼ੁਰੂ < I386_VM_DIR_ENTRIES);
	ਪ੍ਰਤਿਜ੍ਞਾ(nfreepdes == 2);
	ਪ੍ਰਤਿਜ੍ਞਾ(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
ਖਾਲੀ arch_proc_init(ਬਣਤਰ proc *pr, ਅਚਲ u32_t ip, ਅਚਲ u32_t sp,
	ਅਚਲ u32_t ps_str, ਅੱਖਰ *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, ਆਕਾਰ(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

ਸਥਿਰ ਪੂਰਨ oxpcie_mapping_ਸੂਚਕਾਂਕ = -1,
	lapic_mapping_ਸੂਚਕਾਂਕ = -1,
	ioapic_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ = -1,
	ioapic_ਆਖਰੀ_ਸੂਚਕਾਂਕ = -1,
	video_mem_mapping_ਸੂਚਕਾਂਕ = -1,
	usermapped_glo_ਸੂਚਕਾਂਕ = -1,
	usermapped_ਸੂਚਕਾਂਕ = -1, ਪਹਿਲਾ_um_idx = -1;

ਬਾਹਰੀ ਅੱਖਰ *video_mem;

ਬਾਹਰੀ ਅੱਖਰ usermapped_ਸ਼ੁਰੂ, usermapped_ਸਿਰਾ, usermapped_nonglo_ਸ਼ੁਰੂ;

ਪੂਰਨ arch_phys_map(ਅਚਲ ਪੂਰਨ ਸੂਚਕਾਂਕ,
			phys_ਬਾਈਟਾਂ *addr,
			phys_ਬਾਈਟਾਂ *len,
			ਪੂਰਨ *flags)
{
	ਸਥਿਰ ਪੂਰਨ ਪਹਿਲਾ = 1;
	ਪੂਰਨ freeidx = 0;
	ਸਥਿਰ ਅੱਖਰ *ser_var = ਨਿਰਾ;
	u32_t glo_len = (u32_t) &usermapped_nonglo_ਸ਼ੁਰੂ -
			(u32_t) &usermapped_ਸ਼ੁਰੂ;

	ਜੇ(ਪਹਿਲਾ) {
		memset(&minix_kerninfo, 0, ਆਕਾਰ(minix_kerninfo));
		video_mem_mapping_ਸੂਚਕਾਂਕ = freeidx++;
		ਜੇ(glo_len > 0) {
			usermapped_glo_ਸੂਚਕਾਂਕ = freeidx++;
		}
		
		usermapped_ਸੂਚਕਾਂਕ = freeidx++;
		ਪਹਿਲਾ_um_idx = usermapped_ਸੂਚਕਾਂਕ;
		ਜੇ(usermapped_glo_ਸੂਚਕਾਂਕ != -1)
			ਪਹਿਲਾ_um_idx = usermapped_glo_ਸੂਚਕਾਂਕ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
		ਜੇ(lapic_addr)
			lapic_mapping_ਸੂਚਕਾਂਕ = freeidx++;
		ਜੇ (ioapic_enabled) {
			ioapic_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ = freeidx;
			ਪ੍ਰਤਿਜ੍ਞਾ(nioapics > 0);
			freeidx += nioapics;
			ioapic_ਆਖਰੀ_ਸੂਚਕਾਂਕ = freeidx-1;
		}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_OXPCIE
		ਜੇ((ser_var = env_get("oxpcie"))) {
			ਜੇ(ser_var[0] != '0' || ser_var[1] != 'x') {
				ਲਿਖੋ("oxpcie address in hex please\n");
			} ਨਹੀਂਤਾਂ {
				ਲਿਖੋ("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_ਸੂਚਕਾਂਕ = freeidx++;
			}
		}
#ਜੇ_ਅੰਤ

		ਪਹਿਲਾ = 0;
	}

	ਜੇ(ਸੂਚਕਾਂਕ == usermapped_glo_ਸੂਚਕਾਂਕ) {
		*addr = vir2phys(&usermapped_ਸ਼ੁਰੂ);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ਵਾਪਸ OK;
	}
	ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ == usermapped_ਸੂਚਕਾਂਕ) {
		*addr = vir2phys(&usermapped_nonglo_ਸ਼ੁਰੂ);
		*len = (u32_t) &usermapped_ਸਿਰਾ -
			(u32_t) &usermapped_nonglo_ਸ਼ੁਰੂ;
		*flags = VMMF_USER;
		ਵਾਪਸ OK;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਸੂਚਕਾਂਕ == video_mem_mapping_ਸੂਚਕਾਂਕ) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_ਬਫਰ;
		*len = I386_ਪੰਨਾ_ਮਾਪ;
		*flags = VMMF_WRITE;
		ਵਾਪਸ OK;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
	ਨਹੀਂਤਾਂ ਜੇ (ਸੂਚਕਾਂਕ == lapic_mapping_ਸੂਚਕਾਂਕ) {
		/* map the local APIC if enabled */
		ਜੇ (!lapic_addr)
			ਵਾਪਸ EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ਵਾਪਸ OK;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ioapic_enabled && ਸੂਚਕਾਂਕ >= ioapic_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ <= ioapic_ਆਖਰੀ_ਸੂਚਕਾਂਕ) {
		ਪੂਰਨ ioapic_idx = ਸੂਚਕਾਂਕ - ioapic_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ;
		*addr = io_apic[ioapic_idx].paddr;
		ਪ੍ਰਤਿਜ੍ਞਾ(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ਲਿਖੋ("ioapic map: addr 0x%lx\n", *addr);
		ਵਾਪਸ OK;
	}
#ਜੇ_ਅੰਤ

#ਜੇ CONFIG_OXPCIE
	ਜੇ(ਸੂਚਕਾਂਕ == oxpcie_mapping_ਸੂਚਕਾਂਕ) {
		*addr = strtoul(ser_var+2, ਨਿਰਾ, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ਵਾਪਸ OK;
	}
#ਜੇ_ਅੰਤ

	ਵਾਪਸ EINVAL;
}

ਪੂਰਨ arch_phys_map_ਜਵਾਬ(ਅਚਲ ਪੂਰਨ ਸੂਚਕਾਂਕ, ਅਚਲ vir_ਬਾਈਟਾਂ addr)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
	/* if local APIC is enabled */
	ਜੇ (ਸੂਚਕਾਂਕ == lapic_mapping_ਸੂਚਕਾਂਕ && lapic_addr) {
		lapic_addr_vaddr = addr;
		ਵਾਪਸ OK;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ioapic_enabled && ਸੂਚਕਾਂਕ >= ioapic_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ &&
		ਸੂਚਕਾਂਕ <= ioapic_ਆਖਰੀ_ਸੂਚਕਾਂਕ) {
		ਪੂਰਨ i = ਸੂਚਕਾਂਕ - ioapic_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ;
		io_apic[i].vaddr = addr;
		ਵਾਪਸ OK;
	}
#ਜੇ_ਅੰਤ

#ਜੇ CONFIG_OXPCIE
	ਜੇ (ਸੂਚਕਾਂਕ == oxpcie_mapping_ਸੂਚਕਾਂਕ) {
		oxpcie_ਸੈੱਟ_vaddr((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) addr);
		ਵਾਪਸ OK;
	}
#ਜੇ_ਅੰਤ
	ਜੇ(ਸੂਚਕਾਂਕ == ਪਹਿਲਾ_um_idx) {
		ਬਾਹਰੀ ਬਣਤਰ minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		ਬਾਹਰੀ u32_t usermapped_ਵਿੱਥ;
		ਪ੍ਰਤਿਜ੍ਞਾ(addr > (u32_t) &usermapped_ਸ਼ੁਰੂ);
		usermapped_ਵਿੱਥ = addr - (u32_t) &usermapped_ਸ਼ੁਰੂ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIXEDPTR(ptr) (ਖਾਲੀ *) ((u32_t)ptr + usermapped_ਵਿੱਥ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ਸੌਂਪੋ(kinfo);
		ਸੌਂਪੋ(machine);
		ਸੌਂਪੋ(kmessages);
		ਸੌਂਪੋ(loadinfo);
		ਸੌਂਪੋ(kuserinfo);
		ਸੌਂਪੋ(arm_frclock); /* eh, why not. */
		ਸੌਂਪੋ(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		ਜੇ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} ਨਹੀਂਤਾਂ  ਜੇ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} ਨਹੀਂਤਾਂ	{
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
		minix_kerninfo_user = (vir_ਬਾਈਟਾਂ) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		ਜੇ(env_get("libc_ipc")) {
			ਲਿਖੋ("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = ਨਿਰਾ;
		} ਨਹੀਂਤਾਂ {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ਵਾਪਸ OK;
	}

	ਜੇ(ਸੂਚਕਾਂਕ == usermapped_ਸੂਚਕਾਂਕ) ਵਾਪਸ OK;

	ਜੇ (ਸੂਚਕਾਂਕ == video_mem_mapping_ਸੂਚਕਾਂਕ) {
		video_mem_vaddr =  addr;
		ਵਾਪਸ OK;
	}

	ਵਾਪਸ EINVAL;
}

ਪੂਰਨ arch_enable_paging(ਬਣਤਰ proc * caller)
{
	ਪ੍ਰਤਿਜ੍ਞਾ(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_ਪਤਾ_space(caller);

	video_mem = (ਅੱਖਰ *) video_mem_vaddr;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	ਜੇ (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	ਜੇ (ioapic_enabled) {
		ਪੂਰਨ i;

		ਲਈ (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#ਜੇ CONFIG_SMP
	barrier();

	wait_for_APs_ਮੰਜ਼ਿਲ_ਅੰਤ_booting();
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	ਜੇ (watchdog_enabled)
		i386_watchdog_ਸ਼ੁਰੂ();
#ਜੇ_ਅੰਤ

	ਵਾਪਸ OK;
}

ਖਾਲੀ release_ਪਤਾ_space(ਬਣਤਰ proc *pr)
{
	pr->p_seg.p_cr3_v = ਨਿਰਾ;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
ਪੂਰਨ platform_tbl_checksum_ok(ਖਾਲੀ *ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ)
{
	u8_t ਕੁੱਲ = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
	ਲਈ (i = 0; i < ਲੰਮਾਈ; i++)
		ਕੁੱਲ += ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ptr)[i];
	ਵਾਪਸ !ਕੁੱਲ;
}

ਪੂਰਨ platform_tbl_ptr(phys_ਬਾਈਟਾਂ ਸ਼ੁਰੂ,
					phys_ਬਾਈਟਾਂ ਸਿਰਾ,
					ਬਿਨਾਨਿਸ਼ਾਨੀ increment,
					ਖਾਲੀ * buff,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ,
					phys_ਬਾਈਟਾਂ * phys_addr,
					ਪੂਰਨ ((* cmp_f)(ਖਾਲੀ *)))
{
	phys_ਬਾਈਟਾਂ addr;

	ਲਈ (addr = ਸ਼ੁਰੂ; addr < ਸਿਰਾ; addr += increment) {
		phys_copy (addr, (phys_ਬਾਈਟਾਂ) buff, ਮਾਪ);
		ਜੇ (cmp_f(buff)) {
			ਜੇ (phys_addr)
				*phys_addr = addr;
			ਵਾਪਸ 1;
		}
	}
	ਵਾਪਸ 0;
}
