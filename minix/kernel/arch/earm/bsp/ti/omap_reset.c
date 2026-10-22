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

#تعریف_کړه AM335_X_CM_بنسټ 0x44E00000
#تعریف_کړه AM335_X_CM_اندازه 0x1000

#تعریف_کړه AM335_X_PRM_DEVICE_دننه_واټن 0xf00
#تعریف_کړه AM335X_PRM_RSTCTRL_REG 0x00
#تعریف_کړه AM335_X_RST_GLOBAL_WARM_SW_بټ 0

#تعریف_کړه DM37_XX_CM_بنسټ 0x48307000
#تعریف_کړه DM37_XX_CM_اندازه 0x1000
#تعریف_کړه DM37XX_PRM_RSTCTRL_REG 0x250
#تعریف_کړه DM37_XX_RST_DPLL3_بټ 2

بنية omap_reset
{
	vir_بایټونه بنسټ;
	vir_بایټونه اندازه;
};

ساكن بنية omap_reset omap_reset;

ساكن kern_phys_map reset_phys_map;

فراغ
bsp_reset_init(فراغ)
{
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.بنسټ = DM37_XX_CM_بنسټ;
		omap_reset.اندازه = DM37_XX_CM_اندازه;
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		omap_reset.بنسټ = AM335_X_CM_بنسټ;
		omap_reset.اندازه = AM335_X_CM_اندازه;
	}

	kern_phys_map_ptr(omap_reset.بنسټ, omap_reset.اندازه,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_بایټونه) & omap_reset.بنسټ);
}

فراغ
bsp_reset(فراغ)
{
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		mmio_سېټ((omap_reset.بنسټ + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37_XX_RST_DPLL3_بټ));
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		mmio_سېټ((omap_reset.بنسټ + AM335_X_PRM_DEVICE_دننه_واټن +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335_X_RST_GLOBAL_WARM_SW_بټ));
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
		omap3_rtc_پرلپسې();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		طالما (1);
	}
}

فراغ bsp_disable_watchdog(فراغ)
{
        إذا(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335_X_WDT_بنسټ+AM335X_WDT_WSPR, 0xAAAA);
		طالما(mmio_read(AM335_X_WDT_بنسټ+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335_X_WDT_بنسټ+AM335X_WDT_WSPR, 0x5555);
		طالما(mmio_read(AM335_X_WDT_بنسټ+AM335X_WDT_WWPS) != 0) ;
	}
}

