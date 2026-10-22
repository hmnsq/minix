
#ଅନ୍ତର୍ଭାଵ <minix/cpufeature.h>

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

#ଅନ୍ତର୍ଭାଵ <string.h>

/* These are set/computed in kernel.lds. */
ବାହ୍ୟ ଅକ୍ଷର _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
ସ୍ଥିର phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
ସ୍ଥିର phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
ସ୍ଥିର phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
ସ୍ଥିର u32_t pagedir[1024]  __aligned(4096);

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

        ଯଦି((o=start % I386_PAGE_SIZE))
                start -= o;
        ଯଦି((o=end % I386_PAGE_SIZE))
                end += I386_PAGE_SIZE - o;

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

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	ପ୍ରତିଜ୍ଞା(len > 0);
	len = roundup(len, I386_PAGE_SIZE);

	ପ୍ରତିଜ୍ଞା(kernel_may_alloc);

	ପାଇଁ(m = 0; m < cbi->mmap_size; m++) {
		ଯଦି(cbi->memmap[m].mm_length < len) ଜାରି;
		ଯଦି(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	ପ୍ରତିଜ୍ଞା(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	ଫେରନ୍ତୁ lowest;
}

ଶୂନ୍ୟ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        ଯଦି(addr > LIMIT) ଫେରନ୍ତୁ;
        ଯଦି(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        ପ୍ରତିଜ୍ଞା(cbi->mmap_size < MAXMEMMAP);
        ଯଦି(len == 0) ଫେରନ୍ତୁ;
	addr = roundup(addr, I386_PAGE_SIZE);
	len = rounddown(len, I386_PAGE_SIZE);

	ପ୍ରତିଜ୍ଞା(kernel_may_alloc);

        ପାଇଁ(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                ଯଦି(cbi->memmap[m].mm_length) ଜାରି;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_length = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                ଯଦି(m >= cbi->mmap_size)
                        cbi->mmap_size = m+1;
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
#define PG_PAGETABLES 6
	ସ୍ଥିର u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pt_inuse = 0;
	ଯଦି(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	ପ୍ରତିଜ୍ଞା(ଆକାର(pagetables[pt_inuse]) == I386_PAGE_SIZE);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ଫେରନ୍ତୁ ret;
}

#define PAGE_KB (I386_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;
	multiboot_memory_map_t *mmap;

	ପ୍ରତିଜ୍ଞା(kernel_may_alloc);

	ପାଇଁ(m = cbi->mmap_size-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		ଯଦି(!mmap->mm_length) ଜାରି;
		ପ୍ରତିଜ୍ଞା(mmap->mm_length > 0);
		ପ୍ରତିଜ୍ଞା(!(mmap->mm_length % I386_PAGE_SIZE));
		ପ୍ରତିଜ୍ଞା(!(mmap->mm_base_addr % I386_PAGE_SIZE));

		mmap->mm_length -= I386_PAGE_SIZE;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_SIZE;

		ଫେରନ୍ତୁ mmap->mm_base_addr + mmap->mm_length;
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
        ପାଇଁ(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_SIZE;
		ଯଦି((cbi->mem_ଉପର_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pg_mapkernel(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        ପ୍ରତିଜ୍ଞା(!(kern_vir_start % I386_BIG_PAGE_SIZE));
        ପ୍ରତିଜ୍ଞା(!(kern_phys % I386_BIG_PAGE_SIZE));
        pde = kern_vir_start / I386_BIG_PAGE_SIZE; /* start pde */
	ଯେତେବେଳେ(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_SIZE;
		kern_phys += I386_BIG_PAGE_SIZE;
		pde++;
	}
	ଫେରନ୍ତୁ pde;	/* free pde */
}

ଶୂନ୍ୟ vm_enable_paging(ଶୂନ୍ୟ)
{
        u32_t cr0, cr4;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	ଯଦି(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	ପ୍ରତିଜ୍ଞା(cr0 & I386_CR0_PE);

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
        ଯଦି(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(ଶୂନ୍ୟ)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	ଫେରନ୍ତୁ phpagedir;
}

ଶୂନ୍ୟ pg_clear(ଶୂନ୍ୟ)
{
	memset(pagedir, 0, ଆକାର(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	ଯଦି(!(o = b % I386_PAGE_SIZE))
		ଫେରନ୍ତୁ b;
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
		ପ୍ରତିଜ୍ଞା(!(vaddr % I386_PAGE_SIZE));
	} ଅନ୍ୟଥା  {
		ପ୍ରତିଜ୍ଞା((vaddr % I386_PAGE_SIZE) == (phys % I386_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	ପ୍ରତିଜ୍ଞା(vaddr < kern_vir_start);

	ଯେତେବେଳେ(vaddr < vaddr_end) {
		phys_bytes source = phys;
		ପ୍ରତିଜ୍ଞା(!(vaddr % I386_PAGE_SIZE));
		ଯଦି(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} ଅନ୍ୟଥା {
			ପ୍ରତିଜ୍ଞା(!(phys % I386_PAGE_SIZE));
		}
		ପ୍ରତିଜ୍ଞା(!(source % I386_PAGE_SIZE));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		ଯଦି(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		ପ୍ରତିଜ୍ଞା(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_SIZE;
		ଯଦି(phys != PG_ALLOCATEME)
			phys += I386_PAGE_SIZE;
	}
}

ଶୂନ୍ୟ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

