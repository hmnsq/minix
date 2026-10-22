/* Implements sys_padconf() for the AM335X and DM37XX. */

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <minix/mmio.h>
#అన్తర్భావ <minix/padconf.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <minix/com.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <io.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdio.h>

#అన్తర్భావ "bsp_padconf.h"

నిర్మాణం omap_padconf
{
	vir_బైట్లు పునాది;
	vir_బైట్లు స్థానభ్రంశం;
	vir_బైట్లు కొలత;
	గుర్తులేని పూర్ణాంకం board_filter_విలువ;
	గుర్తులేని పూర్ణాంకం board_filter_ముసుగు;
};

స్థిర నిర్మాణం omap_padconf omap_padconfs[] = {
	{
		    .పునాది = PADCONF_DM37_XX_REGISTERS_పునాది,
		    .స్థానభ్రంశం = PADCONF_DM37_XX_REGISTERS_స్థానభ్రంశం,
		    .కొలత = PADCONF_DM37_XX_REGISTERS_కొలత,
		    .board_filter_విలువ = BOARD_FILTER_BBXM_విలువ,
		    .board_filter_ముసుగు = BOARD_FILTER_BBXM_ముసుగు,
	    },
	{
		    .పునాది = PADCONF_AM335_X_REGISTERS_పునాది,
		    .స్థానభ్రంశం = PADCONF_AM335_X_REGISTERS_స్థానభ్రంశం,
		    .కొలత = PADCONF_AM335_X_REGISTERS_కొలత,
		    .board_filter_విలువ = BOARD_FILTER_BB_విలువ,
		    .board_filter_ముసుగు = BOARD_FILTER_BB_ముసుగు,
	    },
};

/* initialized in init */
స్థిర నిర్మాణం omap_padconf *omap_padconf;

స్థిర kern_phys_map padconf_phys_map;

పూర్ణాంకం
bsp_padconf_సమితి(u32_t padconf, u32_t ముసుగు, u32_t విలువ)
{
	/* check that the value will be inside the padconf memory range */
	ఒకవేళ (padconf >= (omap_padconf->కొలత - omap_padconf->స్థానభ్రంశం)) {
		తిరిగి EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->పునాది + omap_padconf->స్థానభ్రంశం, ముసుగు,
	    విలువ);

	తిరిగి OK;
}

శూన్య
bsp_padconf_init(శూన్య)
{
	పూర్ణాంకం x;
	omap_padconf = ఖాళీ;
	/* find the correct padconf */
	కొరకు (x = 0; x < పరిమాణం(omap_padconfs) / పరిమాణం(omap_padconfs[0]); x++) {
		ఒకవేళ ((omap_padconfs[x].board_filter_ముసుగు & machine.board_id) ==
		    omap_padconfs[x].board_filter_విలువ) {
			omap_padconf = &omap_padconfs[x];
			ఆపు;
		}
	}
	ప్రతిజ్ఞా(omap_padconf);

	kern_phys_map_ptr(omap_padconf->పునాది, omap_padconf->కొలత,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_బైట్లు) & omap_padconf->పునాది);

	తిరిగి;
}
