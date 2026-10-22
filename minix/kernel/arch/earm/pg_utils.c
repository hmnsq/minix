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
extern char _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
static phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
static phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
static phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
static u32_t pagedir[4096]  __aligned(16384);

void print_memmap(kinfo_t *cbi)
{
	ціле m;
	assert(cbi->mmap_size < MAXMEMMAP);
	для(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
		printf("%08lx-%08lx ",addr, endit);
	}
	printf("\nsize %08lx\n", cbi->mmap_size);
}

void cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
	ціле m;
	phys_bytes o;

	якщо((o=start % ARM_PAGE_SIZE))
		start -= o;
	якщо((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

	assert(kernel_may_alloc);

	для(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes substart = start, subend = end;
		phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
		    memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

		/* adjust cut range to be a subset of the free memory */
		якщо(substart < memaddr) substart = memaddr;
		якщо(subend > memend) subend = memend;
		якщо(substart >= subend) continue;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
		якщо(substart > memaddr)
			add_memmap(cbi, memaddr, substart-memaddr);
		якщо(subend < memend)
			add_memmap(cbi, subend, memend-subend);
	}
}

void add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	ціле m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	якщо(addr > LIMIT) {
		повернути;
	}

	якщо(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	assert(cbi->mmap_size < MAXMEMMAP);
	якщо(len == 0) {
		повернути;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	assert(kernel_may_alloc);

        для(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		якщо(cbi->memmap[m].mm_length) {
			continue;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		якщо(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
		highmark = addr + len;
		якщо(highmark > cbi->mem_high_phys) {
			cbi->mem_high_phys = highmark;
		}
		повернути;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 24
	static u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	static ціле pt_inuse = 0;
	якщо(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	assert(sizeof(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	повернути ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	ціле m;
	multiboot_memory_map_t *mmap;

	assert(kernel_may_alloc);

	для(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		якщо(!mmap->mm_length) {
			continue;
		}
		assert(mmap->mm_length > 0);
		assert(!(mmap->mm_length % ARM_PAGE_SIZE));
		assert(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		повернути addr;
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
	 для(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		якщо (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} інакше {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

ціле pg_mapkernel(void)
{
	ціле pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	assert(!(kern_vir_start % ARM_SECTION_SIZE));
	assert(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	поки(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	повернути pde;	/* free pde */
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
	повернути phpagedir;
}

void pg_clear(void)
{
	memset(pagedir, 0, sizeof(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	якщо(!(o = b % ARM_PAGE_SIZE)) {
		повернути b;
	}
	повернути b  - o;
}

void pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	static ціле mapped_pde = -1;
	static u32_t *pt = NULL;
	ціле pde, pte;

	assert(kernel_may_alloc);

	якщо(phys == PG_ALLOCATEME) {
		assert(!(vaddr % ARM_PAGE_SIZE));
	} інакше  {
		assert((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	assert(vaddr < kern_vir_start);

	поки(vaddr < vaddr_end) {
		phys_bytes source = phys;
		assert(!(vaddr % ARM_PAGE_SIZE));
		якщо(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} інакше {
			assert(!(phys % ARM_PAGE_SIZE));
		}
		assert(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		якщо(mapped_pde < pde) {
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
		якщо(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

void pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
