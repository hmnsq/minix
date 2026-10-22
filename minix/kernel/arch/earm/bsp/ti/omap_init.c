#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ "bsp_init.h"
#അന്തര്ഭാവ "bsp_padconf.h"
#അന്തര്ഭാവ "omap_rtc.h"
#അന്തര്ഭാവ "bsp_reset.h"

ശൂന്യം
bsp_init(ശൂന്യം)
{

	/* map memory for padconf */
	bsp_padconf_init();

	/* map memory for rtc */
	omap3_rtc_init();

	/* map memory for reset control */
	bsp_reset_init();

	/* disable watchdog */
	bsp_disable_watchdog();
}
