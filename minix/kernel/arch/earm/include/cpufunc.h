#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#जर 0
/* check interrupt state */
स्थिर ओळीत शून्य check_int(अचिन्हित पूर्णांक state, पूर्णांक ओळ)
{
	अचिन्हित पूर्णांक cpsr = 0;

	यंत्र अस्थिर("mrs %0, cpsr" : "=r" (cpsr));

	जर ((cpsr & PSR_F) != (state & PSR_F))
	    म_लिखो("%d: FIQs are unexpectedly %s\n", ओळ, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	जर ((cpsr & PSR_I) != (state & PSR_I))
	    म_लिखो("%d: IRQs are unexpectedly %s\n", ओळ, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
स्थिर ओळीत शून्य dmb(शून्य)
{
	यंत्र अस्थिर("dmb" : : : "memory");
}

/* Data synchronization barrier */
स्थिर ओळीत शून्य dsb(शून्य)
{
	यंत्र अस्थिर("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
स्थिर ओळीत शून्य isb(शून्य)
{
	यंत्र अस्थिर("isb" : : : "memory");
}

स्थिर ओळीत शून्य barrier(शून्य)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
स्थिर ओळीत u32_t read_clidr(शून्य){
	u32_t clidr;
	यंत्र अस्थिर("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	परत clidr;
}


/* Read CSSELR, Cache Size Selection Register */
स्थिर ओळीत u32_t read_csselr(शून्य){
	u32_t csselr;
	यंत्र अस्थिर("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	परत csselr;
}

/* Write CSSELR, Cache Size Selection Register */
स्थिर ओळीत शून्य write_csselr(u32_t csselr){
	यंत्र अस्थिर("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
स्थिर ओळीत u32_t read_ccsidr(शून्य)
{
	u32_t ccsidr;
	यंत्र अस्थिर("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	परत ccsidr;
}

/* Read TLBTR, TLB Type Register */
स्थिर ओळीत u32_t read_tlbtr(शून्य)
{
	u32_t tlbtr;
	यंत्र अस्थिर("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	परत tlbtr;
}

/* keesj:move these out */
स्थिर ओळीत u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	पर्यंत( (t = t >> 1) ) counter ++;
	परत counter;
}

/* keesj:move these out */
स्थिर ओळीत u32_t ipow2(u32_t t)
{
	परत 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
स्थिर ओळीत शून्य dcache_maint(पूर्णांक type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  ओळ_size,ओळ_length;
	u32_t  संख्या_of_sets,संख्या_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	साठी (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		ओळ_size = ccsidr & 0x7;
		ओळ_length = 2 << (ओळ_size + 1) ; /* 2**(line_size + 2) */
		संख्या_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		संख्या_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(संख्या_of_ways);
		जर(ipow2(ilog2(संख्या_of_ways) < संख्या_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(ओळ_length);
		साठी (way =0 ; way < संख्या_of_ways; way++) {
			साठी (set =0 ; set < संख्या_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				जर (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					यंत्र अस्थिर("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} अन्यथा जर (type ==2 ){
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
स्थिर ओळीत शून्य dcache_clean(शून्य){
	dcache_maint(1);
}
स्थिर ओळीत शून्य dcache_invalidate(शून्य){
	dcache_maint(2);
}

स्थिर ओळीत शून्य refresh_tlb(शून्य)
{
	dsb();

	/* Invalidate entire unified TLB */
	यंत्र अस्थिर("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#जर 0
	/* Invalidate entire data TLB */
	यंत्र अस्थिर("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	यंत्र अस्थिर("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

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
स्थिर ओळीत u32_t read_sctlr(शून्य)
{
	u32_t ctl;

	यंत्र अस्थिर("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	परत ctl;
}

/* Write System Control Register */
स्थिर ओळीत शून्य write_sctlr(u32_t ctl)
{
	यंत्र अस्थिर("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
स्थिर ओळीत u32_t read_ttbr0(शून्य)
{
	u32_t bar;

	यंत्र अस्थिर("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	परत bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
स्थिर ओळीत शून्य write_ttbr0(u32_t bar)
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
स्थिर ओळीत शून्य reload_ttbr0(शून्य)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
स्थिर ओळीत u32_t read_ttbr1(शून्य)
{
	u32_t bar;

	यंत्र अस्थिर("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	परत bar;
}

/* Write Translation Table Base Register 1 */
स्थिर ओळीत शून्य write_ttbr1(u32_t bar)
{
	barrier();

	यंत्र अस्थिर("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
स्थिर ओळीत शून्य reload_ttbr1(शून्य)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
स्थिर ओळीत u32_t read_ttbcr(शून्य)
{
	u32_t bcr;

	यंत्र अस्थिर("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	परत bcr;
}

/* Write Translation Table Base Control Register */
स्थिर ओळीत शून्य write_ttbcr(u32_t bcr)
{
	यंत्र अस्थिर("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
स्थिर ओळीत u32_t read_dacr(शून्य)
{
	u32_t dacr;

	यंत्र अस्थिर("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	परत dacr;
}

/* Write Domain Access Control Register */
स्थिर ओळीत शून्य write_dacr(u32_t dacr)
{
	यंत्र अस्थिर("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
स्थिर ओळीत u32_t read_dfsr(शून्य)
{
	u32_t fsr;

	यंत्र अस्थिर("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	परत fsr;
}

/* Write Data Fault Status Register */
स्थिर ओळीत शून्य write_dfsr(u32_t fsr)
{
	यंत्र अस्थिर("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
स्थिर ओळीत u32_t read_ifsr(शून्य)
{
	u32_t fsr;

	यंत्र अस्थिर("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	परत fsr;
}

/* Write Instruction Fault Status Register */
स्थिर ओळीत शून्य write_ifsr(u32_t fsr)
{
	यंत्र अस्थिर("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
स्थिर ओळीत u32_t read_dfar(शून्य)
{
	u32_t far;

	यंत्र अस्थिर("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	परत far;
}

/* Write Data Fault Address Register */
स्थिर ओळीत शून्य write_dfar(u32_t far)
{
	यंत्र अस्थिर("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
स्थिर ओळीत u32_t read_ifar(शून्य)
{
	u32_t far;

	यंत्र अस्थिर("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	परत far;
}

/* Write Instruction Fault Address Register */
स्थिर ओळीत शून्य write_ifar(u32_t far)
{
	यंत्र अस्थिर("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
स्थिर ओळीत u32_t read_vbar(शून्य)
{
	u32_t vbar;

	यंत्र अस्थिर("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	परत vbar;
}

/* Write Vector Base Address Register */
स्थिर ओळीत शून्य write_vbar(u32_t vbar)
{
	यंत्र अस्थिर("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
स्थिर ओळीत u32_t read_midr(शून्य)
{
	u32_t id;

	यंत्र अस्थिर("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	परत id;
}

/* Read Auxiliary Control Register */
स्थिर ओळीत u32_t read_actlr(शून्य)
{
	u32_t ctl;

	यंत्र अस्थिर("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	परत ctl;
}

/* Write Auxiliary Control Register */
स्थिर ओळीत शून्य write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	यंत्र अस्थिर("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
स्थिर ओळीत u32_t read_cpsr(शून्य)
{
	u32_t status;

	यंत्र अस्थिर("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	परत status;
}

/* Write Current Program Status Register */
स्थिर ओळीत शून्य write_cpsr(u32_t status)
{
	यंत्र अस्थिर("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
