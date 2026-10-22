#నిర్వచితం_కాకపోతే _ARM_CPUFUNC_H
#నిర్వచించు _ARM_CPUFUNC_H

#ఒకవేళ 0
/* check interrupt state */
స్థిర అంతఃవరుస శూన్య check_int(గుర్తులేని పూర్ణాంకం state, పూర్ణాంకం పంక్తి)
{
	గుర్తులేని పూర్ణాంకం cpsr = 0;

	asm అస్థిర("mrs %0, cpsr" : "=r" (cpsr));

	ఒకవేళ ((cpsr & PSR_F) != (state & PSR_F))
	    రాయి("%d: FIQs are unexpectedly %s\n", పంక్తి, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	ఒకవేళ ((cpsr & PSR_I) != (state & PSR_I))
	    రాయి("%d: IRQs are unexpectedly %s\n", పంక్తి, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#అయితే_ముగింపు

/* Data memory barrier */
స్థిర అంతఃవరుస శూన్య dmb(శూన్య)
{
	asm అస్థిర("dmb" : : : "memory");
}

/* Data synchronization barrier */
స్థిర అంతఃవరుస శూన్య dsb(శూన్య)
{
	asm అస్థిర("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
స్థిర అంతఃవరుస శూన్య isb(శూన్య)
{
	asm అస్థిర("isb" : : : "memory");
}

స్థిర అంతఃవరుస శూన్య barrier(శూన్య)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
స్థిర అంతఃవరుస u32_t read_clidr(శూన్య){
	u32_t clidr;
	asm అస్థిర("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	తిరిగి clidr;
}


/* Read CSSELR, Cache Size Selection Register */
స్థిర అంతఃవరుస u32_t read_csselr(శూన్య){
	u32_t csselr;
	asm అస్థిర("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	తిరిగి csselr;
}

/* Write CSSELR, Cache Size Selection Register */
స్థిర అంతఃవరుస శూన్య write_csselr(u32_t csselr){
	asm అస్థిర("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
స్థిర అంతఃవరుస u32_t read_ccsidr(శూన్య)
{
	u32_t ccsidr;
	asm అస్థిర("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	తిరిగి ccsidr;
}

/* Read TLBTR, TLB Type Register */
స్థిర అంతఃవరుస u32_t read_tlbtr(శూన్య)
{
	u32_t tlbtr;
	asm అస్థిర("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	తిరిగి tlbtr;
}

/* keesj:move these out */
స్థిర అంతఃవరుస u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	అయితే( (t = t >> 1) ) counter ++;
	తిరిగి counter;
}

/* keesj:move these out */
స్థిర అంతఃవరుస u32_t ipow2(u32_t t)
{
	తిరిగి 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
స్థిర అంతఃవరుస శూన్య dcache_maint(పూర్ణాంకం type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  పంక్తి_కొలత,పంక్తి_నిడివి;
	u32_t  సంఖ్య_యొక్క_sets,సంఖ్య_యొక్క_ways;
	u32_t  సమితి,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	కొరకు (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		పంక్తి_కొలత = ccsidr & 0x7;
		పంక్తి_నిడివి = 2 << (పంక్తి_కొలత + 1) ; /* 2**(line_size + 2) */
		సంఖ్య_యొక్క_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		సంఖ్య_యొక్క_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_బిట్లు = ilog2(సంఖ్య_యొక్క_ways);
		ఒకవేళ(ipow2(ilog2(సంఖ్య_యొక్క_ways) < సంఖ్య_యొక్క_ways) ) {
			way_బిట్లు++;
		}

		u32_t l = ilog2(పంక్తి_నిడివి);
		కొరకు (way =0 ; way < సంఖ్య_యొక్క_ways; way++) {
			కొరకు (సమితి =0 ; సమితి < సంఖ్య_యొక్క_sets; సమితి++) {
				u32_t val = ( way << (32 - way_బిట్లు) ) |  (సమితి << l) | (cache_level << 1 );
				ఒకవేళ (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm అస్థిర("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [సమితి] "r" (val));
				} లేకపోతే ఒకవేళ (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm అస్థిర("mcr p15, 0, %[set], c7, c6, 2" 
							: : [సమితి] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
స్థిర అంతఃవరుస శూన్య dcache_clean(శూన్య){
	dcache_maint(1);
}
స్థిర అంతఃవరుస శూన్య dcache_invalidate(శూన్య){
	dcache_maint(2);
}

స్థిర అంతఃవరుస శూన్య refresh_tlb(శూన్య)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm అస్థిర("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#ఒకవేళ 0
	/* Invalidate entire data TLB */
	asm అస్థిర("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm అస్థిర("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#అయితే_ముగింపు

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm అస్థిర("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm అస్థిర("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
స్థిర అంతఃవరుస u32_t read_sctlr(శూన్య)
{
	u32_t ctl;

	asm అస్థిర("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	తిరిగి ctl;
}

/* Write System Control Register */
స్థిర అంతఃవరుస శూన్య write_sctlr(u32_t ctl)
{
	asm అస్థిర("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
స్థిర అంతఃవరుస u32_t read_ttbr0(శూన్య)
{
	u32_t bar;

	asm అస్థిర("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	తిరిగి bar & ARM_TTBR_ADDR_ముసుగు;
}

/* Write Translation Table Base Register 0 */
స్థిర అంతఃవరుస శూన్య write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_ముసుగు ) | ARM_TTBR_FLAGS_CACHED;
	asm అస్థిర("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
స్థిర అంతఃవరుస శూన్య reload_ttbr0(శూన్య)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
స్థిర అంతఃవరుస u32_t read_ttbr1(శూన్య)
{
	u32_t bar;

	asm అస్థిర("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	తిరిగి bar;
}

/* Write Translation Table Base Register 1 */
స్థిర అంతఃవరుస శూన్య write_ttbr1(u32_t bar)
{
	barrier();

	asm అస్థిర("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
స్థిర అంతఃవరుస శూన్య reload_ttbr1(శూన్య)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
స్థిర అంతఃవరుస u32_t read_ttbcr(శూన్య)
{
	u32_t bcr;

	asm అస్థిర("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	తిరిగి bcr;
}

/* Write Translation Table Base Control Register */
స్థిర అంతఃవరుస శూన్య write_ttbcr(u32_t bcr)
{
	asm అస్థిర("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
స్థిర అంతఃవరుస u32_t read_dacr(శూన్య)
{
	u32_t dacr;

	asm అస్థిర("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	తిరిగి dacr;
}

/* Write Domain Access Control Register */
స్థిర అంతఃవరుస శూన్య write_dacr(u32_t dacr)
{
	asm అస్థిర("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
స్థిర అంతఃవరుస u32_t read_dfsr(శూన్య)
{
	u32_t fsr;

	asm అస్థిర("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	తిరిగి fsr;
}

/* Write Data Fault Status Register */
స్థిర అంతఃవరుస శూన్య write_dfsr(u32_t fsr)
{
	asm అస్థిర("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
స్థిర అంతఃవరుస u32_t read_ifsr(శూన్య)
{
	u32_t fsr;

	asm అస్థిర("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	తిరిగి fsr;
}

/* Write Instruction Fault Status Register */
స్థిర అంతఃవరుస శూన్య write_ifsr(u32_t fsr)
{
	asm అస్థిర("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
స్థిర అంతఃవరుస u32_t read_dfar(శూన్య)
{
	u32_t far;

	asm అస్థిర("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	తిరిగి far;
}

/* Write Data Fault Address Register */
స్థిర అంతఃవరుస శూన్య write_dfar(u32_t far)
{
	asm అస్థిర("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
స్థిర అంతఃవరుస u32_t read_ifar(శూన్య)
{
	u32_t far;

	asm అస్థిర("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	తిరిగి far;
}

/* Write Instruction Fault Address Register */
స్థిర అంతఃవరుస శూన్య write_ifar(u32_t far)
{
	asm అస్థిర("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
స్థిర అంతఃవరుస u32_t read_vbar(శూన్య)
{
	u32_t vbar;

	asm అస్థిర("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	తిరిగి vbar;
}

/* Write Vector Base Address Register */
స్థిర అంతఃవరుస శూన్య write_vbar(u32_t vbar)
{
	asm అస్థిర("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
స్థిర అంతఃవరుస u32_t read_midr(శూన్య)
{
	u32_t id;

	asm అస్థిర("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	తిరిగి id;
}

/* Read Auxiliary Control Register */
స్థిర అంతఃవరుస u32_t read_actlr(శూన్య)
{
	u32_t ctl;

	asm అస్థిర("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	తిరిగి ctl;
}

/* Write Auxiliary Control Register */
స్థిర అంతఃవరుస శూన్య write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm అస్థిర("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
స్థిర అంతఃవరుస u32_t read_cpsr(శూన్య)
{
	u32_t స్థితి_2;

	asm అస్థిర("mrs %[status], cpsr @ read CPSR"
			: [స్థితి_2] "=r" (స్థితి_2));

	తిరిగి స్థితి_2;
}

/* Write Current Program Status Register */
స్థిర అంతఃవరుస శూన్య write_cpsr(u32_t స్థితి_2)
{
	asm అస్థిర("msr cpsr_c, %[status] @ write CPSR"
			: : [స్థితి_2] "r" (స్థితి_2));
}

#అయితే_ముగింపు /* _ARM_CPUFUNC_H */
