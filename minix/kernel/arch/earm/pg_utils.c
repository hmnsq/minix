#अन्तर्भाव <minix/cpufeature.h>

#अन्तर्भाव <minix/type.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <arm/armreg.h>

#अन्तर्भाव <string.h>
#अन्तर्भाव <minix/type.h>

/* These are set/computed in kernel.lds. */
बाह्य वर्ण _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
स्थिर phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
स्थिर phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
स्थिर phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
स्थिर u32_t pagedir[4096]  __aligned(16384);

शून्य print_memmap(kinfo_t *cbi)
{
	पूर्णाङ्क m;
	प्रतिज्ञा(cbi->mmap_size < MAXMEMMAP);
	प्रति(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
		म_लिखो("%08lx-%08lx ",addr, endit);
	}
	म_लिखो("\nsize %08lx\n", cbi->mmap_size);
}

शून्य cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
	पूर्णाङ्क m;
	phys_bytes o;

	यदि((o=start % ARM_PAGE_SIZE))
		start -= o;
	यदि((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

	प्रतिज्ञा(kernel_may_alloc);

	प्रति(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes substart = start, subend = end;
		phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
		    memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

		/* adjust cut range to be a subset of the free memory */
		यदि(substart < memaddr) substart = memaddr;
		यदि(subend > memend) subend = memend;
		यदि(substart >= subend) अनुवर्त;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
		यदि(substart > memaddr)
			add_memmap(cbi, memaddr, substart-memaddr);
		यदि(subend < memend)
			add_memmap(cbi, subend, memend-subend);
	}
}

शून्य add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	पूर्णाङ्क m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	यदि(addr > LIMIT) {
		प्रत्यागम;
	}

	यदि(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	प्रतिज्ञा(cbi->mmap_size < MAXMEMMAP);
	यदि(len == 0) {
		प्रत्यागम;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	प्रतिज्ञा(kernel_may_alloc);

        प्रति(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		यदि(cbi->memmap[m].mm_length) {
			अनुवर्त;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		यदि(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
		highmark = addr + len;
		यदि(highmark > cbi->mem_ऊपरी_phys) {
			cbi->mem_ऊपरी_phys = highmark;
		}
		प्रत्यागम;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 24
	स्थिर u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	स्थिर पूर्णाङ्क pt_inuse = 0;
	यदि(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	प्रतिज्ञा(परिमाण(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	प्रत्यागम ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	पूर्णाङ्क m;
	multiboot_memory_map_t *mmap;

	प्रतिज्ञा(kernel_may_alloc);

	प्रति(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		यदि(!mmap->mm_length) {
			अनुवर्त;
		}
		प्रतिज्ञा(mmap->mm_length > 0);
		प्रतिज्ञा(!(mmap->mm_length % ARM_PAGE_SIZE));
		प्रतिज्ञा(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		प्रत्यागम addr;
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
	प्रतिज्ञा(cbi->mem_ऊपरी_phys);

        /* Set up an identity mapping page directory */
	 प्रति(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		यदि (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} अन्यथा {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

पूर्णाङ्क pg_mapkernel(शून्य)
{
	पूर्णाङ्क pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	प्रतिज्ञा(!(kern_vir_start % ARM_SECTION_SIZE));
	प्रतिज्ञा(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	यावत्(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	प्रत्यागम pde;	/* free pde */
}

शून्य vm_enable_paging(शून्य)
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

phys_bytes pg_load(शून्य)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	प्रत्यागम phpagedir;
}

शून्य pg_clear(शून्य)
{
	स_रखो(pagedir, 0, परिमाण(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	यदि(!(o = b % ARM_PAGE_SIZE)) {
		प्रत्यागम b;
	}
	प्रत्यागम b  - o;
}

शून्य pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	स्थिर पूर्णाङ्क mapped_pde = -1;
	स्थिर u32_t *pt = रिक्त;
	पूर्णाङ्क pde, pte;

	प्रतिज्ञा(kernel_may_alloc);

	यदि(phys == PG_ALLOCATEME) {
		प्रतिज्ञा(!(vaddr % ARM_PAGE_SIZE));
	} अन्यथा  {
		प्रतिज्ञा((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	प्रतिज्ञा(vaddr < kern_vir_start);

	यावत्(vaddr < vaddr_end) {
		phys_bytes source = phys;
		प्रतिज्ञा(!(vaddr % ARM_PAGE_SIZE));
		यदि(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} अन्यथा {
			प्रतिज्ञा(!(phys % ARM_PAGE_SIZE));
		}
		प्रतिज्ञा(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		यदि(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		प्रतिज्ञा(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		यदि(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

शून्य pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
