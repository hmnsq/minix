#అన్తర్భావ <sys/types.h>
#అన్తర్భావ "bsp_init.h"
#అన్తర్భావ "bsp_padconf.h"
#అన్తర్భావ "omap_rtc.h"
#అన్తర్భావ "bsp_reset.h"

శూన్య
bsp_init(శూన్య)
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
