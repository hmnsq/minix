
#অন্তর্ভাব <minix/cpufeature.h>

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "arch_proto.h"

#অন্তর্ভাব <string.h>

/* These are set/computed in kernel.lds. */
বাহ্যিক অক্ষর _kern_vir_base, _kern_phys_base, kern_আকার;

/* Retrieve the absolute values to something we can use. */
স্থির phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
স্থির phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
স্থির phys_bytes kern_kernlen = (phys_bytes) &kern_আকার;

/* page directory we can use to map things */
স্থির u32_t pagedir[1024]  __aligned(4096);

শূন্য print_memmap(kinfo_t *cbi)
{
        পূর্ণসংখ্যা m;
        প্রতিজ্ঞা(cbi->mmap_আকার < MAXMEMMAP);
        জন্য(m = 0; m < cbi->mmap_আকার; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_দৈর্ঘ্য;
                printf("%08lx-%08lx ",addr, endit);
        }
        printf("\nsize %08lx\n", cbi->mmap_আকার);
}

শূন্য cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
        পূর্ণসংখ্যা m;
        phys_bytes o;

        যদি((o=start % I386_PAGE_আকার))
                start -= o;
        যদি((o=end % I386_PAGE_আকার))
                end += I386_PAGE_আকার - o;

	প্রতিজ্ঞা(kernel_may_alloc);

        জন্য(m = 0; m < cbi->mmap_আকার; m++) {
                phys_bytes substart = start, subend = end;
                phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
                        memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_দৈর্ঘ্য;

                /* adjust cut range to be a subset of the free memory */
                যদি(substart < memaddr) substart = memaddr;
                যদি(subend > memend) subend = memend;
                যদি(substart >= subend) চালু;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_দৈর্ঘ্য = 0;
                যদি(substart > memaddr)
                        add_memmap(cbi, memaddr, substart-memaddr);
                যদি(subend < memend)
                        add_memmap(cbi, subend, memend-subend);
        }
}

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	পূর্ণসংখ্যা m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	প্রতিজ্ঞা(len > 0);
	len = roundup(len, I386_PAGE_আকার);

	প্রতিজ্ঞা(kernel_may_alloc);

	জন্য(m = 0; m < cbi->mmap_আকার; m++) {
		যদি(cbi->memmap[m].mm_দৈর্ঘ্য < len) চালু;
		যদি(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	প্রতিজ্ঞা(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	ফেরত lowest;
}

শূন্য add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        পূর্ণসংখ্যা m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        যদি(addr > LIMIT) ফেরত;
        যদি(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        প্রতিজ্ঞা(cbi->mmap_আকার < MAXMEMMAP);
        যদি(len == 0) ফেরত;
	addr = roundup(addr, I386_PAGE_আকার);
	len = rounddown(len, I386_PAGE_আকার);

	প্রতিজ্ঞা(kernel_may_alloc);

        জন্য(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                যদি(cbi->memmap[m].mm_দৈর্ঘ্য) চালু;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_দৈর্ঘ্য = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                যদি(m >= cbi->mmap_আকার)
                        cbi->mmap_আকার = m+1;
		highmark = addr + len;
		যদি(highmark > cbi->mem_উপরের_phys) {
			cbi->mem_উপরের_phys = highmark;
		}

                ফেরত;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 6
	স্থির u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	স্থির পূর্ণসংখ্যা pt_inuse = 0;
	যদি(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	প্রতিজ্ঞা(আকার(pagetables[pt_inuse]) == I386_PAGE_আকার);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ফেরত ret;
}

#define PAGE_KB (I386_PAGE_আকার / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	পূর্ণসংখ্যা m;
	multiboot_memory_map_t *mmap;

	প্রতিজ্ঞা(kernel_may_alloc);

	জন্য(m = cbi->mmap_আকার-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		যদি(!mmap->mm_দৈর্ঘ্য) চালু;
		প্রতিজ্ঞা(mmap->mm_দৈর্ঘ্য > 0);
		প্রতিজ্ঞা(!(mmap->mm_দৈর্ঘ্য % I386_PAGE_আকার));
		প্রতিজ্ঞা(!(mmap->mm_base_addr % I386_PAGE_আকার));

		mmap->mm_দৈর্ঘ্য -= I386_PAGE_আকার;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_আকার;

		ফেরত mmap->mm_base_addr + mmap->mm_দৈর্ঘ্য;
	}

	panic("can't find free memory");
}

শূন্য pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	প্রতিজ্ঞা(cbi->mem_উপরের_phys);

        /* Set up an identity mapping page directory */
        জন্য(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_আকার;
		যদি((cbi->mem_উপরের_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

পূর্ণসংখ্যা pg_mapkernel(শূন্য)
{
	পূর্ণসংখ্যা pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        প্রতিজ্ঞা(!(kern_vir_start % I386_BIG_PAGE_আকার));
        প্রতিজ্ঞা(!(kern_phys % I386_BIG_PAGE_আকার));
        pde = kern_vir_start / I386_BIG_PAGE_আকার; /* start pde */
	যখন(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_আকার;
		kern_phys += I386_BIG_PAGE_আকার;
		pde++;
	}
	ফেরত pde;	/* free pde */
}

শূন্য vm_enable_paging(শূন্য)
{
        u32_t cr0, cr4;
        পূর্ণসংখ্যা pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	যদি(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	প্রতিজ্ঞা(cr0 & I386_CR0_PE);

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
        যদি(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(শূন্য)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	ফেরত phpagedir;
}

শূন্য pg_clear(শূন্য)
{
	memset(pagedir, 0, আকার(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	যদি(!(o = b % I386_PAGE_আকার))
		ফেরত b;
	ফেরত b  - o;
}

শূন্য pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	স্থির পূর্ণসংখ্যা mapped_pde = -1;
	স্থির u32_t *pt = ফাঁকা;
	পূর্ণসংখ্যা pde, pte;

	প্রতিজ্ঞা(kernel_may_alloc);

	যদি(phys == PG_ALLOCATEME) {
		প্রতিজ্ঞা(!(vaddr % I386_PAGE_আকার));
	} অন্যথা  {
		প্রতিজ্ঞা((vaddr % I386_PAGE_আকার) == (phys % I386_PAGE_আকার));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	প্রতিজ্ঞা(vaddr < kern_vir_start);

	যখন(vaddr < vaddr_end) {
		phys_bytes source = phys;
		প্রতিজ্ঞা(!(vaddr % I386_PAGE_আকার));
		যদি(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} অন্যথা {
			প্রতিজ্ঞা(!(phys % I386_PAGE_আকার));
		}
		প্রতিজ্ঞা(!(source % I386_PAGE_আকার));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		যদি(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		প্রতিজ্ঞা(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_আকার;
		যদি(phys != PG_ALLOCATEME)
			phys += I386_PAGE_আকার;
	}
}

শূন্য pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

