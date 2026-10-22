/* Implements sys_padconf() for the AM335X and DM37XX. */

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव <sys/types.h>
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <minix/mmio.h>
#अन्तर्भाव <minix/padconf.h>
#अन्तर्भाव <minix/board.h>
#अन्तर्भाव <minix/com.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <io.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "bsp_padconf.h"

संरचना omap_padconf
{
	vir_अष्टकानि आधारः;
	vir_अष्टकानि विस्थापनम्;
	vir_अष्टकानि आकार;
	अचिह्नित पूर्णाङ्क board_filter_मानम्;
	अचिह्नित पूर्णाङ्क board_filter_आवरणम्;
};

स्थिर संरचना omap_padconf omap_padconfs[] = {
	{
		    .आधारः = PADCONF_DM37_XX_REGISTERS_आधारः,
		    .विस्थापनम् = PADCONF_DM37_XX_REGISTERS_विस्थापनम्,
		    .आकार = PADCONF_DM37_XX_REGISTERS_आकार,
		    .board_filter_मानम् = BOARD_FILTER_BBXM_मानम्,
		    .board_filter_आवरणम् = BOARD_FILTER_BBXM_आवरणम्,
	    },
	{
		    .आधारः = PADCONF_AM335_X_REGISTERS_आधारः,
		    .विस्थापनम् = PADCONF_AM335_X_REGISTERS_विस्थापनम्,
		    .आकार = PADCONF_AM335_X_REGISTERS_आकार,
		    .board_filter_मानम् = BOARD_FILTER_BB_मानम्,
		    .board_filter_आवरणम् = BOARD_FILTER_BB_आवरणम्,
	    },
};

/* initialized in init */
स्थिर संरचना omap_padconf *omap_padconf;

स्थिर kern_phys_map padconf_phys_map;

पूर्णाङ्क
bsp_padconf_समुच्चयः(u32_t padconf, u32_t आवरणम्, u32_t मानम्)
{
	/* check that the value will be inside the padconf memory range */
	यदि (padconf >= (omap_padconf->आकार - omap_padconf->विस्थापनम्)) {
		प्रत्यागम EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->आधारः + omap_padconf->विस्थापनम्, आवरणम्,
	    मानम्);

	प्रत्यागम OK;
}

शून्य
bsp_padconf_init(शून्य)
{
	पूर्णाङ्क x;
	omap_padconf = रिक्त;
	/* find the correct padconf */
	प्रति (x = 0; x < परिमाण(omap_padconfs) / परिमाण(omap_padconfs[0]); x++) {
		यदि ((omap_padconfs[x].board_filter_आवरणम् & machine.board_id) ==
		    omap_padconfs[x].board_filter_मानम्) {
			omap_padconf = &omap_padconfs[x];
			भङ्ग;
		}
	}
	प्रतिज्ञा(omap_padconf);

	kern_phys_map_ptr(omap_padconf->आधारः, omap_padconf->आकार,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_अष्टकानि) & omap_padconf->आधारः);

	प्रत्यागम;
}
