
#include "kernel/kernel.h"
#include "kernel/proc.h"
#include "kernel/vm.h"

#include <machine/vm.h>

#include <minix/type.h>
#include <minix/board.h>
#include <minix/syslib.h>
#include <minix/cpufeature.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>

#include <machine/vm.h>

#include "arch_proto.h"
#include "kernel/proto.h"
#include "kernel/debug.h"
#include "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
static số_nguyên nfreepdes = 0;
#define MAXFREEPDES	2
static số_nguyên freepdes[MAXFREEPDES];

static u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
static kern_phys_map *kern_phys_map_head;

void mem_clear_mapcache(void)
{
	số_nguyên i;
	cho(i = 0; i < nfreepdes; i++) {
		struct proc *ptproc = get_cpulocal_var(ptproc);
		số_nguyên pde = freepdes[i];
		u32_t *ptv;
		assert(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		assert(ptv);
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
static phys_bytes createpde(
	const struct proc *pr,	/* Requested process, NULL for physical. */
	const phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	số_nguyên free_pde_idx,	/* index of the free slot to use */
	số_nguyên *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	số_nguyên pde;

	assert(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	assert(pde >= 0 && pde < 4096);

	nếu(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		trả_về linaddr;
	}

	nếu(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		assert(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} không_thì {
		/* Requested address is physical. Make up the PDE entry. */
		assert (linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

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
	assert(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	nếu(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
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
	trả_về ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
static số_nguyên check_resumed_caller(struct proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	nếu (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		assert(caller->p_vmrequest.vmresult != VMSUSPEND);
		trả_về caller->p_vmrequest.vmresult;
	}

	trả_về OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
static số_nguyên lin_lin_copy(struct proc *srcproc, vir_bytes srclinaddr, 
	struct proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	assert(get_cpulocal_var(ptproc));
	assert(get_cpulocal_var(proc_ptr));
	assert(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	assert(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	nếu(srcproc) assert(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	nếu(dstproc) assert(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	assert(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	assert(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	nếu(srcproc) assert(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	nếu(dstproc) assert(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	trong_khi(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		số_nguyên changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		nếu (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		nếu (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		nếu(changed)
			reload_ttbr0();

		/* Check for overflow. */
		nếu (srcptr + chunk < srcptr) trả_về EFAULT_SRC;
		nếu (dstptr + chunk < dstptr) trả_về EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		nếu(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			nếu(addr >= src_aligned && addr < (srcptr + chunk)) {
				trả_về EFAULT_SRC;
			}
			nếu(addr >= dst_aligned && addr < (dstptr + chunk)) {
				trả_về EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			trả_về EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	nếu(srcproc) assert(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	nếu(dstproc) assert(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	assert(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	assert(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	trả_về OK;
}

static u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	số_nguyên r;

	nếu((r=lin_lin_copy(NULL, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, sizeof(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	trả_về v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  register struct proc *rp,		/* pointer to proc table entry for process */
  số_nguyên seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	nếu(vm_lookup(rp, vir_addr, &phys, NULL) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} không_thì {
		nếu(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	nếu(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		trả_về 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	nếu(bytes > 0 && vm_lookup_range(rp, vir_addr, NULL, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		trả_về 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	assert(phys);

	trả_về phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
số_nguyên vm_lookup(const struct proc *proc, const vir_bytes virtual,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *root, *pt;
	số_nguyên pde, pte;
	u32_t pde_v, pte_v;

	assert(proc);
	assert(physical);
	assert(!isemptyp(proc));
	assert(HASPT(proc));

	/* Retrieve page directory entry. */
	root = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	assert(!((u32_t) root % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(virtual);
	assert(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (root + pde));

	nếu(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		trả_về EFAULT;
	}

	nếu(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		nếu(ptent) *ptent = pde_v;
		*physical += virtual & ARM_VM_OFFSET_MASK_1MB;
	} không_thì  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		assert(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(virtual);
		assert(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		nếu(!(pte_v & ARM_VM_PTE_PRESENT)) {
			trả_về EFAULT;
		}

		nếu(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += virtual % ARM_PAGE_SIZE;
	}

	trả_về OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(const struct proc *proc, vir_bytes vir_addr,
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

	assert(proc);
	assert(bytes > 0);
	assert(HASPT(proc));

	/* Look up the first page. */
	nếu (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
		trả_về 0;

	nếu (phys_addr != NULL)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	trong_khi (len < bytes) {
		nếu (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
			break;

		nếu (next_phys != phys)
			break;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	trả_về MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
số_nguyên vm_check_range(struct proc *caller, struct proc *target,
	vir_bytes vir_addr, size_t bytes, số_nguyên writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	số_nguyên r;

	nếu ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		trả_về r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	trả_về VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
số_nguyên vm_memset(struct proc* caller, endpoint_t who, phys_bytes ph, số_nguyên c,
	phys_bytes count)
{
	u32_t pattern;
	struct proc *whoptr = NULL;
	phys_bytes cur_ph = ph;
	phys_bytes left = count;
	phys_bytes ptr, chunk, pfa = 0;
	số_nguyên new_ttbr, r = OK;

	nếu ((r = check_resumed_caller(caller)) != OK)
		trả_về r;

	/* NONE for physical, otherwise virtual */
	nếu (who != NONE && !(whoptr = endpoint_lookup(who)))
		trả_về ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	assert(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	assert(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	trong_khi (left > 0) {
		new_ttbr = 0;
		chunk = left;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		nếu (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		nếu ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			nếu (whoptr) {
				vm_suspend(caller, whoptr, ph, count,
						   VMSTYPE_KERNELCALL, 1);
				assert(catch_pagefaults);
				catch_pagefaults = 0;
				trả_về VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		left -= chunk;
	}

	assert(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	assert(catch_pagefaults);
	catch_pagefaults = 0;

	trả_về OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
số_nguyên virtual_copy_f(
  struct proc * caller,
  struct vir_addr *src_addr,		/* source virtual address */
  struct vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  số_nguyên vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  struct vir_addr *vir_addr[2];	/* virtual source and destination address */
  số_nguyên i, r;
  struct proc *procs[2];

  assert((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  nếu (bytes <= 0) trả_về(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  cho (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	số_nguyên proc_nr;
	struct proc *p;

	nếu(proc_e == NONE) {
		p = NULL;
	} không_thì {
		nếu(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			trả_về ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  nếu ((r = check_resumed_caller(caller)) != OK)
	trả_về r;

  nếu((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	số_nguyên writeflag;
  	struct proc *target = NULL;
  	phys_bytes lin;
  	nếu(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	nếu(!vmcheck || !caller) {
    		trả_về r;
  	}

  	nếu(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} không_thì nếu(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} không_thì {
  		panic("r strange: %d",  r);
  	}

	assert(caller);
	assert(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	trả_về VMSUSPEND;
  }

  trả_về OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
số_nguyên data_copy(const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	size_t bytes)
{
  struct vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  assert(src.proc_nr_e != NONE);
  assert(dst.proc_nr_e != NONE);

  trả_về virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
số_nguyên data_copy_vmcheck(struct proc * caller,
	const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	size_t bytes)
{
  struct vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  assert(src.proc_nr_e != NONE);
  assert(dst.proc_nr_e != NONE);

  trả_về virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

void memory_init(void)
{
	assert(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	assert(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	assert(nfreepdes == 2);
	assert(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
void arch_proc_init(struct proc *pr, const u32_t ip, const u32_t sp,
	const u32_t ps_str, char *name)
{
	arch_proc_reset(pr);
	strcpy(pr->p_name, name);

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

static số_nguyên usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
extern char usermapped_start, usermapped_end, usermapped_nonglo_start;

số_nguyên arch_phys_map(const số_nguyên index,
			phys_bytes *addr,
			phys_bytes *len,
			số_nguyên *flags)
{
	static số_nguyên first = 1;
	kern_phys_map *phys_maps;

	số_nguyên freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	nếu(first) {
		memset(&minix_kerninfo, 0, sizeof(minix_kerninfo));
		nếu(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		nếu(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		trong_khi(phys_maps != NULL){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	nếu(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		trả_về OK;
	}
	không_thì nếu(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		trả_về OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	trong_khi(phys_maps != NULL){
		nếu(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			trả_về OK;
		}
		phys_maps = phys_maps->next;
	}

	trả_về EINVAL;
}

số_nguyên arch_phys_map_reply(const số_nguyên index, const vir_bytes addr)
{
	kern_phys_map *phys_maps;

	nếu(index == first_um_idx) {
		u32_t usermapped_offset;
		assert(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (void *) ((u32_t)ptr + usermapped_offset)
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

		trả_về OK;
	}

	nếu (index == usermapped_index) {
		trả_về OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	trong_khi(phys_maps != NULL){
		nếu(phys_maps->index == index){
			assert(phys_maps->cb != NULL);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			trả_về OK;
		}
		phys_maps = phys_maps->next;
	}

	trả_về EINVAL;
}

số_nguyên arch_enable_paging(struct proc * caller)
{
	kern_phys_map *phys_maps;
	assert(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	trong_khi(phys_maps != NULL){
		assert(phys_maps->cb != NULL);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	trả_về OK;
}

void release_address_space(struct proc *pr)
{
	pr->p_seg.p_ttbr_v = NULL;
	barrier();
}



/*
 * Request a physical mapping
 */
số_nguyên kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       số_nguyên vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	assert(base_address != 0);
	assert(io_size % ARM_PAGE_SIZE == 0);
	assert(cb != NULL);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = NULL;
	

	nếu (kern_phys_map_head == NULL){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = NULL;
	} không_thì {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	trả_về 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
số_nguyên kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	trả_về 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
số_nguyên kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	số_nguyên vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	trả_về kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

