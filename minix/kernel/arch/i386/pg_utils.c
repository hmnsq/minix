
#شامل <minix/cpufeature.h>

#شامل <assert.h>
#شامل "kernel/kernel.h"
#شامل "arch_proto.h"

#شامل <string.h>

/* These are set/computed in kernel.lds. */
بیرونی نویسه _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
ایستا phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
ایستا phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
ایستا phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
ایستا u32_t pagedir[1024]  __aligned(4096);

تهی print_memmap(kinfo_t *cbi)
{
        صحیح m;
        أكد(cbi->mmap_size < MAXMEMMAP);
        برای(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
                printf("%08lx-%08lx ",addr, endit);
        }
        printf("\nsize %08lx\n", cbi->mmap_size);
}

تهی cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
        صحیح m;
        phys_bytes o;

        اگر((o=start % I386_PAGE_SIZE))
                start -= o;
        اگر((o=end % I386_PAGE_SIZE))
                end += I386_PAGE_SIZE - o;

	أكد(kernel_may_alloc);

        برای(m = 0; m < cbi->mmap_size; m++) {
                phys_bytes substart = start, subend = end;
                phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
                        memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

                /* adjust cut range to be a subset of the free memory */
                اگر(substart < memaddr) substart = memaddr;
                اگر(subend > memend) subend = memend;
                اگر(substart >= subend) ادامه;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
                اگر(substart > memaddr)
                        add_memmap(cbi, memaddr, substart-memaddr);
                اگر(subend < memend)
                        add_memmap(cbi, subend, memend-subend);
        }
}

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	صحیح m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	أكد(len > 0);
	len = roundup(len, I386_PAGE_SIZE);

	أكد(kernel_may_alloc);

	برای(m = 0; m < cbi->mmap_size; m++) {
		اگر(cbi->memmap[m].mm_length < len) ادامه;
		اگر(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	أكد(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	بازگشت lowest;
}

تهی add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        صحیح m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        اگر(addr > LIMIT) بازگشت;
        اگر(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        أكد(cbi->mmap_size < MAXMEMMAP);
        اگر(len == 0) بازگشت;
	addr = roundup(addr, I386_PAGE_SIZE);
	len = rounddown(len, I386_PAGE_SIZE);

	أكد(kernel_may_alloc);

        برای(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                اگر(cbi->memmap[m].mm_length) ادامه;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_length = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                اگر(m >= cbi->mmap_size)
                        cbi->mmap_size = m+1;
		highmark = addr + len;
		اگر(highmark > cbi->mem_بالا_phys) {
			cbi->mem_بالا_phys = highmark;
		}

                بازگشت;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 6
	ایستا u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ایستا صحیح pt_inuse = 0;
	اگر(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	أكد(اندازه(pagetables[pt_inuse]) == I386_PAGE_SIZE);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	بازگشت ret;
}

#define PAGE_KB (I386_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	صحیح m;
	multiboot_memory_map_t *mmap;

	أكد(kernel_may_alloc);

	برای(m = cbi->mmap_size-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		اگر(!mmap->mm_length) ادامه;
		أكد(mmap->mm_length > 0);
		أكد(!(mmap->mm_length % I386_PAGE_SIZE));
		أكد(!(mmap->mm_base_addr % I386_PAGE_SIZE));

		mmap->mm_length -= I386_PAGE_SIZE;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_SIZE;

		بازگشت mmap->mm_base_addr + mmap->mm_length;
	}

	panic("can't find free memory");
}

تهی pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	أكد(cbi->mem_بالا_phys);

        /* Set up an identity mapping page directory */
        برای(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_SIZE;
		اگر((cbi->mem_بالا_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

صحیح pg_mapkernel(تهی)
{
	صحیح pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        أكد(!(kern_vir_start % I386_BIG_PAGE_SIZE));
        أكد(!(kern_phys % I386_BIG_PAGE_SIZE));
        pde = kern_vir_start / I386_BIG_PAGE_SIZE; /* start pde */
	وقتی(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_SIZE;
		kern_phys += I386_BIG_PAGE_SIZE;
		pde++;
	}
	بازگشت pde;	/* free pde */
}

تهی vm_enable_paging(تهی)
{
        u32_t cr0, cr4;
        صحیح pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	اگر(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	أكد(cr0 & I386_CR0_PE);

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
        اگر(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(تهی)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	بازگشت phpagedir;
}

تهی pg_clear(تهی)
{
	memset(pagedir, 0, اندازه(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	اگر(!(o = b % I386_PAGE_SIZE))
		بازگشت b;
	بازگشت b  - o;
}

تهی pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	ایستا صحیح mapped_pde = -1;
	ایستا u32_t *pt = پوچ;
	صحیح pde, pte;

	أكد(kernel_may_alloc);

	اگر(phys == PG_ALLOCATEME) {
		أكد(!(vaddr % I386_PAGE_SIZE));
	} وگرنه  {
		أكد((vaddr % I386_PAGE_SIZE) == (phys % I386_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	أكد(vaddr < kern_vir_start);

	وقتی(vaddr < vaddr_end) {
		phys_bytes source = phys;
		أكد(!(vaddr % I386_PAGE_SIZE));
		اگر(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} وگرنه {
			أكد(!(phys % I386_PAGE_SIZE));
		}
		أكد(!(source % I386_PAGE_SIZE));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		اگر(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_SIZE;
		اگر(phys != PG_ALLOCATEME)
			phys += I386_PAGE_SIZE;
	}
}

تهی pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

