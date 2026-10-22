#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _ARM_CPUFUNC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ARM_CPUFUNC_H

#ਜੇ 0
/* check interrupt state */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ check_int(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ state, ਪੂਰਨ ਸਤਰ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cpsr = 0;

	asm ਅਸਥਿਰ("mrs %0, cpsr" : "=r" (cpsr));

	ਜੇ ((cpsr & PSR_F) != (state & PSR_F))
	    ਲਿਖੋ("%d: FIQs are unexpectedly %s\n", ਸਤਰ, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	ਜੇ ((cpsr & PSR_I) != (state & PSR_I))
	    ਲਿਖੋ("%d: IRQs are unexpectedly %s\n", ਸਤਰ, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#ਜੇ_ਅੰਤ

/* Data memory barrier */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ dmb(ਖਾਲੀ)
{
	asm ਅਸਥਿਰ("dmb" : : : "memory");
}

/* Data synchronization barrier */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ dsb(ਖਾਲੀ)
{
	asm ਅਸਥਿਰ("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ isb(ਖਾਲੀ)
{
	asm ਅਸਥਿਰ("isb" : : : "memory");
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ barrier(ਖਾਲੀ)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_clidr(ਖਾਲੀ){
	u32_t clidr;
	asm ਅਸਥਿਰ("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	ਵਾਪਸ clidr;
}


/* Read CSSELR, Cache Size Selection Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_csselr(ਖਾਲੀ){
	u32_t csselr;
	asm ਅਸਥਿਰ("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	ਵਾਪਸ csselr;
}

/* Write CSSELR, Cache Size Selection Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_csselr(u32_t csselr){
	asm ਅਸਥਿਰ("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_ccsidr(ਖਾਲੀ)
{
	u32_t ccsidr;
	asm ਅਸਥਿਰ("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	ਵਾਪਸ ccsidr;
}

/* Read TLBTR, TLB Type Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_tlbtr(ਖਾਲੀ)
{
	u32_t tlbtr;
	asm ਅਸਥਿਰ("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	ਵਾਪਸ tlbtr;
}

/* keesj:move these out */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	ਜਦੋਂ( (t = t >> 1) ) counter ++;
	ਵਾਪਸ counter;
}

/* keesj:move these out */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t ipow2(u32_t t)
{
	ਵਾਪਸ 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ dcache_maint(ਪੂਰਨ type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  ਸਤਰ_ਮਾਪ,ਸਤਰ_ਲੰਮਾਈ;
	u32_t  ਸੰਖਿਆ_ਦਾ_sets,ਸੰਖਿਆ_ਦਾ_ways;
	u32_t  ਸੈੱਟ,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	ਲਈ (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		ਸਤਰ_ਮਾਪ = ccsidr & 0x7;
		ਸਤਰ_ਲੰਮਾਈ = 2 << (ਸਤਰ_ਮਾਪ + 1) ; /* 2**(line_size + 2) */
		ਸੰਖਿਆ_ਦਾ_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		ਸੰਖਿਆ_ਦਾ_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_ਬਿੱਟ = ilog2(ਸੰਖਿਆ_ਦਾ_ways);
		ਜੇ(ipow2(ilog2(ਸੰਖਿਆ_ਦਾ_ways) < ਸੰਖਿਆ_ਦਾ_ways) ) {
			way_ਬਿੱਟ++;
		}

		u32_t l = ilog2(ਸਤਰ_ਲੰਮਾਈ);
		ਲਈ (way =0 ; way < ਸੰਖਿਆ_ਦਾ_ways; way++) {
			ਲਈ (ਸੈੱਟ =0 ; ਸੈੱਟ < ਸੰਖਿਆ_ਦਾ_sets; ਸੈੱਟ++) {
				u32_t val = ( way << (32 - way_ਬਿੱਟ) ) |  (ਸੈੱਟ << l) | (cache_level << 1 );
				ਜੇ (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm ਅਸਥਿਰ("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [ਸੈੱਟ] "r" (val));
				} ਨਹੀਂਤਾਂ ਜੇ (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm ਅਸਥਿਰ("mcr p15, 0, %[set], c7, c6, 2" 
							: : [ਸੈੱਟ] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ dcache_clean(ਖਾਲੀ){
	dcache_maint(1);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ dcache_invalidate(ਖਾਲੀ){
	dcache_maint(2);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ refresh_tlb(ਖਾਲੀ)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm ਅਸਥਿਰ("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#ਜੇ 0
	/* Invalidate entire data TLB */
	asm ਅਸਥਿਰ("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm ਅਸਥਿਰ("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#ਜੇ_ਅੰਤ

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm ਅਸਥਿਰ("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm ਅਸਥਿਰ("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_sctlr(ਖਾਲੀ)
{
	u32_t ctl;

	asm ਅਸਥਿਰ("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	ਵਾਪਸ ctl;
}

/* Write System Control Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_sctlr(u32_t ctl)
{
	asm ਅਸਥਿਰ("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_ttbr0(ਖਾਲੀ)
{
	u32_t bar;

	asm ਅਸਥਿਰ("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	ਵਾਪਸ bar & ARM_TTBR_ADDR_ਨਕਾਬ;
}

/* Write Translation Table Base Register 0 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_ਨਕਾਬ ) | ARM_TTBR_FLAGS_CACHED;
	asm ਅਸਥਿਰ("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ reload_ttbr0(ਖਾਲੀ)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_ttbr1(ਖਾਲੀ)
{
	u32_t bar;

	asm ਅਸਥਿਰ("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	ਵਾਪਸ bar;
}

/* Write Translation Table Base Register 1 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ttbr1(u32_t bar)
{
	barrier();

	asm ਅਸਥਿਰ("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ reload_ttbr1(ਖਾਲੀ)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_ttbcr(ਖਾਲੀ)
{
	u32_t bcr;

	asm ਅਸਥਿਰ("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	ਵਾਪਸ bcr;
}

/* Write Translation Table Base Control Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ttbcr(u32_t bcr)
{
	asm ਅਸਥਿਰ("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_dacr(ਖਾਲੀ)
{
	u32_t dacr;

	asm ਅਸਥਿਰ("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	ਵਾਪਸ dacr;
}

/* Write Domain Access Control Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_dacr(u32_t dacr)
{
	asm ਅਸਥਿਰ("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_dfsr(ਖਾਲੀ)
{
	u32_t fsr;

	asm ਅਸਥਿਰ("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	ਵਾਪਸ fsr;
}

/* Write Data Fault Status Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_dfsr(u32_t fsr)
{
	asm ਅਸਥਿਰ("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_ifsr(ਖਾਲੀ)
{
	u32_t fsr;

	asm ਅਸਥਿਰ("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	ਵਾਪਸ fsr;
}

/* Write Instruction Fault Status Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ifsr(u32_t fsr)
{
	asm ਅਸਥਿਰ("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_dfar(ਖਾਲੀ)
{
	u32_t far;

	asm ਅਸਥਿਰ("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	ਵਾਪਸ far;
}

/* Write Data Fault Address Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_dfar(u32_t far)
{
	asm ਅਸਥਿਰ("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_ifar(ਖਾਲੀ)
{
	u32_t far;

	asm ਅਸਥਿਰ("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	ਵਾਪਸ far;
}

/* Write Instruction Fault Address Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ifar(u32_t far)
{
	asm ਅਸਥਿਰ("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_vbar(ਖਾਲੀ)
{
	u32_t vbar;

	asm ਅਸਥਿਰ("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	ਵਾਪਸ vbar;
}

/* Write Vector Base Address Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_vbar(u32_t vbar)
{
	asm ਅਸਥਿਰ("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_midr(ਖਾਲੀ)
{
	u32_t id;

	asm ਅਸਥਿਰ("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	ਵਾਪਸ id;
}

/* Read Auxiliary Control Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_actlr(ਖਾਲੀ)
{
	u32_t ctl;

	asm ਅਸਥਿਰ("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	ਵਾਪਸ ctl;
}

/* Write Auxiliary Control Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm ਅਸਥਿਰ("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ u32_t read_cpsr(ਖਾਲੀ)
{
	u32_t ਸਥਿਤੀ;

	asm ਅਸਥਿਰ("mrs %[status], cpsr @ read CPSR"
			: [ਸਥਿਤੀ] "=r" (ਸਥਿਤੀ));

	ਵਾਪਸ ਸਥਿਤੀ;
}

/* Write Current Program Status Register */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_cpsr(u32_t ਸਥਿਤੀ)
{
	asm ਅਸਥਿਰ("msr cpsr_c, %[status] @ write CPSR"
			: : [ਸਥਿਤੀ] "r" (ਸਥਿਤੀ));
}

#ਜੇ_ਅੰਤ /* _ARM_CPUFUNC_H */
