#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _ARM_CPUFUNC_H
#ವ್ಯಾಖ್ಯಾನಿಸು _ARM_CPUFUNC_H

#ಆದರೆ 0
/* check interrupt state */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ check_int(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ state, ಪೂರ್ಣಾಂಕ ಸಾಲು)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cpsr = 0;

	asm ಅಸ್ಥಿರ("mrs %0, cpsr" : "=r" (cpsr));

	ಆದರೆ ((cpsr & PSR_F) != (state & PSR_F))
	    ಬರೆ("%d: FIQs are unexpectedly %s\n", ಸಾಲು, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	ಆದರೆ ((cpsr & PSR_I) != (state & PSR_I))
	    ಬರೆ("%d: IRQs are unexpectedly %s\n", ಸಾಲು, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#ಆದರೆ_ಅಂತ್ಯ

/* Data memory barrier */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ dmb(ಶೂನ್ಯ)
{
	asm ಅಸ್ಥಿರ("dmb" : : : "memory");
}

/* Data synchronization barrier */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ dsb(ಶೂನ್ಯ)
{
	asm ಅಸ್ಥಿರ("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ isb(ಶೂನ್ಯ)
{
	asm ಅಸ್ಥಿರ("isb" : : : "memory");
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ barrier(ಶೂನ್ಯ)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_clidr(ಶೂನ್ಯ){
	u32_t clidr;
	asm ಅಸ್ಥಿರ("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	ಹಿಂತಿರುಗಿ clidr;
}


/* Read CSSELR, Cache Size Selection Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_csselr(ಶೂನ್ಯ){
	u32_t csselr;
	asm ಅಸ್ಥಿರ("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	ಹಿಂತಿರುಗಿ csselr;
}

/* Write CSSELR, Cache Size Selection Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_csselr(u32_t csselr){
	asm ಅಸ್ಥಿರ("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_ccsidr(ಶೂನ್ಯ)
{
	u32_t ccsidr;
	asm ಅಸ್ಥಿರ("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	ಹಿಂತಿರುಗಿ ccsidr;
}

/* Read TLBTR, TLB Type Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_tlbtr(ಶೂನ್ಯ)
{
	u32_t tlbtr;
	asm ಅಸ್ಥಿರ("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	ಹಿಂತಿರುಗಿ tlbtr;
}

/* keesj:move these out */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	ಆಗ( (t = t >> 1) ) counter ++;
	ಹಿಂತಿರುಗಿ counter;
}

/* keesj:move these out */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t ipow2(u32_t t)
{
	ಹಿಂತಿರುಗಿ 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ dcache_maint(ಪೂರ್ಣಾಂಕ type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  ಸಾಲು_ಗಾತ್ರ,ಸಾಲು_ಉದ್ದಳತೆ;
	u32_t  ಸಂಖ್ಯೆ_ದ_sets,ಸಂಖ್ಯೆ_ದ_ways;
	u32_t  ಗಣ,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	ಗಾಗಿ (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		ಸಾಲು_ಗಾತ್ರ = ccsidr & 0x7;
		ಸಾಲು_ಉದ್ದಳತೆ = 2 << (ಸಾಲು_ಗಾತ್ರ + 1) ; /* 2**(line_size + 2) */
		ಸಂಖ್ಯೆ_ದ_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		ಸಂಖ್ಯೆ_ದ_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_ಬಿಟ್ಟುಗಳು = ilog2(ಸಂಖ್ಯೆ_ದ_ways);
		ಆದರೆ(ipow2(ilog2(ಸಂಖ್ಯೆ_ದ_ways) < ಸಂಖ್ಯೆ_ದ_ways) ) {
			way_ಬಿಟ್ಟುಗಳು++;
		}

		u32_t l = ilog2(ಸಾಲು_ಉದ್ದಳತೆ);
		ಗಾಗಿ (way =0 ; way < ಸಂಖ್ಯೆ_ದ_ways; way++) {
			ಗಾಗಿ (ಗಣ =0 ; ಗಣ < ಸಂಖ್ಯೆ_ದ_sets; ಗಣ++) {
				u32_t val = ( way << (32 - way_ಬಿಟ್ಟುಗಳು) ) |  (ಗಣ << l) | (cache_level << 1 );
				ಆದರೆ (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm ಅಸ್ಥಿರ("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [ಗಣ] "r" (val));
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm ಅಸ್ಥಿರ("mcr p15, 0, %[set], c7, c6, 2" 
							: : [ಗಣ] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ dcache_clean(ಶೂನ್ಯ){
	dcache_maint(1);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ dcache_invalidate(ಶೂನ್ಯ){
	dcache_maint(2);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ refresh_tlb(ಶೂನ್ಯ)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#ಆದರೆ 0
	/* Invalidate entire data TLB */
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_sctlr(ಶೂನ್ಯ)
{
	u32_t ctl;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	ಹಿಂತಿರುಗಿ ctl;
}

/* Write System Control Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_sctlr(u32_t ctl)
{
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_ttbr0(ಶೂನ್ಯ)
{
	u32_t bar;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	ಹಿಂತಿರುಗಿ bar & ARM_TTBR_ADDR_ಮುಖವಾಡ;
}

/* Write Translation Table Base Register 0 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_ಮುಖವಾಡ ) | ARM_TTBR_FLAGS_CACHED;
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ reload_ttbr0(ಶೂನ್ಯ)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_ttbr1(ಶೂನ್ಯ)
{
	u32_t bar;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	ಹಿಂತಿರುಗಿ bar;
}

/* Write Translation Table Base Register 1 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ttbr1(u32_t bar)
{
	barrier();

	asm ಅಸ್ಥಿರ("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ reload_ttbr1(ಶೂನ್ಯ)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_ttbcr(ಶೂನ್ಯ)
{
	u32_t bcr;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	ಹಿಂತಿರುಗಿ bcr;
}

/* Write Translation Table Base Control Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ttbcr(u32_t bcr)
{
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_dacr(ಶೂನ್ಯ)
{
	u32_t dacr;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	ಹಿಂತಿರುಗಿ dacr;
}

/* Write Domain Access Control Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_dacr(u32_t dacr)
{
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_dfsr(ಶೂನ್ಯ)
{
	u32_t fsr;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	ಹಿಂತಿರುಗಿ fsr;
}

/* Write Data Fault Status Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_dfsr(u32_t fsr)
{
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_ifsr(ಶೂನ್ಯ)
{
	u32_t fsr;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	ಹಿಂತಿರುಗಿ fsr;
}

/* Write Instruction Fault Status Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ifsr(u32_t fsr)
{
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_dfar(ಶೂನ್ಯ)
{
	u32_t far;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	ಹಿಂತಿರುಗಿ far;
}

/* Write Data Fault Address Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_dfar(u32_t far)
{
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_ifar(ಶೂನ್ಯ)
{
	u32_t far;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	ಹಿಂತಿರುಗಿ far;
}

/* Write Instruction Fault Address Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ifar(u32_t far)
{
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_vbar(ಶೂನ್ಯ)
{
	u32_t vbar;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	ಹಿಂತಿರುಗಿ vbar;
}

/* Write Vector Base Address Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_vbar(u32_t vbar)
{
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_midr(ಶೂನ್ಯ)
{
	u32_t id;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	ಹಿಂತಿರುಗಿ id;
}

/* Read Auxiliary Control Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_actlr(ಶೂನ್ಯ)
{
	u32_t ctl;

	asm ಅಸ್ಥಿರ("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	ಹಿಂತಿರುಗಿ ctl;
}

/* Write Auxiliary Control Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm ಅಸ್ಥಿರ("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32_t read_cpsr(ಶೂನ್ಯ)
{
	u32_t ಸ್ಥಿತಿ_2;

	asm ಅಸ್ಥಿರ("mrs %[status], cpsr @ read CPSR"
			: [ಸ್ಥಿತಿ_2] "=r" (ಸ್ಥಿತಿ_2));

	ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/* Write Current Program Status Register */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_cpsr(u32_t ಸ್ಥಿತಿ_2)
{
	asm ಅಸ್ಥಿರ("msr cpsr_c, %[status] @ write CPSR"
			: : [ಸ್ಥಿತಿ_2] "r" (ಸ್ಥಿತಿ_2));
}

#ಆದರೆ_ಅಂತ್ಯ /* _ARM_CPUFUNC_H */
