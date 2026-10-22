#அந்தர்பாவ <minix/cpufeature.h>

#அந்தர்பாவ <minix/type.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "arch_proto.h"
#அந்தர்பாவ <machine/cpu.h>
#அந்தர்பாவ <arm/armreg.h>

#அந்தர்பாவ <string.h>
#அந்தர்பாவ <minix/type.h>

/* These are set/computed in kernel.lds. */
வெளி எழுத்து _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
நிலையான phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
நிலையான phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
நிலையான phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
நிலையான u32_t pagedir[4096]  __aligned(16384);

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

	எனில்((o=start % ARM_PAGE_SIZE))
		start -= o;
	எனில்((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

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

வெறுமை add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	முழு m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	எனில்(addr > LIMIT) {
		திரும்பு;
	}

	எனில்(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	ப்ரதிஜ்ஞா(cbi->mmap_size < MAXMEMMAP);
	எனில்(len == 0) {
		திரும்பு;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	ப்ரதிஜ்ஞா(kernel_may_alloc);

        க்கு(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		எனில்(cbi->memmap[m].mm_length) {
			தொடர்;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		எனில்(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
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
#define PG_PAGETABLES 24
	நிலையான u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	நிலையான முழு pt_inuse = 0;
	எனில்(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	ப்ரதிஜ்ஞா(அளவு(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	திரும்பு ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	முழு m;
	multiboot_memory_map_t *mmap;

	ப்ரதிஜ்ஞா(kernel_may_alloc);

	க்கு(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		எனில்(!mmap->mm_length) {
			தொடர்;
		}
		ப்ரதிஜ்ஞா(mmap->mm_length > 0);
		ப்ரதிஜ்ஞா(!(mmap->mm_length % ARM_PAGE_SIZE));
		ப்ரதிஜ்ஞா(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		திரும்பு addr;
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
	 க்கு(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		எனில் (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} இல்லையெனில் {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

முழு pg_mapkernel(வெறுமை)
{
	முழு pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	ப்ரதிஜ்ஞா(!(kern_vir_start % ARM_SECTION_SIZE));
	ப்ரதிஜ்ஞா(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	போது(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	திரும்பு pde;	/* free pde */
}

வெறுமை vm_enable_paging(வெறுமை)
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
	#define CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_bytes pg_load(வெறுமை)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	திரும்பு phpagedir;
}

வெறுமை pg_clear(வெறுமை)
{
	memset(pagedir, 0, அளவு(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	எனில்(!(o = b % ARM_PAGE_SIZE)) {
		திரும்பு b;
	}
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
		ப்ரதிஜ்ஞா(!(vaddr % ARM_PAGE_SIZE));
	} இல்லையெனில்  {
		ப்ரதிஜ்ஞா((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	ப்ரதிஜ்ஞா(vaddr < kern_vir_start);

	போது(vaddr < vaddr_end) {
		phys_bytes source = phys;
		ப்ரதிஜ்ஞா(!(vaddr % ARM_PAGE_SIZE));
		எனில்(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} இல்லையெனில் {
			ப்ரதிஜ்ஞா(!(phys % ARM_PAGE_SIZE));
		}
		ப்ரதிஜ்ஞா(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		எனில்(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		ப்ரதிஜ்ஞா(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		எனில்(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

வெறுமை pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
