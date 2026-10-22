
#ضمن "kernel/kernel.h"
#ضمن "kernel/vm.h"

#ضمن <machine/vm.h>

#ضمن <minix/syslib.h>
#ضمن <minix/cpufeature.h>
#ضمن <string.h>
#ضمن <assert.h>
#ضمن <signal.h>
#ضمن <stdlib.h>

#ضمن <machine/vm.h>

#ضمن "oxpcie.h"
#ضمن "arch_proto.h"

#که_تعریف_شوی USE_APIC
#ضمن "apic.h"
#که_تعریف_شوی USE_WATCHDOG
#ضمن "kernel/watchdog.h"
#د_که_پای
#د_که_پای

phys_بایټونه video_mem_vaddr = 0;

#تعریف_کړه HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
ساكن صحيح nfreepdes = 0;
#تعریف_کړه MAXFREEPDES	2
ساكن صحيح freepdes[MAXFREEPDES];

ساكن u32_t phys_get32(phys_بایټونه v);

فراغ mem_clear_mapcache(فراغ)
{
	صحيح i;
	لكل(i = 0; i < nfreepdes; i++) {
		بنية proc *ptproc = get_cpulocal_var(ptproc);
		صحيح pde = freepdes[i];
		u32_t *ptv;
		أكد(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		أكد(ptv);
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
ساكن phys_بایټونه createpde(
	ثابت بنية proc *pr,	/* Requested process, NULL for physical. */
	ثابت phys_بایټونه linaddr,/* Address after segment translation. */
	phys_بایټونه *بایټونه,	/* Size of chunk, function may truncate it. */
	صحيح free_pde_idx,	/* index of the free slot to use */
	صحيح *بدل_شوی		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_بایټونه دننه_واټن;
	صحيح pde;

	أكد(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	أكد(pde >= 0 && pde < 1024);

	إذا(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		أرجع linaddr;
	}

	إذا(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		أكد(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} وإلا {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_ماسک_4_MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	إذا(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*بدل_شوی = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	دننه_واټن = linaddr & I386_VM_دننه_واټن_ماسک_4_MB; /* Offset in 4MB window. */
	*بایټونه = MIN(*بایټونه, I386_BIG_پاڼه_اندازه - دننه_واټن); 

	/* Return the linear address of the start of the new mapping. */
	أرجع I386_BIG_پاڼه_اندازه*pde + دننه_واټن;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ساكن صحيح check_resumed_caller(بنية proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	إذا (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		أكد(caller->p_vmrequest.vmresult != VMSUSPEND);
		أرجع caller->p_vmrequest.vmresult;
	}

	أرجع OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ساكن صحيح lin_lin_copy(بنية proc *srcproc, vir_بایټونه srclinaddr, 
	بنية proc *dstproc, vir_بایټونه dstlinaddr, vir_بایټونه بایټونه)
{
	u32_t addr;
	proc_nr_t procslot;

	أكد(get_cpulocal_var(ptproc));
	أكد(get_cpulocal_var(proc_ptr));
	أكد(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	أكد(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	إذا(srcproc) أكد(!RTS_ISSET(srcproc, RTS_خونه_FREE));
	إذا(dstproc) أكد(!RTS_ISSET(dstproc, RTS_خونه_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_خونه_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	إذا(srcproc) أكد(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	إذا(dstproc) أكد(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	طالما(بایټونه > 0) {
		phys_بایټونه srcptr, dstptr;
		vir_بایټونه chunk = بایټونه;
		صحيح بدل_شوی = 0;

#که_تعریف_شوی CONFIG_SMP
		بینښه cpu = cpuid;

		إذا (srcproc && GET_بټ(srcproc->p_stale_tlb, cpu)) {
			بدل_شوی = 1;
			UNSET_بټ(srcproc->p_stale_tlb, cpu);
		}
		إذا (dstproc && GET_بټ(dstproc->p_stale_tlb, cpu)) {
			بدل_شوی = 1;
			UNSET_بټ(dstproc->p_stale_tlb, cpu);
		}
#د_که_پای

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &بدل_شوی);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &بدل_شوی);
		إذا(بدل_شوی)
			reload_cr3(); 

		/* Check for overflow. */
		إذا (srcptr + chunk < srcptr) أرجع EFAULT_SRC;
		إذا (dstptr + chunk < dstptr) أرجع EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		إذا(addr) {
			/* If addr is nonzero, a page fault was caught. */

			إذا(addr >= srcptr && addr < (srcptr + chunk)) {
				أرجع EFAULT_SRC;
			}
			إذا(addr >= dstptr && addr < (dstptr + chunk)) {
				أرجع EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			أرجع EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		بایټونه -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	إذا(srcproc) أكد(!RTS_ISSET(srcproc, RTS_خونه_FREE));
	إذا(dstproc) أكد(!RTS_ISSET(dstproc, RTS_خونه_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_خونه_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	أرجع OK;
}


ساكن u32_t phys_get32(phys_بایټونه addr)
{
	u32_t v;
	صحيح r;

	إذا((r=lin_lin_copy(عدم, addr, 
		proc_addr(SYSTEM), (phys_بایټونه) &v, حجم(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	أرجع v;
}

#إذا 0
ساكن حرف *cr0_str(u32_t e)
{
	ساكن حرف str[80];
	strcpy(str, "");
#تعریف_کړه FLAG(v) افعل { إذا(e & (v)) { strcat(str, #v " "); e &= ~v; } } طالما(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	إذا(e) { strcat(str, " (++)"); }
	أرجع str;
}

ساكن حرف *cr4_str(u32_t e)
{
	ساكن حرف str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	إذا(e) { strcat(str, " (++)"); }
	أرجع str;
}
#د_که_پای

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_بایټونه umap_virtual(
  سجل بنية proc *rp,		/* pointer to proc table entry for process */
  صحيح seg,				/* T, D, or S segment */
  vir_بایټونه vir_addr,			/* virtual address in bytes within the seg */
  vir_بایټونه بایټونه			/* # of bytes to be copied */
)
{
	phys_بایټونه phys = 0;

	إذا(vm_lookup(rp, vir_addr, &phys, عدم) != OK) {
		چاپ_کړه("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} وإلا {
		إذا(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	إذا(phys == 0) {
		چاپ_کړه("SYSTEM:umap_virtual: lookup failed\n");
		أرجع 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	إذا(بایټونه > 0 && vm_lookup_range(rp, vir_addr, عدم, بایټونه) != بایټونه) {
		چاپ_کړه("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, بایټونه, vir_addr, vir_addr);
		أرجع 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	أكد(phys);

	أرجع phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
صحيح vm_lookup(ثابت بنية proc *proc, ثابت vir_بایټونه مجازي,
 phys_بایټونه *physical, u32_t *ptent)
{
	u32_t *ریښه, *pt;
	صحيح pde, pte;
	u32_t pde_v, pte_v;

	أكد(proc);
	أكد(physical);
	أكد(!isemptyp(proc));
	أكد(HASPT(proc));

	/* Retrieve page directory entry. */
	ریښه = (u32_t *) proc->p_seg.p_cr3;
	أكد(!((u32_t) ریښه % I386_پاڼه_اندازه));
	pde = I386_VM_PDE(مجازي);
	أكد(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ریښه + pde));

	إذا(!(pde_v & I386_VM_PRESENT)) {
		أرجع EFAULT;
	}

	/* We don't expect to ever see this. */
	إذا(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_ماسک_4_MB;
		إذا(ptent) *ptent = pde_v;
		*physical += مجازي & I386_VM_دننه_واټن_ماسک_4_MB;
	} وإلا {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		أكد(!((u32_t) pt % I386_پاڼه_اندازه));
		pte = I386_VM_PTE(مجازي);
		أكد(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		إذا(!(pte_v & I386_VM_PRESENT)) {
			أرجع EFAULT;
		}

		إذا(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += مجازي % I386_پاڼه_اندازه;
	}

	أرجع OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ثابت بنية proc *proc, vir_بایټونه vir_addr,
	phys_بایټونه *phys_addr, size_t بایټونه)
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
	phys_بایټونه phys, بل_phys;
	size_t len;

	أكد(proc);
	أكد(بایټونه > 0);
	أكد(HASPT(proc));

	/* Look up the first page. */
	إذا (vm_lookup(proc, vir_addr, &phys, عدم) != OK)
		أرجع 0;

	إذا (phys_addr != عدم)
		*phys_addr = phys;

	len = I386_پاڼه_اندازه - (vir_addr % I386_پاڼه_اندازه);
	vir_addr += len;
	بل_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	طالما (len < بایټونه) {
		إذا (vm_lookup(proc, vir_addr, &phys, عدم) != OK)
			اكسر;

		إذا (بل_phys != phys)
			اكسر;

		len += I386_پاڼه_اندازه;
		vir_addr += I386_پاڼه_اندازه;
		بل_phys += I386_پاڼه_اندازه;
	}

	/* We might now have overshot the requested length somewhat. */
	أرجع MIN(بایټونه, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
صحيح vm_check_range(بنية proc *caller, بنية proc *موخه,
	vir_بایټونه vir_addr, size_t بایټونه, صحيح writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	صحيح r;

	إذا ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		أرجع r;

	vm_suspend(caller, موخه, vir_addr, بایټونه, VMSTYPE_KERNELCALL,
		writeflag);

	أرجع VMSUSPEND;
}

#إذا 0
ساكن حرف *flagstr(u32_t e, ثابت صحيح dir)
{
	ساكن حرف str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	إذا(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	وإلا
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	أرجع str;
}

ساكن فراغ vm_pt_print(u32_t *pagetable, ثابت u32_t v)
{
	صحيح pte;
	صحيح ستون = 0;

	أكد(!((u32_t) pagetable % I386_پاڼه_اندازه));

	لكل(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		إذا(!(pte_v & I386_VM_PRESENT))
			استمر;
		pfa = I386_VM_PFA(pte_v);
		چاپ_کړه("%4d:%08lx:%08lx %2s ",
			pte, v + I386_پاڼه_اندازه*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		ستون++;
		إذا(ستون == 3) { چاپ_کړه("\n"); ستون = 0; }
	}
	إذا(ستون > 0) چاپ_کړه("\n");

	أرجع;
}

ساكن فراغ vm_print(u32_t *ریښه)
{
	صحيح pde;

	أكد(!((u32_t) ریښه % I386_پاڼه_اندازه));

	چاپ_کړه("page table 0x%lx:\n", ریښه);

	لكل(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (ریښه + pde));
		إذا(!(pde_v & I386_VM_PRESENT))
			استمر;
		إذا(pde_v & I386_VM_BIGPAGE) {
			چاپ_کړه("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} وإلا {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			چاپ_کړه("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_پاڼه_اندازه);
			چاپ_کړه("\n");
		}
	}


	أرجع;
}
#د_که_پای

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
صحيح vm_memset(بنية proc* caller, endpoint_t who, phys_بایټونه ph, صحيح c,
	phys_بایټونه شمېر)
{
	u32_t نمونه;
	بنية proc *whoptr = عدم;
	phys_بایټونه cur_ph = ph;
	phys_بایټونه کیڼ = شمېر;
	phys_بایټونه ptr, chunk, pfa = 0;
	صحيح new_cr3, r = OK;

	إذا ((r = check_resumed_caller(caller)) != OK)
		أرجع r;

	/* NONE for physical, otherwise virtual */
	إذا (who != NONE && !(whoptr = endpoint_lookup(who)))
		أرجع ESRCH;

	c &= 0xFF;
	نمونه = c | (c << 8) | (c << 16) | (c << 24);

	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	أكد(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	طالما (کیڼ > 0) {
		new_cr3 = 0;
		chunk = کیڼ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		إذا (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		إذا ((pfa = phys_memset(ptr, نمونه, chunk))) {

			/* If a process pagefaults, VM may help out */
			إذا (whoptr) {
				vm_suspend(caller, whoptr, ph, شمېر,
						   VMSTYPE_KERNELCALL, 1);
				أكد(catch_pagefaults);
				catch_pagefaults = 0;
				أرجع VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		کیڼ -= chunk;
	}

	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	أكد(catch_pagefaults);
	catch_pagefaults = 0;

	أرجع OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
صحيح virtual_copy_f(
  بنية proc * caller,
  بنية vir_addr *src_addr,	/* source virtual address */
  بنية vir_addr *dst_addr,	/* destination virtual address */
  vir_بایټونه بایټونه,		/* # of bytes to copy  */
  صحيح vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  بنية vir_addr *vir_addr[2];	/* virtual source and destination address */
  صحيح i, r;
  بنية proc *procs[2];

  أكد((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  إذا (بایټونه <= 0) أرجع(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  لكل (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	صحيح proc_nr;
	بنية proc *p;

	إذا(proc_e == NONE) {
		p = عدم;
	} وإلا {
		إذا(!isokendpt(proc_e, &proc_nr)) {
			چاپ_کړه("virtual_copy: no reasonable endpoint\n");
			أرجع ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  إذا ((r = check_resumed_caller(caller)) != OK)
	أرجع r;

  إذا((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->دننه_واټن,
  	procs[_DST_], vir_addr[_DST_]->دننه_واټن, بایټونه)) != OK) {
	صحيح writeflag;
  	بنية proc *موخه = عدم;
  	phys_بایټونه lin;
  	إذا(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	إذا(!vmcheck || !caller) {
    		أرجع r;
  	}

  	إذا(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->دننه_واټن;
  		موخه = procs[_SRC_];
		writeflag = 0;
  	} وإلا إذا(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->دننه_واټن;
  		موخه = procs[_DST_];
		writeflag = 1;
  	} وإلا {
  		panic("r strange: %d",  r);
  	}

	أكد(caller);
	أكد(موخه);

	vm_suspend(caller, موخه, lin, بایټونه, VMSTYPE_KERNELCALL, writeflag);
	أرجع VMSUSPEND;
  }

  أرجع OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
صحيح معلومات_copy(ثابت endpoint_t from_proc, ثابت vir_بایټونه from_addr,
	ثابت endpoint_t منزل_proc, ثابت vir_بایټونه منزل_addr,
	size_t بایټونه)
{
  بنية vir_addr src, dst;

  src.دننه_واټن = from_addr;
  dst.دننه_واټن = منزل_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = منزل_proc;
  أكد(src.proc_nr_e != NONE);
  أكد(dst.proc_nr_e != NONE);

  أرجع virtual_copy(&src, &dst, بایټونه);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
صحيح معلومات_copy_vmcheck(بنية proc * caller,
	ثابت endpoint_t from_proc, ثابت vir_بایټونه from_addr,
	ثابت endpoint_t منزل_proc, ثابت vir_بایټونه منزل_addr,
	size_t بایټونه)
{
  بنية vir_addr src, dst;

  src.دننه_واټن = from_addr;
  dst.دننه_واټن = منزل_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = منزل_proc;
  أكد(src.proc_nr_e != NONE);
  أكد(dst.proc_nr_e != NONE);

  أرجع virtual_copy_vmcheck(caller, &src, &dst, بایټونه);
}

فراغ حافظه_init(فراغ)
{
	أكد(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_پیل++;
	freepdes[nfreepdes++] = kinfo.freepde_پیل++;

	أكد(kinfo.freepde_پیل < I386_VM_DIR_ENTRIES);
	أكد(nfreepdes == 2);
	أكد(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
فراغ arch_proc_init(بنية proc *pr, ثابت u32_t ip, ثابت u32_t sp,
	ثابت u32_t ps_str, حرف *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, حجم(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

ساكن صحيح oxpcie_mapping_شاخص = -1,
	lapic_mapping_شاخص = -1,
	ioapic_لومړی_شاخص = -1,
	ioapic_وروستی_شاخص = -1,
	video_mem_mapping_شاخص = -1,
	usermapped_glo_شاخص = -1,
	usermapped_شاخص = -1, لومړی_um_idx = -1;

خارجي حرف *video_mem;

خارجي حرف usermapped_پیل, usermapped_پای_ټکی, usermapped_nonglo_پیل;

صحيح arch_phys_map(ثابت صحيح شاخص,
			phys_بایټونه *addr,
			phys_بایټونه *len,
			صحيح *flags)
{
	ساكن صحيح لومړی = 1;
	صحيح freeidx = 0;
	ساكن حرف *ser_var = عدم;
	u32_t glo_len = (u32_t) &usermapped_nonglo_پیل -
			(u32_t) &usermapped_پیل;

	إذا(لومړی) {
		memset(&minix_kerninfo, 0, حجم(minix_kerninfo));
		video_mem_mapping_شاخص = freeidx++;
		إذا(glo_len > 0) {
			usermapped_glo_شاخص = freeidx++;
		}
		
		usermapped_شاخص = freeidx++;
		لومړی_um_idx = usermapped_شاخص;
		إذا(usermapped_glo_شاخص != -1)
			لومړی_um_idx = usermapped_glo_شاخص;

#که_تعریف_شوی USE_APIC
		إذا(lapic_addr)
			lapic_mapping_شاخص = freeidx++;
		إذا (ioapic_enabled) {
			ioapic_لومړی_شاخص = freeidx;
			أكد(nioapics > 0);
			freeidx += nioapics;
			ioapic_وروستی_شاخص = freeidx-1;
		}
#د_که_پای

#که_تعریف_شوی CONFIG_OXPCIE
		إذا((ser_var = env_get("oxpcie"))) {
			إذا(ser_var[0] != '0' || ser_var[1] != 'x') {
				چاپ_کړه("oxpcie address in hex please\n");
			} وإلا {
				چاپ_کړه("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_شاخص = freeidx++;
			}
		}
#د_که_پای

		لومړی = 0;
	}

	إذا(شاخص == usermapped_glo_شاخص) {
		*addr = vir2phys(&usermapped_پیل);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		أرجع OK;
	}
	وإلا إذا(شاخص == usermapped_شاخص) {
		*addr = vir2phys(&usermapped_nonglo_پیل);
		*len = (u32_t) &usermapped_پای_ټکی -
			(u32_t) &usermapped_nonglo_پیل;
		*flags = VMMF_USER;
		أرجع OK;
	}
	وإلا إذا (شاخص == video_mem_mapping_شاخص) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_بفر;
		*len = I386_پاڼه_اندازه;
		*flags = VMMF_WRITE;
		أرجع OK;
	}
#که_تعریف_شوی USE_APIC
	وإلا إذا (شاخص == lapic_mapping_شاخص) {
		/* map the local APIC if enabled */
		إذا (!lapic_addr)
			أرجع EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		أرجع OK;
	}
	وإلا إذا (ioapic_enabled && شاخص >= ioapic_لومړی_شاخص && شاخص <= ioapic_وروستی_شاخص) {
		صحيح ioapic_idx = شاخص - ioapic_لومړی_شاخص;
		*addr = io_apic[ioapic_idx].paddr;
		أكد(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		چاپ_کړه("ioapic map: addr 0x%lx\n", *addr);
		أرجع OK;
	}
#د_که_پای

#إذا CONFIG_OXPCIE
	إذا(شاخص == oxpcie_mapping_شاخص) {
		*addr = strtoul(ser_var+2, عدم, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		أرجع OK;
	}
#د_که_پای

	أرجع EINVAL;
}

صحيح arch_phys_map_ځواب(ثابت صحيح شاخص, ثابت vir_بایټونه addr)
{
#که_تعریف_شوی USE_APIC
	/* if local APIC is enabled */
	إذا (شاخص == lapic_mapping_شاخص && lapic_addr) {
		lapic_addr_vaddr = addr;
		أرجع OK;
	}
	وإلا إذا (ioapic_enabled && شاخص >= ioapic_لومړی_شاخص &&
		شاخص <= ioapic_وروستی_شاخص) {
		صحيح i = شاخص - ioapic_لومړی_شاخص;
		io_apic[i].vaddr = addr;
		أرجع OK;
	}
#د_که_پای

#إذا CONFIG_OXPCIE
	إذا (شاخص == oxpcie_mapping_شاخص) {
		oxpcie_سېټ_vaddr((بینښه حرف *) addr);
		أرجع OK;
	}
#د_که_پای
	إذا(شاخص == لومړی_um_idx) {
		خارجي بنية minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		خارجي u32_t usermapped_دننه_واټن;
		أكد(addr > (u32_t) &usermapped_پیل);
		usermapped_دننه_واټن = addr - (u32_t) &usermapped_پیل;
#تعریف_کړه FIXEDPTR(ptr) (فراغ *) ((u32_t)ptr + usermapped_دننه_واټن)
#تعریف_کړه FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#تعریف_کړه ټاکل(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ټاکل(kinfo);
		ټاکل(machine);
		ټاکل(kmessages);
		ټاکل(loadinfo);
		ټاکل(kuserinfo);
		ټاکل(arm_frclock); /* eh, why not. */
		ټاکل(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		إذا(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} وإلا  إذا(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} وإلا	{
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
		minix_kerninfo_user = (vir_بایټونه) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		إذا(env_get("libc_ipc")) {
			چاپ_کړه("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = عدم;
		} وإلا {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		أرجع OK;
	}

	إذا(شاخص == usermapped_شاخص) أرجع OK;

	إذا (شاخص == video_mem_mapping_شاخص) {
		video_mem_vaddr =  addr;
		أرجع OK;
	}

	أرجع EINVAL;
}

صحيح arch_enable_paging(بنية proc * caller)
{
	أكد(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_پته_space(caller);

	video_mem = (حرف *) video_mem_vaddr;

#که_تعریف_شوی USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	إذا (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	إذا (ioapic_enabled) {
		صحيح i;

		لكل (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#إذا CONFIG_SMP
	barrier();

	wait_for_APs_منزل_پای_booting();
#د_که_پای
#د_که_پای

#که_تعریف_شوی USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	إذا (watchdog_enabled)
		i386_watchdog_پیل();
#د_که_پای

	أرجع OK;
}

فراغ release_پته_space(بنية proc *pr)
{
	pr->p_seg.p_cr3_v = عدم;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
صحيح platform_tbl_checksum_ok(فراغ *ptr, بینښه صحيح اوږدتیا)
{
	u8_t ټول = 0;
	بینښه صحيح i;
	لكل (i = 0; i < اوږدتیا; i++)
		ټول += ((بینښه حرف *)ptr)[i];
	أرجع !ټول;
}

صحيح platform_tbl_ptr(phys_بایټونه پیل,
					phys_بایټونه پای_ټکی,
					بینښه increment,
					فراغ * buff,
					بینښه اندازه,
					phys_بایټونه * phys_addr,
					صحيح ((* cmp_f)(فراغ *)))
{
	phys_بایټونه addr;

	لكل (addr = پیل; addr < پای_ټکی; addr += increment) {
		phys_copy (addr, (phys_بایټونه) buff, اندازه);
		إذا (cmp_f(buff)) {
			إذا (phys_addr)
				*phys_addr = addr;
			أرجع 1;
		}
	}
	أرجع 0;
}
