/* Implements sys_padconf() for the AM335X and DM37XX. */

#شامل "kernel/kernel.h"
#شامل "arch_proto.h"
#شامل <sys/types.h>
#شامل <machine/cpu.h>
#شامل <minix/mmio.h>
#شامل <minix/padconf.h>
#شامل <minix/board.h>
#شامل <minix/com.h>
#شامل <assert.h>
#شامل <io.h>
#شامل <stdlib.h>
#شامل <stdio.h>

#شامل "bsp_padconf.h"

ساختار omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned صحیح board_filter_مقدار;
	unsigned صحیح board_filter_mask;
};

ایستا ساختار omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_مقدار = BOARD_FILTER_BBXM_مقدار,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_مقدار = BOARD_FILTER_BB_مقدار,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
ایستا ساختار omap_padconf *omap_padconf;

ایستا kern_phys_map padconf_phys_map;

صحیح
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t مقدار)
{
	/* check that the value will be inside the padconf memory range */
	اگر (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		بازگشت EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    مقدار);

	بازگشت OK;
}

تهی
bsp_padconf_init(تهی)
{
	صحیح x;
	omap_padconf = پوچ;
	/* find the correct padconf */
	برای (x = 0; x < اندازه(omap_padconfs) / اندازه(omap_padconfs[0]); x++) {
		اگر ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_مقدار) {
			omap_padconf = &omap_padconfs[x];
			بشکن;
		}
	}
	أكد(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	بازگشت;
}
