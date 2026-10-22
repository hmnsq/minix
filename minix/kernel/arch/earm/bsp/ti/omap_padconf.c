/* Implements sys_padconf() for the AM335X and DM37XX. */

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"
#ଅନ୍ତର୍ଭାଵ <sys/types.h>
#ଅନ୍ତର୍ଭାଵ <machine/cpu.h>
#ଅନ୍ତର୍ଭାଵ <minix/mmio.h>
#ଅନ୍ତର୍ଭାଵ <minix/padconf.h>
#ଅନ୍ତର୍ଭାଵ <minix/board.h>
#ଅନ୍ତର୍ଭାଵ <minix/com.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <io.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "bsp_padconf.h"

ସଂରଚନା omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା board_filter_ମୂଲ୍ୟ;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା board_filter_mask;
};

ସ୍ଥିର ସଂରଚନା omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_ମୂଲ୍ୟ = BOARD_FILTER_BBXM_ମୂଲ୍ୟ,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_ମୂଲ୍ୟ = BOARD_FILTER_BB_ମୂଲ୍ୟ,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
ସ୍ଥିର ସଂରଚନା omap_padconf *omap_padconf;

ସ୍ଥିର kern_phys_map padconf_phys_map;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t ମୂଲ୍ୟ)
{
	/* check that the value will be inside the padconf memory range */
	ଯଦି (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		ଫେରନ୍ତୁ EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    ମୂଲ୍ୟ);

	ଫେରନ୍ତୁ OK;
}

ଶୂନ୍ୟ
bsp_padconf_init(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା x;
	omap_padconf = ଖାଲି;
	/* find the correct padconf */
	ପାଇଁ (x = 0; x < ଆକାର(omap_padconfs) / ଆକାର(omap_padconfs[0]); x++) {
		ଯଦି ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_ମୂଲ୍ୟ) {
			omap_padconf = &omap_padconfs[x];
			ଭାଙ୍ଗ;
		}
	}
	ପ୍ରତିଜ୍ଞା(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	ଫେରନ୍ତୁ;
}
