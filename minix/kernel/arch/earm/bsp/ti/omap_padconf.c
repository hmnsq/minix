/* Implements sys_padconf() for the AM335X and DM37XX. */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/mmio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/padconf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/com.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_padconf.h"

ਬਣਤਰ omap_padconf
{
	vir_ਬਾਈਟਾਂ ਅਧਾਰ;
	vir_ਬਾਈਟਾਂ ਵਿੱਥ;
	vir_ਬਾਈਟਾਂ ਮਾਪ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ board_filter_ਮੁੱਲ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ board_filter_ਨਕਾਬ;
};

ਸਥਿਰ ਬਣਤਰ omap_padconf omap_padconfs[] = {
	{
		    .ਅਧਾਰ = PADCONF_DM37_XX_REGISTERS_ਅਧਾਰ,
		    .ਵਿੱਥ = PADCONF_DM37_XX_REGISTERS_ਵਿੱਥ,
		    .ਮਾਪ = PADCONF_DM37_XX_REGISTERS_ਮਾਪ,
		    .board_filter_ਮੁੱਲ = BOARD_FILTER_BBXM_ਮੁੱਲ,
		    .board_filter_ਨਕਾਬ = BOARD_FILTER_BBXM_ਨਕਾਬ,
	    },
	{
		    .ਅਧਾਰ = PADCONF_AM335_X_REGISTERS_ਅਧਾਰ,
		    .ਵਿੱਥ = PADCONF_AM335_X_REGISTERS_ਵਿੱਥ,
		    .ਮਾਪ = PADCONF_AM335_X_REGISTERS_ਮਾਪ,
		    .board_filter_ਮੁੱਲ = BOARD_FILTER_BB_ਮੁੱਲ,
		    .board_filter_ਨਕਾਬ = BOARD_FILTER_BB_ਨਕਾਬ,
	    },
};

/* initialized in init */
ਸਥਿਰ ਬਣਤਰ omap_padconf *omap_padconf;

ਸਥਿਰ kern_phys_map padconf_phys_map;

ਪੂਰਨ
bsp_padconf_ਸੈੱਟ(u32_t padconf, u32_t ਨਕਾਬ, u32_t ਮੁੱਲ)
{
	/* check that the value will be inside the padconf memory range */
	ਜੇ (padconf >= (omap_padconf->ਮਾਪ - omap_padconf->ਵਿੱਥ)) {
		ਵਾਪਸ EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->ਅਧਾਰ + omap_padconf->ਵਿੱਥ, ਨਕਾਬ,
	    ਮੁੱਲ);

	ਵਾਪਸ OK;
}

ਖਾਲੀ
bsp_padconf_init(ਖਾਲੀ)
{
	ਪੂਰਨ x;
	omap_padconf = ਨਿਰਾ;
	/* find the correct padconf */
	ਲਈ (x = 0; x < ਆਕਾਰ(omap_padconfs) / ਆਕਾਰ(omap_padconfs[0]); x++) {
		ਜੇ ((omap_padconfs[x].board_filter_ਨਕਾਬ & machine.board_id) ==
		    omap_padconfs[x].board_filter_ਮੁੱਲ) {
			omap_padconf = &omap_padconfs[x];
			ਰੋਕੋ;
		}
	}
	ਪ੍ਰਤਿਜ੍ਞਾ(omap_padconf);

	kern_phys_map_ptr(omap_padconf->ਅਧਾਰ, omap_padconf->ਮਾਪ,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_ਬਾਈਟਾਂ) & omap_padconf->ਅਧਾਰ);

	ਵਾਪਸ;
}
