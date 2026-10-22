
#अन्तर्भाव <minix/cpufeature.h>

#अन्तर्भाव <assert.h>
#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव <string.h>

/* These are set/computed in kernel.lds. */
बाह्य वर्ण _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
स्थिर phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
स्थिर phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
स्थिर phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
स्थिर u32_t pagedir[1024]  __aligned(4096);

शून्य print_memmap(kinfo_t *cbi)
{
        पूर्णाङ्क m;
        प्रतिज्ञा(cbi->mmap_size < MAXMEMMAP);
        अत्थाय(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
                printf("%08lx-%08lx ",addr, endit);
        }
        printf("\nsize %08lx\n", cbi->mmap_size);
}

शून्य cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
        पूर्णाङ्क m;
        phys_bytes o;

        सचे((o=start % I386_PAGE_SIZE))
                start -= o;
        सचे((o=end % I386_PAGE_SIZE))
                end += I386_PAGE_SIZE - o;

	प्रतिज्ञा(kernel_may_alloc);

        अत्थाय(m = 0; m < cbi->mmap_size; m++) {
                phys_bytes substart = start, subend = end;
                phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
                        memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

                /* adjust cut range to be a subset of the free memory */
                सचे(substart < memaddr) substart = memaddr;
                सचे(subend > memend) subend = memend;
                सचे(substart >= subend) अनुवर्त;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
                सचे(substart > memaddr)
                        add_memmap(cbi, memaddr, substart-memaddr);
                सचे(subend < memend)
                        add_memmap(cbi, subend, memend-subend);
        }
}

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	पूर्णाङ्क m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	प्रतिज्ञा(len > 0);
	len = roundup(len, I386_PAGE_SIZE);

	प्रतिज्ञा(kernel_may_alloc);

	अत्थाय(m = 0; m < cbi->mmap_size; m++) {
		सचे(cbi->memmap[m].mm_length < len) अनुवर्त;
		सचे(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	प्रतिज्ञा(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	पटिनिवत्त lowest;
}

शून्य add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        पूर्णाङ्क m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        सचे(addr > LIMIT) पटिनिवत्त;
        सचे(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        प्रतिज्ञा(cbi->mmap_size < MAXMEMMAP);
        सचे(len == 0) पटिनिवत्त;
	addr = roundup(addr, I386_PAGE_SIZE);
	len = rounddown(len, I386_PAGE_SIZE);

	प्रतिज्ञा(kernel_may_alloc);

        अत्थाय(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                सचे(cbi->memmap[m].mm_length) अनुवर्त;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_length = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                सचे(m >= cbi->mmap_size)
                        cbi->mmap_size = m+1;
		highmark = addr + len;
		सचे(highmark > cbi->mem_उच्चः_phys) {
			cbi->mem_उच्चः_phys = highmark;
		}

                पटिनिवत्त;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 6
	स्थिर u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	स्थिर पूर्णाङ्क pt_inuse = 0;
	सचे(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	प्रतिज्ञा(परिमाण(pagetables[pt_inuse]) == I386_PAGE_SIZE);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	पटिनिवत्त ret;
}

#define PAGE_KB (I386_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	पूर्णाङ्क m;
	multiboot_memory_map_t *mmap;

	प्रतिज्ञा(kernel_may_alloc);

	अत्थाय(m = cbi->mmap_size-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		सचे(!mmap->mm_length) अनुवर्त;
		प्रतिज्ञा(mmap->mm_length > 0);
		प्रतिज्ञा(!(mmap->mm_length % I386_PAGE_SIZE));
		प्रतिज्ञा(!(mmap->mm_base_addr % I386_PAGE_SIZE));

		mmap->mm_length -= I386_PAGE_SIZE;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_SIZE;

		पटिनिवत्त mmap->mm_base_addr + mmap->mm_length;
	}

	panic("can't find free memory");
}

शून्य pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	प्रतिज्ञा(cbi->mem_उच्चः_phys);

        /* Set up an identity mapping page directory */
        अत्थाय(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_SIZE;
		सचे((cbi->mem_उच्चः_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

पूर्णाङ्क pg_mapkernel(शून्य)
{
	पूर्णाङ्क pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        प्रतिज्ञा(!(kern_vir_start % I386_BIG_PAGE_SIZE));
        प्रतिज्ञा(!(kern_phys % I386_BIG_PAGE_SIZE));
        pde = kern_vir_start / I386_BIG_PAGE_SIZE; /* start pde */
	याव(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_SIZE;
		kern_phys += I386_BIG_PAGE_SIZE;
		pde++;
	}
	पटिनिवत्त pde;	/* free pde */
}

शून्य vm_enable_paging(शून्य)
{
        u32_t cr0, cr4;
        पूर्णाङ्क pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	सचे(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	प्रतिज्ञा(cr0 & I386_CR0_PE);

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
        सचे(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(शून्य)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	पटिनिवत्त phpagedir;
}

शून्य pg_clear(शून्य)
{
	memset(pagedir, 0, परिमाण(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	सचे(!(o = b % I386_PAGE_SIZE))
		पटिनिवत्त b;
	पटिनिवत्त b  - o;
}

शून्य pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	स्थिर पूर्णाङ्क mapped_pde = -1;
	स्थिर u32_t *pt = रिक्त;
	पूर्णाङ्क pde, pte;

	प्रतिज्ञा(kernel_may_alloc);

	सचे(phys == PG_ALLOCATEME) {
		प्रतिज्ञा(!(vaddr % I386_PAGE_SIZE));
	} अञ्ञथा  {
		प्रतिज्ञा((vaddr % I386_PAGE_SIZE) == (phys % I386_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	प्रतिज्ञा(vaddr < kern_vir_start);

	याव(vaddr < vaddr_end) {
		phys_bytes source = phys;
		प्रतिज्ञा(!(vaddr % I386_PAGE_SIZE));
		सचे(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} अञ्ञथा {
			प्रतिज्ञा(!(phys % I386_PAGE_SIZE));
		}
		प्रतिज्ञा(!(source % I386_PAGE_SIZE));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		सचे(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		प्रतिज्ञा(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_SIZE;
		सचे(phys != PG_ALLOCATEME)
			phys += I386_PAGE_SIZE;
	}
}

शून्य pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

