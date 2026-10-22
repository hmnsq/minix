#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "debugreg.h"

ਪੂਰਨ breakpoint_ਸੈੱਟ(phys_ਬਾਈਟਾਂ linaddr, ਪੂਰਨ bp, ਅਚਲ ਪੂਰਨ flags)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ dr7, dr7flags;
	
	ਜੇ (bp >= BREAKPOINT_ਗਿਣਤੀ)
		ਵਾਪਸ EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	ਚੋਣ (flags & BREAKPOINT_FLAG_RW_ਨਕਾਬ) {
		ਹਾਲਤ BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  ਰੋਕੋ;
		ਹਾਲਤ BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); ਰੋਕੋ;
		ਹਾਲਤ BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    ਰੋਕੋ;
		ਮੂਲ: ਵਾਪਸ EINVAL;			
	}
	ਚੋਣ (flags & BREAKPOINT_FLAG_LEN_ਨਕਾਬ) {
		ਹਾਲਤ BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); ਰੋਕੋ;
		ਹਾਲਤ BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); ਰੋਕੋ;
		ਹਾਲਤ BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); ਰੋਕੋ;
		ਮੂਲ: ਵਾਪਸ EINVAL;	
	}
	ਚੋਣ (flags & BREAKPOINT_FLAG_ਬਹੁਲਕ_ਨਕਾਬ) {
		ਹਾਲਤ BREAKPOINT_FLAG_ਬਹੁਲਕ_OFF: ਰੋਕੋ;
		ਹਾਲਤ BREAKPOINT_FLAG_ਬਹੁਲਕ_LOCAL: dr7flags |= DR7_L(bp); ਰੋਕੋ;
		ਹਾਲਤ BREAKPOINT_FLAG_ਬਹੁਲਕ_GLOBAL: dr7flags |= DR7_G(bp); ਰੋਕੋ;
		ਮੂਲ: ਵਾਪਸ EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_ਨਕਾਬ(bp) | DR7_LN_ਨਕਾਬ(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	ਜੇ ((flags & BREAKPOINT_FLAG_ਬਹੁਲਕ_ਨਕਾਬ) == BREAKPOINT_FLAG_ਬਹੁਲਕ_OFF)
		ਵਾਪਸ 0;
			
	/* set breakpoint address */
	ਚੋਣ (bp) {
		ਹਾਲਤ 0: ld_dr0(linaddr); ਰੋਕੋ;
		ਹਾਲਤ 1: ld_dr1(linaddr); ਰੋਕੋ;
		ਹਾਲਤ 2: ld_dr2(linaddr); ਰੋਕੋ;
		ਹਾਲਤ 3: ld_dr3(linaddr); ਰੋਕੋ;
		ਮੂਲ: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	ਵਾਪਸ 0;
}

