
#ضمن "kernel/kernel.h"
#ضمن "kernel/proc.h"
#ضمن "kernel/vm.h"

#ضمن <machine/vm.h>

#ضمن <minix/type.h>
#ضمن <minix/board.h>
#ضمن <minix/syslib.h>
#ضمن <minix/cpufeature.h>
#ضمن <string.h>
#ضمن <assert.h>
#ضمن <signal.h>
#ضمن <stdlib.h>

#ضمن <machine/vm.h>

#ضمن "arch_proto.h"
#ضمن "kernel/proto.h"
#ضمن "kernel/debug.h"
#ضمن "bsp_timer.h"


#تعریف_کړه HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
ساكن صحيح nfreepdes = 0;
#تعریف_کړه MAXFREEPDES	2
ساكن صحيح freepdes[MAXFREEPDES];

ساكن u32_t phys_get32(phys_بایټونه v);

/* list of requested physical mapping */
ساكن kern_phys_map *kern_phys_map_سر;

فراغ mem_clear_mapcache(فراغ)
{
	صحيح i;
	لكل(i = 0; i < nfreepdes; i++) {
		بنية proc *ptproc = get_cpulocal_var(ptproc);
		صحيح pde = freepdes[i];
		u32_t *ptv;
		أكد(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		أكد(ptv);
		ptv[pde] = 0;
	}
}

/* This function sets up a mapping from within the kernel's address
 * space to any other area of memory, either straight physical
 * memory (pr == NULL) or a process view of memory, in 1MB windows.
 * I.e., it maps in 1MB chunks of virtual (or physical) address space
 * to 1MB chunks of kernel virtual address space.
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
	أكد(pde >= 0 && pde < 4096);

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
		أكد(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} وإلا {
		/* Requested address is physical. Make up the PDE entry. */
		أكد (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_پای_ټکی);

		/*حافظه*/
		pdeval = (linaddr & ARM_VM_SECTION_ماسک)
			| ARM_VM_SECTION
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED
			| ARM_VM_SECTION_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	إذا(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*بدل_شوی = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	دننه_واټن = linaddr & ARM_VM_دننه_واټن_ماسک_1_MB; /* Offset in 1MB window. */
	*بایټونه = MIN(*بایټونه, ARM_SECTION_اندازه - دننه_واټن);

	/* Return the linear address of the start of the new mapping. */
	أرجع ARM_SECTION_اندازه*pde + دننه_واټن;
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
	أكد(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	أكد(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	إذا(srcproc) أكد(!RTS_ISSET(srcproc, RTS_خونه_FREE));
	إذا(dstproc) أكد(!RTS_ISSET(dstproc, RTS_خونه_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_خونه_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
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

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &بدل_شوی);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &بدل_شوی);
		إذا(بدل_شوی)
			reload_ttbr0();

		/* Check for overflow. */
		إذا (srcptr + chunk < srcptr) أرجع EFAULT_SRC;
		إذا (dstptr + chunk < dstptr) أرجع EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		إذا(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_بایټونه src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			إذا(addr >= src_aligned && addr < (srcptr + chunk)) {
				أرجع EFAULT_SRC;
			}
			إذا(addr >= dst_aligned && addr < (dstptr + chunk)) {
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
	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

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
	ریښه = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_ماسک);
	أكد(!((u32_t) ریښه % ARM_PAGEDIR_اندازه));
	pde = ARM_VM_PDE(مجازي);
	أكد(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ریښه + pde));

	إذا(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		أرجع EFAULT;
	}

	إذا(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_ماسک;
		إذا(ptent) *ptent = pde_v;
		*physical += مجازي & ARM_VM_دننه_واټن_ماسک_1_MB;
	} وإلا  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_ماسک);
		أكد(!((u32_t) pt % ARM_PAGETABLE_اندازه));
		pte = ARM_VM_PTE(مجازي);
		أكد(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		إذا(!(pte_v & ARM_VM_PTE_PRESENT)) {
			أرجع EFAULT;
		}

		إذا(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_ماسک;
		*physical += مجازي % ARM_پاڼه_اندازه;
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

	len = ARM_پاڼه_اندازه - (vir_addr % ARM_پاڼه_اندازه);
	vir_addr += len;
	بل_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	طالما (len < بایټونه) {
		إذا (vm_lookup(proc, vir_addr, &phys, عدم) != OK)
			اكسر;

		إذا (بل_phys != phys)
			اكسر;

		len += ARM_پاڼه_اندازه;
		vir_addr += ARM_پاڼه_اندازه;
		بل_phys += ARM_پاڼه_اندازه;
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
	صحيح new_ttbr, r = OK;

	إذا ((r = check_resumed_caller(caller)) != OK)
		أرجع r;

	/* NONE for physical, otherwise virtual */
	إذا (who != NONE && !(whoptr = endpoint_lookup(who)))
		أرجع ESRCH;

	c &= 0xFF;
	نمونه = c | (c << 8) | (c << 16) | (c << 24);

	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	أكد(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	طالما (کیڼ > 0) {
		new_ttbr = 0;
		chunk = کیڼ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		إذا (new_ttbr) {
			reload_ttbr0();
		}
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

	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	أكد(catch_pagefaults);
	catch_pagefaults = 0;

	أرجع OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
صحيح virtual_copy_f(
  بنية proc * caller,
  بنية vir_addr *src_addr,		/* source virtual address */
  بنية vir_addr *dst_addr,		/* destination virtual address */
  vir_بایټونه بایټونه,			/* # of bytes to copy  */
  صحيح vmcheck				/* if nonzero, can return VMSUSPEND */
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

	أكد(kinfo.freepde_پیل < ARM_VM_DIR_ENTRIES);
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
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

ساكن صحيح usermapped_glo_شاخص = -1,
	usermapped_شاخص = -1, لومړی_um_idx = -1;


/* defined in kernel.lds */
خارجي حرف usermapped_پیل, usermapped_پای_ټکی, usermapped_nonglo_پیل;

صحيح arch_phys_map(ثابت صحيح شاخص,
			phys_بایټونه *addr,
			phys_بایټونه *len,
			صحيح *flags)
{
	ساكن صحيح لومړی = 1;
	kern_phys_map *phys_maps;

	صحيح freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_پیل -
			(u32_t) &usermapped_پیل;

	إذا(لومړی) {
		memset(&minix_kerninfo, 0, حجم(minix_kerninfo));
		إذا(glo_len > 0) {
			usermapped_glo_شاخص = freeidx++;
		}

		usermapped_شاخص = freeidx++;
		لومړی_um_idx = usermapped_شاخص;
		إذا(usermapped_glo_شاخص != -1)
			لومړی_um_idx = usermapped_glo_شاخص;
		لومړی = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_سر;
		طالما(phys_maps != عدم){
			phys_maps->شاخص = freeidx++;
			phys_maps = phys_maps->بل;
		}

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

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_سر;
	طالما(phys_maps != عدم){
		إذا(phys_maps->شاخص == شاخص){
			*addr = phys_maps->addr;
			*len =  phys_maps->اندازه;
			*flags = phys_maps->vm_flags;
			أرجع OK;
		}
		phys_maps = phys_maps->بل;
	}

	أرجع EINVAL;
}

صحيح arch_phys_map_ځواب(ثابت صحيح شاخص, ثابت vir_بایټونه addr)
{
	kern_phys_map *phys_maps;

	إذا(شاخص == لومړی_um_idx) {
		u32_t usermapped_دننه_واټن;
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
		ټاکل(arm_frclock);
		ټاکل(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_بایټونه) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		أرجع OK;
	}

	إذا (شاخص == usermapped_شاخص) {
		أرجع OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_سر;
	طالما(phys_maps != عدم){
		إذا(phys_maps->شاخص == شاخص){
			أكد(phys_maps->cb != عدم);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			أرجع OK;
		}
		phys_maps = phys_maps->بل;
	}

	أرجع EINVAL;
}

صحيح arch_enable_paging(بنية proc * caller)
{
	kern_phys_map *phys_maps;
	أكد(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_پته_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_سر;
	طالما(phys_maps != عدم){
		أكد(phys_maps->cb != عدم);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->بل;
	}

	أرجع OK;
}

فراغ release_پته_space(بنية proc *pr)
{
	pr->p_seg.p_ttbr_v = عدم;
	barrier();
}



/*
 * Request a physical mapping
 */
صحيح kern_req_phys_map( phys_بایټونه بنسټ_پته, vir_بایټونه io_اندازه, 
		       صحيح vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_بایټونه id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	أكد(بنسټ_پته != 0);
	أكد(io_اندازه % ARM_پاڼه_اندازه == 0);
	أكد(cb != عدم);

	priv->addr  = بنسټ_پته;
	priv->اندازه  = io_اندازه;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->شاخص = -1;
	priv->بل = عدم;
	

	إذا (kern_phys_map_سر == عدم){
		/* keep a list of items this is the first one */
		kern_phys_map_سر = priv;
		kern_phys_map_سر->بل = عدم;
	} وإلا {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->بل = kern_phys_map_سر;
		/* replace the head */
		kern_phys_map_سر = priv;
	}
	أرجع 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
صحيح kern_phys_map_mapped_ptr(vir_بایټونه id, phys_بایټونه پته){
	*((vir_بایټونه*)id) = پته;
	أرجع 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
صحيح kern_phys_map_ptr(
	phys_بایټونه بنسټ_پته, 
	vir_بایټونه io_اندازه, 
	صحيح vm_flags, 
	kern_phys_map * priv,
	vir_بایټونه ptr)
{
	أرجع kern_req_phys_map(بنسټ_پته,io_اندازه,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

