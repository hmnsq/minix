
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
static 정수 nfreepdes = 0;
#define MAXFREEPDES	2
static 정수 freepdes[MAXFREEPDES];

static u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
static kern_phys_map *kern_phys_map_head;

void mem_clear_mapcache(void)
{
	정수 i;
	위해(i = 0; i < nfreepdes; i++) {
		struct proc *ptproc = get_cpulocal_var(ptproc);
		정수 pde = freepdes[i];
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
	정수 free_pde_idx,	/* index of the free slot to use */
	정수 *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	정수 pde;

	assert(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	assert(pde >= 0 && pde < 4096);

	만약(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		반환 linaddr;
	}

	만약(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		assert(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} 아니면 {
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
	만약(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
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
	반환 ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
static 정수 check_resumed_caller(struct proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	만약 (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		assert(caller->p_vmrequest.vmresult != VMSUSPEND);
		반환 caller->p_vmrequest.vmresult;
	}

	반환 OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
static 정수 lin_lin_copy(struct proc *srcproc, vir_bytes srclinaddr, 
	struct proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	assert(get_cpulocal_var(ptproc));
	assert(get_cpulocal_var(proc_ptr));
	assert(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	assert(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	만약(srcproc) assert(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	만약(dstproc) assert(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	assert(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	assert(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	만약(srcproc) assert(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	만약(dstproc) assert(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	동안(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		정수 changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		만약 (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		만약 (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		만약(changed)
			reload_ttbr0();

		/* Check for overflow. */
		만약 (srcptr + chunk < srcptr) 반환 EFAULT_SRC;
		만약 (dstptr + chunk < dstptr) 반환 EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		만약(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			만약(addr >= src_aligned && addr < (srcptr + chunk)) {
				반환 EFAULT_SRC;
			}
			만약(addr >= dst_aligned && addr < (dstptr + chunk)) {
				반환 EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			반환 EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	만약(srcproc) assert(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	만약(dstproc) assert(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	assert(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	assert(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	반환 OK;
}

static u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	정수 r;

	만약((r=lin_lin_copy(NULL, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, sizeof(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	반환 v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  register struct proc *rp,		/* pointer to proc table entry for process */
  정수 seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	만약(vm_lookup(rp, vir_addr, &phys, NULL) != OK) {
		printf("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr);
		phys = 0;
	} 아니면 {
		만약(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	만약(phys == 0) {
		printf("SYSTEM:umap_virtual: lookup failed\n");
		반환 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	만약(bytes > 0 && vm_lookup_range(rp, vir_addr, NULL, bytes) != bytes) {
		printf("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n",
			rp->p_name, bytes, vir_addr, vir_addr);
		반환 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	assert(phys);

	반환 phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
정수 vm_lookup(const struct proc *proc, const vir_bytes virtual,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *root, *pt;
	정수 pde, pte;
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

	만약(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		반환 EFAULT;
	}

	만약(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		만약(ptent) *ptent = pde_v;
		*physical += virtual & ARM_VM_OFFSET_MASK_1MB;
	} 아니면  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		assert(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(virtual);
		assert(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		만약(!(pte_v & ARM_VM_PTE_PRESENT)) {
			반환 EFAULT;
		}

		만약(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += virtual % ARM_PAGE_SIZE;
	}

	반환 OK;
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
	만약 (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
		반환 0;

	만약 (phys_addr != NULL)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	동안 (len < bytes) {
		만약 (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
			break;

		만약 (next_phys != phys)
			break;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	반환 MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
정수 vm_check_range(struct proc *caller, struct proc *target,
	vir_bytes vir_addr, size_t bytes, 정수 writeflag)
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	정수 r;

	만약 ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		반환 r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	반환 VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
정수 vm_memset(struct proc* caller, endpoint_t who, phys_bytes ph, 정수 c,
	phys_bytes count)
{
	u32_t pattern;
	struct proc *whoptr = NULL;
	phys_bytes cur_ph = ph;
	phys_bytes left = count;
	phys_bytes ptr, chunk, pfa = 0;
	정수 new_ttbr, r = OK;

	만약 ((r = check_resumed_caller(caller)) != OK)
		반환 r;

	/* NONE for physical, otherwise virtual */
	만약 (who != NONE && !(whoptr = endpoint_lookup(who)))
		반환 ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	assert(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	assert(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	동안 (left > 0) {
		new_ttbr = 0;
		chunk = left;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		만약 (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		만약 ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			만약 (whoptr) {
				vm_suspend(caller, whoptr, ph, count,
						   VMSTYPE_KERNELCALL, 1);
				assert(catch_pagefaults);
				catch_pagefaults = 0;
				반환 VMSUSPEND;
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

	반환 OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
정수 virtual_copy_f(
  struct proc * caller,
  struct vir_addr *src_addr,		/* source virtual address */
  struct vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  정수 vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  struct vir_addr *vir_addr[2];	/* virtual source and destination address */
  정수 i, r;
  struct proc *procs[2];

  assert((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  만약 (bytes <= 0) 반환(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  위해 (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	정수 proc_nr;
	struct proc *p;

	만약(proc_e == NONE) {
		p = NULL;
	} 아니면 {
		만약(!isokendpt(proc_e, &proc_nr)) {
			printf("virtual_copy: no reasonable endpoint\n");
			반환 ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  만약 ((r = check_resumed_caller(caller)) != OK)
	반환 r;

  만약((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	정수 writeflag;
  	struct proc *target = NULL;
  	phys_bytes lin;
  	만약(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	만약(!vmcheck || !caller) {
    		반환 r;
  	}

  	만약(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} 아니면 만약(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} 아니면 {
  		panic("r strange: %d",  r);
  	}

	assert(caller);
	assert(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	반환 VMSUSPEND;
  }

  반환 OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
정수 data_copy(const endpoint_t from_proc, const vir_bytes from_addr,
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

  반환 virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
정수 data_copy_vmcheck(struct proc * caller,
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

  반환 virtual_copy_vmcheck(caller, &src, &dst, bytes);
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

static 정수 usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
extern char usermapped_start, usermapped_end, usermapped_nonglo_start;

정수 arch_phys_map(const 정수 index,
			phys_bytes *addr,
			phys_bytes *len,
			정수 *flags)
{
	static 정수 first = 1;
	kern_phys_map *phys_maps;

	정수 freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	만약(first) {
		memset(&minix_kerninfo, 0, sizeof(minix_kerninfo));
		만약(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		만약(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		동안(phys_maps != NULL){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	만약(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		반환 OK;
	}
	아니면 만약(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		반환 OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	동안(phys_maps != NULL){
		만약(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			반환 OK;
		}
		phys_maps = phys_maps->next;
	}

	반환 EINVAL;
}

정수 arch_phys_map_reply(const 정수 index, const vir_bytes addr)
{
	kern_phys_map *phys_maps;

	만약(index == first_um_idx) {
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

		반환 OK;
	}

	만약 (index == usermapped_index) {
		반환 OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	동안(phys_maps != NULL){
		만약(phys_maps->index == index){
			assert(phys_maps->cb != NULL);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			반환 OK;
		}
		phys_maps = phys_maps->next;
	}

	반환 EINVAL;
}

정수 arch_enable_paging(struct proc * caller)
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
	동안(phys_maps != NULL){
		assert(phys_maps->cb != NULL);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	반환 OK;
}

void release_address_space(struct proc *pr)
{
	pr->p_seg.p_ttbr_v = NULL;
	barrier();
}



/*
 * Request a physical mapping
 */
정수 kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       정수 vm_flags, kern_phys_map * priv, 
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
	

	만약 (kern_phys_map_head == NULL){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = NULL;
	} 아니면 {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	반환 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
정수 kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	반환 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
정수 kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	정수 vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	반환 kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}

