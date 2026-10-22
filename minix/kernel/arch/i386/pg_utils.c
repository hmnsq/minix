
#அந்தர்பாவ <minix/cpufeature.h>

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "arch_proto.h"

#அந்தர்பாவ <string.h>

/* These are set/computed in kernel.lds. */
வெளி எழுத்து _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
நிலையான phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
நிலையான phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
நிலையான phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
நிலையான u32_t pagedir[1024]  __aligned(4096);

வெறுமை print_memmap(kinfo_t *cbi)
{
        முழு m;
        ப்ரதிஜ்ஞா(cbi->mmap_size < MAXMEMMAP);
        க்கு(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
                printf("%08lx-%08lx ",addr, endit);
        }
        printf("\nsize %08lx\n", cbi->mmap_size);
}

வெறுமை cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
        முழு m;
        phys_bytes o;

        எனில்((o=start % I386_PAGE_SIZE))
                start -= o;
        எனில்((o=end % I386_PAGE_SIZE))
                end += I386_PAGE_SIZE - o;

	ப்ரதிஜ்ஞா(kernel_may_alloc);

        க்கு(m = 0; m < cbi->mmap_size; m++) {
                phys_bytes substart = start, subend = end;
                phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
                        memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

                /* adjust cut range to be a subset of the free memory */
                எனில்(substart < memaddr) substart = memaddr;
                எனில்(subend > memend) subend = memend;
                எனில்(substart >= subend) தொடர்;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
                எனில்(substart > memaddr)
                        add_memmap(cbi, memaddr, substart-memaddr);
                எனில்(subend < memend)
                        add_memmap(cbi, subend, memend-subend);
        }
}

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	முழு m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	ப்ரதிஜ்ஞா(len > 0);
	len = roundup(len, I386_PAGE_SIZE);

	ப்ரதிஜ்ஞா(kernel_may_alloc);

	க்கு(m = 0; m < cbi->mmap_size; m++) {
		எனில்(cbi->memmap[m].mm_length < len) தொடர்;
		எனில்(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	ப்ரதிஜ்ஞா(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	திரும்பு lowest;
}

வெறுமை add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        முழு m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        எனில்(addr > LIMIT) திரும்பு;
        எனில்(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        ப்ரதிஜ்ஞா(cbi->mmap_size < MAXMEMMAP);
        எனில்(len == 0) திரும்பு;
	addr = roundup(addr, I386_PAGE_SIZE);
	len = rounddown(len, I386_PAGE_SIZE);

	ப்ரதிஜ்ஞா(kernel_may_alloc);

        க்கு(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                எனில்(cbi->memmap[m].mm_length) தொடர்;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_length = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                எனில்(m >= cbi->mmap_size)
                        cbi->mmap_size = m+1;
		highmark = addr + len;
		எனில்(highmark > cbi->mem_உயர்வு_phys) {
			cbi->mem_உயர்வு_phys = highmark;
		}

                திரும்பு;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 6
	நிலையான u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	நிலையான முழு pt_inuse = 0;
	எனில்(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	ப்ரதிஜ்ஞா(அளவு(pagetables[pt_inuse]) == I386_PAGE_SIZE);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	திரும்பு ret;
}

#define PAGE_KB (I386_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	முழு m;
	multiboot_memory_map_t *mmap;

	ப்ரதிஜ்ஞா(kernel_may_alloc);

	க்கு(m = cbi->mmap_size-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		எனில்(!mmap->mm_length) தொடர்;
		ப்ரதிஜ்ஞா(mmap->mm_length > 0);
		ப்ரதிஜ்ஞா(!(mmap->mm_length % I386_PAGE_SIZE));
		ப்ரதிஜ்ஞா(!(mmap->mm_base_addr % I386_PAGE_SIZE));

		mmap->mm_length -= I386_PAGE_SIZE;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_SIZE;

		திரும்பு mmap->mm_base_addr + mmap->mm_length;
	}

	panic("can't find free memory");
}

வெறுமை pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	ப்ரதிஜ்ஞா(cbi->mem_உயர்வு_phys);

        /* Set up an identity mapping page directory */
        க்கு(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_SIZE;
		எனில்((cbi->mem_உயர்வு_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

முழு pg_mapkernel(வெறுமை)
{
	முழு pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        ப்ரதிஜ்ஞா(!(kern_vir_start % I386_BIG_PAGE_SIZE));
        ப்ரதிஜ்ஞா(!(kern_phys % I386_BIG_PAGE_SIZE));
        pde = kern_vir_start / I386_BIG_PAGE_SIZE; /* start pde */
	போது(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_SIZE;
		kern_phys += I386_BIG_PAGE_SIZE;
		pde++;
	}
	திரும்பு pde;	/* free pde */
}

வெறுமை vm_enable_paging(வெறுமை)
{
        u32_t cr0, cr4;
        முழு pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	எனில்(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	ப்ரதிஜ்ஞா(cr0 & I386_CR0_PE);

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
        எனில்(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(வெறுமை)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	திரும்பு phpagedir;
}

வெறுமை pg_clear(வெறுமை)
{
	memset(pagedir, 0, அளவு(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	எனில்(!(o = b % I386_PAGE_SIZE))
		திரும்பு b;
	திரும்பு b  - o;
}

வெறுமை pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	நிலையான முழு mapped_pde = -1;
	நிலையான u32_t *pt = வெற்று;
	முழு pde, pte;

	ப்ரதிஜ்ஞா(kernel_may_alloc);

	எனில்(phys == PG_ALLOCATEME) {
		ப்ரதிஜ்ஞா(!(vaddr % I386_PAGE_SIZE));
	} இல்லையெனில்  {
		ப்ரதிஜ்ஞா((vaddr % I386_PAGE_SIZE) == (phys % I386_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	ப்ரதிஜ்ஞா(vaddr < kern_vir_start);

	போது(vaddr < vaddr_end) {
		phys_bytes source = phys;
		ப்ரதிஜ்ஞா(!(vaddr % I386_PAGE_SIZE));
		எனில்(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} இல்லையெனில் {
			ப்ரதிஜ்ஞா(!(phys % I386_PAGE_SIZE));
		}
		ப்ரதிஜ்ஞா(!(source % I386_PAGE_SIZE));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		எனில்(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		ப்ரதிஜ்ஞா(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_SIZE;
		எனில்(phys != PG_ALLOCATEME)
			phys += I386_PAGE_SIZE;
	}
}

வெறுமை pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

