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

কাঠামো omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes আকার_2;
	unsigned পূর্ণসংখ্যা board_filter_মান;
	unsigned পূর্ণসংখ্যা board_filter_mask;
};

স্থির কাঠামো omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .আকার_2 = PADCONF_DM37_XX_REGISTERS_আকার,
		    .board_filter_মান = BOARD_FILTER_BBXM_মান,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .আকার_2 = PADCONF_AM335_X_REGISTERS_আকার,
		    .board_filter_মান = BOARD_FILTER_BB_মান,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
স্থির কাঠামো omap_padconf *omap_padconf;

স্থির kern_phys_map padconf_phys_map;

পূর্ণসংখ্যা
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t মান)
{
	/* check that the value will be inside the padconf memory range */
	যদি (padconf >= (omap_padconf->আকার_2 - omap_padconf->offset)) {
		ফেরত EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    মান);

	ফেরত OK;
}

শূন্য
bsp_padconf_init(শূন্য)
{
	পূর্ণসংখ্যা x;
	omap_padconf = ফাঁকা;
	/* find the correct padconf */
	জন্য (x = 0; x < আকার(omap_padconfs) / আকার(omap_padconfs[0]); x++) {
		যদি ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_মান) {
			omap_padconf = &omap_padconfs[x];
			বিরতি;
		}
	}
	প্রতিজ্ঞা(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->আকার_2,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	ফেরত;
}
