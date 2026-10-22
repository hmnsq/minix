#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>

#ಅನ್ತರ್ಭಾವ <minix/type.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <arm/armreg.h>

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <minix/type.h>

/* These are set/computed in kernel.lds. */
ಬಾಹ್ಯ ವರ್ಣ _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
ಸ್ಥಿರ phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
ಸ್ಥಿರ phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
ಸ್ಥಿರ phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
ಸ್ಥಿರ u32_t pagedir[4096]  __aligned(16384);

ಶೂನ್ಯ print_memmap(kinfo_t *cbi)
{
	ಪೂರ್ಣಾಙ್ಕ m;
	ಪ್ರತಿಜ್ಞಾ(cbi->mmap_size < MAXMEMMAP);
	ಪ್ರತಿ(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
		printf("%08lx-%08lx ",addr, endit);
	}
	printf("\nsize %08lx\n", cbi->mmap_size);
}

ಶೂನ್ಯ cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
	ಪೂರ್ಣಾಙ್ಕ m;
	phys_bytes o;

	ಯದಿ((o=start % ARM_PAGE_SIZE))
		start -= o;
	ಯದಿ((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಪ್ರತಿ(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes substart = start, subend = end;
		phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
		    memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

		/* adjust cut range to be a subset of the free memory */
		ಯದಿ(substart < memaddr) substart = memaddr;
		ಯದಿ(subend > memend) subend = memend;
		ಯದಿ(substart >= subend) ಅನುವರ್ತ;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
		ಯದಿ(substart > memaddr)
			add_memmap(cbi, memaddr, substart-memaddr);
		ಯದಿ(subend < memend)
			add_memmap(cbi, subend, memend-subend);
	}
}

ಶೂನ್ಯ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	ಪೂರ್ಣಾಙ್ಕ m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	ಯದಿ(addr > LIMIT) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	ಪ್ರತಿಜ್ಞಾ(cbi->mmap_size < MAXMEMMAP);
	ಯದಿ(len == 0) {
		ಪ್ರತ್ಯಾಗಮ;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

        ಪ್ರತಿ(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		ಯದಿ(cbi->memmap[m].mm_length) {
			ಅನುವರ್ತ;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		ಯದಿ(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
		highmark = addr + len;
		ಯದಿ(highmark > cbi->mem_ಮೇಲಿನ_phys) {
			cbi->mem_ಮೇಲಿನ_phys = highmark;
		}
		ಪ್ರತ್ಯಾಗಮ;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 24
	ಸ್ಥಿರ u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pt_inuse = 0;
	ಯದಿ(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ಪ್ರತ್ಯಾಗಮ ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	ಪೂರ್ಣಾಙ್ಕ m;
	multiboot_memory_map_t *mmap;

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಪ್ರತಿ(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		ಯದಿ(!mmap->mm_length) {
			ಅನುವರ್ತ;
		}
		ಪ್ರತಿಜ್ಞಾ(mmap->mm_length > 0);
		ಪ್ರತಿಜ್ಞಾ(!(mmap->mm_length % ARM_PAGE_SIZE));
		ಪ್ರತಿಜ್ಞಾ(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		ಪ್ರತ್ಯಾಗಮ addr;
	}

	panic("can't find free memory");
}

ಶೂನ್ಯ pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	ಪ್ರತಿಜ್ಞಾ(cbi->mem_ಮೇಲಿನ_phys);

        /* Set up an identity mapping page directory */
	 ಪ್ರತಿ(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		ಯದಿ (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} ಅನ್ಯಥಾ {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

ಪೂರ್ಣಾಙ್ಕ pg_mapkernel(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	ಪ್ರತಿಜ್ಞಾ(!(kern_vir_start % ARM_SECTION_SIZE));
	ಪ್ರತಿಜ್ಞಾ(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	ಯಾವತ್(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	ಪ್ರತ್ಯಾಗಮ pde;	/* free pde */
}

ಶೂನ್ಯ vm_enable_paging(ಶೂನ್ಯ)
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

phys_bytes pg_load(ಶೂನ್ಯ)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	ಪ್ರತ್ಯಾಗಮ phpagedir;
}

ಶೂನ್ಯ pg_clear(ಶೂನ್ಯ)
{
	memset(pagedir, 0, ಪರಿಮಾಣ(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	ಯದಿ(!(o = b % ARM_PAGE_SIZE)) {
		ಪ್ರತ್ಯಾಗಮ b;
	}
	ಪ್ರತ್ಯಾಗಮ b  - o;
}

ಶೂನ್ಯ pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mapped_pde = -1;
	ಸ್ಥಿರ u32_t *pt = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ pde, pte;

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಯದಿ(phys == PG_ALLOCATEME) {
		ಪ್ರತಿಜ್ಞಾ(!(vaddr % ARM_PAGE_SIZE));
	} ಅನ್ಯಥಾ  {
		ಪ್ರತಿಜ್ಞಾ((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	ಪ್ರತಿಜ್ಞಾ(vaddr < kern_vir_start);

	ಯಾವತ್(vaddr < vaddr_end) {
		phys_bytes source = phys;
		ಪ್ರತಿಜ್ಞಾ(!(vaddr % ARM_PAGE_SIZE));
		ಯದಿ(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} ಅನ್ಯಥಾ {
			ಪ್ರತಿಜ್ಞಾ(!(phys % ARM_PAGE_SIZE));
		}
		ಪ್ರತಿಜ್ಞಾ(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		ಯದಿ(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		ಪ್ರತಿಜ್ಞಾ(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		ಯದಿ(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

ಶೂನ್ಯ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
