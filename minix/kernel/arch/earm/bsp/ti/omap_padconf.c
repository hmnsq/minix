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

ساخت omap_padconf
{
	vir_بائٹیں بنیاد;
	vir_بائٹیں انحراف;
	vir_بائٹیں سائز;
	غیرعلامتی صحیح board_filter_قدر;
	غیرعلامتی صحیح board_filter_نقاب;
};

ساکن ساخت omap_padconf omap_padconfs[] = {
	{
		    .بنیاد = PADCONF_DM37_XX_REGISTERS_بنیاد,
		    .انحراف = PADCONF_DM37_XX_REGISTERS_انحراف,
		    .سائز = PADCONF_DM37_XX_REGISTERS_سائز,
		    .board_filter_قدر = BOARD_FILTER_BBXM_قدر,
		    .board_filter_نقاب = BOARD_FILTER_BBXM_نقاب,
	    },
	{
		    .بنیاد = PADCONF_AM335_X_REGISTERS_بنیاد,
		    .انحراف = PADCONF_AM335_X_REGISTERS_انحراف,
		    .سائز = PADCONF_AM335_X_REGISTERS_سائز,
		    .board_filter_قدر = BOARD_FILTER_BB_قدر,
		    .board_filter_نقاب = BOARD_FILTER_BB_نقاب,
	    },
};

/* initialized in init */
ساکن ساخت omap_padconf *omap_padconf;

ساکن kern_phys_map padconf_phys_map;

صحیح
bsp_padconf_سیٹ(u32_t padconf, u32_t نقاب, u32_t قدر)
{
	/* check that the value will be inside the padconf memory range */
	اگر (padconf >= (omap_padconf->سائز - omap_padconf->انحراف)) {
		واپس EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->بنیاد + omap_padconf->انحراف, نقاب,
	    قدر);

	واپس OK;
}

خالی
bsp_padconf_init(خالی)
{
	صحیح x;
	omap_padconf = تہی;
	/* find the correct padconf */
	برائے (x = 0; x < حجم(omap_padconfs) / حجم(omap_padconfs[0]); x++) {
		اگر ((omap_padconfs[x].board_filter_نقاب & machine.board_id) ==
		    omap_padconfs[x].board_filter_قدر) {
			omap_padconf = &omap_padconfs[x];
			توڑو;
		}
	}
	أكد(omap_padconf);

	kern_phys_map_ptr(omap_padconf->بنیاد, omap_padconf->سائز,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_بائٹیں) & omap_padconf->بنیاد);

	واپس;
}
