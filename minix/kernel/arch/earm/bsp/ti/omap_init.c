#અન્તર્ભાવ <sys/types.h>
#અન્તર્ભાવ "bsp_init.h"
#અન્તર્ભાવ "bsp_padconf.h"
#અન્તર્ભાવ "omap_rtc.h"
#અન્તર્ભાવ "bsp_reset.h"

શૂન્ય
bsp_init(શૂન્ય)
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
