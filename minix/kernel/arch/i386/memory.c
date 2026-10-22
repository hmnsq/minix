
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

#جيڪڏهن_وصف_ٿيل USE_APIC
#ضمن "apic.h"
#جيڪڏهن_وصف_ٿيل USE_WATCHDOG
#ضمن "kernel/watchdog.h"
#جيڪڏهن_پورو
#جيڪڏهن_پورو

phys_بائيٽون video_mem_vaddr = 0;

#وصف_ڪريو HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
ساكن صحيح nfreepdes = 0;
#وصف_ڪريو MAXFREEPDES	2
ساكن صحيح freepdes[MAXFREEPDES];

ساكن u32_t phys_get32(phys_بائيٽون v);

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
ساكن phys_بائيٽون createpde(
	ثابت بنية proc *pr,	/* Requested process, NULL for physical. */
	ثابت phys_بائيٽون linaddr,/* Address after segment translation. */
	phys_بائيٽون *بائيٽون,	/* Size of chunk, function may truncate it. */
	صحيح free_pde_idx,	/* index of the free slot to use */
	صحيح *بدليو		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_بائيٽون وٿي;
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
		pdeval = (linaddr & I386_VM_ADDR_نقاب_4_MB) | 
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
		*بدليو = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	وٿي = linaddr & I386_VM_وٿي_نقاب_4_MB; /* Offset in 4MB window. */
	*بائيٽون = MIN(*بائيٽون, I386_BIG_صفحو_سائيز - وٿي); 

	/* Return the linear address of the start of the new mapping. */
	أرجع I386_BIG_صفحو_سائيز*pde + وٿي;
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
ساكن صحيح lin_lin_copy(بنية proc *srcproc, vir_بائيٽون srclinaddr, 
	بنية proc *dstproc, vir_بائيٽون dstlinaddr, vir_بائيٽون بائيٽون)
{
	u32_t addr;
	proc_nr_t procslot;

	أكد(get_cpulocal_var(ptproc));
	أكد(get_cpulocal_var(proc_ptr));
	أكد(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	أكد(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	إذا(srcproc) أكد(!RTS_ISSET(srcproc, RTS_جڳهه_FREE));
	إذا(dstproc) أكد(!RTS_ISSET(dstproc, RTS_جڳهه_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_جڳهه_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	إذا(srcproc) أكد(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	إذا(dstproc) أكد(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	طالما(بائيٽون > 0) {
		phys_بائيٽون srcptr, dstptr;
		vir_بائيٽون chunk = بائيٽون;
		صحيح بدليو = 0;

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
		بنانشان cpu = cpuid;

		إذا (srcproc && GET_ٻٽو_انگ(srcproc->p_stale_tlb, cpu)) {
			بدليو = 1;
			UNSET_ٻٽو_انگ(srcproc->p_stale_tlb, cpu);
		}
		إذا (dstproc && GET_ٻٽو_انگ(dstproc->p_stale_tlb, cpu)) {
			بدليو = 1;
			UNSET_ٻٽو_انگ(dstproc->p_stale_tlb, cpu);
		}
#جيڪڏهن_پورو

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &بدليو);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &بدليو);
		إذا(بدليو)
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
		بائيٽون -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	إذا(srcproc) أكد(!RTS_ISSET(srcproc, RTS_جڳهه_FREE));
	إذا(dstproc) أكد(!RTS_ISSET(dstproc, RTS_جڳهه_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_جڳهه_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	أرجع OK;
}


ساكن u32_t phys_get32(phys_بائيٽون addr)
{
	u32_t v;
	صحيح r;

	إذا((r=lin_lin_copy(عدم, addr, 
		proc_addr(SYSTEM), (phys_بائيٽون) &v, حجم(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	أرجع v;
}

#إذا 0
ساكن حرف *cr0_str(u32_t e)
{
	ساكن حرف str[80];
	strcpy(str, "");
#وصف_ڪريو FLAG(v) افعل { إذا(e & (v)) { strcat(str, #v " "); e &= ~v; } } طالما(0)
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
#جيڪڏهن_پورو

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_بائيٽون umap_virtual(
  سجل بنية proc *rp,		/* pointer to proc table entry for process */
  صحيح seg,				/* T, D, or S segment */
  vir_بائيٽون vir_addr,			/* virtual address in bytes within the seg */
  vir_بائيٽون بائيٽون			/* # of bytes to be copied */
)
{
	phys_بائيٽون phys = 0;

	إذا(vm_lookup(rp, vir_addr, &phys, عدم) != OK) {
		لکو("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} وإلا {
		إذا(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	إذا(phys == 0) {
		لکو("SYSTEM:umap_virtual: lookup failed\n");
		أرجع 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	إذا(بائيٽون > 0 && vm_lookup_range(rp, vir_addr, عدم, بائيٽون) != بائيٽون) {
		لکو("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, بائيٽون, vir_addr, vir_addr);
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
صحيح vm_lookup(ثابت بنية proc *proc, ثابت vir_بائيٽون مجازي,
 phys_بائيٽون *physical, u32_t *ptent)
{
	u32_t *پاڙ, *pt;
	صحيح pde, pte;
	u32_t pde_v, pte_v;

	أكد(proc);
	أكد(physical);
	أكد(!isemptyp(proc));
	أكد(HASPT(proc));

	/* Retrieve page directory entry. */
	پاڙ = (u32_t *) proc->p_seg.p_cr3;
	أكد(!((u32_t) پاڙ % I386_صفحو_سائيز));
	pde = I386_VM_PDE(مجازي);
	أكد(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (پاڙ + pde));

	إذا(!(pde_v & I386_VM_PRESENT)) {
		أرجع EFAULT;
	}

	/* We don't expect to ever see this. */
	إذا(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_نقاب_4_MB;
		إذا(ptent) *ptent = pde_v;
		*physical += مجازي & I386_VM_وٿي_نقاب_4_MB;
	} وإلا {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		أكد(!((u32_t) pt % I386_صفحو_سائيز));
		pte = I386_VM_PTE(مجازي);
		أكد(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		إذا(!(pte_v & I386_VM_PRESENT)) {
			أرجع EFAULT;
		}

		إذا(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += مجازي % I386_صفحو_سائيز;
	}

	أرجع OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ثابت بنية proc *proc, vir_بائيٽون vir_addr,
	phys_بائيٽون *phys_addr, size_t بائيٽون)
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
	phys_بائيٽون phys, ايندڙ_phys;
	size_t len;

	أكد(proc);
	أكد(بائيٽون > 0);
	أكد(HASPT(proc));

	/* Look up the first page. */
	إذا (vm_lookup(proc, vir_addr, &phys, عدم) != OK)
		أرجع 0;

	إذا (phys_addr != عدم)
		*phys_addr = phys;

	len = I386_صفحو_سائيز - (vir_addr % I386_صفحو_سائيز);
	vir_addr += len;
	ايندڙ_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	طالما (len < بائيٽون) {
		إذا (vm_lookup(proc, vir_addr, &phys, عدم) != OK)
			اكسر;

		إذا (ايندڙ_phys != phys)
			اكسر;

		len += I386_صفحو_سائيز;
		vir_addr += I386_صفحو_سائيز;
		ايندڙ_phys += I386_صفحو_سائيز;
	}

	/* We might now have overshot the requested length somewhat. */
	أرجع MIN(بائيٽون, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
صحيح vm_check_range(بنية proc *caller, بنية proc *هدف,
	vir_بائيٽون vir_addr, size_t بائيٽون, صحيح writeflag)
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

	vm_suspend(caller, هدف, vir_addr, بائيٽون, VMSTYPE_KERNELCALL,
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
	صحيح کانو = 0;

	أكد(!((u32_t) pagetable % I386_صفحو_سائيز));

	لكل(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		إذا(!(pte_v & I386_VM_PRESENT))
			استمر;
		pfa = I386_VM_PFA(pte_v);
		لکو("%4d:%08lx:%08lx %2s ",
			pte, v + I386_صفحو_سائيز*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		کانو++;
		إذا(کانو == 3) { لکو("\n"); کانو = 0; }
	}
	إذا(کانو > 0) لکو("\n");

	أرجع;
}

ساكن فراغ vm_print(u32_t *پاڙ)
{
	صحيح pde;

	أكد(!((u32_t) پاڙ % I386_صفحو_سائيز));

	لکو("page table 0x%lx:\n", پاڙ);

	لكل(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (پاڙ + pde));
		إذا(!(pde_v & I386_VM_PRESENT))
			استمر;
		إذا(pde_v & I386_VM_BIGPAGE) {
			لکو("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} وإلا {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			لکو("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_صفحو_سائيز);
			لکو("\n");
		}
	}


	أرجع;
}
#جيڪڏهن_پورو

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
صحيح vm_memset(بنية proc* caller, endpoint_t who, phys_بائيٽون ph, صحيح c,
	phys_بائيٽون ڳڻپ)
{
	u32_t نمونو;
	بنية proc *whoptr = عدم;
	phys_بائيٽون cur_ph = ph;
	phys_بائيٽون کاٻو = ڳڻپ;
	phys_بائيٽون ptr, chunk, pfa = 0;
	صحيح new_cr3, r = OK;

	إذا ((r = check_resumed_caller(caller)) != OK)
		أرجع r;

	/* NONE for physical, otherwise virtual */
	إذا (who != NONE && !(whoptr = endpoint_lookup(who)))
		أرجع ESRCH;

	c &= 0xFF;
	نمونو = c | (c << 8) | (c << 16) | (c << 24);

	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	أكد(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	طالما (کاٻو > 0) {
		new_cr3 = 0;
		chunk = کاٻو;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		إذا (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		إذا ((pfa = phys_memset(ptr, نمونو, chunk))) {

			/* If a process pagefaults, VM may help out */
			إذا (whoptr) {
				vm_suspend(caller, whoptr, ph, ڳڻپ,
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
		کاٻو -= chunk;
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
  vir_بائيٽون بائيٽون,		/* # of bytes to copy  */
  صحيح vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  بنية vir_addr *vir_addr[2];	/* virtual source and destination address */
  صحيح i, r;
  بنية proc *procs[2];

  أكد((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  إذا (بائيٽون <= 0) أرجع(EDOM);

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
			لکو("virtual_copy: no reasonable endpoint\n");
			أرجع ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  إذا ((r = check_resumed_caller(caller)) != OK)
	أرجع r;

  إذا((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->وٿي,
  	procs[_DST_], vir_addr[_DST_]->وٿي, بائيٽون)) != OK) {
	صحيح writeflag;
  	بنية proc *هدف = عدم;
  	phys_بائيٽون lin;
  	إذا(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	إذا(!vmcheck || !caller) {
    		أرجع r;
  	}

  	إذا(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->وٿي;
  		هدف = procs[_SRC_];
		writeflag = 0;
  	} وإلا إذا(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->وٿي;
  		هدف = procs[_DST_];
		writeflag = 1;
  	} وإلا {
  		panic("r strange: %d",  r);
  	}

	أكد(caller);
	أكد(هدف);

	vm_suspend(caller, هدف, lin, بائيٽون, VMSTYPE_KERNELCALL, writeflag);
	أرجع VMSUSPEND;
  }

  أرجع OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
صحيح معلومات_copy(ثابت endpoint_t from_proc, ثابت vir_بائيٽون from_addr,
	ثابت endpoint_t منزل_proc, ثابت vir_بائيٽون منزل_addr,
	size_t بائيٽون)
{
  بنية vir_addr src, dst;

  src.وٿي = from_addr;
  dst.وٿي = منزل_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = منزل_proc;
  أكد(src.proc_nr_e != NONE);
  أكد(dst.proc_nr_e != NONE);

  أرجع virtual_copy(&src, &dst, بائيٽون);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
صحيح معلومات_copy_vmcheck(بنية proc * caller,
	ثابت endpoint_t from_proc, ثابت vir_بائيٽون from_addr,
	ثابت endpoint_t منزل_proc, ثابت vir_بائيٽون منزل_addr,
	size_t بائيٽون)
{
  بنية vir_addr src, dst;

  src.وٿي = from_addr;
  dst.وٿي = منزل_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = منزل_proc;
  أكد(src.proc_nr_e != NONE);
  أكد(dst.proc_nr_e != NONE);

  أرجع virtual_copy_vmcheck(caller, &src, &dst, بائيٽون);
}

فراغ يادگيري_init(فراغ)
{
	أكد(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_شروعات++;
	freepdes[nfreepdes++] = kinfo.freepde_شروعات++;

	أكد(kinfo.freepde_شروعات < I386_VM_DIR_ENTRIES);
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

ساكن صحيح oxpcie_mapping_اشاريو = -1,
	lapic_mapping_اشاريو = -1,
	ioapic_پهريون_اشاريو = -1,
	ioapic_آخري_اشاريو = -1,
	video_mem_mapping_اشاريو = -1,
	usermapped_glo_اشاريو = -1,
	usermapped_اشاريو = -1, پهريون_um_idx = -1;

خارجي حرف *video_mem;

خارجي حرف usermapped_شروعات, usermapped_پڇاڙي, usermapped_nonglo_شروعات;

صحيح arch_phys_map(ثابت صحيح اشاريو,
			phys_بائيٽون *addr,
			phys_بائيٽون *len,
			صحيح *flags)
{
	ساكن صحيح پهريون = 1;
	صحيح freeidx = 0;
	ساكن حرف *ser_var = عدم;
	u32_t glo_len = (u32_t) &usermapped_nonglo_شروعات -
			(u32_t) &usermapped_شروعات;

	إذا(پهريون) {
		memset(&minix_kerninfo, 0, حجم(minix_kerninfo));
		video_mem_mapping_اشاريو = freeidx++;
		إذا(glo_len > 0) {
			usermapped_glo_اشاريو = freeidx++;
		}
		
		usermapped_اشاريو = freeidx++;
		پهريون_um_idx = usermapped_اشاريو;
		إذا(usermapped_glo_اشاريو != -1)
			پهريون_um_idx = usermapped_glo_اشاريو;

#جيڪڏهن_وصف_ٿيل USE_APIC
		إذا(lapic_addr)
			lapic_mapping_اشاريو = freeidx++;
		إذا (ioapic_enabled) {
			ioapic_پهريون_اشاريو = freeidx;
			أكد(nioapics > 0);
			freeidx += nioapics;
			ioapic_آخري_اشاريو = freeidx-1;
		}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_OXPCIE
		إذا((ser_var = env_get("oxpcie"))) {
			إذا(ser_var[0] != '0' || ser_var[1] != 'x') {
				لکو("oxpcie address in hex please\n");
			} وإلا {
				لکو("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_اشاريو = freeidx++;
			}
		}
#جيڪڏهن_پورو

		پهريون = 0;
	}

	إذا(اشاريو == usermapped_glo_اشاريو) {
		*addr = vir2phys(&usermapped_شروعات);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		أرجع OK;
	}
	وإلا إذا(اشاريو == usermapped_اشاريو) {
		*addr = vir2phys(&usermapped_nonglo_شروعات);
		*len = (u32_t) &usermapped_پڇاڙي -
			(u32_t) &usermapped_nonglo_شروعات;
		*flags = VMMF_USER;
		أرجع OK;
	}
	وإلا إذا (اشاريو == video_mem_mapping_اشاريو) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_بفر;
		*len = I386_صفحو_سائيز;
		*flags = VMMF_WRITE;
		أرجع OK;
	}
#جيڪڏهن_وصف_ٿيل USE_APIC
	وإلا إذا (اشاريو == lapic_mapping_اشاريو) {
		/* map the local APIC if enabled */
		إذا (!lapic_addr)
			أرجع EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		أرجع OK;
	}
	وإلا إذا (ioapic_enabled && اشاريو >= ioapic_پهريون_اشاريو && اشاريو <= ioapic_آخري_اشاريو) {
		صحيح ioapic_idx = اشاريو - ioapic_پهريون_اشاريو;
		*addr = io_apic[ioapic_idx].paddr;
		أكد(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		لکو("ioapic map: addr 0x%lx\n", *addr);
		أرجع OK;
	}
#جيڪڏهن_پورو

#إذا CONFIG_OXPCIE
	إذا(اشاريو == oxpcie_mapping_اشاريو) {
		*addr = strtoul(ser_var+2, عدم, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		أرجع OK;
	}
#جيڪڏهن_پورو

	أرجع EINVAL;
}

صحيح arch_phys_map_جواب(ثابت صحيح اشاريو, ثابت vir_بائيٽون addr)
{
#جيڪڏهن_وصف_ٿيل USE_APIC
	/* if local APIC is enabled */
	إذا (اشاريو == lapic_mapping_اشاريو && lapic_addr) {
		lapic_addr_vaddr = addr;
		أرجع OK;
	}
	وإلا إذا (ioapic_enabled && اشاريو >= ioapic_پهريون_اشاريو &&
		اشاريو <= ioapic_آخري_اشاريو) {
		صحيح i = اشاريو - ioapic_پهريون_اشاريو;
		io_apic[i].vaddr = addr;
		أرجع OK;
	}
#جيڪڏهن_پورو

#إذا CONFIG_OXPCIE
	إذا (اشاريو == oxpcie_mapping_اشاريو) {
		oxpcie_سيٽ_vaddr((بنانشان حرف *) addr);
		أرجع OK;
	}
#جيڪڏهن_پورو
	إذا(اشاريو == پهريون_um_idx) {
		خارجي بنية minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		خارجي u32_t usermapped_وٿي;
		أكد(addr > (u32_t) &usermapped_شروعات);
		usermapped_وٿي = addr - (u32_t) &usermapped_شروعات;
#وصف_ڪريو FIXEDPTR(ptr) (فراغ *) ((u32_t)ptr + usermapped_وٿي)
#وصف_ڪريو FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#وصف_ڪريو سونپيو(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		سونپيو(kinfo);
		سونپيو(machine);
		سونپيو(kmessages);
		سونپيو(loadinfo);
		سونپيو(kuserinfo);
		سونپيو(arm_frclock); /* eh, why not. */
		سونپيو(kclockinfo);

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
		minix_kerninfo_user = (vir_بائيٽون) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		إذا(env_get("libc_ipc")) {
			لکو("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = عدم;
		} وإلا {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		أرجع OK;
	}

	إذا(اشاريو == usermapped_اشاريو) أرجع OK;

	إذا (اشاريو == video_mem_mapping_اشاريو) {
		video_mem_vaddr =  addr;
		أرجع OK;
	}

	أرجع EINVAL;
}

صحيح arch_enable_paging(بنية proc * caller)
{
	أكد(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_پتو_space(caller);

	video_mem = (حرف *) video_mem_vaddr;

#جيڪڏهن_وصف_ٿيل USE_APIC
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

	wait_for_APs_منزل_پڄاڻي_booting();
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	إذا (watchdog_enabled)
		i386_watchdog_شروعات();
#جيڪڏهن_پورو

	أرجع OK;
}

فراغ release_پتو_space(بنية proc *pr)
{
	pr->p_seg.p_cr3_v = عدم;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
صحيح platform_tbl_checksum_ok(فراغ *ptr, بنانشان صحيح ڊيگهائي)
{
	u8_t ڪل = 0;
	بنانشان صحيح i;
	لكل (i = 0; i < ڊيگهائي; i++)
		ڪل += ((بنانشان حرف *)ptr)[i];
	أرجع !ڪل;
}

صحيح platform_tbl_ptr(phys_بائيٽون شروعات,
					phys_بائيٽون پڇاڙي,
					بنانشان increment,
					فراغ * buff,
					بنانشان سائيز,
					phys_بائيٽون * phys_addr,
					صحيح ((* cmp_f)(فراغ *)))
{
	phys_بائيٽون addr;

	لكل (addr = شروعات; addr < پڇاڙي; addr += increment) {
		phys_copy (addr, (phys_بائيٽون) buff, سائيز);
		إذا (cmp_f(buff)) {
			إذا (phys_addr)
				*phys_addr = addr;
			أرجع 1;
		}
	}
	أرجع 0;
}
