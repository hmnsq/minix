#അന്തര്ഭാവ <minix/cpufeature.h>

#അന്തര്ഭാവ <minix/type.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_proto.h"
#അന്തര്ഭാവ <machine/cpu.h>
#അന്തര്ഭാവ <arm/armreg.h>

#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <minix/type.h>

/* These are set/computed in kernel.lds. */
ബാഹ്യ അക്ഷരം _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
സ്ഥിതം phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
സ്ഥിതം phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
സ്ഥിതം phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
സ്ഥിതം u32_t pagedir[4096]  __aligned(16384);

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

	എങ്കിൽ((o=start % ARM_PAGE_SIZE))
		start -= o;
	എങ്കിൽ((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

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

ശൂന്യം add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	പൂർണ്ണസംഖ്യ m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	എങ്കിൽ(addr > LIMIT) {
		തിരികെ;
	}

	എങ്കിൽ(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	പ്രതിജ്ഞാ(cbi->mmap_size < MAXMEMMAP);
	എങ്കിൽ(len == 0) {
		തിരികെ;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	പ്രതിജ്ഞാ(kernel_may_alloc);

        വേണ്ടി(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		എങ്കിൽ(cbi->memmap[m].mm_length) {
			തുടരുക;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		എങ്കിൽ(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
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
#define PG_PAGETABLES 24
	സ്ഥിതം u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	സ്ഥിതം പൂർണ്ണസംഖ്യ pt_inuse = 0;
	എങ്കിൽ(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	പ്രതിജ്ഞാ(വലുപ്പം(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	തിരികെ ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	പൂർണ്ണസംഖ്യ m;
	multiboot_memory_map_t *mmap;

	പ്രതിജ്ഞാ(kernel_may_alloc);

	വേണ്ടി(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		എങ്കിൽ(!mmap->mm_length) {
			തുടരുക;
		}
		പ്രതിജ്ഞാ(mmap->mm_length > 0);
		പ്രതിജ്ഞാ(!(mmap->mm_length % ARM_PAGE_SIZE));
		പ്രതിജ്ഞാ(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		തിരികെ addr;
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
	 വേണ്ടി(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		എങ്കിൽ (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} അല്ലെങ്കിൽ {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

പൂർണ്ണസംഖ്യ pg_mapkernel(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	പ്രതിജ്ഞാ(!(kern_vir_start % ARM_SECTION_SIZE));
	പ്രതിജ്ഞാ(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	ആകുമ്പോൾ(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	തിരികെ pde;	/* free pde */
}

ശൂന്യം vm_enable_paging(ശൂന്യം)
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

phys_bytes pg_load(ശൂന്യം)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	തിരികെ phpagedir;
}

ശൂന്യം pg_clear(ശൂന്യം)
{
	memset(pagedir, 0, വലുപ്പം(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	എങ്കിൽ(!(o = b % ARM_PAGE_SIZE)) {
		തിരികെ b;
	}
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
		പ്രതിജ്ഞാ(!(vaddr % ARM_PAGE_SIZE));
	} അല്ലെങ്കിൽ  {
		പ്രതിജ്ഞാ((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	പ്രതിജ്ഞാ(vaddr < kern_vir_start);

	ആകുമ്പോൾ(vaddr < vaddr_end) {
		phys_bytes source = phys;
		പ്രതിജ്ഞാ(!(vaddr % ARM_PAGE_SIZE));
		എങ്കിൽ(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} അല്ലെങ്കിൽ {
			പ്രതിജ്ഞാ(!(phys % ARM_PAGE_SIZE));
		}
		പ്രതിജ്ഞാ(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		എങ്കിൽ(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		പ്രതിജ്ഞാ(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		എങ്കിൽ(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

ശൂന്യം pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
