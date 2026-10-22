
#समावेश <minix/cpufeature.h>

#समावेश <assert.h>
#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"

#समावेश <string.h>

/* These are set/computed in kernel.lds. */
बाह्य अक्षर _kern_vir_base, _kern_phys_base, kern_आकार;

/* Retrieve the absolute values to something we can use. */
अटल phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
अटल phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
अटल phys_bytes kern_kernlen = (phys_bytes) &kern_आकार;

/* page directory we can use to map things */
अटल u32_t pagedir[1024]  __aligned(4096);

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

        अगर((o=start % I386_PAGE_आकार))
                start -= o;
        अगर((o=end % I386_PAGE_आकार))
                end += I386_PAGE_आकार - o;

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

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	पूर्णांक m;
#घोषणा EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	निश्चित(len > 0);
	len = roundup(len, I386_PAGE_आकार);

	निश्चित(kernel_may_alloc);

	क्रम(m = 0; m < cbi->mmap_आकार; m++) {
		अगर(cbi->memmap[m].mm_लंबाई < len) जारी;
		अगर(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	निश्चित(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	वापस lowest;
}

व्योम add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        पूर्णांक m;
#घोषणा LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        अगर(addr > LIMIT) वापस;
        अगर(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        निश्चित(cbi->mmap_आकार < MAXMEMMAP);
        अगर(len == 0) वापस;
	addr = roundup(addr, I386_PAGE_आकार);
	len = rounddown(len, I386_PAGE_आकार);

	निश्चित(kernel_may_alloc);

        क्रम(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                अगर(cbi->memmap[m].mm_लंबाई) जारी;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_लंबाई = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                अगर(m >= cbi->mmap_आकार)
                        cbi->mmap_आकार = m+1;
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
#घोषणा PG_PAGETABLES 6
	अटल u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	अटल पूर्णांक pt_inuse = 0;
	अगर(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	निश्चित(माप(pagetables[pt_inuse]) == I386_PAGE_आकार);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	वापस ret;
}

#घोषणा PAGE_KB (I386_PAGE_आकार / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	पूर्णांक m;
	multiboot_memory_map_t *mmap;

	निश्चित(kernel_may_alloc);

	क्रम(m = cbi->mmap_आकार-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		अगर(!mmap->mm_लंबाई) जारी;
		निश्चित(mmap->mm_लंबाई > 0);
		निश्चित(!(mmap->mm_लंबाई % I386_PAGE_आकार));
		निश्चित(!(mmap->mm_base_addr % I386_PAGE_आकार));

		mmap->mm_लंबाई -= I386_PAGE_आकार;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_आकार;

		वापस mmap->mm_base_addr + mmap->mm_लंबाई;
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
        क्रम(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_आकार;
		अगर((cbi->mem_ऊपरी_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

पूर्णांक pg_mapkernel(व्योम)
{
	पूर्णांक pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        निश्चित(!(kern_vir_start % I386_BIG_PAGE_आकार));
        निश्चित(!(kern_phys % I386_BIG_PAGE_आकार));
        pde = kern_vir_start / I386_BIG_PAGE_आकार; /* start pde */
	जबतक(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_आकार;
		kern_phys += I386_BIG_PAGE_आकार;
		pde++;
	}
	वापस pde;	/* free pde */
}

व्योम vm_enable_paging(व्योम)
{
        u32_t cr0, cr4;
        पूर्णांक pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#अगर_घोषित PAE
	अगर(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#पूर्ण_अगर

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	निश्चित(cr0 & I386_CR0_PE);

        /* First clear PG and PGE flag, as PGE must be enabled after PG. */
        write_cr0(cr0 & ~I386_CR0_PG);
        write_cr4(cr4 & ~(I386_CR4_PGE | I386_CR4_PSE));

        cr0= read_cr0();
        cr4= read_cr4();

        /* Our page table contains 4MB entries. */
        cr4 |= I386_CR4_PSE;

        write_cr4(cr4);

        /* First enable paging, then enable global page flag. */
        cr0 |= I386_CR0_PG;
        write_cr0(cr0);
        cr0 |= I386_CR0_WP;
        write_cr0(cr0);

        /* May we enable these features? */
        अगर(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(व्योम)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	वापस phpagedir;
}

व्योम pg_clear(व्योम)
{
	स_रखो(pagedir, 0, माप(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	अगर(!(o = b % I386_PAGE_आकार))
		वापस b;
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
		निश्चित(!(vaddr % I386_PAGE_आकार));
	} अन्यथा  {
		निश्चित((vaddr % I386_PAGE_आकार) == (phys % I386_PAGE_आकार));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	निश्चित(vaddr < kern_vir_start);

	जबतक(vaddr < vaddr_end) {
		phys_bytes source = phys;
		निश्चित(!(vaddr % I386_PAGE_आकार));
		अगर(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} अन्यथा {
			निश्चित(!(phys % I386_PAGE_आकार));
		}
		निश्चित(!(source % I386_PAGE_आकार));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		अगर(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		निश्चित(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_आकार;
		अगर(phys != PG_ALLOCATEME)
			phys += I386_PAGE_आकार;
	}
}

व्योम pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

