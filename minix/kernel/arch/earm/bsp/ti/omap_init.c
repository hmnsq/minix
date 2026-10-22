#அந்தர்பாவ <sys/types.h>
#அந்தர்பாவ "bsp_init.h"
#அந்தர்பாவ "bsp_padconf.h"
#அந்தர்பாவ "omap_rtc.h"
#அந்தர்பாவ "bsp_reset.h"

வெறுமை
bsp_init(வெறுமை)
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
