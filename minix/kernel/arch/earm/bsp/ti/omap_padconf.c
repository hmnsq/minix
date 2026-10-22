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

ರಚನೆ omap_padconf
{
	vir_ಬೈಟುಗಳು ತಳ;
	vir_ಬೈಟುಗಳು ಸ್ಥಾನಾಂತರ;
	vir_ಬೈಟುಗಳು ಗಾತ್ರ_2;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ board_filter_ಮೌಲ್ಯ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ board_filter_ಮುಖವಾಡ;
};

ಸ್ಥಿರ ರಚನೆ omap_padconf omap_padconfs[] = {
	{
		    .ತಳ = PADCONF_DM37_XX_REGISTERS_ತಳ,
		    .ಸ್ಥಾನಾಂತರ = PADCONF_DM37_XX_REGISTERS_ಸ್ಥಾನಾಂತರ,
		    .ಗಾತ್ರ_2 = PADCONF_DM37_XX_REGISTERS_ಗಾತ್ರ,
		    .board_filter_ಮೌಲ್ಯ = BOARD_FILTER_BBXM_ಮೌಲ್ಯ,
		    .board_filter_ಮುಖವಾಡ = BOARD_FILTER_BBXM_ಮುಖವಾಡ,
	    },
	{
		    .ತಳ = PADCONF_AM335_X_REGISTERS_ತಳ,
		    .ಸ್ಥಾನಾಂತರ = PADCONF_AM335_X_REGISTERS_ಸ್ಥಾನಾಂತರ,
		    .ಗಾತ್ರ_2 = PADCONF_AM335_X_REGISTERS_ಗಾತ್ರ,
		    .board_filter_ಮೌಲ್ಯ = BOARD_FILTER_BB_ಮೌಲ್ಯ,
		    .board_filter_ಮುಖವಾಡ = BOARD_FILTER_BB_ಮುಖವಾಡ,
	    },
};

/* initialized in init */
ಸ್ಥಿರ ರಚನೆ omap_padconf *omap_padconf;

ಸ್ಥಿರ kern_phys_map padconf_phys_map;

ಪೂರ್ಣಾಂಕ
bsp_padconf_ಗಣ(u32_t padconf, u32_t ಮುಖವಾಡ, u32_t ಮೌಲ್ಯ)
{
	/* check that the value will be inside the padconf memory range */
	ಆದರೆ (padconf >= (omap_padconf->ಗಾತ್ರ_2 - omap_padconf->ಸ್ಥಾನಾಂತರ)) {
		ಹಿಂತಿರುಗಿ EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->ತಳ + omap_padconf->ಸ್ಥಾನಾಂತರ, ಮುಖವಾಡ,
	    ಮೌಲ್ಯ);

	ಹಿಂತಿರುಗಿ OK;
}

ಶೂನ್ಯ
bsp_padconf_init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ x;
	omap_padconf = ಖಾಲಿ;
	/* find the correct padconf */
	ಗಾಗಿ (x = 0; x < ಗಾತ್ರ(omap_padconfs) / ಗಾತ್ರ(omap_padconfs[0]); x++) {
		ಆದರೆ ((omap_padconfs[x].board_filter_ಮುಖವಾಡ & machine.board_id) ==
		    omap_padconfs[x].board_filter_ಮೌಲ್ಯ) {
			omap_padconf = &omap_padconfs[x];
			ನಿಲ್ಲಿಸು;
		}
	}
	ಪ್ರತಿಜ್ಞಾ(omap_padconf);

	kern_phys_map_ptr(omap_padconf->ತಳ, omap_padconf->ಗಾತ್ರ_2,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_ಬೈಟುಗಳು) & omap_padconf->ತಳ);

	ಹಿಂತಿರುಗಿ;
}
