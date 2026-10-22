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
	vir_بائٹاں ادھار;
	vir_بائٹاں وتھ;
	vir_بائٹاں ناپ;
	بنانشانی صحيح board_filter_قدر;
	بنانشانی صحيح board_filter_نقاب;
};

ساكن بنية omap_padconf omap_padconfs[] = {
	{
		    .ادھار = PADCONF_DM37_XX_REGISTERS_ادھار,
		    .وتھ = PADCONF_DM37_XX_REGISTERS_وتھ,
		    .ناپ = PADCONF_DM37_XX_REGISTERS_ناپ,
		    .board_filter_قدر = BOARD_FILTER_BBXM_قدر,
		    .board_filter_نقاب = BOARD_FILTER_BBXM_نقاب,
	    },
	{
		    .ادھار = PADCONF_AM335_X_REGISTERS_ادھار,
		    .وتھ = PADCONF_AM335_X_REGISTERS_وتھ,
		    .ناپ = PADCONF_AM335_X_REGISTERS_ناپ,
		    .board_filter_قدر = BOARD_FILTER_BB_قدر,
		    .board_filter_نقاب = BOARD_FILTER_BB_نقاب,
	    },
};

/* initialized in init */
ساكن بنية omap_padconf *omap_padconf;

ساكن kern_phys_map padconf_phys_map;

صحيح
bsp_padconf_سیٹ(u32_t padconf, u32_t نقاب, u32_t قدر)
{
	/* check that the value will be inside the padconf memory range */
	إذا (padconf >= (omap_padconf->ناپ - omap_padconf->وتھ)) {
		أرجع EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->ادھار + omap_padconf->وتھ, نقاب,
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

	kern_phys_map_ptr(omap_padconf->ادھار, omap_padconf->ناپ,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_بائٹاں) & omap_padconf->ادھار);

	أرجع;
}
