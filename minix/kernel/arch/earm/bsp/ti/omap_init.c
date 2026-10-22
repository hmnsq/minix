#ضمن <sys/types.h>
#ضمن "bsp_init.h"
#ضمن "bsp_padconf.h"
#ضمن "omap_rtc.h"
#ضمن "bsp_reset.h"

فراغ
bsp_init(فراغ)
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
