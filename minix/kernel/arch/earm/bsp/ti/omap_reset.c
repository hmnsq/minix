#అన్తర్భావ <assert.h>
#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <minix/type.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <io.h>

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "kernel/proc.h"
#అన్తర్భావ "kernel/vm.h"
#అన్తర్భావ "kernel/proto.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "bsp_reset.h"

#అన్తర్భావ "omap_timer_registers.h"
#అన్తర్భావ "omap_rtc.h"

#నిర్వచించు AM335_X_CM_పునాది 0x44E00000
#నిర్వచించు AM335_X_CM_కొలత 0x1000

#నిర్వచించు AM335_X_PRM_DEVICE_స్థానభ్రంశం 0xf00
#నిర్వచించు AM335X_PRM_RSTCTRL_REG 0x00
#నిర్వచించు AM335_X_RST_GLOBAL_WARM_SW_బిట్టు 0

#నిర్వచించు DM37_XX_CM_పునాది 0x48307000
#నిర్వచించు DM37_XX_CM_కొలత 0x1000
#నిర్వచించు DM37XX_PRM_RSTCTRL_REG 0x250
#నిర్వచించు DM37_XX_RST_DPLL3_బిట్టు 2

నిర్మాణం omap_reset
{
	vir_బైట్లు పునాది;
	vir_బైట్లు కొలత;
};

స్థిర నిర్మాణం omap_reset omap_reset;

స్థిర kern_phys_map reset_phys_map;

శూన్య
bsp_reset_init(శూన్య)
{
	ఒకవేళ (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.పునాది = DM37_XX_CM_పునాది;
		omap_reset.కొలత = DM37_XX_CM_కొలత;
	} లేకపోతే ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		omap_reset.పునాది = AM335_X_CM_పునాది;
		omap_reset.కొలత = AM335_X_CM_కొలత;
	}

	kern_phys_map_ptr(omap_reset.పునాది, omap_reset.కొలత,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_బైట్లు) & omap_reset.పునాది);
}

శూన్య
bsp_reset(శూన్య)
{
	ఒకవేళ (BOARD_IS_BBXM(machine.board_id)) {
		mmio_సమితి((omap_reset.పునాది + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37_XX_RST_DPLL3_బిట్టు));
	} లేకపోతే ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		mmio_సమితి((omap_reset.పునాది + AM335_X_PRM_DEVICE_స్థానభ్రంశం +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335_X_RST_GLOBAL_WARM_SW_బిట్టు));
	}
}

శూన్య
bsp_poweroff(శూన్య)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_నిరంతరం();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		అయితే (1);
	}
}

శూన్య bsp_disable_watchdog(శూన్య)
{
        ఒకవేళ(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335_X_WDT_పునాది+AM335X_WDT_WSPR, 0xAAAA);
		అయితే(mmio_read(AM335_X_WDT_పునాది+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335_X_WDT_పునాది+AM335X_WDT_WSPR, 0x5555);
		అయితే(mmio_read(AM335_X_WDT_పునాది+AM335X_WDT_WWPS) != 0) ;
	}
}

