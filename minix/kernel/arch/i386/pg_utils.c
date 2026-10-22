
#അന്തര്ഭാവ <minix/cpufeature.h>

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_proto.h"

#അന്തര്ഭാവ <string.h>

/* These are set/computed in kernel.lds. */
ബാഹ്യ അക്ഷരം _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
സ്ഥിതം phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
സ്ഥിതം phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
സ്ഥിതം phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
സ്ഥിതം u32_t pagedir[1024]  __aligned(4096);

ശൂന്യം print_memmap(kinfo_t *cbi)
{
        പൂർണ്ണസംഖ്യ m;
        പ്രതിജ്ഞാ(cbi->mmap_size < MAXMEMMAP);
        വേണ്ടി(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
                printf("%08lx-%08lx ",addr, endit);
        }
        printf("\nsize %08lx\n", cbi->mmap_size);
}

ശൂന്യം cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
        പൂർണ്ണസംഖ്യ m;
        phys_bytes o;

        എങ്കിൽ((o=start % I386_PAGE_SIZE))
                start -= o;
        എങ്കിൽ((o=end % I386_PAGE_SIZE))
                end += I386_PAGE_SIZE - o;

	പ്രതിജ്ഞാ(kernel_may_alloc);

        വേണ്ടി(m = 0; m < cbi->mmap_size; m++) {
                phys_bytes substart = start, subend = end;
                phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
                        memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

                /* adjust cut range to be a subset of the free memory */
                എങ്കിൽ(substart < memaddr) substart = memaddr;
                എങ്കിൽ(subend > memend) subend = memend;
                എങ്കിൽ(substart >= subend) തുടരുക;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
                എങ്കിൽ(substart > memaddr)
                        add_memmap(cbi, memaddr, substart-memaddr);
                എങ്കിൽ(subend < memend)
                        add_memmap(cbi, subend, memend-subend);
        }
}

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	പൂർണ്ണസംഖ്യ m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	പ്രതിജ്ഞാ(len > 0);
	len = roundup(len, I386_PAGE_SIZE);

	പ്രതിജ്ഞാ(kernel_may_alloc);

	വേണ്ടി(m = 0; m < cbi->mmap_size; m++) {
		എങ്കിൽ(cbi->memmap[m].mm_length < len) തുടരുക;
		എങ്കിൽ(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	പ്രതിജ്ഞാ(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	തിരികെ lowest;
}

ശൂന്യം add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        പൂർണ്ണസംഖ്യ m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        എങ്കിൽ(addr > LIMIT) തിരികെ;
        എങ്കിൽ(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        പ്രതിജ്ഞാ(cbi->mmap_size < MAXMEMMAP);
        എങ്കിൽ(len == 0) തിരികെ;
	addr = roundup(addr, I386_PAGE_SIZE);
	len = rounddown(len, I386_PAGE_SIZE);

	പ്രതിജ്ഞാ(kernel_may_alloc);

        വേണ്ടി(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                എങ്കിൽ(cbi->memmap[m].mm_length) തുടരുക;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_length = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                എങ്കിൽ(m >= cbi->mmap_size)
                        cbi->mmap_size = m+1;
		highmark = addr + len;
		എങ്കിൽ(highmark > cbi->mem_മുകളിലെ_phys) {
			cbi->mem_മുകളിലെ_phys = highmark;
		}

                തിരികെ;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 6
	സ്ഥിതം u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	സ്ഥിതം പൂർണ്ണസംഖ്യ pt_inuse = 0;
	എങ്കിൽ(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	പ്രതിജ്ഞാ(വലുപ്പം(pagetables[pt_inuse]) == I386_PAGE_SIZE);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	തിരികെ ret;
}

#define PAGE_KB (I386_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	പൂർണ്ണസംഖ്യ m;
	multiboot_memory_map_t *mmap;

	പ്രതിജ്ഞാ(kernel_may_alloc);

	വേണ്ടി(m = cbi->mmap_size-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		എങ്കിൽ(!mmap->mm_length) തുടരുക;
		പ്രതിജ്ഞാ(mmap->mm_length > 0);
		പ്രതിജ്ഞാ(!(mmap->mm_length % I386_PAGE_SIZE));
		പ്രതിജ്ഞാ(!(mmap->mm_base_addr % I386_PAGE_SIZE));

		mmap->mm_length -= I386_PAGE_SIZE;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_SIZE;

		തിരികെ mmap->mm_base_addr + mmap->mm_length;
	}

	panic("can't find free memory");
}

ശൂന്യം pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	പ്രതിജ്ഞാ(cbi->mem_മുകളിലെ_phys);

        /* Set up an identity mapping page directory */
        വേണ്ടി(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_SIZE;
		എങ്കിൽ((cbi->mem_മുകളിലെ_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

പൂർണ്ണസംഖ്യ pg_mapkernel(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        പ്രതിജ്ഞാ(!(kern_vir_start % I386_BIG_PAGE_SIZE));
        പ്രതിജ്ഞാ(!(kern_phys % I386_BIG_PAGE_SIZE));
        pde = kern_vir_start / I386_BIG_PAGE_SIZE; /* start pde */
	ആകുമ്പോൾ(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_SIZE;
		kern_phys += I386_BIG_PAGE_SIZE;
		pde++;
	}
	തിരികെ pde;	/* free pde */
}

ശൂന്യം vm_enable_paging(ശൂന്യം)
{
        u32_t cr0, cr4;
        പൂർണ്ണസംഖ്യ pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	എങ്കിൽ(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	പ്രതിജ്ഞാ(cr0 & I386_CR0_PE);

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
        എങ്കിൽ(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(ശൂന്യം)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	തിരികെ phpagedir;
}

ശൂന്യം pg_clear(ശൂന്യം)
{
	memset(pagedir, 0, വലുപ്പം(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	എങ്കിൽ(!(o = b % I386_PAGE_SIZE))
		തിരികെ b;
	തിരികെ b  - o;
}

ശൂന്യം pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	സ്ഥിതം പൂർണ്ണസംഖ്യ mapped_pde = -1;
	സ്ഥിതം u32_t *pt = ശൂന്യ;
	പൂർണ്ണസംഖ്യ pde, pte;

	പ്രതിജ്ഞാ(kernel_may_alloc);

	എങ്കിൽ(phys == PG_ALLOCATEME) {
		പ്രതിജ്ഞാ(!(vaddr % I386_PAGE_SIZE));
	} അല്ലെങ്കിൽ  {
		പ്രതിജ്ഞാ((vaddr % I386_PAGE_SIZE) == (phys % I386_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	പ്രതിജ്ഞാ(vaddr < kern_vir_start);

	ആകുമ്പോൾ(vaddr < vaddr_end) {
		phys_bytes source = phys;
		പ്രതിജ്ഞാ(!(vaddr % I386_PAGE_SIZE));
		എങ്കിൽ(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} അല്ലെങ്കിൽ {
			പ്രതിജ്ഞാ(!(phys % I386_PAGE_SIZE));
		}
		പ്രതിജ്ഞാ(!(source % I386_PAGE_SIZE));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		എങ്കിൽ(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		പ്രതിജ്ഞാ(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_SIZE;
		എങ്കിൽ(phys != PG_ALLOCATEME)
			phys += I386_PAGE_SIZE;
	}
}

ശൂന്യം pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

