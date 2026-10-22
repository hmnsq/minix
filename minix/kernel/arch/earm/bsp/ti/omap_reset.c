#ضمن <assert.h>
#ضمن <sys/types.h>
#ضمن <machine/cpu.h>
#ضمن <minix/type.h>
#ضمن <minix/board.h>
#ضمن <io.h>

#ضمن "kernel/kernel.h"
#ضمن "kernel/proc.h"
#ضمن "kernel/vm.h"
#ضمن "kernel/proto.h"
#ضمن "arch_proto.h"
#ضمن "bsp_reset.h"

#ضمن "omap_timer_registers.h"
#ضمن "omap_rtc.h"

#وصف_ڪريو AM335_X_CM_بنياد 0x44E00000
#وصف_ڪريو AM335_X_CM_سائيز 0x1000

#وصف_ڪريو AM335_X_PRM_DEVICE_وٿي 0xf00
#وصف_ڪريو AM335X_PRM_RSTCTRL_REG 0x00
#وصف_ڪريو AM335_X_RST_GLOBAL_WARM_SW_ٻٽو_انگ 0

#وصف_ڪريو DM37_XX_CM_بنياد 0x48307000
#وصف_ڪريو DM37_XX_CM_سائيز 0x1000
#وصف_ڪريو DM37XX_PRM_RSTCTRL_REG 0x250
#وصف_ڪريو DM37_XX_RST_DPLL3_ٻٽو_انگ 2

بنية omap_reset
{
	vir_بائيٽون بنياد;
	vir_بائيٽون سائيز;
};

ساكن بنية omap_reset omap_reset;

ساكن kern_phys_map reset_phys_map;

فراغ
bsp_reset_init(فراغ)
{
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.بنياد = DM37_XX_CM_بنياد;
		omap_reset.سائيز = DM37_XX_CM_سائيز;
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		omap_reset.بنياد = AM335_X_CM_بنياد;
		omap_reset.سائيز = AM335_X_CM_سائيز;
	}

	kern_phys_map_ptr(omap_reset.بنياد, omap_reset.سائيز,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_بائيٽون) & omap_reset.بنياد);
}

فراغ
bsp_reset(فراغ)
{
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		mmio_سيٽ((omap_reset.بنياد + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37_XX_RST_DPLL3_ٻٽو_انگ));
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		mmio_سيٽ((omap_reset.بنياد + AM335_X_PRM_DEVICE_وٿي +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335_X_RST_GLOBAL_WARM_SW_ٻٽو_انگ));
	}
}

فراغ
bsp_poweroff(فراغ)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	إذا (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_لاڳيتو();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		طالما (1);
	}
}

فراغ bsp_disable_watchdog(فراغ)
{
        إذا(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335_X_WDT_بنياد+AM335X_WDT_WSPR, 0xAAAA);
		طالما(mmio_read(AM335_X_WDT_بنياد+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335_X_WDT_بنياد+AM335X_WDT_WSPR, 0x5555);
		طالما(mmio_read(AM335_X_WDT_بنياد+AM335X_WDT_WWPS) != 0) ;
	}
}

