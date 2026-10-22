
#شامل "kernel/kernel.h"
#شامل "kernel/vm.h"

#شامل <machine/vm.h>

#شامل <minix/syslib.h>
#شامل <minix/cpufeature.h>
#شامل <string.h>
#شامل <assert.h>
#شامل <signal.h>
#شامل <stdlib.h>

#شامل <machine/vm.h>

#شامل "oxpcie.h"
#شامل "arch_proto.h"

#اگر_تعریف_شدہ USE_APIC
#شامل "apic.h"
#اگر_تعریف_شدہ USE_WATCHDOG
#شامل "kernel/watchdog.h"
#اگر_ختم
#اگر_ختم

phys_بائٹیں video_mem_vaddr = 0;

#تعریف_کرو HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
ساکن صحیح nfreepdes = 0;
#تعریف_کرو MAXFREEPDES	2
ساکن صحیح freepdes[MAXFREEPDES];

ساکن u32_t phys_get32(phys_بائٹیں v);

خالی mem_clear_mapcache(خالی)
{
	صحیح i;
	برائے(i = 0; i < nfreepdes; i++) {
		ساخت proc *ptproc = get_cpulocal_var(ptproc);
		صحیح pde = freepdes[i];
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
ساکن phys_بائٹیں createpde(
	ثابت ساخت proc *pr,	/* Requested process, NULL for physical. */
	ثابت phys_بائٹیں linaddr,/* Address after segment translation. */
	phys_بائٹیں *بائٹیں,	/* Size of chunk, function may truncate it. */
	صحیح free_pde_idx,	/* index of the free slot to use */
	صحیح *بدلا		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_بائٹیں انحراف;
	صحیح pde;

	أكد(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	أكد(pde >= 0 && pde < 1024);

	اگر(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		واپس linaddr;
	}

	اگر(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		أكد(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} ورنہ {
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
	اگر(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*بدلا = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	انحراف = linaddr & I386_VM_انحراف_نقاب_4_MB; /* Offset in 4MB window. */
	*بائٹیں = MIN(*بائٹیں, I386_BIG_صفحہ_سائز - انحراف); 

	/* Return the linear address of the start of the new mapping. */
	واپس I386_BIG_صفحہ_سائز*pde + انحراف;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
ساکن صحیح check_resumed_caller(ساخت proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	اگر (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		أكد(caller->p_vmrequest.vmresult != VMSUSPEND);
		واپس caller->p_vmrequest.vmresult;
	}

	واپس OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
ساکن صحیح lin_lin_copy(ساخت proc *srcproc, vir_بائٹیں srclinaddr, 
	ساخت proc *dstproc, vir_بائٹیں dstlinaddr, vir_بائٹیں بائٹیں)
{
	u32_t addr;
	proc_nr_t procslot;

	أكد(get_cpulocal_var(ptproc));
	أكد(get_cpulocal_var(proc_ptr));
	أكد(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	أكد(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	اگر(srcproc) أكد(!RTS_ISSET(srcproc, RTS_خانہ_FREE));
	اگر(dstproc) أكد(!RTS_ISSET(dstproc, RTS_خانہ_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_خانہ_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	اگر(srcproc) أكد(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	اگر(dstproc) أكد(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	جب(بائٹیں > 0) {
		phys_بائٹیں srcptr, dstptr;
		vir_بائٹیں chunk = بائٹیں;
		صحیح بدلا = 0;

#اگر_تعریف_شدہ CONFIG_SMP
		غیرعلامتی cpu = cpuid;

		اگر (srcproc && GET_ثنائی_ہندسہ(srcproc->p_stale_tlb, cpu)) {
			بدلا = 1;
			UNSET_ثنائی_ہندسہ(srcproc->p_stale_tlb, cpu);
		}
		اگر (dstproc && GET_ثنائی_ہندسہ(dstproc->p_stale_tlb, cpu)) {
			بدلا = 1;
			UNSET_ثنائی_ہندسہ(dstproc->p_stale_tlb, cpu);
		}
#اگر_ختم

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &بدلا);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &بدلا);
		اگر(بدلا)
			reload_cr3(); 

		/* Check for overflow. */
		اگر (srcptr + chunk < srcptr) واپس EFAULT_SRC;
		اگر (dstptr + chunk < dstptr) واپس EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		اگر(addr) {
			/* If addr is nonzero, a page fault was caught. */

			اگر(addr >= srcptr && addr < (srcptr + chunk)) {
				واپس EFAULT_SRC;
			}
			اگر(addr >= dstptr && addr < (dstptr + chunk)) {
				واپس EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			واپس EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		بائٹیں -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	اگر(srcproc) أكد(!RTS_ISSET(srcproc, RTS_خانہ_FREE));
	اگر(dstproc) أكد(!RTS_ISSET(dstproc, RTS_خانہ_FREE));
	أكد(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_خانہ_FREE));
	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	واپس OK;
}


ساکن u32_t phys_get32(phys_بائٹیں addr)
{
	u32_t v;
	صحیح r;

	اگر((r=lin_lin_copy(تہی, addr, 
		proc_addr(SYSTEM), (phys_بائٹیں) &v, حجم(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	واپس v;
}

#اگر 0
ساکن حرف *cr0_str(u32_t e)
{
	ساکن حرف str[80];
	strcpy(str, "");
#تعریف_کرو FLAG(v) کرو { اگر(e & (v)) { strcat(str, #v " "); e &= ~v; } } جب(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	اگر(e) { strcat(str, " (++)"); }
	واپس str;
}

ساکن حرف *cr4_str(u32_t e)
{
	ساکن حرف str[80];
	strcpy(str, "");
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	اگر(e) { strcat(str, " (++)"); }
	واپس str;
}
#اگر_ختم

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_بائٹیں umap_virtual(
  اندراج ساخت proc *rp,		/* pointer to proc table entry for process */
  صحیح seg,				/* T, D, or S segment */
  vir_بائٹیں vir_addr,			/* virtual address in bytes within the seg */
  vir_بائٹیں بائٹیں			/* # of bytes to be copied */
)
{
	phys_بائٹیں phys = 0;

	اگر(vm_lookup(rp, vir_addr, &phys, تہی) != OK) {
		لکھو("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} ورنہ {
		اگر(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	اگر(phys == 0) {
		لکھو("SYSTEM:umap_virtual: lookup failed\n");
		واپس 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	اگر(بائٹیں > 0 && vm_lookup_range(rp, vir_addr, تہی, بائٹیں) != بائٹیں) {
		لکھو("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, بائٹیں, vir_addr, vir_addr);
		واپس 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	أكد(phys);

	واپس phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
صحیح vm_lookup(ثابت ساخت proc *proc, ثابت vir_بائٹیں مجازی,
 phys_بائٹیں *physical, u32_t *ptent)
{
	u32_t *جڑ, *pt;
	صحیح pde, pte;
	u32_t pde_v, pte_v;

	أكد(proc);
	أكد(physical);
	أكد(!isemptyp(proc));
	أكد(HASPT(proc));

	/* Retrieve page directory entry. */
	جڑ = (u32_t *) proc->p_seg.p_cr3;
	أكد(!((u32_t) جڑ % I386_صفحہ_سائز));
	pde = I386_VM_PDE(مجازی);
	أكد(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (جڑ + pde));

	اگر(!(pde_v & I386_VM_PRESENT)) {
		واپس EFAULT;
	}

	/* We don't expect to ever see this. */
	اگر(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_نقاب_4_MB;
		اگر(ptent) *ptent = pde_v;
		*physical += مجازی & I386_VM_انحراف_نقاب_4_MB;
	} ورنہ {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		أكد(!((u32_t) pt % I386_صفحہ_سائز));
		pte = I386_VM_PTE(مجازی);
		أكد(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		اگر(!(pte_v & I386_VM_PRESENT)) {
			واپس EFAULT;
		}

		اگر(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += مجازی % I386_صفحہ_سائز;
	}

	واپس OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(ثابت ساخت proc *proc, vir_بائٹیں vir_addr,
	phys_بائٹیں *phys_addr, size_t بائٹیں)
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
	phys_بائٹیں phys, اگلا_phys;
	size_t len;

	أكد(proc);
	أكد(بائٹیں > 0);
	أكد(HASPT(proc));

	/* Look up the first page. */
	اگر (vm_lookup(proc, vir_addr, &phys, تہی) != OK)
		واپس 0;

	اگر (phys_addr != تہی)
		*phys_addr = phys;

	len = I386_صفحہ_سائز - (vir_addr % I386_صفحہ_سائز);
	vir_addr += len;
	اگلا_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	جب (len < بائٹیں) {
		اگر (vm_lookup(proc, vir_addr, &phys, تہی) != OK)
			توڑو;

		اگر (اگلا_phys != phys)
			توڑو;

		len += I386_صفحہ_سائز;
		vir_addr += I386_صفحہ_سائز;
		اگلا_phys += I386_صفحہ_سائز;
	}

	/* We might now have overshot the requested length somewhat. */
	واپس MIN(بائٹیں, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
صحیح vm_check_range(ساخت proc *caller, ساخت proc *ہدف,
	vir_بائٹیں vir_addr, size_t بائٹیں, صحیح writeflag)
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
		واپس r;

	vm_suspend(caller, ہدف, vir_addr, بائٹیں, VMSTYPE_KERNELCALL,
		writeflag);

	واپس VMSUSPEND;
}

#اگر 0
ساکن حرف *flagstr(u32_t e, ثابت صحیح dir)
{
	ساکن حرف str[80];
	strcpy(str, "");
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	اگر(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	ورنہ
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	واپس str;
}

ساکن خالی vm_pt_print(u32_t *pagetable, ثابت u32_t v)
{
	صحیح pte;
	صحیح ستون = 0;

	أكد(!((u32_t) pagetable % I386_صفحہ_سائز));

	برائے(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		اگر(!(pte_v & I386_VM_PRESENT))
			جاری;
		pfa = I386_VM_PFA(pte_v);
		لکھو("%4d:%08lx:%08lx %2s ",
			pte, v + I386_صفحہ_سائز*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		ستون++;
		اگر(ستون == 3) { لکھو("\n"); ستون = 0; }
	}
	اگر(ستون > 0) لکھو("\n");

	واپس;
}

ساکن خالی vm_print(u32_t *جڑ)
{
	صحیح pde;

	أكد(!((u32_t) جڑ % I386_صفحہ_سائز));

	لکھو("page table 0x%lx:\n", جڑ);

	برائے(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (جڑ + pde));
		اگر(!(pde_v & I386_VM_PRESENT))
			جاری;
		اگر(pde_v & I386_VM_BIGPAGE) {
			لکھو("%4d: 0x%lx, flags %s\n",
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} ورنہ {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			لکھو("%4d: pt %08lx %s\n",
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_صفحہ_سائز);
			لکھو("\n");
		}
	}


	واپس;
}
#اگر_ختم

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
صحیح vm_memset(ساخت proc* caller, endpoint_t who, phys_بائٹیں ph, صحیح c,
	phys_بائٹیں گنتی)
{
	u32_t نمونہ;
	ساخت proc *whoptr = تہی;
	phys_بائٹیں cur_ph = ph;
	phys_بائٹیں بایاں = گنتی;
	phys_بائٹیں ptr, chunk, pfa = 0;
	صحیح new_cr3, r = OK;

	اگر ((r = check_resumed_caller(caller)) != OK)
		واپس r;

	/* NONE for physical, otherwise virtual */
	اگر (who != NONE && !(whoptr = endpoint_lookup(who)))
		واپس ESRCH;

	c &= 0xFF;
	نمونہ = c | (c << 8) | (c << 16) | (c << 24);

	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	أكد(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	جب (بایاں > 0) {
		new_cr3 = 0;
		chunk = بایاں;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		اگر (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		اگر ((pfa = phys_memset(ptr, نمونہ, chunk))) {

			/* If a process pagefaults, VM may help out */
			اگر (whoptr) {
				vm_suspend(caller, whoptr, ph, گنتی,
						   VMSTYPE_KERNELCALL, 1);
				أكد(catch_pagefaults);
				catch_pagefaults = 0;
				واپس VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		بایاں -= chunk;
	}

	أكد(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	أكد(catch_pagefaults);
	catch_pagefaults = 0;

	واپس OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
صحیح virtual_copy_f(
  ساخت proc * caller,
  ساخت vir_addr *src_addr,	/* source virtual address */
  ساخت vir_addr *dst_addr,	/* destination virtual address */
  vir_بائٹیں بائٹیں,		/* # of bytes to copy  */
  صحیح vmcheck			/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  ساخت vir_addr *vir_addr[2];	/* virtual source and destination address */
  صحیح i, r;
  ساخت proc *procs[2];

  أكد((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  اگر (بائٹیں <= 0) واپس(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  برائے (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	صحیح proc_nr;
	ساخت proc *p;

	اگر(proc_e == NONE) {
		p = تہی;
	} ورنہ {
		اگر(!isokendpt(proc_e, &proc_nr)) {
			لکھو("virtual_copy: no reasonable endpoint\n");
			واپس ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  اگر ((r = check_resumed_caller(caller)) != OK)
	واپس r;

  اگر((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->انحراف,
  	procs[_DST_], vir_addr[_DST_]->انحراف, بائٹیں)) != OK) {
	صحیح writeflag;
  	ساخت proc *ہدف = تہی;
  	phys_بائٹیں lin;
  	اگر(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	اگر(!vmcheck || !caller) {
    		واپس r;
  	}

  	اگر(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->انحراف;
  		ہدف = procs[_SRC_];
		writeflag = 0;
  	} ورنہ اگر(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->انحراف;
  		ہدف = procs[_DST_];
		writeflag = 1;
  	} ورنہ {
  		panic("r strange: %d",  r);
  	}

	أكد(caller);
	أكد(ہدف);

	vm_suspend(caller, ہدف, lin, بائٹیں, VMSTYPE_KERNELCALL, writeflag);
	واپس VMSUSPEND;
  }

  واپس OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
صحیح ڈیٹا_copy(ثابت endpoint_t from_proc, ثابت vir_بائٹیں from_addr,
	ثابت endpoint_t مقصد_proc, ثابت vir_بائٹیں مقصد_addr,
	size_t بائٹیں)
{
  ساخت vir_addr src, dst;

  src.انحراف = from_addr;
  dst.انحراف = مقصد_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = مقصد_proc;
  أكد(src.proc_nr_e != NONE);
  أكد(dst.proc_nr_e != NONE);

  واپس virtual_copy(&src, &dst, بائٹیں);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
صحیح ڈیٹا_copy_vmcheck(ساخت proc * caller,
	ثابت endpoint_t from_proc, ثابت vir_بائٹیں from_addr,
	ثابت endpoint_t مقصد_proc, ثابت vir_بائٹیں مقصد_addr,
	size_t بائٹیں)
{
  ساخت vir_addr src, dst;

  src.انحراف = from_addr;
  dst.انحراف = مقصد_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = مقصد_proc;
  أكد(src.proc_nr_e != NONE);
  أكد(dst.proc_nr_e != NONE);

  واپس virtual_copy_vmcheck(caller, &src, &dst, بائٹیں);
}

خالی حافظہ_init(خالی)
{
	أكد(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_آغاز++;
	freepdes[nfreepdes++] = kinfo.freepde_آغاز++;

	أكد(kinfo.freepde_آغاز < I386_VM_DIR_ENTRIES);
	أكد(nfreepdes == 2);
	أكد(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
خالی arch_proc_init(ساخت proc *pr, ثابت u32_t ip, ثابت u32_t sp,
	ثابت u32_t ps_str, حرف *name)
{
	arch_proc_reset(pr);
	strlcpy(pr->p_name, name, حجم(pr->p_name));

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

ساکن صحیح oxpcie_mapping_اشاریہ = -1,
	lapic_mapping_اشاریہ = -1,
	ioapic_پہلا_اشاریہ = -1,
	ioapic_آخری_اشاریہ = -1,
	video_mem_mapping_اشاریہ = -1,
	usermapped_glo_اشاریہ = -1,
	usermapped_اشاریہ = -1, پہلا_um_idx = -1;

خارجی حرف *video_mem;

خارجی حرف usermapped_آغاز, usermapped_انتہا, usermapped_nonglo_آغاز;

صحیح arch_phys_map(ثابت صحیح اشاریہ,
			phys_بائٹیں *addr,
			phys_بائٹیں *len,
			صحیح *flags)
{
	ساکن صحیح پہلا = 1;
	صحیح freeidx = 0;
	ساکن حرف *ser_var = تہی;
	u32_t glo_len = (u32_t) &usermapped_nonglo_آغاز -
			(u32_t) &usermapped_آغاز;

	اگر(پہلا) {
		memset(&minix_kerninfo, 0, حجم(minix_kerninfo));
		video_mem_mapping_اشاریہ = freeidx++;
		اگر(glo_len > 0) {
			usermapped_glo_اشاریہ = freeidx++;
		}
		
		usermapped_اشاریہ = freeidx++;
		پہلا_um_idx = usermapped_اشاریہ;
		اگر(usermapped_glo_اشاریہ != -1)
			پہلا_um_idx = usermapped_glo_اشاریہ;

#اگر_تعریف_شدہ USE_APIC
		اگر(lapic_addr)
			lapic_mapping_اشاریہ = freeidx++;
		اگر (ioapic_enabled) {
			ioapic_پہلا_اشاریہ = freeidx;
			أكد(nioapics > 0);
			freeidx += nioapics;
			ioapic_آخری_اشاریہ = freeidx-1;
		}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_OXPCIE
		اگر((ser_var = env_get("oxpcie"))) {
			اگر(ser_var[0] != '0' || ser_var[1] != 'x') {
				لکھو("oxpcie address in hex please\n");
			} ورنہ {
				لکھو("oxpcie address is %s\n", ser_var);
				oxpcie_mapping_اشاریہ = freeidx++;
			}
		}
#اگر_ختم

		پہلا = 0;
	}

	اگر(اشاریہ == usermapped_glo_اشاریہ) {
		*addr = vir2phys(&usermapped_آغاز);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		واپس OK;
	}
	ورنہ اگر(اشاریہ == usermapped_اشاریہ) {
		*addr = vir2phys(&usermapped_nonglo_آغاز);
		*len = (u32_t) &usermapped_انتہا -
			(u32_t) &usermapped_nonglo_آغاز;
		*flags = VMMF_USER;
		واپس OK;
	}
	ورنہ اگر (اشاریہ == video_mem_mapping_اشاریہ) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_بفر;
		*len = I386_صفحہ_سائز;
		*flags = VMMF_WRITE;
		واپس OK;
	}
#اگر_تعریف_شدہ USE_APIC
	ورنہ اگر (اشاریہ == lapic_mapping_اشاریہ) {
		/* map the local APIC if enabled */
		اگر (!lapic_addr)
			واپس EINVAL;
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		واپس OK;
	}
	ورنہ اگر (ioapic_enabled && اشاریہ >= ioapic_پہلا_اشاریہ && اشاریہ <= ioapic_آخری_اشاریہ) {
		صحیح ioapic_idx = اشاریہ - ioapic_پہلا_اشاریہ;
		*addr = io_apic[ioapic_idx].paddr;
		أكد(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		لکھو("ioapic map: addr 0x%lx\n", *addr);
		واپس OK;
	}
#اگر_ختم

#اگر CONFIG_OXPCIE
	اگر(اشاریہ == oxpcie_mapping_اشاریہ) {
		*addr = strtoul(ser_var+2, تہی, 16);
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		واپس OK;
	}
#اگر_ختم

	واپس EINVAL;
}

صحیح arch_phys_map_جواب(ثابت صحیح اشاریہ, ثابت vir_بائٹیں addr)
{
#اگر_تعریف_شدہ USE_APIC
	/* if local APIC is enabled */
	اگر (اشاریہ == lapic_mapping_اشاریہ && lapic_addr) {
		lapic_addr_vaddr = addr;
		واپس OK;
	}
	ورنہ اگر (ioapic_enabled && اشاریہ >= ioapic_پہلا_اشاریہ &&
		اشاریہ <= ioapic_آخری_اشاریہ) {
		صحیح i = اشاریہ - ioapic_پہلا_اشاریہ;
		io_apic[i].vaddr = addr;
		واپس OK;
	}
#اگر_ختم

#اگر CONFIG_OXPCIE
	اگر (اشاریہ == oxpcie_mapping_اشاریہ) {
		oxpcie_سیٹ_vaddr((غیرعلامتی حرف *) addr);
		واپس OK;
	}
#اگر_ختم
	اگر(اشاریہ == پہلا_um_idx) {
		خارجی ساخت minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		خارجی u32_t usermapped_انحراف;
		أكد(addr > (u32_t) &usermapped_آغاز);
		usermapped_انحراف = addr - (u32_t) &usermapped_آغاز;
#تعریف_کرو FIXEDPTR(ptr) (خالی *) ((u32_t)ptr + usermapped_انحراف)
#تعریف_کرو FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#تعریف_کرو تفویض(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		تفویض(kinfo);
		تفویض(machine);
		تفویض(kmessages);
		تفویض(loadinfo);
		تفویض(kuserinfo);
		تفویض(arm_frclock); /* eh, why not. */
		تفویض(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		اگر(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} ورنہ  اگر(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} ورنہ	{
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
		minix_kerninfo_user = (vir_بائٹیں) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		اگر(env_get("libc_ipc")) {
			لکھو("kernel: forcing in-libc fallback ipc style\n");
			minix_kerninfo.minix_ipcvecs = تہی;
		} ورنہ {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		واپس OK;
	}

	اگر(اشاریہ == usermapped_اشاریہ) واپس OK;

	اگر (اشاریہ == video_mem_mapping_اشاریہ) {
		video_mem_vaddr =  addr;
		واپس OK;
	}

	واپس EINVAL;
}

صحیح arch_enable_paging(ساخت proc * caller)
{
	أكد(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_پتہ_space(caller);

	video_mem = (حرف *) video_mem_vaddr;

#اگر_تعریف_شدہ USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	اگر (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	اگر (ioapic_enabled) {
		صحیح i;

		برائے (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#اگر CONFIG_SMP
	barrier();

	wait_for_APs_مقصد_اختتام_booting();
#اگر_ختم
#اگر_ختم

#اگر_تعریف_شدہ USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	اگر (watchdog_enabled)
		i386_watchdog_آغاز();
#اگر_ختم

	واپس OK;
}

خالی release_پتہ_space(ساخت proc *pr)
{
	pr->p_seg.p_cr3_v = تہی;
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
صحیح platform_tbl_checksum_ok(خالی *ptr, غیرعلامتی صحیح لمبائی)
{
	u8_t کل = 0;
	غیرعلامتی صحیح i;
	برائے (i = 0; i < لمبائی; i++)
		کل += ((غیرعلامتی حرف *)ptr)[i];
	واپس !کل;
}

صحیح platform_tbl_ptr(phys_بائٹیں آغاز,
					phys_بائٹیں انتہا,
					غیرعلامتی increment,
					خالی * buff,
					غیرعلامتی سائز,
					phys_بائٹیں * phys_addr,
					صحیح ((* cmp_f)(خالی *)))
{
	phys_بائٹیں addr;

	برائے (addr = آغاز; addr < انتہا; addr += increment) {
		phys_copy (addr, (phys_بائٹیں) buff, سائز);
		اگر (cmp_f(buff)) {
			اگر (phys_addr)
				*phys_addr = addr;
			واپس 1;
		}
	}
	واپس 0;
}
