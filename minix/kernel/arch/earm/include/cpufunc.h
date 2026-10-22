#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#إذا 0
/* check interrupt state */
ساكن مضمن فراغ check_int(unsigned صحيح state, صحيح باريس)
{
	unsigned صحيح cpsr = 0;

	asm متطاير("mrs %0, cpsr" : "=r" (cpsr));

	إذا ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", باريس, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	إذا ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", باريس, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
ساكن مضمن فراغ dmb(فراغ)
{
	asm متطاير("dmb" : : : "memory");
}

/* Data synchronization barrier */
ساكن مضمن فراغ dsb(فراغ)
{
	asm متطاير("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
ساكن مضمن فراغ isb(فراغ)
{
	asm متطاير("isb" : : : "memory");
}

ساكن مضمن فراغ barrier(فراغ)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
ساكن مضمن u32_t read_clidr(فراغ){
	u32_t clidr;
	asm متطاير("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	أرجع clidr;
}


/* Read CSSELR, Cache Size Selection Register */
ساكن مضمن u32_t read_csselr(فراغ){
	u32_t csselr;
	asm متطاير("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	أرجع csselr;
}

/* Write CSSELR, Cache Size Selection Register */
ساكن مضمن فراغ write_csselr(u32_t csselr){
	asm متطاير("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
ساكن مضمن u32_t read_ccsidr(فراغ)
{
	u32_t ccsidr;
	asm متطاير("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	أرجع ccsidr;
}

/* Read TLBTR, TLB Type Register */
ساكن مضمن u32_t read_tlbtr(فراغ)
{
	u32_t tlbtr;
	asm متطاير("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	أرجع tlbtr;
}

/* keesj:move these out */
ساكن مضمن u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	طالما( (t = t >> 1) ) counter ++;
	أرجع counter;
}

/* keesj:move these out */
ساكن مضمن u32_t ipow2(u32_t t)
{
	أرجع 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
ساكن مضمن فراغ dcache_maint(صحيح type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  باريس_size,باريس_length;
	u32_t  نومبور_of_sets,نومبور_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	لكل (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		باريس_size = ccsidr & 0x7;
		باريس_length = 2 << (باريس_size + 1) ; /* 2**(line_size + 2) */
		نومبور_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		نومبور_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(نومبور_of_ways);
		إذا(ipow2(ilog2(نومبور_of_ways) < نومبور_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(باريس_length);
		لكل (way =0 ; way < نومبور_of_ways; way++) {
			لكل (set =0 ; set < نومبور_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				إذا (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm متطاير("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} وإلا إذا (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm متطاير("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
ساكن مضمن فراغ dcache_clean(فراغ){
	dcache_maint(1);
}
ساكن مضمن فراغ dcache_invalidate(فراغ){
	dcache_maint(2);
}

ساكن مضمن فراغ refresh_tlb(فراغ)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm متطاير("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#إذا 0
	/* Invalidate entire data TLB */
	asm متطاير("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm متطاير("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm متطاير("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm متطاير("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
ساكن مضمن u32_t read_sctlr(فراغ)
{
	u32_t ctl;

	asm متطاير("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	أرجع ctl;
}

/* Write System Control Register */
ساكن مضمن فراغ write_sctlr(u32_t ctl)
{
	asm متطاير("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
ساكن مضمن u32_t read_ttbr0(فراغ)
{
	u32_t bar;

	asm متطاير("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	أرجع bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
ساكن مضمن فراغ write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm متطاير("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
ساكن مضمن فراغ reload_ttbr0(فراغ)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
ساكن مضمن u32_t read_ttbr1(فراغ)
{
	u32_t bar;

	asm متطاير("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	أرجع bar;
}

/* Write Translation Table Base Register 1 */
ساكن مضمن فراغ write_ttbr1(u32_t bar)
{
	barrier();

	asm متطاير("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
ساكن مضمن فراغ reload_ttbr1(فراغ)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
ساكن مضمن u32_t read_ttbcr(فراغ)
{
	u32_t bcr;

	asm متطاير("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	أرجع bcr;
}

/* Write Translation Table Base Control Register */
ساكن مضمن فراغ write_ttbcr(u32_t bcr)
{
	asm متطاير("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
ساكن مضمن u32_t read_dacr(فراغ)
{
	u32_t dacr;

	asm متطاير("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	أرجع dacr;
}

/* Write Domain Access Control Register */
ساكن مضمن فراغ write_dacr(u32_t dacr)
{
	asm متطاير("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
ساكن مضمن u32_t read_dfsr(فراغ)
{
	u32_t fsr;

	asm متطاير("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	أرجع fsr;
}

/* Write Data Fault Status Register */
ساكن مضمن فراغ write_dfsr(u32_t fsr)
{
	asm متطاير("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
ساكن مضمن u32_t read_ifsr(فراغ)
{
	u32_t fsr;

	asm متطاير("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	أرجع fsr;
}

/* Write Instruction Fault Status Register */
ساكن مضمن فراغ write_ifsr(u32_t fsr)
{
	asm متطاير("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
ساكن مضمن u32_t read_dfar(فراغ)
{
	u32_t far;

	asm متطاير("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	أرجع far;
}

/* Write Data Fault Address Register */
ساكن مضمن فراغ write_dfar(u32_t far)
{
	asm متطاير("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
ساكن مضمن u32_t read_ifar(فراغ)
{
	u32_t far;

	asm متطاير("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	أرجع far;
}

/* Write Instruction Fault Address Register */
ساكن مضمن فراغ write_ifar(u32_t far)
{
	asm متطاير("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
ساكن مضمن u32_t read_vbar(فراغ)
{
	u32_t vbar;

	asm متطاير("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	أرجع vbar;
}

/* Write Vector Base Address Register */
ساكن مضمن فراغ write_vbar(u32_t vbar)
{
	asm متطاير("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
ساكن مضمن u32_t read_midr(فراغ)
{
	u32_t id;

	asm متطاير("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	أرجع id;
}

/* Read Auxiliary Control Register */
ساكن مضمن u32_t read_actlr(فراغ)
{
	u32_t ctl;

	asm متطاير("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	أرجع ctl;
}

/* Write Auxiliary Control Register */
ساكن مضمن فراغ write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm متطاير("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
ساكن مضمن u32_t read_cpsr(فراغ)
{
	u32_t status;

	asm متطاير("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	أرجع status;
}

/* Write Current Program Status Register */
ساكن مضمن فراغ write_cpsr(u32_t status)
{
	asm متطاير("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
