#شامل <assert.h>
#شامل <sys/types.h>
#شامل <machine/cpu.h>
#شامل <minix/type.h>
#شامل <minix/board.h>
#شامل <io.h>

#شامل "kernel/kernel.h"
#شامل "kernel/proc.h"
#شامل "kernel/vm.h"
#شامل "kernel/proto.h"
#شامل "arch_proto.h"
#شامل "bsp_reset.h"

#شامل "omap_timer_registers.h"
#شامل "omap_rtc.h"

#تعریف_کرو AM335_X_CM_بنیاد 0x44E00000
#تعریف_کرو AM335_X_CM_سائز 0x1000

#تعریف_کرو AM335_X_PRM_DEVICE_انحراف 0xf00
#تعریف_کرو AM335X_PRM_RSTCTRL_REG 0x00
#تعریف_کرو AM335_X_RST_GLOBAL_WARM_SW_ثنائی_ہندسہ 0

#تعریف_کرو DM37_XX_CM_بنیاد 0x48307000
#تعریف_کرو DM37_XX_CM_سائز 0x1000
#تعریف_کرو DM37XX_PRM_RSTCTRL_REG 0x250
#تعریف_کرو DM37_XX_RST_DPLL3_ثنائی_ہندسہ 2

ساخت omap_reset
{
	vir_بائٹیں بنیاد;
	vir_بائٹیں سائز;
};

ساکن ساخت omap_reset omap_reset;

ساکن kern_phys_map reset_phys_map;

خالی
bsp_reset_init(خالی)
{
	اگر (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.بنیاد = DM37_XX_CM_بنیاد;
		omap_reset.سائز = DM37_XX_CM_سائز;
	} ورنہ اگر (BOARD_IS_BB(machine.board_id)) {
		omap_reset.بنیاد = AM335_X_CM_بنیاد;
		omap_reset.سائز = AM335_X_CM_سائز;
	}

	kern_phys_map_ptr(omap_reset.بنیاد, omap_reset.سائز,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_بائٹیں) & omap_reset.بنیاد);
}

خالی
bsp_reset(خالی)
{
	اگر (BOARD_IS_BBXM(machine.board_id)) {
		mmio_سیٹ((omap_reset.بنیاد + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37_XX_RST_DPLL3_ثنائی_ہندسہ));
	} ورنہ اگر (BOARD_IS_BB(machine.board_id)) {
		mmio_سیٹ((omap_reset.بنیاد + AM335_X_PRM_DEVICE_انحراف +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335_X_RST_GLOBAL_WARM_SW_ثنائی_ہندسہ));
	}
}

خالی
bsp_poweroff(خالی)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	اگر (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_تسلسل();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		جب (1);
	}
}

خالی bsp_disable_watchdog(خالی)
{
        اگر(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335_X_WDT_بنیاد+AM335X_WDT_WSPR, 0xAAAA);
		جب(mmio_read(AM335_X_WDT_بنیاد+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335_X_WDT_بنیاد+AM335X_WDT_WSPR, 0x5555);
		جب(mmio_read(AM335_X_WDT_بنیاد+AM335X_WDT_WWPS) != 0) ;
	}
}

