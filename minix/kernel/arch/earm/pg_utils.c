#ضمن <minix/cpufeature.h>

#ضمن <minix/type.h>
#ضمن <assert.h>
#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"
#ضمن <machine/cpu.h>
#ضمن <arm/armreg.h>

#ضمن <string.h>
#ضمن <minix/type.h>

/* These are set/computed in kernel.lds. */
خارجي حرف _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
ساكن phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
ساكن phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
ساكن phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
ساكن u32_t pagedir[4096]  __aligned(16384);

فراغ print_memmap(kinfo_t *cbi)
{
	صحيح m;
	أكد(cbi->mmap_size < MAXMEMMAP);
	لكل(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
		printf("%08lx-%08lx ",addr, endit);
	}
	printf("\nsize %08lx\n", cbi->mmap_size);
}

فراغ cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
	صحيح m;
	phys_bytes o;

	إذا((o=start % ARM_PAGE_SIZE))
		start -= o;
	إذا((o=end % ARM_PAGE_SIZE))
		end += ARM_PAGE_SIZE - o;

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes substart = start, subend = end;
		phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
		    memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

		/* adjust cut range to be a subset of the free memory */
		إذا(substart < memaddr) substart = memaddr;
		إذا(subend > memend) subend = memend;
		إذا(substart >= subend) استمر;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
		إذا(substart > memaddr)
			add_memmap(cbi, memaddr, substart-memaddr);
		إذا(subend < memend)
			add_memmap(cbi, subend, memend-subend);
	}
}

فراغ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	صحيح m;
#define LIMIT 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	إذا(addr > LIMIT) {
		أرجع;
	}

	إذا(addr + len > LIMIT) {
		len -= (addr + len - LIMIT);
	}

	أكد(cbi->mmap_size < MAXMEMMAP);
	إذا(len == 0) {
		أرجع;
	}
	addr = roundup(addr, ARM_PAGE_SIZE);
	len = rounddown(len, ARM_PAGE_SIZE);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
		إذا(cbi->memmap[m].mm_length) {
			استمر;
		}
		cbi->memmap[m].mm_base_addr = addr;
		cbi->memmap[m].mm_length = len;
		cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;
		إذا(m >= cbi->mmap_size) {
			cbi->mmap_size = m+1;
		}
		highmark = addr + len;
		إذا(highmark > cbi->mem_بەرز_phys) {
			cbi->mem_بەرز_phys = highmark;
		}
		أرجع;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 24
	ساكن u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	ساكن صحيح pt_inuse = 0;
	إذا(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	أكد(حجم(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	أرجع ret;
}

#define PAGE_KB (ARM_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	صحيح m;
	multiboot_memory_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_size; m++) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_length) {
			استمر;
		}
		أكد(mmap->mm_length > 0);
		أكد(!(mmap->mm_length % ARM_PAGE_SIZE));
		أكد(!(mmap->mm_base_addr % ARM_PAGE_SIZE));

		u32_t addr = mmap->mm_base_addr;
		mmap->mm_base_addr += ARM_PAGE_SIZE;
		mmap->mm_length  -= ARM_PAGE_SIZE;

		cbi->kernel_allocated_bytes_dynamic += ARM_PAGE_SIZE;

		أرجع addr;
	}

	panic("can't find free memory");
}

فراغ pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	أكد(cbi->mem_بەرز_phys);

        /* Set up an identity mapping page directory */
	 لكل(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_SIZE;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		إذا (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_END) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} وإلا {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

صحيح pg_mapkernel(فراغ)
{
	صحيح pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

	أكد(!(kern_vir_start % ARM_SECTION_SIZE));
	أكد(!(kern_phys_start % ARM_SECTION_SIZE));
	pde = kern_vir_start / ARM_SECTION_SIZE; /* start pde */
	طالما(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_MASK) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_SIZE;
		kern_phys += ARM_SECTION_SIZE;
		pde++;
	}
	أرجع pde;	/* free pde */
}

فراغ vm_enable_paging(فراغ)
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

phys_bytes pg_load(فراغ)
{
	phys_bytes phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	إذا(!(o = b % ARM_PAGE_SIZE)) {
		أرجع b;
	}
	أرجع b  - o;
}

فراغ pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	ساكن صحيح mapped_pde = -1;
	ساكن u32_t *pt = عدم;
	صحيح pde, pte;

	أكد(kernel_may_alloc);

	إذا(phys == PG_ALLOCATEME) {
		أكد(!(vaddr % ARM_PAGE_SIZE));
	} وإلا  {
		أكد((vaddr % ARM_PAGE_SIZE) == (phys % ARM_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	أكد(vaddr < kern_vir_start);

	طالما(vaddr < vaddr_end) {
		phys_bytes source = phys;
		أكد(!(vaddr % ARM_PAGE_SIZE));
		إذا(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} وإلا {
			أكد(!(phys % ARM_PAGE_SIZE));
		}
		أكد(!(source % ARM_PAGE_SIZE));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_MASK)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (source & ARM_VM_PTE_MASK)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_PAGE_SIZE;
		إذا(phys != PG_ALLOCATEME) {
			phys += ARM_PAGE_SIZE;
		}
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
