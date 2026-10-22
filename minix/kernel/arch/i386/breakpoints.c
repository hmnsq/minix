#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"

#ضمن "debugreg.h"

صحيح breakpoint_set(phys_bytes linaddr, صحيح bp, ثابت صحيح flags)
{
	unsigned long dr7, dr7flags;
	
	إذا (bp >= BREAKPOINT_گنتی)
		أرجع EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	بدل (flags & BREAKPOINT_FLAG_RW_MASK) {
		حالة BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  اكسر;
		حالة BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); اكسر;
		حالة BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    اكسر;
		افتراضي: أرجع EINVAL;			
	}
	بدل (flags & BREAKPOINT_FLAG_LEN_MASK) {
		حالة BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); اكسر;
		حالة BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); اكسر;
		حالة BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); اكسر;
		افتراضي: أرجع EINVAL;	
	}
	بدل (flags & BREAKPOINT_FLAG_MODE_MASK) {
		حالة BREAKPOINT_FLAG_MODE_OFF: اكسر;
		حالة BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); اكسر;
		حالة BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); اكسر;
		افتراضي: أرجع EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	إذا ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		أرجع 0;
			
	/* set breakpoint address */
	بدل (bp) {
		حالة 0: ld_dr0(linaddr); اكسر;
		حالة 1: ld_dr1(linaddr); اكسر;
		حالة 2: ld_dr2(linaddr); اكسر;
		حالة 3: ld_dr3(linaddr); اكسر;
		افتراضي: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	أرجع 0;
}

