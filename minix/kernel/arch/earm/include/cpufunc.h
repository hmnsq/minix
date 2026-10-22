#اگر_غیر_تعریف_شدہ _ARM_CPUFUNC_H
#تعریف_کرو _ARM_CPUFUNC_H

#اگر 0
/* check interrupt state */
ساکن اندرونی خالی check_int(غیرعلامتی صحیح state, صحیح سطر)
{
	غیرعلامتی صحیح cpsr = 0;

	asm متلون("mrs %0, cpsr" : "=r" (cpsr));

	اگر ((cpsr & PSR_F) != (state & PSR_F))
	    لکھو("%d: FIQs are unexpectedly %s\n", سطر, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	اگر ((cpsr & PSR_I) != (state & PSR_I))
	    لکھو("%d: IRQs are unexpectedly %s\n", سطر, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#اگر_ختم

/* Data memory barrier */
ساکن اندرونی خالی dmb(خالی)
{
	asm متلون("dmb" : : : "memory");
}

/* Data synchronization barrier */
ساکن اندرونی خالی dsb(خالی)
{
	asm متلون("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
ساکن اندرونی خالی isb(خالی)
{
	asm متلون("isb" : : : "memory");
}

ساکن اندرونی خالی barrier(خالی)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
ساکن اندرونی u32_t read_clidr(خالی){
	u32_t clidr;
	asm متلون("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	واپس clidr;
}


/* Read CSSELR, Cache Size Selection Register */
ساکن اندرونی u32_t read_csselr(خالی){
	u32_t csselr;
	asm متلون("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	واپس csselr;
}

/* Write CSSELR, Cache Size Selection Register */
ساکن اندرونی خالی write_csselr(u32_t csselr){
	asm متلون("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
ساکن اندرونی u32_t read_ccsidr(خالی)
{
	u32_t ccsidr;
	asm متلون("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	واپس ccsidr;
}

/* Read TLBTR, TLB Type Register */
ساکن اندرونی u32_t read_tlbtr(خالی)
{
	u32_t tlbtr;
	asm متلون("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	واپس tlbtr;
}

/* keesj:move these out */
ساکن اندرونی u32_t ilog2(u32_t t)
{
	u32_t شمار_2 =0;
	جب( (t = t >> 1) ) شمار_2 ++;
	واپس شمار_2;
}

/* keesj:move these out */
ساکن اندرونی u32_t ipow2(u32_t t)
{
	واپس 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
ساکن اندرونی خالی dcache_maint(صحیح type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  سطر_سائز,سطر_لمبائی;
	u32_t  نمبر_کا_sets,نمبر_کا_ways;
	u32_t  سیٹ,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	برائے (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		سطر_سائز = ccsidr & 0x7;
		سطر_لمبائی = 2 << (سطر_سائز + 1) ; /* 2**(line_size + 2) */
		نمبر_کا_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		نمبر_کا_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_بٹ = ilog2(نمبر_کا_ways);
		اگر(ipow2(ilog2(نمبر_کا_ways) < نمبر_کا_ways) ) {
			way_بٹ++;
		}

		u32_t l = ilog2(سطر_لمبائی);
		برائے (way =0 ; way < نمبر_کا_ways; way++) {
			برائے (سیٹ =0 ; سیٹ < نمبر_کا_sets; سیٹ++) {
				u32_t val = ( way << (32 - way_بٹ) ) |  (سیٹ << l) | (cache_level << 1 );
				اگر (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm متلون("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [سیٹ] "r" (val));
				} ورنہ اگر (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm متلون("mcr p15, 0, %[set], c7, c6, 2" 
							: : [سیٹ] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
ساکن اندرونی خالی dcache_clean(خالی){
	dcache_maint(1);
}
ساکن اندرونی خالی dcache_invalidate(خالی){
	dcache_maint(2);
}

ساکن اندرونی خالی refresh_tlb(خالی)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm متلون("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#اگر 0
	/* Invalidate entire data TLB */
	asm متلون("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm متلون("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#اگر_ختم

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm متلون("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm متلون("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
ساکن اندرونی u32_t read_sctlr(خالی)
{
	u32_t ctl;

	asm متلون("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	واپس ctl;
}

/* Write System Control Register */
ساکن اندرونی خالی write_sctlr(u32_t ctl)
{
	asm متلون("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
ساکن اندرونی u32_t read_ttbr0(خالی)
{
	u32_t bar;

	asm متلون("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	واپس bar & ARM_TTBR_ADDR_نقاب;
}

/* Write Translation Table Base Register 0 */
ساکن اندرونی خالی write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_نقاب ) | ARM_TTBR_FLAGS_CACHED;
	asm متلون("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
ساکن اندرونی خالی reload_ttbr0(خالی)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
ساکن اندرونی u32_t read_ttbr1(خالی)
{
	u32_t bar;

	asm متلون("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	واپس bar;
}

/* Write Translation Table Base Register 1 */
ساکن اندرونی خالی write_ttbr1(u32_t bar)
{
	barrier();

	asm متلون("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
ساکن اندرونی خالی reload_ttbr1(خالی)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
ساکن اندرونی u32_t read_ttbcr(خالی)
{
	u32_t bcr;

	asm متلون("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	واپس bcr;
}

/* Write Translation Table Base Control Register */
ساکن اندرونی خالی write_ttbcr(u32_t bcr)
{
	asm متلون("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
ساکن اندرونی u32_t read_dacr(خالی)
{
	u32_t dacr;

	asm متلون("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	واپس dacr;
}

/* Write Domain Access Control Register */
ساکن اندرونی خالی write_dacr(u32_t dacr)
{
	asm متلون("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
ساکن اندرونی u32_t read_dfsr(خالی)
{
	u32_t fsr;

	asm متلون("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	واپس fsr;
}

/* Write Data Fault Status Register */
ساکن اندرونی خالی write_dfsr(u32_t fsr)
{
	asm متلون("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
ساکن اندرونی u32_t read_ifsr(خالی)
{
	u32_t fsr;

	asm متلون("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	واپس fsr;
}

/* Write Instruction Fault Status Register */
ساکن اندرونی خالی write_ifsr(u32_t fsr)
{
	asm متلون("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
ساکن اندرونی u32_t read_dfar(خالی)
{
	u32_t far;

	asm متلون("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	واپس far;
}

/* Write Data Fault Address Register */
ساکن اندرونی خالی write_dfar(u32_t far)
{
	asm متلون("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
ساکن اندرونی u32_t read_ifar(خالی)
{
	u32_t far;

	asm متلون("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	واپس far;
}

/* Write Instruction Fault Address Register */
ساکن اندرونی خالی write_ifar(u32_t far)
{
	asm متلون("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
ساکن اندرونی u32_t read_vbar(خالی)
{
	u32_t vbar;

	asm متلون("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	واپس vbar;
}

/* Write Vector Base Address Register */
ساکن اندرونی خالی write_vbar(u32_t vbar)
{
	asm متلون("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
ساکن اندرونی u32_t read_midr(خالی)
{
	u32_t id;

	asm متلون("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	واپس id;
}

/* Read Auxiliary Control Register */
ساکن اندرونی u32_t read_actlr(خالی)
{
	u32_t ctl;

	asm متلون("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	واپس ctl;
}

/* Write Auxiliary Control Register */
ساکن اندرونی خالی write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm متلون("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
ساکن اندرونی u32_t read_cpsr(خالی)
{
	u32_t حالت_2;

	asm متلون("mrs %[status], cpsr @ read CPSR"
			: [حالت_2] "=r" (حالت_2));

	واپس حالت_2;
}

/* Write Current Program Status Register */
ساکن اندرونی خالی write_cpsr(u32_t حالت_2)
{
	asm متلون("msr cpsr_c, %[status] @ write CPSR"
			: : [حالت_2] "r" (حالت_2));
}

#اگر_ختم /* _ARM_CPUFUNC_H */
