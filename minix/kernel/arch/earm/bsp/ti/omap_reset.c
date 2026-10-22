#अन्तर्भाव <assert.h>
#अन्तर्भाव <sys/types.h>
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <minix/type.h>
#अन्तर्भाव <minix/board.h>
#अन्तर्भाव <io.h>

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "kernel/proc.h"
#अन्तर्भाव "kernel/vm.h"
#अन्तर्भाव "kernel/proto.h"
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव "bsp_reset.h"

#अन्तर्भाव "omap_timer_registers.h"
#अन्तर्भाव "omap_rtc.h"

#परिभाषय AM335_X_CM_आधारः 0x44E00000
#परिभाषय AM335_X_CM_आकार 0x1000

#परिभाषय AM335_X_PRM_DEVICE_विस्थापनम् 0xf00
#परिभाषय AM335X_PRM_RSTCTRL_REG 0x00
#परिभाषय AM335X_RST_GLOBAL_WARM_SW_BIT 0

#परिभाषय DM37_XX_CM_आधारः 0x48307000
#परिभाषय DM37_XX_CM_आकार 0x1000
#परिभाषय DM37XX_PRM_RSTCTRL_REG 0x250
#परिभाषय DM37XX_RST_DPLL3_BIT 2

संरचना omap_reset
{
	vir_अष्टकानि आधारः;
	vir_अष्टकानि आकार;
};

स्थिर संरचना omap_reset omap_reset;

स्थिर kern_phys_map reset_phys_map;

शून्य
bsp_reset_init(शून्य)
{
	यदि (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.आधारः = DM37_XX_CM_आधारः;
		omap_reset.आकार = DM37_XX_CM_आकार;
	} अन्यथा यदि (BOARD_IS_BB(machine.board_id)) {
		omap_reset.आधारः = AM335_X_CM_आधारः;
		omap_reset.आकार = AM335_X_CM_आकार;
	}

	kern_phys_map_ptr(omap_reset.आधारः, omap_reset.आकार,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_अष्टकानि) & omap_reset.आधारः);
}

शून्य
bsp_reset(शून्य)
{
	यदि (BOARD_IS_BBXM(machine.board_id)) {
		mmio_समुच्चयः((omap_reset.आधारः + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37XX_RST_DPLL3_BIT));
	} अन्यथा यदि (BOARD_IS_BB(machine.board_id)) {
		mmio_समुच्चयः((omap_reset.आधारः + AM335_X_PRM_DEVICE_विस्थापनम् +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335X_RST_GLOBAL_WARM_SW_BIT));
	}
}

शून्य
bsp_poweroff(शून्य)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	यदि (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_सततम्();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		यावत् (1);
	}
}

शून्य bsp_disable_watchdog(शून्य)
{
        यदि(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335_X_WDT_आधारः+AM335X_WDT_WSPR, 0xAAAA);
		यावत्(mmio_read(AM335_X_WDT_आधारः+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335_X_WDT_आधारः+AM335X_WDT_WSPR, 0x5555);
		यावत्(mmio_read(AM335_X_WDT_आधारः+AM335X_WDT_WWPS) != 0) ;
	}
}

