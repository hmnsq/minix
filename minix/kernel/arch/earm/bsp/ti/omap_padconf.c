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
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	अचिन्हित पूर्णाङ्क board_filter_मूल्य;
	अचिन्हित पूर्णाङ्क board_filter_mask;
};

स्थिर संरचना omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_मूल्य = BOARD_FILTER_BBXM_मूल्य,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_मूल्य = BOARD_FILTER_BB_मूल्य,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
स्थिर संरचना omap_padconf *omap_padconf;

स्थिर kern_phys_map padconf_phys_map;

पूर्णाङ्क
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t मूल्य)
{
	/* check that the value will be inside the padconf memory range */
	यदि (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		प्रत्यागम EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    मूल्य);

	प्रत्यागम OK;
}

शून्य
bsp_padconf_init(शून्य)
{
	पूर्णाङ्क x;
	omap_padconf = रिक्त;
	/* find the correct padconf */
	प्रति (x = 0; x < परिमाण(omap_padconfs) / परिमाण(omap_padconfs[0]); x++) {
		यदि ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_मूल्य) {
			omap_padconf = &omap_padconfs[x];
			भङ्ग;
		}
	}
	प्रतिज्ञा(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	प्रत्यागम;
}
