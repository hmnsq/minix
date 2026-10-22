
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ <string.h>

/* These are set/computed in kernel.lds. */
ಬಾಹ್ಯ ವರ್ಣ _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
ಸ್ಥಿರ phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
ಸ್ಥಿರ phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
ಸ್ಥಿರ phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
ಸ್ಥಿರ u32_t pagedir[1024]  __aligned(4096);

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

        ಯದಿ((o=start % I386_PAGE_SIZE))
                start -= o;
        ಯದಿ((o=end % I386_PAGE_SIZE))
                end += I386_PAGE_SIZE - o;

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

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	ಪೂರ್ಣಾಙ್ಕ m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	ಪ್ರತಿಜ್ಞಾ(len > 0);
	len = roundup(len, I386_PAGE_SIZE);

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಪ್ರತಿ(m = 0; m < cbi->mmap_size; m++) {
		ಯದಿ(cbi->memmap[m].mm_length < len) ಅನುವರ್ತ;
		ಯದಿ(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	ಪ್ರತಿಜ್ಞಾ(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	ಪ್ರತ್ಯಾಗಮ lowest;
}

ಶೂನ್ಯ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        ಪೂರ್ಣಾಙ್ಕ m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        ಯದಿ(addr > LIMIT) ಪ್ರತ್ಯಾಗಮ;
        ಯದಿ(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        ಪ್ರತಿಜ್ಞಾ(cbi->mmap_size < MAXMEMMAP);
        ಯದಿ(len == 0) ಪ್ರತ್ಯಾಗಮ;
	addr = roundup(addr, I386_PAGE_SIZE);
	len = rounddown(len, I386_PAGE_SIZE);

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

        ಪ್ರತಿ(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                ಯದಿ(cbi->memmap[m].mm_length) ಅನುವರ್ತ;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_length = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                ಯದಿ(m >= cbi->mmap_size)
                        cbi->mmap_size = m+1;
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
#define PG_PAGETABLES 6
	ಸ್ಥಿರ u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pt_inuse = 0;
	ಯದಿ(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(pagetables[pt_inuse]) == I386_PAGE_SIZE);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ಪ್ರತ್ಯಾಗಮ ret;
}

#define PAGE_KB (I386_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	ಪೂರ್ಣಾಙ್ಕ m;
	multiboot_memory_map_t *mmap;

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಪ್ರತಿ(m = cbi->mmap_size-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		ಯದಿ(!mmap->mm_length) ಅನುವರ್ತ;
		ಪ್ರತಿಜ್ಞಾ(mmap->mm_length > 0);
		ಪ್ರತಿಜ್ಞಾ(!(mmap->mm_length % I386_PAGE_SIZE));
		ಪ್ರತಿಜ್ಞಾ(!(mmap->mm_base_addr % I386_PAGE_SIZE));

		mmap->mm_length -= I386_PAGE_SIZE;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_SIZE;

		ಪ್ರತ್ಯಾಗಮ mmap->mm_base_addr + mmap->mm_length;
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
        ಪ್ರತಿ(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_SIZE;
		ಯದಿ((cbi->mem_ಮೇಲಿನ_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

ಪೂರ್ಣಾಙ್ಕ pg_mapkernel(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        ಪ್ರತಿಜ್ಞಾ(!(kern_vir_start % I386_BIG_PAGE_SIZE));
        ಪ್ರತಿಜ್ಞಾ(!(kern_phys % I386_BIG_PAGE_SIZE));
        pde = kern_vir_start / I386_BIG_PAGE_SIZE; /* start pde */
	ಯಾವತ್(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_SIZE;
		kern_phys += I386_BIG_PAGE_SIZE;
		pde++;
	}
	ಪ್ರತ್ಯಾಗಮ pde;	/* free pde */
}

ಶೂನ್ಯ vm_enable_paging(ಶೂನ್ಯ)
{
        u32_t cr0, cr4;
        ಪೂರ್ಣಾಙ್ಕ pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	ಯದಿ(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	ಪ್ರತಿಜ್ಞಾ(cr0 & I386_CR0_PE);

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
        ಯದಿ(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(ಶೂನ್ಯ)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	ಪ್ರತ್ಯಾಗಮ phpagedir;
}

ಶೂನ್ಯ pg_clear(ಶೂನ್ಯ)
{
	memset(pagedir, 0, ಪರಿಮಾಣ(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	ಯದಿ(!(o = b % I386_PAGE_SIZE))
		ಪ್ರತ್ಯಾಗಮ b;
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
		ಪ್ರತಿಜ್ಞಾ(!(vaddr % I386_PAGE_SIZE));
	} ಅನ್ಯಥಾ  {
		ಪ್ರತಿಜ್ಞಾ((vaddr % I386_PAGE_SIZE) == (phys % I386_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	ಪ್ರತಿಜ್ಞಾ(vaddr < kern_vir_start);

	ಯಾವತ್(vaddr < vaddr_end) {
		phys_bytes source = phys;
		ಪ್ರತಿಜ್ಞಾ(!(vaddr % I386_PAGE_SIZE));
		ಯದಿ(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} ಅನ್ಯಥಾ {
			ಪ್ರತಿಜ್ಞಾ(!(phys % I386_PAGE_SIZE));
		}
		ಪ್ರತಿಜ್ಞಾ(!(source % I386_PAGE_SIZE));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		ಯದಿ(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		ಪ್ರತಿಜ್ಞಾ(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_SIZE;
		ಯದಿ(phys != PG_ALLOCATEME)
			phys += I386_PAGE_SIZE;
	}
}

ಶೂನ್ಯ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

