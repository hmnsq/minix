#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ "bsp_init.h"
#ಅನ್ತರ್ಭಾವ "bsp_padconf.h"
#ಅನ್ತರ್ಭಾವ "omap_rtc.h"
#ಅನ್ತರ್ಭಾವ "bsp_reset.h"

ಶೂನ್ಯ
bsp_init(ಶೂನ್ಯ)
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
