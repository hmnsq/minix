#समावेश <minix/cpufeature.h>

#समावेश <minix/type.h>
#समावेश <assert.h>
#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"
#समावेश <machine/cpu.h>
#समावेश <arm/armreg.h>

#समावेश <string.h>
#समावेश <minix/type.h>

/* These are set/computed in kernel.lds. */
बाह्य अक्षर _kern_vir_base, _kern_phys_base, kern_आकार;

/* Retrieve the absolute values to something we can use. */
अटल phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
अटल phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
अटल phys_bytes kern_kernlen = (phys_bytes) &kern_आकार;

/* page directory we can use to map things */
अटल u32_t pagedir[4096]  __aligned(16384);

व्योम print_memmap(kinfo_t *cbi)
{
	पूर्णांक m;
	निश्चित(cbi->mmap_आकार < MAXMEMMAP);
	क्रम(m = 0; m < cbi->mmap_आकार; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_लंबाई;
		म_लिखो("%08lx-%08lx ",addr, endit);
	}
	म_लिखो("\nsize %08lx\n", cbi->mmap_आकार);
}

व्योम cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
	पूर्णांक m;
	phys_bytes o;

	अगर((o=start % ARM_PAGE_आकार))
		start -= o;
	अगर((o=end % ARM_PAGE_आकार))
		end += ARM_PAGE_आकार - o;

	निश्चित(kernel_may_alloc);

	क्रम(m = 0; m < cbi->mmap_आकार; m++) {
		phys_bytes substart = start, subend = end;
		phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
		    memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_लंबाई;

		/* adjust cut range to be a subset of the free memory */
		अगर(substart < memaddr) substart = memaddr;
		अगर(subend > memend) subend = memend;
		अगर(substart >= subend) जारी;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_लंबाई = 0;
		अगर(substart > memaddr)
			add_memmap(cbi, memaddr, substart-memaddr);
		अगर(subend < memend)
			add_memmap(cbi, subend, memend-subend);
	}
}

व्योम add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	पूर्णांक m;
#घोषणा LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	अगर(addr > LIMIT) {
		वापस;
	}

	अगर(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	निश्चित(cbi->mmap_आकार < MAXMEMMAP);
	अगर(len == 0) {
		वापस;
	}
	addr = roundup(addr, ARM_PAGE_आकार);
	len = rounddown(len, ARM_PAGE_आकार);

	निश्चित(kernel_may_alloc);

        क्रम(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		अगर(cbi->memmap[m].mm_लंबाई) {
			जारी;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_लंबाई = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		अगर(m >= cbi->mmap_आकार) {
			cbi->mmap_आकार = m+1;
		}
		highmark = addr + len;
		अगर(highmark > cbi->mem_ऊपरी_phys) {
			cbi->mem_ऊपरी_phys = highmark;
		}
		वापस;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#घोषणा PG_PAGETABLES 24
	अटल u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	अटल पूर्णांक pt_inuse = 0;
	अगर(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	निश्चित(माप(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	वापस ret;
}

#घोषणा PAGE_KB (ARM_PAGE_आकार / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	पूर्णांक m;
	multiboot_memory_map_t *mmap;

	निश्चित(kernel_may_alloc);

	क्रम(m = 0; m < cbi->mmap_आकार; m++) {
		mmap = &cbi->memmap[m];
		अगर(!mmap->mm_लंबाई) {
			जारी;
		}
		निश्चित(mmap->mm_लंबाई > 0);
		निश्चित(!(mmap->mm_लंबाई % ARM_PAGE_आकार));
		निश्चित(!(mmap->mm_base_addr % ARM_PAGE_आकार));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_आकार;
		mmap->mm_लंबाई  -= ARM_PAGE_आकार;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_आकार;

		वापस addr;
	}

	panic("can't find free memory");
}

व्योम pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	निश्चित(cbi->mem_ऊपरी_phys);

        /* Set up an identity mapping page directory */
	 क्रम(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_आकार;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		अगर (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} अन्यथा {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

पूर्णांक pg_mapkernel(व्योम)
{
	पूर्णांक pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	निश्चित(!(kern_vir_start % ARM_SECTION_आकार));
	निश्चित(!(kern_phys_start % ARM_SECTION_आकार));
	pde = kern_vir_start / ARM_SECTION_आकार; /* start pde */
	जबतक(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_आकार;
		kern_phys += ARM_SECTION_आकार;
		pde++;
	}
	वापस pde;	/* free pde */
}

व्योम vm_enable_paging(व्योम)
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
	#घोषणा CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_bytes pg_load(व्योम)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	वापस phpagedir;
}

व्योम pg_clear(व्योम)
{
	स_रखो(pagedir, 0, माप(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	अगर(!(o = b % ARM_PAGE_आकार)) {
		वापस b;
	}
	वापस b  - o;
}

व्योम pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	अटल पूर्णांक mapped_pde = -1;
	अटल u32_t *pt = शून्य;
	पूर्णांक pde, pte;

	निश्चित(kernel_may_alloc);

	अगर(phys == PG_ALLOCATEME) {
		निश्चित(!(vaddr % ARM_PAGE_आकार));
	} अन्यथा  {
		निश्चित((vaddr % ARM_PAGE_आकार) == (phys % ARM_PAGE_आकार));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	निश्चित(vaddr < kern_vir_start);

	जबतक(vaddr < vaddr_end) {
		phys_bytes source = phys;
		निश्चित(!(vaddr % ARM_PAGE_आकार));
		अगर(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} अन्यथा {
			निश्चित(!(phys % ARM_PAGE_आकार));
		}
		निश्चित(!(source % ARM_PAGE_आकार));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		अगर(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		निश्चित(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_आकार;
		अगर(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_आकार;
		}
	}
}

व्योम pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
