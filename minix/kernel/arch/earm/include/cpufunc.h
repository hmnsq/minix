#यदि_अपरिभाषितम् _ARM_CPUFUNC_H
#परिभाषय _ARM_CPUFUNC_H

#यदि 0
/* check interrupt state */
स्थिर अन्तःस्थ शून्य check_int(अचिह्नित पूर्णाङ्क state, पूर्णाङ्क पङ्क्तिः)
{
	अचिह्नित पूर्णाङ्क cpsr = 0;

	asm चञ्चल("mrs %0, cpsr" : "=r" (cpsr));

	यदि ((cpsr & PSR_F) != (state & PSR_F))
	    प्रारूप_लिख("%d: FIQs are unexpectedly %s\n", पङ्क्तिः, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	यदि ((cpsr & PSR_I) != (state & PSR_I))
	    प्रारूप_लिख("%d: IRQs are unexpectedly %s\n", पङ्क्तिः, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#यदि_अन्तः

/* Data memory barrier */
स्थिर अन्तःस्थ शून्य dmb(शून्य)
{
	asm चञ्चल("dmb" : : : "memory");
}

/* Data synchronization barrier */
स्थिर अन्तःस्थ शून्य dsb(शून्य)
{
	asm चञ्चल("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
स्थिर अन्तःस्थ शून्य isb(शून्य)
{
	asm चञ्चल("isb" : : : "memory");
}

स्थिर अन्तःस्थ शून्य barrier(शून्य)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
स्थिर अन्तःस्थ u32_t read_clidr(शून्य){
	u32_t clidr;
	asm चञ्चल("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	प्रत्यागम clidr;
}


/* Read CSSELR, Cache Size Selection Register */
स्थिर अन्तःस्थ u32_t read_csselr(शून्य){
	u32_t csselr;
	asm चञ्चल("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	प्रत्यागम csselr;
}

/* Write CSSELR, Cache Size Selection Register */
स्थिर अन्तःस्थ शून्य write_csselr(u32_t csselr){
	asm चञ्चल("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
स्थिर अन्तःस्थ u32_t read_ccsidr(शून्य)
{
	u32_t ccsidr;
	asm चञ्चल("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	प्रत्यागम ccsidr;
}

/* Read TLBTR, TLB Type Register */
स्थिर अन्तःस्थ u32_t read_tlbtr(शून्य)
{
	u32_t tlbtr;
	asm चञ्चल("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	प्रत्यागम tlbtr;
}

/* keesj:move these out */
स्थिर अन्तःस्थ u32_t ilog2(u32_t t)
{
	u32_t गणक =0;
	यावत्( (t = t >> 1) ) गणक ++;
	प्रत्यागम गणक;
}

/* keesj:move these out */
स्थिर अन्तःस्थ u32_t ipow2(u32_t t)
{
	प्रत्यागम 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
स्थिर अन्तःस्थ शून्य dcache_maint(पूर्णाङ्क type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  पङ्क्तिः_आकार,पङ्क्तिः_दीर्घता;
	u32_t  सङ्ख्या_सम्बन्धि_sets,सङ्ख्या_सम्बन्धि_ways;
	u32_t  समुच्चयः,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	प्रति (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		पङ्क्तिः_आकार = ccsidr & 0x7;
		पङ्क्तिः_दीर्घता = 2 << (पङ्क्तिः_आकार + 1) ; /* 2**(line_size + 2) */
		सङ्ख्या_सम्बन्धि_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		सङ्ख्या_सम्बन्धि_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_द्वयङ्काः = ilog2(सङ्ख्या_सम्बन्धि_ways);
		यदि(ipow2(ilog2(सङ्ख्या_सम्बन्धि_ways) < सङ्ख्या_सम्बन्धि_ways) ) {
			way_द्वयङ्काः++;
		}

		u32_t l = ilog2(पङ्क्तिः_दीर्घता);
		प्रति (way =0 ; way < सङ्ख्या_सम्बन्धि_ways; way++) {
			प्रति (समुच्चयः =0 ; समुच्चयः < सङ्ख्या_सम्बन्धि_sets; समुच्चयः++) {
				u32_t val = ( way << (32 - way_द्वयङ्काः) ) |  (समुच्चयः << l) | (cache_level << 1 );
				यदि (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm चञ्चल("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [समुच्चयः] "r" (val));
				} अन्यथा यदि (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm चञ्चल("mcr p15, 0, %[set], c7, c6, 2" 
							: : [समुच्चयः] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
स्थिर अन्तःस्थ शून्य dcache_clean(शून्य){
	dcache_maint(1);
}
स्थिर अन्तःस्थ शून्य dcache_invalidate(शून्य){
	dcache_maint(2);
}

स्थिर अन्तःस्थ शून्य refresh_tlb(शून्य)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm चञ्चल("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#यदि 0
	/* Invalidate entire data TLB */
	asm चञ्चल("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm चञ्चल("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#यदि_अन्तः

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm चञ्चल("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm चञ्चल("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
स्थिर अन्तःस्थ u32_t read_sctlr(शून्य)
{
	u32_t ctl;

	asm चञ्चल("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	प्रत्यागम ctl;
}

/* Write System Control Register */
स्थिर अन्तःस्थ शून्य write_sctlr(u32_t ctl)
{
	asm चञ्चल("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
स्थिर अन्तःस्थ u32_t read_ttbr0(शून्य)
{
	u32_t bar;

	asm चञ्चल("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	प्रत्यागम bar & ARM_TTBR_ADDR_आवरणम्;
}

/* Write Translation Table Base Register 0 */
स्थिर अन्तःस्थ शून्य write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_आवरणम् ) | ARM_TTBR_FLAGS_CACHED;
	asm चञ्चल("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
स्थिर अन्तःस्थ शून्य reload_ttbr0(शून्य)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
स्थिर अन्तःस्थ u32_t read_ttbr1(शून्य)
{
	u32_t bar;

	asm चञ्चल("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	प्रत्यागम bar;
}

/* Write Translation Table Base Register 1 */
स्थिर अन्तःस्थ शून्य write_ttbr1(u32_t bar)
{
	barrier();

	asm चञ्चल("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
स्थिर अन्तःस्थ शून्य reload_ttbr1(शून्य)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
स्थिर अन्तःस्थ u32_t read_ttbcr(शून्य)
{
	u32_t bcr;

	asm चञ्चल("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	प्रत्यागम bcr;
}

/* Write Translation Table Base Control Register */
स्थिर अन्तःस्थ शून्य write_ttbcr(u32_t bcr)
{
	asm चञ्चल("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
स्थिर अन्तःस्थ u32_t read_dacr(शून्य)
{
	u32_t dacr;

	asm चञ्चल("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	प्रत्यागम dacr;
}

/* Write Domain Access Control Register */
स्थिर अन्तःस्थ शून्य write_dacr(u32_t dacr)
{
	asm चञ्चल("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
स्थिर अन्तःस्थ u32_t read_dfsr(शून्य)
{
	u32_t fsr;

	asm चञ्चल("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	प्रत्यागम fsr;
}

/* Write Data Fault Status Register */
स्थिर अन्तःस्थ शून्य write_dfsr(u32_t fsr)
{
	asm चञ्चल("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
स्थिर अन्तःस्थ u32_t read_ifsr(शून्य)
{
	u32_t fsr;

	asm चञ्चल("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	प्रत्यागम fsr;
}

/* Write Instruction Fault Status Register */
स्थिर अन्तःस्थ शून्य write_ifsr(u32_t fsr)
{
	asm चञ्चल("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
स्थिर अन्तःस्थ u32_t read_dfar(शून्य)
{
	u32_t far;

	asm चञ्चल("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	प्रत्यागम far;
}

/* Write Data Fault Address Register */
स्थिर अन्तःस्थ शून्य write_dfar(u32_t far)
{
	asm चञ्चल("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
स्थिर अन्तःस्थ u32_t read_ifar(शून्य)
{
	u32_t far;

	asm चञ्चल("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	प्रत्यागम far;
}

/* Write Instruction Fault Address Register */
स्थिर अन्तःस्थ शून्य write_ifar(u32_t far)
{
	asm चञ्चल("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
स्थिर अन्तःस्थ u32_t read_vbar(शून्य)
{
	u32_t vbar;

	asm चञ्चल("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	प्रत्यागम vbar;
}

/* Write Vector Base Address Register */
स्थिर अन्तःस्थ शून्य write_vbar(u32_t vbar)
{
	asm चञ्चल("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
स्थिर अन्तःस्थ u32_t read_midr(शून्य)
{
	u32_t id;

	asm चञ्चल("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	प्रत्यागम id;
}

/* Read Auxiliary Control Register */
स्थिर अन्तःस्थ u32_t read_actlr(शून्य)
{
	u32_t ctl;

	asm चञ्चल("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	प्रत्यागम ctl;
}

/* Write Auxiliary Control Register */
स्थिर अन्तःस्थ शून्य write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm चञ्चल("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
स्थिर अन्तःस्थ u32_t read_cpsr(शून्य)
{
	u32_t स्थितिः;

	asm चञ्चल("mrs %[status], cpsr @ read CPSR"
			: [स्थितिः] "=r" (स्थितिः));

	प्रत्यागम स्थितिः;
}

/* Write Current Program Status Register */
स्थिर अन्तःस्थ शून्य write_cpsr(u32_t स्थितिः)
{
	asm चञ्चल("msr cpsr_c, %[status] @ write CPSR"
			: : [स्थितिः] "r" (स्थितिः));
}

#यदि_अन्तः /* _ARM_CPUFUNC_H */
