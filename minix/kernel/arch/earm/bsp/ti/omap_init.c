#समावेश <sys/types.h>
#समावेश "bsp_init.h"
#समावेश "bsp_padconf.h"
#समावेश "omap_rtc.h"
#समावेश "bsp_reset.h"

व्योम
bsp_init(व्योम)
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
