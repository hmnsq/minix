/* Implements sys_padconf() for the AM335X and DM37XX. */

#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "arch_proto.h"
#அந்தர்பாவ <sys/types.h>
#அந்தர்பாவ <machine/cpu.h>
#அந்தர்பாவ <minix/mmio.h>
#அந்தர்பாவ <minix/padconf.h>
#அந்தர்பாவ <minix/board.h>
#அந்தர்பாவ <minix/com.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <io.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "bsp_padconf.h"

கட்டமைப்பு omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned முழு board_filter_மதிப்பு;
	unsigned முழு board_filter_mask;
};

நிலையான கட்டமைப்பு omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_மதிப்பு = BOARD_FILTER_BBXM_மதிப்பு,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_மதிப்பு = BOARD_FILTER_BB_மதிப்பு,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
நிலையான கட்டமைப்பு omap_padconf *omap_padconf;

நிலையான kern_phys_map padconf_phys_map;

முழு
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t மதிப்பு)
{
	/* check that the value will be inside the padconf memory range */
	எனில் (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		திரும்பு EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    மதிப்பு);

	திரும்பு OK;
}

வெறுமை
bsp_padconf_init(வெறுமை)
{
	முழு x;
	omap_padconf = வெற்று;
	/* find the correct padconf */
	க்கு (x = 0; x < அளவு(omap_padconfs) / அளவு(omap_padconfs[0]); x++) {
		எனில் ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_மதிப்பு) {
			omap_padconf = &omap_padconfs[x];
			நிறுத்து;
		}
	}
	ப்ரதிஜ்ஞா(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	திரும்பு;
}
