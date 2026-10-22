#شامل "kernel/kernel.h"
#شامل "arch_proto.h"

#شامل "debugreg.h"

صحیح breakpoint_set(phys_bytes linaddr, صحیح bp, ثابت صحیح flags)
{
	unsigned long dr7, dr7flags;
	
	اگر (bp >= BREAKPOINT_شمار)
		بازگشت EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	انتخاب (flags & BREAKPOINT_FLAG_RW_MASK) {
		حالت BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  بشکن;
		حالت BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); بشکن;
		حالت BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    بشکن;
		پیشفرض: بازگشت EINVAL;			
	}
	انتخاب (flags & BREAKPOINT_FLAG_LEN_MASK) {
		حالت BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); بشکن;
		حالت BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); بشکن;
		حالت BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); بشکن;
		پیشفرض: بازگشت EINVAL;	
	}
	انتخاب (flags & BREAKPOINT_FLAG_MODE_MASK) {
		حالت BREAKPOINT_FLAG_MODE_OFF: بشکن;
		حالت BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); بشکن;
		حالت BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); بشکن;
		پیشفرض: بازگشت EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	اگر ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		بازگشت 0;
			
	/* set breakpoint address */
	انتخاب (bp) {
		حالت 0: ld_dr0(linaddr); بشکن;
		حالت 1: ld_dr1(linaddr); بشکن;
		حالت 2: ld_dr2(linaddr); بشکن;
		حالت 3: ld_dr3(linaddr); بشکن;
		پیشفرض: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	بازگشت 0;
}

