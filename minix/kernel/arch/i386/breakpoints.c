#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "arch_proto.h"

#অন্তর্ভাব "debugreg.h"

পূর্ণসংখ্যা breakpoint_set(phys_bytes linaddr, পূর্ণসংখ্যা bp, ধ্রুবক পূর্ণসংখ্যা flags)
{
	unsigned long dr7, dr7flags;
	
	যদি (bp >= BREAKPOINT_গুনতি)
		ফেরত EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	নির্বাচন (flags & BREAKPOINT_FLAG_RW_MASK) {
		ক্ষেত্র BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  বিরতি;
		ক্ষেত্র BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); বিরতি;
		ক্ষেত্র BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    বিরতি;
		পূর্বনির্ধারিত: ফেরত EINVAL;			
	}
	নির্বাচন (flags & BREAKPOINT_FLAG_LEN_MASK) {
		ক্ষেত্র BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); বিরতি;
		ক্ষেত্র BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); বিরতি;
		ক্ষেত্র BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); বিরতি;
		পূর্বনির্ধারিত: ফেরত EINVAL;	
	}
	নির্বাচন (flags & BREAKPOINT_FLAG_MODE_MASK) {
		ক্ষেত্র BREAKPOINT_FLAG_MODE_OFF: বিরতি;
		ক্ষেত্র BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); বিরতি;
		ক্ষেত্র BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); বিরতি;
		পূর্বনির্ধারিত: ফেরত EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	যদি ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		ফেরত 0;
			
	/* set breakpoint address */
	নির্বাচন (bp) {
		ক্ষেত্র 0: ld_dr0(linaddr); বিরতি;
		ক্ষেত্র 1: ld_dr1(linaddr); বিরতি;
		ক্ষেত্র 2: ld_dr2(linaddr); বিরতি;
		ক্ষেত্র 3: ld_dr3(linaddr); বিরতি;
		পূর্বনির্ধারিত: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	ফেরত 0;
}

