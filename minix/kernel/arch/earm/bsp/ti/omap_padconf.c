/* Implements sys_padconf() for the AM335X and DM37XX. */

#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"
#ضمن <sys/types.h>
#ضمن <machine/cpu.h>
#ضمن <minix/mmio.h>
#ضمن <minix/padconf.h>
#ضمن <minix/board.h>
#ضمن <minix/com.h>
#ضمن <assert.h>
#ضمن <io.h>
#ضمن <stdlib.h>
#ضمن <stdio.h>

#ضمن "bsp_padconf.h"

بنية omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned صحيح board_filter_value;
	unsigned صحيح board_filter_mask;
};

ساكن بنية omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_value = BOARD_FILTER_BBXM_VALUE,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_value = BOARD_FILTER_BB_VALUE,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
ساكن بنية omap_padconf *omap_padconf;

ساكن kern_phys_map padconf_phys_map;

صحيح
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t value)
{
	/* check that the value will be inside the padconf memory range */
	𐤀𐤌 (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		أرجع EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    value);

	أرجع OK;
}

فراغ
bsp_padconf_init(فراغ)
{
	صحيح x;
	omap_padconf = عدم;
	/* find the correct padconf */
	لكل (x = 0; x < حجم(omap_padconfs) / حجم(omap_padconfs[0]); x++) {
		𐤀𐤌 ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_value) {
			omap_padconf = &omap_padconfs[x];
			اكسر;
		}
	}
	أكد(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	أرجع;
}
