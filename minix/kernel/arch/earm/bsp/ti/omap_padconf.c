/* Implements sys_padconf() for the AM335X and DM37XX. */

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ <sys/types.h>
#અન્તર્ભાવ <machine/cpu.h>
#અન્તર્ભાવ <minix/mmio.h>
#અન્તર્ભાવ <minix/padconf.h>
#અન્તર્ભાવ <minix/board.h>
#અન્તર્ભાવ <minix/com.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <io.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "bsp_padconf.h"

સંરચના omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned પૂર્ણાંક board_filter_કિંમત;
	unsigned પૂર્ણાંક board_filter_mask;
};

સ્થિર સંરચના omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_કિંમત = BOARD_FILTER_BBXM_કિંમત,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_કિંમત = BOARD_FILTER_BB_કિંમત,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
સ્થિર સંરચના omap_padconf *omap_padconf;

સ્થિર kern_phys_map padconf_phys_map;

પૂર્ણાંક
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t કિંમત)
{
	/* check that the value will be inside the padconf memory range */
	જો (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		પાછા EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    કિંમત);

	પાછા OK;
}

શૂન્ય
bsp_padconf_init(શૂન્ય)
{
	પૂર્ણાંક x;
	omap_padconf = ખાલી;
	/* find the correct padconf */
	માટે (x = 0; x < કદ(omap_padconfs) / કદ(omap_padconfs[0]); x++) {
		જો ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_કિંમત) {
			omap_padconf = &omap_padconfs[x];
			તોડો;
		}
	}
	પ્રતિજ્ઞા(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	પાછા;
}
