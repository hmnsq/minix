#ଅନ୍ତର୍ଭାଵ <minix/cpufeature.h>

#ଅନ୍ତର୍ଭାଵ <minix/type.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"
#ଅନ୍ତର୍ଭାଵ <machine/cpu.h>
#ଅନ୍ତର୍ଭାଵ <arm/armreg.h>

#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <minix/type.h>

/* These are set/computed in kernel.lds. */
ବାହ୍ୟ ଅକ୍ଷର _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
ସ୍ଥିର phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
ସ୍ଥିର phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
ସ୍ଥିର phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
ସ୍ଥିର u32_t pagedir[4096]  __aligned(16384);

ଶୂନ୍ୟ print_memmap(kinfo_t *cbi)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;
	ପ୍ରତିଜ୍ଞା(cbi->mmap_size < MAXMEMMAP);
	ପାଇଁ(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
		printf("%08lx-%08lx ",addr, endit);
	}
	printf("\nsize %08lx\n", cbi->mmap_size);
}

ଶୂନ୍ୟ cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;
	phys_bytes o;

	ଯଦି((o=start % ARM_PAGE_SIZE))
		start -= o;
	ଯଦି((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

	ପ୍ରତିଜ୍ଞା(kernel_may_alloc);

	ପାଇଁ(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes substart = start, subend = end;
		phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
		    memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

		/* adjust cut range to be a subset of the free memory */
		ଯଦି(substart < memaddr) substart = memaddr;
		ଯଦି(subend > memend) subend = memend;
		ଯଦି(substart >= subend) ଜାରି;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
		ଯଦି(substart > memaddr)
			add_memmap(cbi, memaddr, substart-memaddr);
		ଯଦି(subend < memend)
			add_memmap(cbi, subend, memend-subend);
	}
}

ଶୂନ୍ୟ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	ଯଦି(addr > LIMIT) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	ପ୍ରତିଜ୍ଞା(cbi->mmap_size < MAXMEMMAP);
	ଯଦି(len == 0) {
		ଫେରନ୍ତୁ;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	ପ୍ରତିଜ୍ଞା(kernel_may_alloc);

        ପାଇଁ(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		ଯଦି(cbi->memmap[m].mm_length) {
			ଜାରି;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		ଯଦି(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
		highmark = addr + len;
		ଯଦି(highmark > cbi->mem_ଉପର_phys) {
			cbi->mem_ଉପର_phys = highmark;
		}
		ଫେରନ୍ତୁ;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 24
	ସ୍ଥିର u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pt_inuse = 0;
	ଯଦି(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	ପ୍ରତିଜ୍ଞା(ଆକାର(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ଫେରନ୍ତୁ ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;
	multiboot_memory_map_t *mmap;

	ପ୍ରତିଜ୍ଞା(kernel_may_alloc);

	ପାଇଁ(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		ଯଦି(!mmap->mm_length) {
			ଜାରି;
		}
		ପ୍ରତିଜ୍ଞା(mmap->mm_length > 0);
		ପ୍ରତିଜ୍ଞା(!(mmap->mm_length % ARM_PAGE_SIZE));
		ପ୍ରତିଜ୍ଞା(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		ଫେରନ୍ତୁ addr;
	}

	panic("can't find free memory");
}

ଶୂନ୍ୟ pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	ପ୍ରତିଜ୍ଞା(cbi->mem_ଉପର_phys);

        /* Set up an identity mapping page directory */
	 ପାଇଁ(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		ଯଦି (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} ଅନ୍ୟଥା {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pg_mapkernel(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	ପ୍ରତିଜ୍ଞା(!(kern_vir_start % ARM_SECTION_SIZE));
	ପ୍ରତିଜ୍ଞା(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	ଯେତେବେଳେ(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	ଫେରନ୍ତୁ pde;	/* free pde */
}

ଶୂନ୍ୟ vm_enable_paging(ଶୂନ୍ୟ)
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

phys_bytes pg_load(ଶୂନ୍ୟ)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	ଫେରନ୍ତୁ phpagedir;
}

ଶୂନ୍ୟ pg_clear(ଶୂନ୍ୟ)
{
	memset(pagedir, 0, ଆକାର(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	ଯଦି(!(o = b % ARM_PAGE_SIZE)) {
		ଫେରନ୍ତୁ b;
	}
	ଫେରନ୍ତୁ b  - o;
}

ଶୂନ୍ୟ pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mapped_pde = -1;
	ସ୍ଥିର u32_t *pt = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pde, pte;

	ପ୍ରତିଜ୍ଞା(kernel_may_alloc);

	ଯଦି(phys == PG_ALLOCATEME) {
		ପ୍ରତିଜ୍ଞା(!(vaddr % ARM_PAGE_SIZE));
	} ଅନ୍ୟଥା  {
		ପ୍ରତିଜ୍ଞା((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	ପ୍ରତିଜ୍ଞା(vaddr < kern_vir_start);

	ଯେତେବେଳେ(vaddr < vaddr_end) {
		phys_bytes source = phys;
		ପ୍ରତିଜ୍ଞା(!(vaddr % ARM_PAGE_SIZE));
		ଯଦି(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} ଅନ୍ୟଥା {
			ପ୍ରତିଜ୍ଞା(!(phys % ARM_PAGE_SIZE));
		}
		ପ୍ରତିଜ୍ଞା(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		ଯଦି(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		ପ୍ରତିଜ୍ଞା(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		ଯଦି(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

ଶୂନ୍ୟ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
