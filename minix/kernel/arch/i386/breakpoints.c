#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "arch_proto.h"

#অন্তর্ভাব "debugreg.h"

পূর্ণাঙ্ক breakpoint_set(phys_bytes linaddr, পূর্ণাঙ্ক bp, ধ্রুব পূর্ণাঙ্ক flags)
{
	unsigned long dr7, dr7flags;
	
	যদি (bp >= BREAKPOINT_গুনতি)
		প্রত্যাগম EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	বিকল্প (flags & BREAKPOINT_FLAG_RW_MASK) {
		পক্ষ BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  ভঙ্গ;
		পক্ষ BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); ভঙ্গ;
		পক্ষ BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    ভঙ্গ;
		স্বভাব: প্রত্যাগম EINVAL;			
	}
	বিকল্প (flags & BREAKPOINT_FLAG_LEN_MASK) {
		পক্ষ BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); ভঙ্গ;
		পক্ষ BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); ভঙ্গ;
		পক্ষ BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); ভঙ্গ;
		স্বভাব: প্রত্যাগম EINVAL;	
	}
	বিকল্প (flags & BREAKPOINT_FLAG_MODE_MASK) {
		পক্ষ BREAKPOINT_FLAG_MODE_OFF: ভঙ্গ;
		পক্ষ BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); ভঙ্গ;
		পক্ষ BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); ভঙ্গ;
		স্বভাব: প্রত্যাগম EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	যদি ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		প্রত্যাগম 0;
			
	/* set breakpoint address */
	বিকল্প (bp) {
		পক্ষ 0: ld_dr0(linaddr); ভঙ্গ;
		পক্ষ 1: ld_dr1(linaddr); ভঙ্গ;
		পক্ষ 2: ld_dr2(linaddr); ভঙ্গ;
		পক্ষ 3: ld_dr3(linaddr); ভঙ্গ;
		স্বভাব: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	প্রত্যাগম 0;
}

