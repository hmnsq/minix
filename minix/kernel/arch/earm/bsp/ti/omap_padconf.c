/* Implements sys_padconf() for the AM335X and DM37XX. */

#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "arch_proto.h"
#অন্তর্ভাব <sys/types.h>
#অন্তর্ভাব <machine/cpu.h>
#অন্তর্ভাব <minix/mmio.h>
#অন্তর্ভাব <minix/padconf.h>
#অন্তর্ভাব <minix/board.h>
#অন্তর্ভাব <minix/com.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <io.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "bsp_padconf.h"

সংরচনা omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned পূর্ণাঙ্ক board_filter_মান;
	unsigned পূর্ণাঙ্ক board_filter_mask;
};

স্থির সংরচনা omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_মান = BOARD_FILTER_BBXM_মান,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_মান = BOARD_FILTER_BB_মান,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
স্থির সংরচনা omap_padconf *omap_padconf;

স্থির kern_phys_map padconf_phys_map;

পূর্ণাঙ্ক
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t মান)
{
	/* check that the value will be inside the padconf memory range */
	যদি (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		প্রত্যাগম EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    মান);

	প্রত্যাগম OK;
}

শূন্য
bsp_padconf_init(শূন্য)
{
	পূর্ণাঙ্ক x;
	omap_padconf = রিক্ত;
	/* find the correct padconf */
	প্রতি (x = 0; x < পরিমাণ(omap_padconfs) / পরিমাণ(omap_padconfs[0]); x++) {
		যদি ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_মান) {
			omap_padconf = &omap_padconfs[x];
			ভঙ্গ;
		}
	}
	প্রতিজ্ঞা(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	প্রত্যাগম;
}
