#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_init.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_padconf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "omap_rtc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_reset.h"

ਖਾਲੀ
bsp_init(ਖਾਲੀ)
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
