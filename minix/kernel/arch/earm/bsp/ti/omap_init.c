#অন্তর্ভাব <sys/types.h>
#অন্তর্ভাব "bsp_init.h"
#অন্তর্ভাব "bsp_padconf.h"
#অন্তর্ভাব "omap_rtc.h"
#অন্তর্ভাব "bsp_reset.h"

শূন্য
bsp_init(শূন্য)
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
