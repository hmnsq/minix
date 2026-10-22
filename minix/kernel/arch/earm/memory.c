
#شامل "kernel/kernel.h"
#شامل "kernel/proc.h"
#شامل "kernel/vm.h"

#شامل <machine/vm.h>

#شامل <minix/type.h>
#شامل <minix/board.h>
#شامل <minix/syslib.h>
#شامل <minix/cpufeature.h>
#شامل <string.h>
#شامل <assert.h>
#شامل <signal.h>
#شامل <stdlib.h>

#شامل <machine/vm.h>

#شامل "arch_proto.h"
#شامل "kernel/proto.h"
#شامل "kernel/debug.h"
#شامل "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
ایستا صحیح nfreepdes = 0;
#define MAXFREEPDES	2
ایستا صحیح freepdes[MAXFREEPDES];

ایستا u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
ایستا kern_phys_map *kern_phys_map_head;

تهی mem_clear_mapcache(تهی)
{
	صحیح i;
	برای(i = 0; i < nfreepdes; i++) {
		ساختار proc *ptproc = get_cpulocal_var(ptproc);
		صحیح pde = freepdes[i];
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
ایستا phys_bytes createpde(
	ثابت ساختار proc *pr,	/* Requested process, NULL for physical. */
	ثابت phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	صحیح free_pde_idx,	/* index of the free slot to use */
	صحیح *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	صحیح pde;

	أكد(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	أكد(pde >= 0 && pde < 4096);

	اگر(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		بازگشت linaddr;
	}

	اگر(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		أكد(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} وگرنه {
		/* Requested address is physical. Make up the PDE entry. */
		أكد (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

		/* memory */
		pdeval = (linaddr & ARM_VM_SECTION_MASK)
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
	اگر(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & ARM_VM_OFFSET_MASK_1MB; /* Offset in 1MB window. */
	*bytes = MIN(*bytes, ARM_SECTION_SIZE - offset);

	/* Return the linear address of the start of the new mapping. */
	بازگشت ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ایستا صحیح check_resumed_caller(ساختار proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	اگر (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		أكد(caller->p_vmrequest.vmresult != VMSUSPEND);
		بازگشت caller->p_vmrequest.vmresult;
	}

	بازگشت OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ایستا صحیح lin_lin_copy(ساختار proc *srcproc, vir_bytes srclinaddr, 
	ساختار proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	أكد(get_cpulocal_var(ptproc));
	أكد(get_cpulocal_var(proc_ptr));
	أكد(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	أكد(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	اگر(srcproc) أكد(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	اگر(dstproc) أكد(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	اگر(srcproc) أكد(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	اگر(dstproc) أكد(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	وقتی(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		صحیح changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		اگر (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		اگر (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		اگر(changed)
			reload_ttbr0();

		/* Check for overflow. */
		اگر (srcptr + chunk < srcptr) بازگشت EFAULT_SRC;
		اگر (dstptr + chunk < dstptr) بازگشت EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		اگر(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			اگر(addr >= src_aligned && addr < (srcptr + chunk)) {
				بازگشت EFAULT_SRC;
			}
			اگر(addr >= dst_aligned && addr < (dstptr + chunk)) {
				بازگشت EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			بازگشت EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	اگر(srcproc) أكد(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	اگر(dstproc) أكد(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	بازگشت OK;
}

ایستا u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	صحیح r;

	اگر((r=lin_lin_copy(پوچ, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, اندازه(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	بازگشت v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  ثبات ساختار proc *rp,		/* pointer to proc table entry for process */
  صحیح seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	اگر(vm_lookup(rp, vir_addr, &phys, پوچ) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} وگرنه {
		اگر(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	اگر(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		بازگشت 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	اگر(bytes > 0 && vm_lookup_range(rp, vir_addr, پوچ, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		بازگشت 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	أكد(phys);

	بازگشت phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
صحیح vm_lookup(ثابت ساختار proc *proc, ثابت vir_bytes مجازی,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *ریشه, *pt;
	صحیح pde, pte;
	u32_t pde_v, pte_v;

	أكد(proc);
	أكد(physical);
	أكد(!isemptyp(proc));
	أكد(HASPT(proc));

	/* Retrieve page directory entry. */
	ریشه = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	أكد(!((u32_t) ریشه % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(مجازی);
	أكد(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (ریشه + pde));

	اگر(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		بازگشت EFAULT;
	}

	اگر(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		اگر(ptent) *ptent = pde_v;
		*physical += مجازی & ARM_VM_OFFSET_MASK_1MB;
	} وگرنه  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		أكد(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(مجازی);
		أكد(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		اگر(!(pte_v & ARM_VM_PTE_PRESENT)) {
			بازگشت EFAULT;
		}

		اگر(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += مجازی % ARM_PAGE_SIZE;
	}

	بازگشت OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ثابت ساختار proc *proc, vir_bytes vir_addr,
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

	أكد(proc);
	أكد(bytes > 0);
	أكد(HASPT(proc));

	/* Look up the first page. */
	اگر (vm_lookup(proc, vir_addr, &phys, پوچ) != OK)
		بازگشت 0;

	اگر (phys_addr != پوچ)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	وقتی (len < bytes) {
		اگر (vm_lookup(proc, vir_addr, &phys, پوچ) != OK)
			بشکن;

		اگر (next_phys != phys)
			بشکن;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	بازگشت MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
صحیح vm_check_range(ساختار proc *caller, ساختار proc *target,
	vir_bytes vir_addr, size_t bytes, صحیح writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	صحیح r;

	اگر ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		بازگشت r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	بازگشت VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
صحیح vm_memset(ساختار proc* caller, endpoint_t who, phys_bytes ph, صحیح c,
	phys_bytes شمار)
{
	u32_t pattern;
	ساختار proc *whoptr = پوچ;
	phys_bytes cur_ph = ph;
	phys_bytes چپ = شمار;
	phys_bytes ptr, chunk, pfa = 0;
	صحیح new_ttbr, r = OK;

	اگر ((r = check_resumed_caller(caller)) != OK)
		بازگشت r;

	/* NONE for physical, otherwise virtual */
	اگر (who != NONE && !(whoptr = endpoint_lookup(who)))
		بازگشت ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	أكد(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	وقتی (چپ > 0) {
		new_ttbr = 0;
		chunk = چپ;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		اگر (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		اگر ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			اگر (whoptr) {
				vm_suspend(caller, whoptr, ph, شمار,
						   VMSTYPE_KERNELCALL, 1);
				أكد(catch_pagefaults);
				catch_pagefaults = 0;
				بازگشت VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		چپ -= chunk;
	}

	أكد(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	أكد(catch_pagefaults);
	catch_pagefaults = 0;

	بازگشت OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
صحیح virtual_copy_f(
  ساختار proc * caller,
  ساختار vir_addr *src_addr,		/* source virtual address */
  ساختار vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  صحیح vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ساختار vir_addr *vir_addr[2];	/* virtual source and destination address */
  صحیح i, r;
  ساختار proc *procs[2];

  أكد((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  اگر (bytes <= 0) بازگشت(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  برای (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	صحیح proc_nr;
	ساختار proc *p;

	اگر(proc_e == NONE) {
		p = پوچ;
	} وگرنه {
		اگر(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			بازگشت ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  اگر ((r = check_resumed_caller(caller)) != OK)
	بازگشت r;

  اگر((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	صحیح writeflag;
  	ساختار proc *target = پوچ;
  	phys_bytes lin;
  	اگر(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	اگر(!vmcheck || !caller) {
    		بازگشت r;
  	}

  	اگر(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} وگرنه اگر(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} وگرنه {
  		panic("r strange: %d",  r);
  	}

	أكد(caller);
	أكد(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	بازگشت VMSUSPEND;
  }

  بازگشت OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
صحیح data_copy(ثابت endpoint_t from_proc, ثابت vir_bytes from_addr,
	ثابت endpoint_t to_proc, ثابت vir_bytes to_addr,
	size_t bytes)
{
  ساختار vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  أكد(src.proc_nr_e != NONE);
  أكد(dst.proc_nr_e != NONE);

  بازگشت virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
صحیح data_copy_vmcheck(ساختار proc * caller,
	ثابت endpoint_t from_proc, ثابت vir_bytes from_addr,
	ثابت endpoint_t to_proc, ثابت vir_bytes to_addr,
	size_t bytes)
{
  ساختار vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  أكد(src.proc_nr_e != NONE);
  أكد(dst.proc_nr_e != NONE);

  بازگشت virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

تهی memory_init(تهی)
{
	أكد(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	أكد(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	أكد(nfreepdes == 2);
	أكد(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
تهی arch_proc_init(ساختار proc *pr, ثابت u32_t ip, ثابت u32_t sp,
	ثابت u32_t ps_str, نویسه *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

ایستا صحیح usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
بیرونی نویسه usermapped_start, usermapped_end, usermapped_nonglo_start;

صحیح arch_phys_map(ثابت صحیح index,
			phys_bytes *addr,
			phys_bytes *len,
			صحیح *flags)
{
	ایستا صحیح first = 1;
	kern_phys_map *phys_maps;

	صحیح freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	اگر(first) {
		memset(&minix_kerninfo, 0, اندازه(minix_kerninfo));
		اگر(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		اگر(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		وقتی(phys_maps != پوچ){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	اگر(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		بازگشت OK;
	}
	وگرنه اگر(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		بازگشت OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	وقتی(phys_maps != پوچ){
		اگر(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			بازگشت OK;
		}
		phys_maps = phys_maps->next;
	}

	بازگشت EINVAL;
}

صحیح arch_phys_map_reply(ثابت صحیح index, ثابت vir_bytes addr)
{
	kern_phys_map *phys_maps;

	اگر(index == first_um_idx) {
		u32_t usermapped_offset;
		أكد(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (تهی *) ((u32_t)ptr + usermapped_offset)
#define FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#define ASSIGN(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ASSIGN(kinfo);
		ASSIGN(machine);
		ASSIGN(kmessages);
		ASSIGN(loadinfo);
		ASSIGN(kuserinfo);
		ASSIGN(arm_frclock);
		ASSIGN(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		بازگشت OK;
	}

	اگر (index == usermapped_index) {
		بازگشت OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	وقتی(phys_maps != پوچ){
		اگر(phys_maps->index == index){
			أكد(phys_maps->cb != پوچ);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			بازگشت OK;
		}
		phys_maps = phys_maps->next;
	}

	بازگشت EINVAL;
}

صحیح arch_enable_paging(ساختار proc * caller)
{
	kern_phys_map *phys_maps;
	أكد(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	وقتی(phys_maps != پوچ){
		أكد(phys_maps->cb != پوچ);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	بازگشت OK;
}

تهی release_address_space(ساختار proc *pr)
{
	pr->p_seg.p_ttbr_v = پوچ;
	barrier();
}



/*
 * Request a physical mapping
 */
صحیح kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       صحیح vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	أكد(base_address != 0);
	أكد(io_size % ARM_PAGE_SIZE == 0);
	أكد(cb != پوچ);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = پوچ;
	

	اگر (kern_phys_map_head == پوچ){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = پوچ;
	} وگرنه {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	بازگشت 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
صحیح kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	بازگشت 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
صحیح kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	صحیح vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	بازگشت kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

