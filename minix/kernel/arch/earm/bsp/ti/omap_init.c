#ଅନ୍ତର୍ଭାଵ <sys/types.h>
#ଅନ୍ତର୍ଭାଵ "bsp_init.h"
#ଅନ୍ତର୍ଭାଵ "bsp_padconf.h"
#ଅନ୍ତର୍ଭାଵ "omap_rtc.h"
#ଅନ୍ତର୍ଭାଵ "bsp_reset.h"

ଶୂନ୍ୟ
bsp_init(ଶୂନ୍ୟ)
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
