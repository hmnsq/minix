#অন্তর্ভাৱ <minix/cpufeature.h>

#অন্তর্ভাৱ <minix/type.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ "kernel/kernel.h"
#অন্তর্ভাৱ "arch_proto.h"
#অন্তর্ভাৱ <machine/cpu.h>
#অন্তর্ভাৱ <arm/armreg.h>

#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <minix/type.h>

/* These are set/computed in kernel.lds. */
বাহ্য ৱর্ণ _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
স্থির phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
স্থির phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
স্থির phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
স্থির u32_t pagedir[4096]  __aligned(16384);

শূন্য print_memmap(kinfo_t *cbi)
{
	পূর্ণাঙ্ক m;
	প্রতিজ্ঞা(cbi->mmap_size < MAXMEMMAP);
	বাবে(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
		printf("%08lx-%08lx ",addr, endit);
	}
	printf("\nsize %08lx\n", cbi->mmap_size);
}

শূন্য cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
	পূর্ণাঙ্ক m;
	phys_bytes o;

	যদি((o=start % ARM_PAGE_SIZE))
		start -= o;
	যদি((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

	প্রতিজ্ঞা(kernel_may_alloc);

	বাবে(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes substart = start, subend = end;
		phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
		    memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

		/* adjust cut range to be a subset of the free memory */
		যদি(substart < memaddr) substart = memaddr;
		যদি(subend > memend) subend = memend;
		যদি(substart >= subend) অনুৱর্ত;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
		যদি(substart > memaddr)
			add_memmap(cbi, memaddr, substart-memaddr);
		যদি(subend < memend)
			add_memmap(cbi, subend, memend-subend);
	}
}

শূন্য add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	পূর্ণাঙ্ক m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	যদি(addr > LIMIT) {
		ঘূৰাই;
	}

	যদি(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	প্রতিজ্ঞা(cbi->mmap_size < MAXMEMMAP);
	যদি(len == 0) {
		ঘূৰাই;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	প্রতিজ্ঞা(kernel_may_alloc);

        বাবে(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		যদি(cbi->memmap[m].mm_length) {
			অনুৱর্ত;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		যদি(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
		highmark = addr + len;
		যদি(highmark > cbi->mem_উপৰেৰ_phys) {
			cbi->mem_উপৰেৰ_phys = highmark;
		}
		ঘূৰাই;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 24
	স্থির u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	স্থির পূর্ণাঙ্ক pt_inuse = 0;
	যদি(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	প্রতিজ্ঞা(পরিমাণ(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ঘূৰাই ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	পূর্ণাঙ্ক m;
	multiboot_memory_map_t *mmap;

	প্রতিজ্ঞা(kernel_may_alloc);

	বাবে(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		যদি(!mmap->mm_length) {
			অনুৱর্ত;
		}
		প্রতিজ্ঞা(mmap->mm_length > 0);
		প্রতিজ্ঞা(!(mmap->mm_length % ARM_PAGE_SIZE));
		প্রতিজ্ঞা(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		ঘূৰাই addr;
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
	প্রতিজ্ঞা(cbi->mem_উপৰেৰ_phys);

        /* Set up an identity mapping page directory */
	 বাবে(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		যদি (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} অন্যথা {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

পূর্ণাঙ্ক pg_mapkernel(শূন্য)
{
	পূর্ণাঙ্ক pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	প্রতিজ্ঞা(!(kern_vir_start % ARM_SECTION_SIZE));
	প্রতিজ্ঞা(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	যেতিয়ালৈ(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	ঘূৰাই pde;	/* free pde */
}

শূন্য vm_enable_paging(শূন্য)
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

phys_bytes pg_load(শূন্য)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	ঘূৰাই phpagedir;
}

শূন্য pg_clear(শূন্য)
{
	memset(pagedir, 0, পরিমাণ(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	যদি(!(o = b % ARM_PAGE_SIZE)) {
		ঘূৰাই b;
	}
	ঘূৰাই b  - o;
}

শূন্য pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	স্থির পূর্ণাঙ্ক mapped_pde = -1;
	স্থির u32_t *pt = রিক্ত;
	পূর্ণাঙ্ক pde, pte;

	প্রতিজ্ঞা(kernel_may_alloc);

	যদি(phys == PG_ALLOCATEME) {
		প্রতিজ্ঞা(!(vaddr % ARM_PAGE_SIZE));
	} অন্যথা  {
		প্রতিজ্ঞা((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	প্রতিজ্ঞা(vaddr < kern_vir_start);

	যেতিয়ালৈ(vaddr < vaddr_end) {
		phys_bytes source = phys;
		প্রতিজ্ঞা(!(vaddr % ARM_PAGE_SIZE));
		যদি(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} অন্যথা {
			প্রতিজ্ঞা(!(phys % ARM_PAGE_SIZE));
		}
		প্রতিজ্ঞা(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		যদি(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		প্রতিজ্ঞা(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		যদি(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

শূন্য pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
