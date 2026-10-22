
#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "kernel/vm.h"

#અન્તર્ભાવ <machine/vm.h>

#અન્તર્ભાવ <minix/syslib.h>
#અન્તર્ભાવ <minix/cpufeature.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ <machine/vm.h>

#અન્તર્ભાવ "oxpcie.h"
#અન્તર્ભાવ "arch_proto.h"

#ifdef USE_APIC
#અન્તર્ભાવ "apic.h"
#ifdef USE_WATCHDOG
#અન્તર્ભાવ "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
સ્થિર પૂર્ણાંક nfreepdes = 0;
#define MAXFREEPDES	2
સ્થિર પૂર્ણાંક freepdes[MAXFREEPDES];

સ્થિર u32_t phys_get32(phys_bytes v);

શૂન્ય mem_clear_mapcache(શૂન્ય)
{
	પૂર્ણાંક i;
	માટે(i = 0; i < nfreepdes; i++) {
		સંરચના proc *ptproc = get_cpulocal_var(ptproc);
		પૂર્ણાંક pde = freepdes[i];
		u32_t *ptv;
		પ્રતિજ્ઞા(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		પ્રતિજ્ઞા(ptv);
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
	પ્રતિજ્ઞા(pde >= 0 && pde < 1024);

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
		પ્રતિજ્ઞા(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} નહીંતર {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	જો(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
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
	પાછા I386_BIG_PAGE_SIZE*pde + offset;
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
	પ્રતિજ્ઞા(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	પ્રતિજ્ઞા(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	જો(srcproc) પ્રતિજ્ઞા(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	જો(dstproc) પ્રતિજ્ઞા(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	પ્રતિજ્ઞા(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
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

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		જો(changed)
			reload_cr3(); 

		/* Check for overflow. */
		જો (srcptr + chunk < srcptr) પાછા EFAULT_SRC;
		જો (dstptr + chunk < dstptr) પાછા EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		જો(addr) {
			/* If addr is nonzero, a page fault was caught. */

			જો(addr >= srcptr && addr < (srcptr + chunk)) {
				પાછા EFAULT_SRC;
			}
			જો(addr >= dstptr && addr < (dstptr + chunk)) {
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
	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

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

#જો 0
સ્થિર અક્ષર *cr0_str(u32_t e)
{
	સ્થિર અક્ષર str[80];
	strcpy(str, "");
#define FLAG(v) કરો { જો(e & (v)) { strcat(str, #v " "); e &= ~v; } } જ્યારે(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	જો(e) { strcat(str, " (++)"); }
	પાછા str;
}

સ્થિર અક્ષર *cr4_str(u32_t e)
{
	સ્થિર અક્ષર str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	જો(e) { strcat(str, " (++)"); }
	પાછા str;
}
#endif

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
	મૂળ = (u32_t *) proc->p_seg.p_cr3;
	પ્રતિજ્ઞા(!((u32_t) મૂળ % I386_PAGE_SIZE));
	pde = I386_VM_PDE(આભાસી);
	પ્રતિજ્ઞા(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (મૂળ + pde));

	જો(!(pde_v & I386_VM_PRESENT)) {
		પાછા EFAULT;
	}

	/* We don't expect to ever see this. */
	જો(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		જો(ptent) *ptent = pde_v;
		*physical += આભાસી & I386_VM_OFFSET_MASK_4MB;
	} નહીંતર {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		પ્રતિજ્ઞા(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(આભાસી);
		પ્રતિજ્ઞા(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		જો(!(pte_v & I386_VM_PRESENT)) {
			પાછા EFAULT;
		}

		જો(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += આભાસી % I386_PAGE_SIZE;
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

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	જ્યારે (len < bytes) {
		જો (vm_lookup(proc, vir_addr, &phys, ખાલી) != OK)
			તોડો;

		જો (next_phys != phys)
			તોડો;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
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

#જો 0
સ્થિર અક્ષર *flagstr(u32_t e, અચલ પૂર્ણાંક dir)
{
	સ્થિર અક્ષર str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	જો(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	નહીંતર
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	પાછા str;
}

સ્થિર શૂન્ય vm_pt_print(u32_t *pagetable, અચલ u32_t v)
{
	પૂર્ણાંક pte;
	પૂર્ણાંક col = 0;

	પ્રતિજ્ઞા(!((u32_t) pagetable % I386_PAGE_SIZE));

	માટે(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		જો(!(pte_v & I386_VM_PRESENT))
			ચાલુ;
		pfa = I386_VM_PFA(pte_v);
		printf("%4d:%08lx:%08lx %2s ",
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		જો(col == 3) { printf("\n"); col = 0; }
	}
	જો(col > 0) printf("\n");

	પાછા;
}

સ્થિર શૂન્ય vm_print(u32_t *મૂળ)
{
	પૂર્ણાંક pde;

	પ્રતિજ્ઞા(!((u32_t) મૂળ % I386_PAGE_SIZE));

	printf("page table 0x%lx:\n", મૂળ);

	માટે(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (મૂળ + pde));
		જો(!(pde_v & I386_VM_PRESENT))
			ચાલુ;
		જો(pde_v & I386_VM_BIGPAGE) {
			printf("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} નહીંતર {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			printf("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			printf("\n");
		}
	}


	પાછા;
}
#endif

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
	પૂર્ણાંક new_cr3, r = OK;

	જો ((r = check_resumed_caller(caller)) != OK)
		પાછા r;

	/* NONE for physical, otherwise virtual */
	જો (who != NONE && !(whoptr = endpoint_lookup(who)))
		પાછા ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	પ્રતિજ્ઞા(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	જ્યારે (ડાબું > 0) {
		new_cr3 = 0;
		chunk = ડાબું;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		જો (new_cr3)
			reload_cr3();

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

	પ્રતિજ્ઞા(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	પ્રતિજ્ઞા(catch_pagefaults);
	catch_pagefaults = 0;

	પાછા OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
પૂર્ણાંક virtual_copy_f(
  સંરચના proc * caller,
  સંરચના vir_addr *src_addr,	/* source virtual address */
  સંરચના vir_addr *dst_addr,	/* destination virtual address */
  vir_bytes bytes,		/* # of bytes to copy  */
  પૂર્ણાંક vmcheck			/* if nonzero, can return VMSUSPEND */
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

	પ્રતિજ્ઞા(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
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
	strlcpy(pr->p_name, name, કદ(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

સ્થિર પૂર્ણાંક oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

બાહ્ય અક્ષર *video_mem;

બાહ્ય અક્ષર usermapped_start, usermapped_end, usermapped_nonglo_start;

પૂર્ણાંક arch_phys_map(અચલ પૂર્ણાંક index,
			phys_bytes *addr,
			phys_bytes *len,
			પૂર્ણાંક *flags)
{
	સ્થિર પૂર્ણાંક first = 1;
	પૂર્ણાંક freeidx = 0;
	સ્થિર અક્ષર *ser_var = ખાલી;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	જો(first) {
		memset(&minix_kerninfo, 0, કદ(minix_kerninfo));
		video_mem_mapping_index = freeidx++;
		જો(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		જો(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		જો(lapic_addr)
			lapic_mapping_index = freeidx++;
		જો (ioapic_enabled) {
			ioapic_first_index = freeidx;
			પ્રતિજ્ઞા(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		જો((ser_var = env_get("oxpcie"))) {
			જો(ser_var[0] != '0' || ser_var[1] != 'x') {
				printf("oxpcie address in hex please\n");
			} નહીંતર {
				printf("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
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
	નહીંતર જો (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		પાછા OK;
	}
#ifdef USE_APIC
	નહીંતર જો (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		જો (!lapic_addr)
			પાછા EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		પાછા OK;
	}
	નહીંતર જો (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		પૂર્ણાંક ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		પ્રતિજ્ઞા(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		printf("ioapic map: addr 0x%lx\n", *addr);
		પાછા OK;
	}
#endif

#જો CONFIG_OXPCIE
	જો(index == oxpcie_mapping_index) {
		*addr = strtoul(ser_var+2, ખાલી, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		પાછા OK;
	}
#endif

	પાછા EINVAL;
}

પૂર્ણાંક arch_phys_map_reply(અચલ પૂર્ણાંક index, અચલ vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	જો (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		પાછા OK;
	}
	નહીંતર જો (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		પૂર્ણાંક i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		પાછા OK;
	}
#endif

#જો CONFIG_OXPCIE
	જો (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned અક્ષર *) addr);
		પાછા OK;
	}
#endif
	જો(index == first_um_idx) {
		બાહ્ય સંરચના minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		બાહ્ય u32_t usermapped_offset;
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
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		જો(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} નહીંતર  જો(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} નહીંતર	{
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
		જો(env_get("libc_ipc")) {
			printf("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = ખાલી;
		} નહીંતર {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		પાછા OK;
	}

	જો(index == usermapped_index) પાછા OK;

	જો (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		પાછા OK;
	}

	પાછા EINVAL;
}

પૂર્ણાંક arch_enable_paging(સંરચના proc * caller)
{
	પ્રતિજ્ઞા(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (અક્ષર *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	જો (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	જો (ioapic_enabled) {
		પૂર્ણાંક i;

		માટે (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#જો CONFIG_SMP
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
	જો (watchdog_enabled)
		i386_watchdog_start();
#endif

	પાછા OK;
}

શૂન્ય release_address_space(સંરચના proc *pr)
{
	pr->p_seg.p_cr3_v = ખાલી;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
પૂર્ણાંક platform_tbl_checksum_ok(શૂન્ય *ptr, unsigned પૂર્ણાંક length)
{
	u8_t total = 0;
	unsigned પૂર્ણાંક i;
	માટે (i = 0; i < length; i++)
		total += ((unsigned અક્ષર *)ptr)[i];
	પાછા !total;
}

પૂર્ણાંક platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					શૂન્ય * buff,
					unsigned size,
					phys_bytes * phys_addr,
					પૂર્ણાંક ((* cmp_f)(શૂન્ય *)))
{
	phys_bytes addr;

	માટે (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		જો (cmp_f(buff)) {
			જો (phys_addr)
				*phys_addr = addr;
			પાછા 1;
		}
	}
	પાછા 0;
}
