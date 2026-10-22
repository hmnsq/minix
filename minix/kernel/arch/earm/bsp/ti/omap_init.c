#अन्तर्भाव <sys/types.h>
#अन्तर्भाव "bsp_init.h"
#अन्तर्भाव "bsp_padconf.h"
#अन्तर्भाव "omap_rtc.h"
#अन्तर्भाव "bsp_reset.h"

शून्य
bsp_init(शून्य)
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
