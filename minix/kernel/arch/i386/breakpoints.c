#شامل "kernel/kernel.h"
#شامل "arch_proto.h"

#شامل "debugreg.h"

صحیح breakpoint_سیٹ(phys_بائٹیں linaddr, صحیح bp, ثابت صحیح flags)
{
	غیرعلامتی طویل dr7, dr7flags;
	
	اگر (bp >= BREAKPOINT_گنتی)
		واپس EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	انتخاب (flags & BREAKPOINT_FLAG_RW_نقاب) {
		حالت BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  توڑو;
		حالت BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); توڑو;
		حالت BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    توڑو;
		طےشدہ: واپس EINVAL;			
	}
	انتخاب (flags & BREAKPOINT_FLAG_LEN_نقاب) {
		حالت BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); توڑو;
		حالت BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); توڑو;
		حالت BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); توڑو;
		طےشدہ: واپس EINVAL;	
	}
	انتخاب (flags & BREAKPOINT_FLAG_منوال_نقاب) {
		حالت BREAKPOINT_FLAG_منوال_OFF: توڑو;
		حالت BREAKPOINT_FLAG_منوال_LOCAL: dr7flags |= DR7_L(bp); توڑو;
		حالت BREAKPOINT_FLAG_منوال_GLOBAL: dr7flags |= DR7_G(bp); توڑو;
		طےشدہ: واپس EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_نقاب(bp) | DR7_LN_نقاب(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	اگر ((flags & BREAKPOINT_FLAG_منوال_نقاب) == BREAKPOINT_FLAG_منوال_OFF)
		واپس 0;
			
	/* set breakpoint address */
	انتخاب (bp) {
		حالت 0: ld_dr0(linaddr); توڑو;
		حالت 1: ld_dr1(linaddr); توڑو;
		حالت 2: ld_dr2(linaddr); توڑو;
		حالت 3: ld_dr3(linaddr); توڑو;
		طےشدہ: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	واپس 0;
}

