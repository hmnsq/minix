#شامل <minix/cpufeature.h>

#شامل <minix/type.h>
#شامل <assert.h>
#شامل "kernel/kernel.h"
#شامل "arch_proto.h"
#شامل <machine/cpu.h>
#شامل <arm/armreg.h>

#شامل <string.h>
#شامل <minix/type.h>

/* These are set/computed in kernel.lds. */
خارجی حرف kern_vir_بنیاد, kern_phys_بنیاد, kern_سائز;

/* Retrieve the absolute values to something we can use. */
ساکن phys_بائٹیں kern_vir_آغاز = (phys_بائٹیں) &kern_vir_بنیاد;
ساکن phys_بائٹیں kern_phys_آغاز = (phys_بائٹیں) &kern_phys_بنیاد;
ساکن phys_بائٹیں kern_kernlen = (phys_بائٹیں) &kern_سائز;

/* page directory we can use to map things */
ساکن u32_t pagedir[4096]  __aligned(16384);

خالی print_memmap(kinfo_t *cbi)
{
	صحیح m;
	أكد(cbi->mmap_سائز < MAXMEMMAP);
	برائے(m = 0; m < cbi->mmap_سائز; m++) {
		phys_بائٹیں addr = cbi->memmap[m].mm_بنیاد_addr, endit = cbi->memmap[m].mm_بنیاد_addr + cbi->memmap[m].mm_لمبائی;
		لکھو("%08lx-%08lx ",addr, endit);
	}
	لکھو("\nsize %08lx\n", cbi->mmap_سائز);
}

خالی cut_memmap(kinfo_t *cbi, phys_بائٹیں آغاز, phys_بائٹیں انتہا)
{
	صحیح m;
	phys_بائٹیں o;

	اگر((o=آغاز % ARM_صفحہ_سائز))
		آغاز -= o;
	اگر((o=انتہا % ARM_صفحہ_سائز))
		انتہا += ARM_صفحہ_سائز - o;

	أكد(kernel_may_alloc);

	برائے(m = 0; m < cbi->mmap_سائز; m++) {
		phys_بائٹیں substart = آغاز, subend = انتہا;
		phys_بائٹیں memaddr = cbi->memmap[m].mm_بنیاد_addr,
		    memend = cbi->memmap[m].mm_بنیاد_addr + cbi->memmap[m].mm_لمبائی;

		/* adjust cut range to be a subset of the free memory */
		اگر(substart < memaddr) substart = memaddr;
		اگر(subend > memend) subend = memend;
		اگر(substart >= subend) جاری;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_بنیاد_addr = cbi->memmap[m].mm_لمبائی = 0;
		اگر(substart > memaddr)
			جوڑو_memmap(cbi, memaddr, substart-memaddr);
		اگر(subend < memend)
			جوڑو_memmap(cbi, subend, memend-subend);
	}
}

خالی جوڑو_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	صحیح m;
#تعریف_کرو حد_بندی 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	اگر(addr > حد_بندی) {
		واپس;
	}

	اگر(addr + len > حد_بندی) {
		len -= (addr + len - حد_بندی);
	}

	أكد(cbi->mmap_سائز < MAXMEMMAP);
	اگر(len == 0) {
		واپس;
	}
	addr = roundup(addr, ARM_صفحہ_سائز);
	len = rounddown(len, ARM_صفحہ_سائز);

	أكد(kernel_may_alloc);

        برائے(m = 0; m < MAXMEMMAP; m++) {
		phys_بائٹیں highmark;
		اگر(cbi->memmap[m].mm_لمبائی) {
			جاری;
		}
		cbi->memmap[m].mm_بنیاد_addr = addr;
		cbi->memmap[m].mm_لمبائی = len;
		cbi->memmap[m].type = MULTIBOOT_حافظہ_دستیاب;
		اگر(m >= cbi->mmap_سائز) {
			cbi->mmap_سائز = m+1;
		}
		highmark = addr + len;
		اگر(highmark > cbi->mem_بلند_phys) {
			cbi->mem_بلند_phys = highmark;
		}
		واپس;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_بائٹیں *ph)
{
	u32_t *ret;
#تعریف_کرو PG_PAGETABLES 24
	ساکن u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	ساکن صحیح pt_inuse = 0;
	اگر(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	أكد(حجم(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	واپس ret;
}

#تعریف_کرو صفحہ_KB (ARM_صفحہ_سائز / 1024)

phys_بائٹیں pg_alloc_صفحہ(kinfo_t *cbi)
{
	صحیح m;
	multiboot_حافظہ_map_t *mmap;

	أكد(kernel_may_alloc);

	برائے(m = 0; m < cbi->mmap_سائز; m++) {
		mmap = &cbi->memmap[m];
		اگر(!mmap->mm_لمبائی) {
			جاری;
		}
		أكد(mmap->mm_لمبائی > 0);
		أكد(!(mmap->mm_لمبائی % ARM_صفحہ_سائز));
		أكد(!(mmap->mm_بنیاد_addr % ARM_صفحہ_سائز));

		u32_t addr = mmap->mm_بنیاد_addr;
		mmap->mm_بنیاد_addr += ARM_صفحہ_سائز;
		mmap->mm_لمبائی  -= ARM_صفحہ_سائز;

		cbi->kernel_allocated_بائٹیں_dynamic += ARM_صفحہ_سائز;

		واپس addr;
	}

	panic("can't find free memory");
}

خالی pg_شناختی_عنصر(kinfo_t *cbi)
{
	uint32_t i;
	phys_بائٹیں phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	أكد(cbi->mem_بلند_phys);

        /* Set up an identity mapping page directory */
	 برائے(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_سائز;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		اگر (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_انتہا) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} ورنہ {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

صحیح pg_mapkernel(خالی)
{
	صحیح pde;
	u32_t mapped = 0, kern_phys = kern_phys_آغاز;

	أكد(!(kern_vir_آغاز % ARM_SECTION_سائز));
	أكد(!(kern_phys_آغاز % ARM_SECTION_سائز));
	pde = kern_vir_آغاز / ARM_SECTION_سائز; /* start pde */
	جب(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_نقاب) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_سائز;
		kern_phys += ARM_SECTION_سائز;
		pde++;
	}
	واپس pde;	/* free pde */
}

خالی vm_enable_paging(خالی)
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
	#تعریف_کرو CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_بائٹیں pg_بھرو(خالی)
{
	phys_بائٹیں phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	واپس phpagedir;
}

خالی pg_clear(خالی)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بائٹیں pg_rounddown(phys_بائٹیں b)
{
	phys_بائٹیں o;
	اگر(!(o = b % ARM_صفحہ_سائز)) {
		واپس b;
	}
	واپس b  - o;
}

خالی pg_map(phys_بائٹیں phys, vir_بائٹیں vaddr, vir_بائٹیں vaddr_انتہا,
	kinfo_t *cbi)
{
	ساکن صحیح mapped_pde = -1;
	ساکن u32_t *pt = تہی;
	صحیح pde, pte;

	أكد(kernel_may_alloc);

	اگر(phys == PG_ALLOCATEME) {
		أكد(!(vaddr % ARM_صفحہ_سائز));
	} ورنہ  {
		أكد((vaddr % ARM_صفحہ_سائز) == (phys % ARM_صفحہ_سائز));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	أكد(vaddr < kern_vir_آغاز);

	جب(vaddr < vaddr_انتہا) {
		phys_بائٹیں ماخذ = phys;
		أكد(!(vaddr % ARM_صفحہ_سائز));
		اگر(phys == PG_ALLOCATEME) {
			ماخذ = pg_alloc_صفحہ(cbi);
		} ورنہ {
			أكد(!(phys % ARM_صفحہ_سائز));
		}
		أكد(!(ماخذ % ARM_صفحہ_سائز));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		اگر(mapped_pde < pde) {
			phys_بائٹیں ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_نقاب)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (ماخذ & ARM_VM_PTE_نقاب)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_صفحہ_سائز;
		اگر(phys != PG_ALLOCATEME) {
			phys += ARM_صفحہ_سائز;
		}
	}
}

خالی pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
