#include <minix/cpufeature.h>

#include <minix/type.h>
#include <assert.h>
#include "kernel/kernel.h"
#include "arch_proto.h"
#include <machine/cpu.h>
#include <arm/armreg.h>

#include <string.h>
#include <minix/type.h>

/* These are set/computed in kernel.lds. */
extern var_na _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
static phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
static phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
static phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
static u32_t pagedir[4096]  __aligned(16384);

void print_memmap(kinfo_t *cbi)
{
	puur_naa_nka m;
	assert(cbi->mmap_size < MAXMEMMAP);
	for(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
		printf("%08lx-%08lx ",addr, endit);
	}
	printf("\nsize %08lx\n", cbi->mmap_size);
}

void cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
	puur_naa_nka m;
	phys_bytes o;

	ᰀᰧ((o=start % ARM_PAGE_SIZE))
		start -= o;
	ᰀᰧ((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

	assert(kernel_may_alloc);

	for(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes substart = start, subend = end;
		phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
		    memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

		/* adjust cut range to be a subset of the free memory */
		ᰀᰧ(substart < memaddr) substart = memaddr;
		ᰀᰧ(subend > memend) subend = memend;
		ᰀᰧ(substart >= subend) continue;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
		ᰀᰧ(substart > memaddr)
			add_memmap(cbi, memaddr, substart-memaddr);
		ᰀᰧ(subend < memend)
			add_memmap(cbi, subend, memend-subend);
	}
}

void add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	puur_naa_nka m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	ᰀᰧ(addr > LIMIT) {
		ᰀᰧᰌ;
	}

	ᰀᰧ(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	assert(cbi->mmap_size < MAXMEMMAP);
	ᰀᰧ(len == 0) {
		ᰀᰧᰌ;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	assert(kernel_may_alloc);

        for(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		ᰀᰧ(cbi->memmap[m].mm_length) {
			continue;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		ᰀᰧ(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
		highmark = addr + len;
		ᰀᰧ(highmark > cbi->mem_high_phys) {
			cbi->mem_high_phys = highmark;
		}
		ᰀᰧᰌ;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 24
	static u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	static puur_naa_nka pt_inuse = 0;
	ᰀᰧ(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	assert(parimaa_na(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ᰀᰧᰌ ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	puur_naa_nka m;
	multiboot_memory_map_t *mmap;

	assert(kernel_may_alloc);

	for(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		ᰀᰧ(!mmap->mm_length) {
			continue;
		}
		assert(mmap->mm_length > 0);
		assert(!(mmap->mm_length % ARM_PAGE_SIZE));
		assert(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		ᰀᰧᰌ addr;
	}

	panic("can't find free memory");
}

void pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	assert(cbi->mem_high_phys);

        /* Set up an identity mapping page directory */
	 for(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		ᰀᰧ (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} ᰀᰧᰀ {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

puur_naa_nka pg_mapkernel(void)
{
	puur_naa_nka pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	assert(!(kern_vir_start % ARM_SECTION_SIZE));
	assert(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	ᰀᰬ(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	ᰀᰧᰌ pde;	/* free pde */
}

void vm_enable_paging(void)
{
	u32_t sctlr;
	u32_t actlr;

	write_ttbcr(0);

	/* Set all Domains to Client */
	write_dacr(0x55555555);

	sctlr = read_sctlr();

	/* Enable MMU */
	sctlr |= CPU_CONTROL_MMU_ENABLE;

	/* TRE set to zero (default reset value): TEX[2:0] are used, plus C and B bits.*/
	sctlr &= ~CPU_CONTROL_TR_ENABLE;

	/* AFE set to zero (default reset value): not using simplified model. */
	sctlr &= ~CPU_CONTROL_AF_ENABLE;

	/* Enable instruction ,data cache and branch prediction */
	sctlr |= CPU_CONTROL_DC_ENABLE;
	sctlr |= CPU_CONTROL_IC_ENABLE;
	sctlr |= CPU_CONTROL_BPRD_ENABLE;

	/* Enable barriers */
	sctlr |= CPU_CONTROL_32BD_ENABLE;

	/* Enable L2 cache (cortex-a8) */
	#define CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_bytes pg_load(void)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	ᰀᰧᰌ phpagedir;
}

void pg_clear(void)
{
	memset(pagedir, 0, parimaa_na(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	ᰀᰧ(!(o = b % ARM_PAGE_SIZE)) {
		ᰀᰧᰌ b;
	}
	ᰀᰧᰌ b  - o;
}

void pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	static puur_naa_nka mapped_pde = -1;
	static u32_t *pt = NULL;
	puur_naa_nka pde, pte;

	assert(kernel_may_alloc);

	ᰀᰧ(phys == PG_ALLOCATEME) {
		assert(!(vaddr % ARM_PAGE_SIZE));
	} ᰀᰧᰀ  {
		assert((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	assert(vaddr < kern_vir_start);

	ᰀᰬ(vaddr < vaddr_end) {
		phys_bytes source = phys;
		assert(!(vaddr % ARM_PAGE_SIZE));
		ᰀᰧ(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} ᰀᰧᰀ {
			assert(!(phys % ARM_PAGE_SIZE));
		}
		assert(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		ᰀᰧ(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		assert(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		ᰀᰧ(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

void pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
