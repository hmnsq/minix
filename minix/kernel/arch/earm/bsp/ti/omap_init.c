#شامل <sys/types.h>
#شامل "bsp_init.h"
#شامل "bsp_padconf.h"
#شامل "omap_rtc.h"
#شامل "bsp_reset.h"

خالی
bsp_init(خالی)
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
