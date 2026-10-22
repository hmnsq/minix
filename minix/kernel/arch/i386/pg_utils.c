
#અન્તર્ભાવ <minix/cpufeature.h>

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_proto.h"

#અન્તર્ભાવ <string.h>

/* These are set/computed in kernel.lds. */
બાહ્ય અક્ષર _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
સ્થિર phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
સ્થિર phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
સ્થિર phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
સ્થિર u32_t pagedir[1024]  __aligned(4096);

શૂન્ય print_memmap(kinfo_t *cbi)
{
        પૂર્ણાંક m;
        પ્રતિજ્ઞા(cbi->mmap_size < MAXMEMMAP);
        માટે(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
                printf("%08lx-%08lx ",addr, endit);
        }
        printf("\nsize %08lx\n", cbi->mmap_size);
}

શૂન્ય cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
        પૂર્ણાંક m;
        phys_bytes o;

        જો((o=start % I386_PAGE_SIZE))
                start -= o;
        જો((o=end % I386_PAGE_SIZE))
                end += I386_PAGE_SIZE - o;

	પ્રતિજ્ઞા(kernel_may_alloc);

        માટે(m = 0; m < cbi->mmap_size; m++) {
                phys_bytes substart = start, subend = end;
                phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
                        memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

                /* adjust cut range to be a subset of the free memory */
                જો(substart < memaddr) substart = memaddr;
                જો(subend > memend) subend = memend;
                જો(substart >= subend) ચાલુ;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
                જો(substart > memaddr)
                        add_memmap(cbi, memaddr, substart-memaddr);
                જો(subend < memend)
                        add_memmap(cbi, subend, memend-subend);
        }
}

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	પૂર્ણાંક m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	પ્રતિજ્ઞા(len > 0);
	len = roundup(len, I386_PAGE_SIZE);

	પ્રતિજ્ઞા(kernel_may_alloc);

	માટે(m = 0; m < cbi->mmap_size; m++) {
		જો(cbi->memmap[m].mm_length < len) ચાલુ;
		જો(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	પ્રતિજ્ઞા(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	પાછા lowest;
}

શૂન્ય add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        પૂર્ણાંક m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        જો(addr > LIMIT) પાછા;
        જો(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        પ્રતિજ્ઞા(cbi->mmap_size < MAXMEMMAP);
        જો(len == 0) પાછા;
	addr = roundup(addr, I386_PAGE_SIZE);
	len = rounddown(len, I386_PAGE_SIZE);

	પ્રતિજ્ઞા(kernel_may_alloc);

        માટે(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                જો(cbi->memmap[m].mm_length) ચાલુ;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_length = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                જો(m >= cbi->mmap_size)
                        cbi->mmap_size = m+1;
		highmark = addr + len;
		જો(highmark > cbi->mem_ઉપલું_phys) {
			cbi->mem_ઉપલું_phys = highmark;
		}

                પાછા;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 6
	સ્થિર u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	સ્થિર પૂર્ણાંક pt_inuse = 0;
	જો(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	પ્રતિજ્ઞા(કદ(pagetables[pt_inuse]) == I386_PAGE_SIZE);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	પાછા ret;
}

#define PAGE_KB (I386_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	પૂર્ણાંક m;
	multiboot_memory_map_t *mmap;

	પ્રતિજ્ઞા(kernel_may_alloc);

	માટે(m = cbi->mmap_size-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		જો(!mmap->mm_length) ચાલુ;
		પ્રતિજ્ઞા(mmap->mm_length > 0);
		પ્રતિજ્ઞા(!(mmap->mm_length % I386_PAGE_SIZE));
		પ્રતિજ્ઞા(!(mmap->mm_base_addr % I386_PAGE_SIZE));

		mmap->mm_length -= I386_PAGE_SIZE;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_SIZE;

		પાછા mmap->mm_base_addr + mmap->mm_length;
	}

	panic("can't find free memory");
}

શૂન્ય pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	પ્રતિજ્ઞા(cbi->mem_ઉપલું_phys);

        /* Set up an identity mapping page directory */
        માટે(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_SIZE;
		જો((cbi->mem_ઉપલું_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

પૂર્ણાંક pg_mapkernel(શૂન્ય)
{
	પૂર્ણાંક pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        પ્રતિજ્ઞા(!(kern_vir_start % I386_BIG_PAGE_SIZE));
        પ્રતિજ્ઞા(!(kern_phys % I386_BIG_PAGE_SIZE));
        pde = kern_vir_start / I386_BIG_PAGE_SIZE; /* start pde */
	જ્યારે(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_SIZE;
		kern_phys += I386_BIG_PAGE_SIZE;
		pde++;
	}
	પાછા pde;	/* free pde */
}

શૂન્ય vm_enable_paging(શૂન્ય)
{
        u32_t cr0, cr4;
        પૂર્ણાંક pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	જો(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	પ્રતિજ્ઞા(cr0 & I386_CR0_PE);

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
        જો(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(શૂન્ય)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	પાછા phpagedir;
}

શૂન્ય pg_clear(શૂન્ય)
{
	memset(pagedir, 0, કદ(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	જો(!(o = b % I386_PAGE_SIZE))
		પાછા b;
	પાછા b  - o;
}

શૂન્ય pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	સ્થિર પૂર્ણાંક mapped_pde = -1;
	સ્થિર u32_t *pt = ખાલી;
	પૂર્ણાંક pde, pte;

	પ્રતિજ્ઞા(kernel_may_alloc);

	જો(phys == PG_ALLOCATEME) {
		પ્રતિજ્ઞા(!(vaddr % I386_PAGE_SIZE));
	} નહીંતર  {
		પ્રતિજ્ઞા((vaddr % I386_PAGE_SIZE) == (phys % I386_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	પ્રતિજ્ઞા(vaddr < kern_vir_start);

	જ્યારે(vaddr < vaddr_end) {
		phys_bytes source = phys;
		પ્રતિજ્ઞા(!(vaddr % I386_PAGE_SIZE));
		જો(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} નહીંતર {
			પ્રતિજ્ઞા(!(phys % I386_PAGE_SIZE));
		}
		પ્રતિજ્ઞા(!(source % I386_PAGE_SIZE));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		જો(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		પ્રતિજ્ઞા(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_SIZE;
		જો(phys != PG_ALLOCATEME)
			phys += I386_PAGE_SIZE;
	}
}

શૂન્ય pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

