#અન્તર્ભાવ <minix/cpufeature.h>

#અન્તર્ભાવ <minix/type.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ <machine/cpu.h>
#અન્તર્ભાવ <arm/armreg.h>

#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <minix/type.h>

/* These are set/computed in kernel.lds. */
બાહ્ય અક્ષર _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
સ્થિર phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
સ્થિર phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
સ્થિર phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
સ્થિર u32_t pagedir[4096]  __aligned(16384);

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

	જો((o=start % ARM_PAGE_SIZE))
		start -= o;
	જો((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

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

શૂન્ય add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	પૂર્ણાંક m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	જો(addr > LIMIT) {
		પાછા;
	}

	જો(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	પ્રતિજ્ઞા(cbi->mmap_size < MAXMEMMAP);
	જો(len == 0) {
		પાછા;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	પ્રતિજ્ઞા(kernel_may_alloc);

        માટે(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		જો(cbi->memmap[m].mm_length) {
			ચાલુ;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		જો(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
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
#define PG_PAGETABLES 24
	સ્થિર u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	સ્થિર પૂર્ણાંક pt_inuse = 0;
	જો(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	પ્રતિજ્ઞા(કદ(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	પાછા ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	પૂર્ણાંક m;
	multiboot_memory_map_t *mmap;

	પ્રતિજ્ઞા(kernel_may_alloc);

	માટે(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		જો(!mmap->mm_length) {
			ચાલુ;
		}
		પ્રતિજ્ઞા(mmap->mm_length > 0);
		પ્રતિજ્ઞા(!(mmap->mm_length % ARM_PAGE_SIZE));
		પ્રતિજ્ઞા(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		પાછા addr;
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
	 માટે(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		જો (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} નહીંતર {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

પૂર્ણાંક pg_mapkernel(શૂન્ય)
{
	પૂર્ણાંક pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	પ્રતિજ્ઞા(!(kern_vir_start % ARM_SECTION_SIZE));
	પ્રતિજ્ઞા(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	જ્યારે(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	પાછા pde;	/* free pde */
}

શૂન્ય vm_enable_paging(શૂન્ય)
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

phys_bytes pg_load(શૂન્ય)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	પાછા phpagedir;
}

શૂન્ય pg_clear(શૂન્ય)
{
	memset(pagedir, 0, કદ(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	જો(!(o = b % ARM_PAGE_SIZE)) {
		પાછા b;
	}
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
		પ્રતિજ્ઞા(!(vaddr % ARM_PAGE_SIZE));
	} નહીંતર  {
		પ્રતિજ્ઞા((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	પ્રતિજ્ઞા(vaddr < kern_vir_start);

	જ્યારે(vaddr < vaddr_end) {
		phys_bytes source = phys;
		પ્રતિજ્ઞા(!(vaddr % ARM_PAGE_SIZE));
		જો(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} નહીંતર {
			પ્રતિજ્ઞા(!(phys % ARM_PAGE_SIZE));
		}
		પ્રતિજ્ઞા(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		જો(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		પ્રતિજ્ઞા(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		જો(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

શૂન્ય pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
