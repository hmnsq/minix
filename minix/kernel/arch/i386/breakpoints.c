#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "arch_proto.h"

#அந்தர்பாவ "debugreg.h"

முழு breakpoint_set(phys_bytes linaddr, முழு bp, மாறிலி முழு flags)
{
	unsigned long dr7, dr7flags;
	
	எனில் (bp >= BREAKPOINT_மொத்தம்)
		திரும்பு EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	தேர்வு (flags & BREAKPOINT_FLAG_RW_MASK) {
		நிலை BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  நிறுத்து;
		நிலை BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); நிறுத்து;
		நிலை BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    நிறுத்து;
		இயல்பு: திரும்பு EINVAL;			
	}
	தேர்வு (flags & BREAKPOINT_FLAG_LEN_MASK) {
		நிலை BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); நிறுத்து;
		நிலை BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); நிறுத்து;
		நிலை BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); நிறுத்து;
		இயல்பு: திரும்பு EINVAL;	
	}
	தேர்வு (flags & BREAKPOINT_FLAG_MODE_MASK) {
		நிலை BREAKPOINT_FLAG_MODE_OFF: நிறுத்து;
		நிலை BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); நிறுத்து;
		நிலை BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); நிறுத்து;
		இயல்பு: திரும்பு EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	எனில் ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		திரும்பு 0;
			
	/* set breakpoint address */
	தேர்வு (bp) {
		நிலை 0: ld_dr0(linaddr); நிறுத்து;
		நிலை 1: ld_dr1(linaddr); நிறுத்து;
		நிலை 2: ld_dr2(linaddr); நிறுத்து;
		நிலை 3: ld_dr3(linaddr); நிறுத்து;
		இயல்பு: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	திரும்பு 0;
}

