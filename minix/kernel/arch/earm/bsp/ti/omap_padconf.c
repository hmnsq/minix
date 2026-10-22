/* Implements sys_padconf() for the AM335X and DM37XX. */

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_proto.h"
#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ <machine/cpu.h>
#അന്തര്ഭാവ <minix/mmio.h>
#അന്തര്ഭാവ <minix/padconf.h>
#അന്തര്ഭാവ <minix/board.h>
#അന്തര്ഭാവ <minix/com.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <io.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "bsp_padconf.h"

ഘടന omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned പൂർണ്ണസംഖ്യ board_filter_മൂല്യം;
	unsigned പൂർണ്ണസംഖ്യ board_filter_mask;
};

സ്ഥിതം ഘടന omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_മൂല്യം = BOARD_FILTER_BBXM_മൂല്യം,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_മൂല്യം = BOARD_FILTER_BB_മൂല്യം,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
സ്ഥിതം ഘടന omap_padconf *omap_padconf;

സ്ഥിതം kern_phys_map padconf_phys_map;

പൂർണ്ണസംഖ്യ
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t മൂല്യം)
{
	/* check that the value will be inside the padconf memory range */
	എങ്കിൽ (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		തിരികെ EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    മൂല്യം);

	തിരികെ OK;
}

ശൂന്യം
bsp_padconf_init(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ x;
	omap_padconf = ശൂന്യ;
	/* find the correct padconf */
	വേണ്ടി (x = 0; x < വലുപ്പം(omap_padconfs) / വലുപ്പം(omap_padconfs[0]); x++) {
		എങ്കിൽ ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_മൂല്യം) {
			omap_padconf = &omap_padconfs[x];
			നിർത്തുക;
		}
	}
	പ്രതിജ്ഞാ(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	തിരികെ;
}
