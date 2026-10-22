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
	vir_بائيٽون بنياد;
	vir_بائيٽون وٿي;
	vir_بائيٽون سائيز;
	بنانشان صحيح board_filter_قدر;
	بنانشان صحيح board_filter_نقاب;
};

ساكن بنية omap_padconf omap_padconfs[] = {
	{
		    .بنياد = PADCONF_DM37_XX_REGISTERS_بنياد,
		    .وٿي = PADCONF_DM37_XX_REGISTERS_وٿي,
		    .سائيز = PADCONF_DM37_XX_REGISTERS_سائيز,
		    .board_filter_قدر = BOARD_FILTER_BBXM_قدر,
		    .board_filter_نقاب = BOARD_FILTER_BBXM_نقاب,
	    },
	{
		    .بنياد = PADCONF_AM335_X_REGISTERS_بنياد,
		    .وٿي = PADCONF_AM335_X_REGISTERS_وٿي,
		    .سائيز = PADCONF_AM335_X_REGISTERS_سائيز,
		    .board_filter_قدر = BOARD_FILTER_BB_قدر,
		    .board_filter_نقاب = BOARD_FILTER_BB_نقاب,
	    },
};

/* initialized in init */
ساكن بنية omap_padconf *omap_padconf;

ساكن kern_phys_map padconf_phys_map;

صحيح
bsp_padconf_سيٽ(u32_t padconf, u32_t نقاب, u32_t قدر)
{
	/* check that the value will be inside the padconf memory range */
	إذا (padconf >= (omap_padconf->سائيز - omap_padconf->وٿي)) {
		أرجع EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->بنياد + omap_padconf->وٿي, نقاب,
	    قدر);

	أرجع OK;
}

فراغ
bsp_padconf_init(فراغ)
{
	صحيح x;
	omap_padconf = عدم;
	/* find the correct padconf */
	لكل (x = 0; x < حجم(omap_padconfs) / حجم(omap_padconfs[0]); x++) {
		إذا ((omap_padconfs[x].board_filter_نقاب & machine.board_id) ==
		    omap_padconfs[x].board_filter_قدر) {
			omap_padconf = &omap_padconfs[x];
			اكسر;
		}
	}
	أكد(omap_padconf);

	kern_phys_map_ptr(omap_padconf->بنياد, omap_padconf->سائيز,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_بائيٽون) & omap_padconf->بنياد);

	أرجع;
}
