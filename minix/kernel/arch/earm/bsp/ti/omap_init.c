#অন্তর্ভাৱ <sys/types.h>
#অন্তর্ভাৱ "bsp_init.h"
#অন্তর্ভাৱ "bsp_padconf.h"
#অন্তর্ভাৱ "omap_rtc.h"
#অন্তর্ভাৱ "bsp_reset.h"

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
