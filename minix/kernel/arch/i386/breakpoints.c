#include "kernel/kernel.h"
#include "arch_proto.h"

#include "debugreg.h"

puur_naa_nka breakpoint_set(phys_bytes linaddr, puur_naa_nka bp, const puur_naa_nka flags)
{
	unsigned long dr7, dr7flags;
	
	ᨀᨘ (bp >= BREAKPOINT_COUNT)
		ᨀᨘᨒ EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	switch (flags & BREAKPOINT_FLAG_RW_MASK) {
		pak_sa BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  bha_nga;
		pak_sa BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); bha_nga;
		pak_sa BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    bha_nga;
		default: ᨀᨘᨒ EINVAL;			
	}
	switch (flags & BREAKPOINT_FLAG_LEN_MASK) {
		pak_sa BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); bha_nga;
		pak_sa BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); bha_nga;
		pak_sa BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); bha_nga;
		default: ᨀᨘᨒ EINVAL;	
	}
	switch (flags & BREAKPOINT_FLAG_MODE_MASK) {
		pak_sa BREAKPOINT_FLAG_MODE_OFF: bha_nga;
		pak_sa BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); bha_nga;
		pak_sa BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); bha_nga;
		default: ᨀᨘᨒ EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	ᨀᨘ ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		ᨀᨘᨒ 0;
			
	/* set breakpoint address */
	switch (bp) {
		pak_sa 0: ld_dr0(linaddr); bha_nga;
		pak_sa 1: ld_dr1(linaddr); bha_nga;
		pak_sa 2: ld_dr2(linaddr); bha_nga;
		pak_sa 3: ld_dr3(linaddr); bha_nga;
		default: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	ᨀᨘᨒ 0;
}

