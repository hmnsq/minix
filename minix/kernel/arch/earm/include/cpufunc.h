#अगर_अघोषित _ARM_CPUFUNC_H
#घोषणा _ARM_CPUFUNC_H

#अगर 0
/* check interrupt state */
अटल अंतरभूत व्योम check_int(अचिन्हित पूर्णांक state, पूर्णांक पंक्ति_2)
{
	अचिन्हित पूर्णांक cpsr = 0;

	यंत्र अस्थिर("mrs %0, cpsr" : "=r" (cpsr));

	अगर ((cpsr & PSR_F) != (state & PSR_F))
	    म_लिखो("%d: FIQs are unexpectedly %s\n", पंक्ति_2, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	अगर ((cpsr & PSR_I) != (state & PSR_I))
	    म_लिखो("%d: IRQs are unexpectedly %s\n", पंक्ति_2, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#पूर्ण_अगर

/* Data memory barrier */
अटल अंतरभूत व्योम dmb(व्योम)
{
	यंत्र अस्थिर("dmb" : : : "memory");
}

/* Data synchronization barrier */
अटल अंतरभूत व्योम dsb(व्योम)
{
	यंत्र अस्थिर("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
अटल अंतरभूत व्योम isb(व्योम)
{
	यंत्र अस्थिर("isb" : : : "memory");
}

अटल अंतरभूत व्योम barrier(व्योम)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
अटल अंतरभूत u32_t read_clidr(व्योम){
	u32_t clidr;
	यंत्र अस्थिर("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	वापस clidr;
}


/* Read CSSELR, Cache Size Selection Register */
अटल अंतरभूत u32_t read_csselr(व्योम){
	u32_t csselr;
	यंत्र अस्थिर("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	वापस csselr;
}

/* Write CSSELR, Cache Size Selection Register */
अटल अंतरभूत व्योम write_csselr(u32_t csselr){
	यंत्र अस्थिर("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
अटल अंतरभूत u32_t read_ccsidr(व्योम)
{
	u32_t ccsidr;
	यंत्र अस्थिर("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	वापस ccsidr;
}

/* Read TLBTR, TLB Type Register */
अटल अंतरभूत u32_t read_tlbtr(व्योम)
{
	u32_t tlbtr;
	यंत्र अस्थिर("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	वापस tlbtr;
}

/* keesj:move these out */
अटल अंतरभूत u32_t ilog2(u32_t t)
{
	u32_t गणक =0;
	जबतक( (t = t >> 1) ) गणक ++;
	वापस गणक;
}

/* keesj:move these out */
अटल अंतरभूत u32_t ipow2(u32_t t)
{
	वापस 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
अटल अंतरभूत व्योम dcache_maint(पूर्णांक type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  पंक्ति_आकार,पंक्ति_लंबाई;
	u32_t  संख्या_of_sets,संख्या_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	क्रम (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		पंक्ति_आकार = ccsidr & 0x7;
		पंक्ति_लंबाई = 2 << (पंक्ति_आकार + 1) ; /* 2**(line_size + 2) */
		संख्या_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		संख्या_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(संख्या_of_ways);
		अगर(ipow2(ilog2(संख्या_of_ways) < संख्या_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(पंक्ति_लंबाई);
		क्रम (way =0 ; way < संख्या_of_ways; way++) {
			क्रम (set =0 ; set < संख्या_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				अगर (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					यंत्र अस्थिर("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} अन्यथा अगर (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					यंत्र अस्थिर("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
अटल अंतरभूत व्योम dcache_clean(व्योम){
	dcache_maint(1);
}
अटल अंतरभूत व्योम dcache_invalidate(व्योम){
	dcache_maint(2);
}

अटल अंतरभूत व्योम refresh_tlb(व्योम)
{
	dsb();

	/* Invalidate entire unified TLB */
	यंत्र अस्थिर("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#अगर 0
	/* Invalidate entire data TLB */
	यंत्र अस्थिर("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	यंत्र अस्थिर("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#पूर्ण_अगर

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	यंत्र अस्थिर("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	यंत्र अस्थिर("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
अटल अंतरभूत u32_t read_sctlr(व्योम)
{
	u32_t ctl;

	यंत्र अस्थिर("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	वापस ctl;
}

/* Write System Control Register */
अटल अंतरभूत व्योम write_sctlr(u32_t ctl)
{
	यंत्र अस्थिर("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
अटल अंतरभूत u32_t read_ttbr0(व्योम)
{
	u32_t bar;

	यंत्र अस्थिर("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	वापस bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
अटल अंतरभूत व्योम write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	यंत्र अस्थिर("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
अटल अंतरभूत व्योम reload_ttbr0(व्योम)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
अटल अंतरभूत u32_t read_ttbr1(व्योम)
{
	u32_t bar;

	यंत्र अस्थिर("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	वापस bar;
}

/* Write Translation Table Base Register 1 */
अटल अंतरभूत व्योम write_ttbr1(u32_t bar)
{
	barrier();

	यंत्र अस्थिर("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
अटल अंतरभूत व्योम reload_ttbr1(व्योम)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
अटल अंतरभूत u32_t read_ttbcr(व्योम)
{
	u32_t bcr;

	यंत्र अस्थिर("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	वापस bcr;
}

/* Write Translation Table Base Control Register */
अटल अंतरभूत व्योम write_ttbcr(u32_t bcr)
{
	यंत्र अस्थिर("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
अटल अंतरभूत u32_t read_dacr(व्योम)
{
	u32_t dacr;

	यंत्र अस्थिर("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	वापस dacr;
}

/* Write Domain Access Control Register */
अटल अंतरभूत व्योम write_dacr(u32_t dacr)
{
	यंत्र अस्थिर("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
अटल अंतरभूत u32_t read_dfsr(व्योम)
{
	u32_t fsr;

	यंत्र अस्थिर("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	वापस fsr;
}

/* Write Data Fault Status Register */
अटल अंतरभूत व्योम write_dfsr(u32_t fsr)
{
	यंत्र अस्थिर("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
अटल अंतरभूत u32_t read_ifsr(व्योम)
{
	u32_t fsr;

	यंत्र अस्थिर("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	वापस fsr;
}

/* Write Instruction Fault Status Register */
अटल अंतरभूत व्योम write_ifsr(u32_t fsr)
{
	यंत्र अस्थिर("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
अटल अंतरभूत u32_t read_dfar(व्योम)
{
	u32_t far;

	यंत्र अस्थिर("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	वापस far;
}

/* Write Data Fault Address Register */
अटल अंतरभूत व्योम write_dfar(u32_t far)
{
	यंत्र अस्थिर("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
अटल अंतरभूत u32_t read_ifar(व्योम)
{
	u32_t far;

	यंत्र अस्थिर("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	वापस far;
}

/* Write Instruction Fault Address Register */
अटल अंतरभूत व्योम write_ifar(u32_t far)
{
	यंत्र अस्थिर("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
अटल अंतरभूत u32_t read_vbar(व्योम)
{
	u32_t vbar;

	यंत्र अस्थिर("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	वापस vbar;
}

/* Write Vector Base Address Register */
अटल अंतरभूत व्योम write_vbar(u32_t vbar)
{
	यंत्र अस्थिर("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
अटल अंतरभूत u32_t read_midr(व्योम)
{
	u32_t id;

	यंत्र अस्थिर("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	वापस id;
}

/* Read Auxiliary Control Register */
अटल अंतरभूत u32_t read_actlr(व्योम)
{
	u32_t ctl;

	यंत्र अस्थिर("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	वापस ctl;
}

/* Write Auxiliary Control Register */
अटल अंतरभूत व्योम write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	यंत्र अस्थिर("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
अटल अंतरभूत u32_t read_cpsr(व्योम)
{
	u32_t status;

	यंत्र अस्थिर("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	वापस status;
}

/* Write Current Program Status Register */
अटल अंतरभूत व्योम write_cpsr(u32_t status)
{
	यंत्र अस्थिर("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#पूर्ण_अगर /* _ARM_CPUFUNC_H */
