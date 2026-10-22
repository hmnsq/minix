/* Implements sys_padconf() for the AM335X and DM37XX. */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <minix/mmio.h>
#ಅನ್ತರ್ಭಾವ <minix/padconf.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <minix/com.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <io.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "bsp_padconf.h"

ಸಂರಚನಾ omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned ಪೂರ್ಣಾಙ್ಕ board_filter_ಮೌಲ್ಯ;
	unsigned ಪೂರ್ಣಾಙ್ಕ board_filter_mask;
};

ಸ್ಥಿರ ಸಂರಚನಾ omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_ಮೌಲ್ಯ = BOARD_FILTER_BBXM_ಮೌಲ್ಯ,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_ಮೌಲ್ಯ = BOARD_FILTER_BB_ಮೌಲ್ಯ,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
ಸ್ಥಿರ ಸಂರಚನಾ omap_padconf *omap_padconf;

ಸ್ಥಿರ kern_phys_map padconf_phys_map;

ಪೂರ್ಣಾಙ್ಕ
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t ಮೌಲ್ಯ)
{
	/* check that the value will be inside the padconf memory range */
	ಯದಿ (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		ಪ್ರತ್ಯಾಗಮ EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    ಮೌಲ್ಯ);

	ಪ್ರತ್ಯಾಗಮ OK;
}

ಶೂನ್ಯ
bsp_padconf_init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ x;
	omap_padconf = ರಿಕ್ತ;
	/* find the correct padconf */
	ಪ್ರತಿ (x = 0; x < ಪರಿಮಾಣ(omap_padconfs) / ಪರಿಮಾಣ(omap_padconfs[0]); x++) {
		ಯದಿ ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_ಮೌಲ್ಯ) {
			omap_padconf = &omap_padconfs[x];
			ಭಙ್ಗ;
		}
	}
	ಪ್ರತಿಜ್ಞಾ(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	ಪ್ರತ್ಯಾಗಮ;
}
