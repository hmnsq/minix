
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ಅನ್ತರ್ಭಾವ <minix/syslib.h>
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ಅನ್ತರ್ಭಾವ "oxpcie.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
#ಅನ್ತರ್ಭಾವ "apic.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WATCHDOG
#ಅನ್ತರ್ಭಾವ "kernel/watchdog.h"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

phys_ಬೈಟುಗಳು video_mem_vaddr = 0;

#ವ್ಯಾಖ್ಯಾನಿಸು HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nfreepdes = 0;
#ವ್ಯಾಖ್ಯಾನಿಸು MAXFREEPDES	2
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ freepdes[MAXFREEPDES];

ಸ್ಥಿರ u32_t phys_get32(phys_ಬೈಟುಗಳು v);

ಶೂನ್ಯ mem_clear_mapcache(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ(i = 0; i < nfreepdes; i++) {
		ರಚನೆ proc *ptproc = get_cpulocal_var(ptproc);
		ಪೂರ್ಣಾಂಕ pde = freepdes[i];
		u32_t *ptv;
		ಪ್ರತಿಜ್ಞಾ(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		ಪ್ರತಿಜ್ಞಾ(ptv);
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
ಸ್ಥಿರ phys_ಬೈಟುಗಳು createpde(
	ಸ್ಥಿರಾಂಕ ರಚನೆ proc *pr,	/* Requested process, NULL for physical. */
	ಸ್ಥಿರಾಂಕ phys_ಬೈಟುಗಳು linaddr,/* Address after segment translation. */
	phys_ಬೈಟುಗಳು *ಬೈಟುಗಳು,	/* Size of chunk, function may truncate it. */
	ಪೂರ್ಣಾಂಕ free_pde_idx,	/* index of the free slot to use */
	ಪೂರ್ಣಾಂಕ *ಬದಲಾಯಿತು		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_ಬೈಟುಗಳು ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ pde;

	ಪ್ರತಿಜ್ಞಾ(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	ಪ್ರತಿಜ್ಞಾ(pde >= 0 && pde < 1024);

	ಆದರೆ(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		ಹಿಂತಿರುಗಿ linaddr;
	}

	ಆದರೆ(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		ಪ್ರತಿಜ್ಞಾ(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_ಮುಖವಾಡ_4_MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ಆದರೆ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*ಬದಲಾಯಿತು = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	ಸ್ಥಾನಾಂತರ = linaddr & I386_VM_ಸ್ಥಾನಾಂತರ_ಮುಖವಾಡ_4_MB; /* Offset in 4MB window. */
	*ಬೈಟುಗಳು = MIN(*ಬೈಟುಗಳು, I386_BIG_ಪುಟ_ಗಾತ್ರ - ಸ್ಥಾನಾಂತರ); 

	/* Return the linear address of the start of the new mapping. */
	ಹಿಂತಿರುಗಿ I386_BIG_ಪುಟ_ಗಾತ್ರ*pde + ಸ್ಥಾನಾಂತರ;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_resumed_caller(ರಚನೆ proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	ಆದರೆ (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		ಪ್ರತಿಜ್ಞಾ(caller->p_vmrequest.vmresult != VMSUSPEND);
		ಹಿಂತಿರುಗಿ caller->p_vmrequest.vmresult;
	}

	ಹಿಂತಿರುಗಿ OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lin_lin_copy(ರಚನೆ proc *srcproc, vir_ಬೈಟುಗಳು srclinaddr, 
	ರಚನೆ proc *dstproc, vir_ಬೈಟುಗಳು dstlinaddr, vir_ಬೈಟುಗಳು ಬೈಟುಗಳು)
{
	u32_t addr;
	proc_nr_t procslot;

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(proc_ptr));
	ಪ್ರತಿಜ್ಞಾ(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	ಪ್ರತಿಜ್ಞಾ(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	ಆದರೆ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_ಸ್ಥಾನ_FREE));
	ಆದರೆ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ಆದರೆ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	ಆದರೆ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	ಆಗ(ಬೈಟುಗಳು > 0) {
		phys_ಬೈಟುಗಳು srcptr, dstptr;
		vir_ಬೈಟುಗಳು chunk = ಬೈಟುಗಳು;
		ಪೂರ್ಣಾಂಕ ಬದಲಾಯಿತು = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
		ಚಿಹ್ನೆರಹಿತ cpu = cpuid;

		ಆದರೆ (srcproc && GET_ಬಿಟ್(srcproc->p_stale_tlb, cpu)) {
			ಬದಲಾಯಿತು = 1;
			UNSET_ಬಿಟ್(srcproc->p_stale_tlb, cpu);
		}
		ಆದರೆ (dstproc && GET_ಬಿಟ್(dstproc->p_stale_tlb, cpu)) {
			ಬದಲಾಯಿತು = 1;
			UNSET_ಬಿಟ್(dstproc->p_stale_tlb, cpu);
		}
#ಆದರೆ_ಅಂತ್ಯ

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &ಬದಲಾಯಿತು);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &ಬದಲಾಯಿತು);
		ಆದರೆ(ಬದಲಾಯಿತು)
			reload_cr3(); 

		/* Check for overflow. */
		ಆದರೆ (srcptr + chunk < srcptr) ಹಿಂತಿರುಗಿ EFAULT_SRC;
		ಆದರೆ (dstptr + chunk < dstptr) ಹಿಂತಿರುಗಿ EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		ಆದರೆ(addr) {
			/* If addr is nonzero, a page fault was caught. */

			ಆದರೆ(addr >= srcptr && addr < (srcptr + chunk)) {
				ಹಿಂತಿರುಗಿ EFAULT_SRC;
			}
			ಆದರೆ(addr >= dstptr && addr < (dstptr + chunk)) {
				ಹಿಂತಿರುಗಿ EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			ಹಿಂತಿರುಗಿ EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		ಬೈಟುಗಳು -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	ಆದರೆ(srcproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(srcproc, RTS_ಸ್ಥಾನ_FREE));
	ಆದರೆ(dstproc) ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(dstproc, RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	ಹಿಂತಿರುಗಿ OK;
}


ಸ್ಥಿರ u32_t phys_get32(phys_ಬೈಟುಗಳು addr)
{
	u32_t v;
	ಪೂರ್ಣಾಂಕ r;

	ಆದರೆ((r=lin_lin_copy(ಖಾಲಿ, addr, 
		proc_addr(SYSTEM), (phys_ಬೈಟುಗಳು) &v, ಗಾತ್ರ(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	ಹಿಂತಿರುಗಿ v;
}

#ಆದರೆ 0
ಸ್ಥಿರ ಅಕ್ಷರ *cr0_str(u32_t e)
{
	ಸ್ಥಿರ ಅಕ್ಷರ str[80];
	strcpy(str, "");
#ವ್ಯಾಖ್ಯಾನಿಸು FLAG(v) ಮಾಡು { ಆದರೆ(e & (v)) { strcat(str, #v " "); e &= ~v; } } ಆಗ(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	ಆದರೆ(e) { strcat(str, " (++)"); }
	ಹಿಂತಿರುಗಿ str;
}

ಸ್ಥಿರ ಅಕ್ಷರ *cr4_str(u32_t e)
{
	ಸ್ಥಿರ ಅಕ್ಷರ str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	ಆದರೆ(e) { strcat(str, " (++)"); }
	ಹಿಂತಿರುಗಿ str;
}
#ಆದರೆ_ಅಂತ್ಯ

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_ಬೈಟುಗಳು umap_virtual(
  ನೋಂದಣಿ ರಚನೆ proc *rp,		/* pointer to proc table entry for process */
  ಪೂರ್ಣಾಂಕ seg,				/* T, D, or S segment */
  vir_ಬೈಟುಗಳು vir_addr,			/* virtual address in bytes within the seg */
  vir_ಬೈಟುಗಳು ಬೈಟುಗಳು			/* # of bytes to be copied */
)
{
	phys_ಬೈಟುಗಳು phys = 0;

	ಆದರೆ(vm_lookup(rp, vir_addr, &phys, ಖಾಲಿ) != OK) {
		ಬರೆ("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	ಆದರೆ(phys == 0) {
		ಬರೆ("SYSTEM:umap_virtual: lookup failed\n");
		ಹಿಂತಿರುಗಿ 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	ಆದರೆ(ಬೈಟುಗಳು > 0 && vm_lookup_range(rp, vir_addr, ಖಾಲಿ, ಬೈಟುಗಳು) != ಬೈಟುಗಳು) {
		ಬರೆ("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, ಬೈಟುಗಳು, vir_addr, vir_addr);
		ಹಿಂತಿರುಗಿ 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	ಪ್ರತಿಜ್ಞಾ(phys);

	ಹಿಂತಿರುಗಿ phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ vm_lookup(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು ವಾಸ್ತವಿಕ,
 phys_ಬೈಟುಗಳು *physical, u32_t *ptent)
{
	u32_t *ಬೇರು, *pt;
	ಪೂರ್ಣಾಂಕ pde, pte;
	u32_t pde_v, pte_v;

	ಪ್ರತಿಜ್ಞಾ(proc);
	ಪ್ರತಿಜ್ಞಾ(physical);
	ಪ್ರತಿಜ್ಞಾ(!isemptyp(proc));
	ಪ್ರತಿಜ್ಞಾ(HASPT(proc));

	/* Retrieve page directory entry. */
	ಬೇರು = (u32_t *) proc->p_seg.p_cr3;
	ಪ್ರತಿಜ್ಞಾ(!((u32_t) ಬೇರು % I386_ಪುಟ_ಗಾತ್ರ));
	pde = I386_VM_PDE(ವಾಸ್ತವಿಕ);
	ಪ್ರತಿಜ್ಞಾ(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ಬೇರು + pde));

	ಆದರೆ(!(pde_v & I386_VM_PRESENT)) {
		ಹಿಂತಿರುಗಿ EFAULT;
	}

	/* We don't expect to ever see this. */
	ಆದರೆ(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_ಮುಖವಾಡ_4_MB;
		ಆದರೆ(ptent) *ptent = pde_v;
		*physical += ವಾಸ್ತವಿಕ & I386_VM_ಸ್ಥಾನಾಂತರ_ಮುಖವಾಡ_4_MB;
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		ಪ್ರತಿಜ್ಞಾ(!((u32_t) pt % I386_ಪುಟ_ಗಾತ್ರ));
		pte = I386_VM_PTE(ವಾಸ್ತವಿಕ);
		ಪ್ರತಿಜ್ಞಾ(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		ಆದರೆ(!(pte_v & I386_VM_PRESENT)) {
			ಹಿಂತಿರುಗಿ EFAULT;
		}

		ಆದರೆ(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += ವಾಸ್ತವಿಕ % I386_ಪುಟ_ಗಾತ್ರ;
	}

	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *proc, vir_ಬೈಟುಗಳು vir_addr,
	phys_ಬೈಟುಗಳು *phys_addr, size_t ಬೈಟುಗಳು)
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
	phys_ಬೈಟುಗಳು phys, ಮುಂದಿನ_phys;
	size_t len;

	ಪ್ರತಿಜ್ಞಾ(proc);
	ಪ್ರತಿಜ್ಞಾ(ಬೈಟುಗಳು > 0);
	ಪ್ರತಿಜ್ಞಾ(HASPT(proc));

	/* Look up the first page. */
	ಆದರೆ (vm_lookup(proc, vir_addr, &phys, ಖಾಲಿ) != OK)
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (phys_addr != ಖಾಲಿ)
		*phys_addr = phys;

	len = I386_ಪುಟ_ಗಾತ್ರ - (vir_addr % I386_ಪುಟ_ಗಾತ್ರ);
	vir_addr += len;
	ಮುಂದಿನ_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	ಆಗ (len < ಬೈಟುಗಳು) {
		ಆದರೆ (vm_lookup(proc, vir_addr, &phys, ಖಾಲಿ) != OK)
			ನಿಲ್ಲಿಸು;

		ಆದರೆ (ಮುಂದಿನ_phys != phys)
			ನಿಲ್ಲಿಸು;

		len += I386_ಪುಟ_ಗಾತ್ರ;
		vir_addr += I386_ಪುಟ_ಗಾತ್ರ;
		ಮುಂದಿನ_phys += I386_ಪುಟ_ಗಾತ್ರ;
	}

	/* We might now have overshot the requested length somewhat. */
	ಹಿಂತಿರುಗಿ MIN(ಬೈಟುಗಳು, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ vm_check_range(ರಚನೆ proc *caller, ರಚನೆ proc *ಗುರಿ,
	vir_ಬೈಟುಗಳು vir_addr, size_t ಬೈಟುಗಳು, ಪೂರ್ಣಾಂಕ writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	ಪೂರ್ಣಾಂಕ r;

	ಆದರೆ ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		ಹಿಂತಿರುಗಿ r;

	vm_suspend(caller, ಗುರಿ, vir_addr, ಬೈಟುಗಳು, VMSTYPE_KERNELCALL,
		writeflag);

	ಹಿಂತಿರುಗಿ VMSUSPEND;
}

#ಆದರೆ 0
ಸ್ಥಿರ ಅಕ್ಷರ *flagstr(u32_t e, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ dir)
{
	ಸ್ಥಿರ ಅಕ್ಷರ str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	ಆದರೆ(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	ಇಲ್ಲದಿದ್ದರೆ
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	ಹಿಂತಿರುಗಿ str;
}

ಸ್ಥಿರ ಶೂನ್ಯ vm_pt_print(u32_t *pagetable, ಸ್ಥಿರಾಂಕ u32_t v)
{
	ಪೂರ್ಣಾಂಕ pte;
	ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 0;

	ಪ್ರತಿಜ್ಞಾ(!((u32_t) pagetable % I386_ಪುಟ_ಗಾತ್ರ));

	ಗಾಗಿ(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		ಆದರೆ(!(pte_v & I386_VM_PRESENT))
			ಮುಂದುವರಿ;
		pfa = I386_VM_PFA(pte_v);
		ಬರೆ("%4d:%08lx:%08lx %2s ",
			pte, v + I386_ಪುಟ_ಗಾತ್ರ*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		ಲಂಬಸಾಲು++;
		ಆದರೆ(ಲಂಬಸಾಲು == 3) { ಬರೆ("\n"); ಲಂಬಸಾಲು = 0; }
	}
	ಆದರೆ(ಲಂಬಸಾಲು > 0) ಬರೆ("\n");

	ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ vm_print(u32_t *ಬೇರು)
{
	ಪೂರ್ಣಾಂಕ pde;

	ಪ್ರತಿಜ್ಞಾ(!((u32_t) ಬೇರು % I386_ಪುಟ_ಗಾತ್ರ));

	ಬರೆ("page table 0x%lx:\n", ಬೇರು);

	ಗಾಗಿ(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (ಬೇರು + pde));
		ಆದರೆ(!(pde_v & I386_VM_PRESENT))
			ಮುಂದುವರಿ;
		ಆದರೆ(pde_v & I386_VM_BIGPAGE) {
			ಬರೆ("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} ಇಲ್ಲದಿದ್ದರೆ {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			ಬರೆ("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_ಪುಟ_ಗಾತ್ರ);
			ಬರೆ("\n");
		}
	}


	ಹಿಂತಿರುಗಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ vm_memset(ರಚನೆ proc* caller, endpoint_t who, phys_ಬೈಟುಗಳು ph, ಪೂರ್ಣಾಂಕ c,
	phys_ಬೈಟುಗಳು ಎಣಿಕೆ_2)
{
	u32_t ನಮೂನೆ;
	ರಚನೆ proc *whoptr = ಖಾಲಿ;
	phys_ಬೈಟುಗಳು cur_ph = ph;
	phys_ಬೈಟುಗಳು ಎಡ = ಎಣಿಕೆ_2;
	phys_ಬೈಟುಗಳು ptr, chunk, pfa = 0;
	ಪೂರ್ಣಾಂಕ new_cr3, r = OK;

	ಆದರೆ ((r = check_resumed_caller(caller)) != OK)
		ಹಿಂತಿರುಗಿ r;

	/* NONE for physical, otherwise virtual */
	ಆದರೆ (who != NONE && !(whoptr = endpoint_lookup(who)))
		ಹಿಂತಿರುಗಿ ESRCH;

	c &= 0xFF;
	ನಮೂನೆ = c | (c << 8) | (c << 16) | (c << 24);

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ಪ್ರತಿಜ್ಞಾ(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	ಆಗ (ಎಡ > 0) {
		new_cr3 = 0;
		chunk = ಎಡ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		ಆದರೆ (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		ಆದರೆ ((pfa = phys_memset(ptr, ನಮೂನೆ, chunk))) {

			/* If a process pagefaults, VM may help out */
			ಆದರೆ (whoptr) {
				vm_suspend(caller, whoptr, ph, ಎಣಿಕೆ_2,
						   VMSTYPE_KERNELCALL, 1);
				ಪ್ರತಿಜ್ಞಾ(catch_pagefaults);
				catch_pagefaults = 0;
				ಹಿಂತಿರುಗಿ VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		ಎಡ -= chunk;
	}

	ಪ್ರತಿಜ್ಞಾ(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	ಪ್ರತಿಜ್ಞಾ(catch_pagefaults);
	catch_pagefaults = 0;

	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ virtual_copy_f(
  ರಚನೆ proc * caller,
  ರಚನೆ vir_addr *src_addr,	/* source virtual address */
  ರಚನೆ vir_addr *dst_addr,	/* destination virtual address */
  vir_ಬೈಟುಗಳು ಬೈಟುಗಳು,		/* # of bytes to copy  */
  ಪೂರ್ಣಾಂಕ vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ರಚನೆ vir_addr *vir_addr[2];	/* virtual source and destination address */
  ಪೂರ್ಣಾಂಕ i, r;
  ರಚನೆ proc *procs[2];

  ಪ್ರತಿಜ್ಞಾ((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  ಆದರೆ (ಬೈಟುಗಳು <= 0) ಹಿಂತಿರುಗಿ(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  ಗಾಗಿ (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	ಪೂರ್ಣಾಂಕ proc_nr;
	ರಚನೆ proc *p;

	ಆದರೆ(proc_e == NONE) {
		p = ಖಾಲಿ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ(!isokendpt(proc_e, &proc_nr)) {
			ಬರೆ("virtual_copy: no reasonable endpoint\n");
			ಹಿಂತಿರುಗಿ ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  ಆದರೆ ((r = check_resumed_caller(caller)) != OK)
	ಹಿಂತಿರುಗಿ r;

  ಆದರೆ((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->ಸ್ಥಾನಾಂತರ,
  	procs[_DST_], vir_addr[_DST_]->ಸ್ಥಾನಾಂತರ, ಬೈಟುಗಳು)) != OK) {
	ಪೂರ್ಣಾಂಕ writeflag;
  	ರಚನೆ proc *ಗುರಿ = ಖಾಲಿ;
  	phys_ಬೈಟುಗಳು lin;
  	ಆದರೆ(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	ಆದರೆ(!vmcheck || !caller) {
    		ಹಿಂತಿರುಗಿ r;
  	}

  	ಆದರೆ(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->ಸ್ಥಾನಾಂತರ;
  		ಗುರಿ = procs[_SRC_];
		writeflag = 0;
  	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->ಸ್ಥಾನಾಂತರ;
  		ಗುರಿ = procs[_DST_];
		writeflag = 1;
  	} ಇಲ್ಲದಿದ್ದರೆ {
  		panic("r strange: %d",  r);
  	}

	ಪ್ರತಿಜ್ಞಾ(caller);
	ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);

	vm_suspend(caller, ಗುರಿ, lin, ಬೈಟುಗಳು, VMSTYPE_KERNELCALL, writeflag);
	ಹಿಂತಿರುಗಿ VMSUSPEND;
  }

  ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_copy(ಸ್ಥಿರಾಂಕ endpoint_t from_proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು from_addr,
	ಸ್ಥಿರಾಂಕ endpoint_t ಗಮ್ಯ_proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು ಗಮ್ಯ_addr,
	size_t ಬೈಟುಗಳು)
{
  ರಚನೆ vir_addr src, dst;

  src.ಸ್ಥಾನಾಂತರ = from_addr;
  dst.ಸ್ಥಾನಾಂತರ = ಗಮ್ಯ_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = ಗಮ್ಯ_proc;
  ಪ್ರತಿಜ್ಞಾ(src.proc_nr_e != NONE);
  ಪ್ರತಿಜ್ಞಾ(dst.proc_nr_e != NONE);

  ಹಿಂತಿರುಗಿ virtual_copy(&src, &dst, ಬೈಟುಗಳು);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_copy_vmcheck(ರಚನೆ proc * caller,
	ಸ್ಥಿರಾಂಕ endpoint_t from_proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು from_addr,
	ಸ್ಥಿರಾಂಕ endpoint_t ಗಮ್ಯ_proc, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು ಗಮ್ಯ_addr,
	size_t ಬೈಟುಗಳು)
{
  ರಚನೆ vir_addr src, dst;

  src.ಸ್ಥಾನಾಂತರ = from_addr;
  dst.ಸ್ಥಾನಾಂತರ = ಗಮ್ಯ_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = ಗಮ್ಯ_proc;
  ಪ್ರತಿಜ್ಞಾ(src.proc_nr_e != NONE);
  ಪ್ರತಿಜ್ಞಾ(dst.proc_nr_e != NONE);

  ಹಿಂತಿರುಗಿ virtual_copy_vmcheck(caller, &src, &dst, ಬೈಟುಗಳು);
}

ಶೂನ್ಯ ಸ್ಮರಣೆ_init(ಶೂನ್ಯ)
{
	ಪ್ರತಿಜ್ಞಾ(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_ಆರಂಭ++;
	freepdes[nfreepdes++] = kinfo.freepde_ಆರಂಭ++;

	ಪ್ರತಿಜ್ಞಾ(kinfo.freepde_ಆರಂಭ < I386_VM_DIR_ENTRIES);
	ಪ್ರತಿಜ್ಞಾ(nfreepdes == 2);
	ಪ್ರತಿಜ್ಞಾ(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
ಶೂನ್ಯ arch_proc_init(ರಚನೆ proc *pr, ಸ್ಥಿರಾಂಕ u32_t ip, ಸ್ಥಿರಾಂಕ u32_t sp,
	ಸ್ಥಿರಾಂಕ u32_t ps_str, ಅಕ್ಷರ *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, ಗಾತ್ರ(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ oxpcie_mapping_ಸೂಚ್ಯಂಕ = -1,
	lapic_mapping_ಸೂಚ್ಯಂಕ = -1,
	ioapic_ಮೊದಲ_ಸೂಚ್ಯಂಕ = -1,
	ioapic_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ = -1,
	video_mem_mapping_ಸೂಚ್ಯಂಕ = -1,
	usermapped_glo_ಸೂಚ್ಯಂಕ = -1,
	usermapped_ಸೂಚ್ಯಂಕ = -1, ಮೊದಲ_um_idx = -1;

ಬಾಹ್ಯ ಅಕ್ಷರ *video_mem;

ಬಾಹ್ಯ ಅಕ್ಷರ usermapped_ಆರಂಭ, usermapped_ಕೊನೆ, usermapped_nonglo_ಆರಂಭ;

ಪೂರ್ಣಾಂಕ arch_phys_map(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ,
			phys_ಬೈಟುಗಳು *addr,
			phys_ಬೈಟುಗಳು *len,
			ಪೂರ್ಣಾಂಕ *flags)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮೊದಲ = 1;
	ಪೂರ್ಣಾಂಕ freeidx = 0;
	ಸ್ಥಿರ ಅಕ್ಷರ *ser_var = ಖಾಲಿ;
	u32_t glo_len = (u32_t) &usermapped_nonglo_ಆರಂಭ -
			(u32_t) &usermapped_ಆರಂಭ;

	ಆದರೆ(ಮೊದಲ) {
		memset(&minix_kerninfo, 0, ಗಾತ್ರ(minix_kerninfo));
		video_mem_mapping_ಸೂಚ್ಯಂಕ = freeidx++;
		ಆದರೆ(glo_len > 0) {
			usermapped_glo_ಸೂಚ್ಯಂಕ = freeidx++;
		}
		
		usermapped_ಸೂಚ್ಯಂಕ = freeidx++;
		ಮೊದಲ_um_idx = usermapped_ಸೂಚ್ಯಂಕ;
		ಆದರೆ(usermapped_glo_ಸೂಚ್ಯಂಕ != -1)
			ಮೊದಲ_um_idx = usermapped_glo_ಸೂಚ್ಯಂಕ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
		ಆದರೆ(lapic_addr)
			lapic_mapping_ಸೂಚ್ಯಂಕ = freeidx++;
		ಆದರೆ (ioapic_enabled) {
			ioapic_ಮೊದಲ_ಸೂಚ್ಯಂಕ = freeidx;
			ಪ್ರತಿಜ್ಞಾ(nioapics > 0);
			freeidx += nioapics;
			ioapic_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ = freeidx-1;
		}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_OXPCIE
		ಆದರೆ((ser_var = env_get("oxpcie"))) {
			ಆದರೆ(ser_var[0] != '0' || ser_var[1] != 'x') {
				ಬರೆ("oxpcie address in hex please\n");
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಬರೆ("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_ಸೂಚ್ಯಂಕ = freeidx++;
			}
		}
#ಆದರೆ_ಅಂತ್ಯ

		ಮೊದಲ = 0;
	}

	ಆದರೆ(ಸೂಚ್ಯಂಕ == usermapped_glo_ಸೂಚ್ಯಂಕ) {
		*addr = vir2phys(&usermapped_ಆರಂಭ);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		ಹಿಂತಿರುಗಿ OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ == usermapped_ಸೂಚ್ಯಂಕ) {
		*addr = vir2phys(&usermapped_nonglo_ಆರಂಭ);
		*len = (u32_t) &usermapped_ಕೊನೆ -
			(u32_t) &usermapped_nonglo_ಆರಂಭ;
		*flags = VMMF_USER;
		ಹಿಂತಿರುಗಿ OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೂಚ್ಯಂಕ == video_mem_mapping_ಸೂಚ್ಯಂಕ) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_ಬಫರ್;
		*len = I386_ಪುಟ_ಗಾತ್ರ;
		*flags = VMMF_WRITE;
		ಹಿಂತಿರುಗಿ OK;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೂಚ್ಯಂಕ == lapic_mapping_ಸೂಚ್ಯಂಕ) {
		/* map the local APIC if enabled */
		ಆದರೆ (!lapic_addr)
			ಹಿಂತಿರುಗಿ EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ಹಿಂತಿರುಗಿ OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ioapic_enabled && ಸೂಚ್ಯಂಕ >= ioapic_ಮೊದಲ_ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ <= ioapic_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ) {
		ಪೂರ್ಣಾಂಕ ioapic_idx = ಸೂಚ್ಯಂಕ - ioapic_ಮೊದಲ_ಸೂಚ್ಯಂಕ;
		*addr = io_apic[ioapic_idx].paddr;
		ಪ್ರತಿಜ್ಞಾ(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ಬರೆ("ioapic map: addr 0x%lx\n", *addr);
		ಹಿಂತಿರುಗಿ OK;
	}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ CONFIG_OXPCIE
	ಆದರೆ(ಸೂಚ್ಯಂಕ == oxpcie_mapping_ಸೂಚ್ಯಂಕ) {
		*addr = strtoul(ser_var+2, ಖಾಲಿ, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		ಹಿಂತಿರುಗಿ OK;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ EINVAL;
}

ಪೂರ್ಣಾಂಕ arch_phys_map_ಉತ್ತರ(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು addr)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
	/* if local APIC is enabled */
	ಆದರೆ (ಸೂಚ್ಯಂಕ == lapic_mapping_ಸೂಚ್ಯಂಕ && lapic_addr) {
		lapic_addr_vaddr = addr;
		ಹಿಂತಿರುಗಿ OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ioapic_enabled && ಸೂಚ್ಯಂಕ >= ioapic_ಮೊದಲ_ಸೂಚ್ಯಂಕ &&
		ಸೂಚ್ಯಂಕ <= ioapic_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ) {
		ಪೂರ್ಣಾಂಕ i = ಸೂಚ್ಯಂಕ - ioapic_ಮೊದಲ_ಸೂಚ್ಯಂಕ;
		io_apic[i].vaddr = addr;
		ಹಿಂತಿರುಗಿ OK;
	}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ CONFIG_OXPCIE
	ಆದರೆ (ಸೂಚ್ಯಂಕ == oxpcie_mapping_ಸೂಚ್ಯಂಕ) {
		oxpcie_ಗಣ_vaddr((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) addr);
		ಹಿಂತಿರುಗಿ OK;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ(ಸೂಚ್ಯಂಕ == ಮೊದಲ_um_idx) {
		ಬಾಹ್ಯ ರಚನೆ minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		ಬಾಹ್ಯ u32_t usermapped_ಸ್ಥಾನಾಂತರ;
		ಪ್ರತಿಜ್ಞಾ(addr > (u32_t) &usermapped_ಆರಂಭ);
		usermapped_ಸ್ಥಾನಾಂತರ = addr - (u32_t) &usermapped_ಆರಂಭ;
#ವ್ಯಾಖ್ಯಾನಿಸು FIXEDPTR(ptr) (ಶೂನ್ಯ *) ((u32_t)ptr + usermapped_ಸ್ಥಾನಾಂತರ)
#ವ್ಯಾಖ್ಯಾನಿಸು FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ನಿಯೋಜಿಸು(kinfo);
		ನಿಯೋಜಿಸು(machine);
		ನಿಯೋಜಿಸು(kmessages);
		ನಿಯೋಜಿಸು(loadinfo);
		ನಿಯೋಜಿಸು(kuserinfo);
		ನಿಯೋಜಿಸು(arm_frclock); /* eh, why not. */
		ನಿಯೋಜಿಸು(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		ಆದರೆ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} ಇಲ್ಲದಿದ್ದರೆ  ಆದರೆ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} ಇಲ್ಲದಿದ್ದರೆ	{
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
		minix_kerninfo_user = (vir_ಬೈಟುಗಳು) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		ಆದರೆ(env_get("libc_ipc")) {
			ಬರೆ("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = ಖಾಲಿ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		ಹಿಂತಿರುಗಿ OK;
	}

	ಆದರೆ(ಸೂಚ್ಯಂಕ == usermapped_ಸೂಚ್ಯಂಕ) ಹಿಂತಿರುಗಿ OK;

	ಆದರೆ (ಸೂಚ್ಯಂಕ == video_mem_mapping_ಸೂಚ್ಯಂಕ) {
		video_mem_vaddr =  addr;
		ಹಿಂತಿರುಗಿ OK;
	}

	ಹಿಂತಿರುಗಿ EINVAL;
}

ಪೂರ್ಣಾಂಕ arch_enable_paging(ರಚನೆ proc * caller)
{
	ಪ್ರತಿಜ್ಞಾ(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_ವಿಳಾಸ_space(caller);

	video_mem = (ಅಕ್ಷರ *) video_mem_vaddr;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	ಆದರೆ (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	ಆದರೆ (ioapic_enabled) {
		ಪೂರ್ಣಾಂಕ i;

		ಗಾಗಿ (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#ಆದರೆ CONFIG_SMP
	barrier();

	wait_for_APs_ಗಮ್ಯ_ಮುಕ್ತಾಯ_booting();
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	ಆದರೆ (watchdog_enabled)
		i386_watchdog_ಆರಂಭ();
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ OK;
}

ಶೂನ್ಯ release_ವಿಳಾಸ_space(ರಚನೆ proc *pr)
{
	pr->p_seg.p_cr3_v = ಖಾಲಿ;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
ಪೂರ್ಣಾಂಕ platform_tbl_checksum_ok(ಶೂನ್ಯ *ptr, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
	u8_t ಒಟ್ಟು = 0;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ (i = 0; i < ಉದ್ದಳತೆ; i++)
		ಒಟ್ಟು += ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ptr)[i];
	ಹಿಂತಿರುಗಿ !ಒಟ್ಟು;
}

ಪೂರ್ಣಾಂಕ platform_tbl_ptr(phys_ಬೈಟುಗಳು ಆರಂಭ,
					phys_ಬೈಟುಗಳು ಕೊನೆ,
					ಚಿಹ್ನೆರಹಿತ increment,
					ಶೂನ್ಯ * buff,
					ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2,
					phys_ಬೈಟುಗಳು * phys_addr,
					ಪೂರ್ಣಾಂಕ ((* cmp_f)(ಶೂನ್ಯ *)))
{
	phys_ಬೈಟುಗಳು addr;

	ಗಾಗಿ (addr = ಆರಂಭ; addr < ಕೊನೆ; addr += increment) {
		phys_copy (addr, (phys_ಬೈಟುಗಳು) buff, ಗಾತ್ರ_2);
		ಆದರೆ (cmp_f(buff)) {
			ಆದರೆ (phys_addr)
				*phys_addr = addr;
			ಹಿಂತಿರುಗಿ 1;
		}
	}
	ಹಿಂತಿರುಗಿ 0;
}
