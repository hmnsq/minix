#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <minix/type.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <io.h>

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "kernel/proc.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ "bsp_reset.h"

#ಅನ್ತರ್ಭಾವ "omap_timer_registers.h"
#ಅನ್ತರ್ಭಾವ "omap_rtc.h"

#ವ್ಯಾಖ್ಯಾನಿಸು AM335_X_CM_ತಳ 0x44E00000
#ವ್ಯಾಖ್ಯಾನಿಸು AM335_X_CM_ಗಾತ್ರ 0x1000

#ವ್ಯಾಖ್ಯಾನಿಸು AM335_X_PRM_DEVICE_ಸ್ಥಾನಾಂತರ 0xf00
#ವ್ಯಾಖ್ಯಾನಿಸು AM335X_PRM_RSTCTRL_REG 0x00
#ವ್ಯಾಖ್ಯಾನಿಸು AM335_X_RST_GLOBAL_WARM_SW_ಬಿಟ್ 0

#ವ್ಯಾಖ್ಯಾನಿಸು DM37_XX_CM_ತಳ 0x48307000
#ವ್ಯಾಖ್ಯಾನಿಸು DM37_XX_CM_ಗಾತ್ರ 0x1000
#ವ್ಯಾಖ್ಯಾನಿಸು DM37XX_PRM_RSTCTRL_REG 0x250
#ವ್ಯಾಖ್ಯಾನಿಸು DM37_XX_RST_DPLL3_ಬಿಟ್ 2

ರಚನೆ omap_reset
{
	vir_ಬೈಟುಗಳು ತಳ;
	vir_ಬೈಟುಗಳು ಗಾತ್ರ_2;
};

ಸ್ಥಿರ ರಚನೆ omap_reset omap_reset;

ಸ್ಥಿರ kern_phys_map reset_phys_map;

ಶೂನ್ಯ
bsp_reset_init(ಶೂನ್ಯ)
{
	ಆದರೆ (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.ತಳ = DM37_XX_CM_ತಳ;
		omap_reset.ಗಾತ್ರ_2 = DM37_XX_CM_ಗಾತ್ರ;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		omap_reset.ತಳ = AM335_X_CM_ತಳ;
		omap_reset.ಗಾತ್ರ_2 = AM335_X_CM_ಗಾತ್ರ;
	}

	kern_phys_map_ptr(omap_reset.ತಳ, omap_reset.ಗಾತ್ರ_2,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_ಬೈಟುಗಳು) & omap_reset.ತಳ);
}

ಶೂನ್ಯ
bsp_reset(ಶೂನ್ಯ)
{
	ಆದರೆ (BOARD_IS_BBXM(machine.board_id)) {
		mmio_ಗಣ((omap_reset.ತಳ + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37_XX_RST_DPLL3_ಬಿಟ್));
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		mmio_ಗಣ((omap_reset.ತಳ + AM335_X_PRM_DEVICE_ಸ್ಥಾನಾಂತರ +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335_X_RST_GLOBAL_WARM_SW_ಬಿಟ್));
	}
}

ಶೂನ್ಯ
bsp_poweroff(ಶೂನ್ಯ)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_ಸತತ();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		ಆಗ (1);
	}
}

ಶೂನ್ಯ bsp_disable_watchdog(ಶೂನ್ಯ)
{
        ಆದರೆ(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335_X_WDT_ತಳ+AM335X_WDT_WSPR, 0xAAAA);
		ಆಗ(mmio_read(AM335_X_WDT_ತಳ+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335_X_WDT_ತಳ+AM335X_WDT_WSPR, 0x5555);
		ಆಗ(mmio_read(AM335_X_WDT_ತಳ+AM335X_WDT_WWPS) != 0) ;
	}
}

