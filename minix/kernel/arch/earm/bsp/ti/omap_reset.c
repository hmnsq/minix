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

#define AM335X_CM_BASE 0x44E00000
#define AM335X_CM_SIZE 0x1000

#define AM335X_PRM_DEVICE_OFFSET 0xf00
#define AM335X_PRM_RSTCTRL_REG 0x00
#define AM335X_RST_GLOBAL_WARM_SW_BIT 0

#define DM37XX_CM_BASE 0x48307000
#define DM37XX_CM_SIZE 0x1000
#define DM37XX_PRM_RSTCTRL_REG 0x250
#define DM37XX_RST_DPLL3_BIT 2

ಸಂರಚನಾ omap_reset
{
	vir_bytes base;
	vir_bytes size;
};

ಸ್ಥಿರ ಸಂರಚನಾ omap_reset omap_reset;

ಸ್ಥಿರ kern_phys_map reset_phys_map;

ಶೂನ್ಯ
bsp_reset_init(ಶೂನ್ಯ)
{
	ಯದಿ (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.base = DM37XX_CM_BASE;
		omap_reset.size = DM37XX_CM_SIZE;
	} ಅನ್ಯಥಾ ಯದಿ (BOARD_IS_BB(machine.board_id)) {
		omap_reset.base = AM335X_CM_BASE;
		omap_reset.size = AM335X_CM_SIZE;
	}

	kern_phys_map_ptr(omap_reset.base, omap_reset.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_bytes) & omap_reset.base);
}

ಶೂನ್ಯ
bsp_reset(ಶೂನ್ಯ)
{
	ಯದಿ (BOARD_IS_BBXM(machine.board_id)) {
		mmio_set((omap_reset.base + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37XX_RST_DPLL3_BIT));
	} ಅನ್ಯಥಾ ಯದಿ (BOARD_IS_BB(machine.board_id)) {
		mmio_set((omap_reset.base + AM335X_PRM_DEVICE_OFFSET +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335X_RST_GLOBAL_WARM_SW_BIT));
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
	ಯದಿ (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_run();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		ಯಾವತ್ (1);
	}
}

ಶೂನ್ಯ bsp_disable_watchdog(ಶೂನ್ಯ)
{
        ಯದಿ(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0xAAAA);
		ಯಾವತ್(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0x5555);
		ಯಾವತ್(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
	}
}

