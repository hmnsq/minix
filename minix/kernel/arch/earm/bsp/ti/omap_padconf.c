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
	vir_بایټونه بنسټ;
	vir_بایټونه دننه_واټن;
	vir_بایټونه اندازه;
	بینښه صحيح board_filter_ارزښت;
	بینښه صحيح board_filter_ماسک;
};

ساكن بنية omap_padconf omap_padconfs[] = {
	{
		    .بنسټ = PADCONF_DM37_XX_REGISTERS_بنسټ,
		    .دننه_واټن = PADCONF_DM37_XX_REGISTERS_دننه_واټن,
		    .اندازه = PADCONF_DM37_XX_REGISTERS_اندازه,
		    .board_filter_ارزښت = BOARD_FILTER_BBXM_ارزښت,
		    .board_filter_ماسک = BOARD_FILTER_BBXM_ماسک,
	    },
	{
		    .بنسټ = PADCONF_AM335_X_REGISTERS_بنسټ,
		    .دننه_واټن = PADCONF_AM335_X_REGISTERS_دننه_واټن,
		    .اندازه = PADCONF_AM335_X_REGISTERS_اندازه,
		    .board_filter_ارزښت = BOARD_FILTER_BB_ارزښت,
		    .board_filter_ماسک = BOARD_FILTER_BB_ماسک,
	    },
};

/* initialized in init */
ساكن بنية omap_padconf *omap_padconf;

ساكن kern_phys_map padconf_phys_map;

صحيح
bsp_padconf_سېټ(u32_t padconf, u32_t ماسک, u32_t ارزښت)
{
	/* check that the value will be inside the padconf memory range */
	إذا (padconf >= (omap_padconf->اندازه - omap_padconf->دننه_واټن)) {
		أرجع EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->بنسټ + omap_padconf->دننه_واټن, ماسک,
	    ارزښت);

	أرجع OK;
}

فراغ
bsp_padconf_init(فراغ)
{
	صحيح x;
	omap_padconf = عدم;
	/* find the correct padconf */
	لكل (x = 0; x < حجم(omap_padconfs) / حجم(omap_padconfs[0]); x++) {
		إذا ((omap_padconfs[x].board_filter_ماسک & machine.board_id) ==
		    omap_padconfs[x].board_filter_ارزښت) {
			omap_padconf = &omap_padconfs[x];
			اكسر;
		}
	}
	أكد(omap_padconf);

	kern_phys_map_ptr(omap_padconf->بنسټ, omap_padconf->اندازه,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_بایټونه) & omap_padconf->بنسټ);

	أرجع;
}
