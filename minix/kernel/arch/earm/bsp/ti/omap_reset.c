#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <sys/types.h>
#અન્તર્ભાવ <machine/cpu.h>
#અન્તર્ભાવ <minix/type.h>
#અન્તર્ભાવ <minix/board.h>
#અન્તર્ભાવ <io.h>

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "kernel/proc.h"
#અન્તર્ભાવ "kernel/vm.h"
#અન્તર્ભાવ "kernel/proto.h"
#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ "bsp_reset.h"

#અન્તર્ભાવ "omap_timer_registers.h"
#અન્તર્ભાવ "omap_rtc.h"

#define AM335X_CM_BASE 0x44E00000
#define AM335X_CM_SIZE 0x1000

#define AM335X_PRM_DEVICE_OFFSET 0xf00
#define AM335X_PRM_RSTCTRL_REG 0x00
#define AM335X_RST_GLOBAL_WARM_SW_BIT 0

#define DM37XX_CM_BASE 0x48307000
#define DM37XX_CM_SIZE 0x1000
#define DM37XX_PRM_RSTCTRL_REG 0x250
#define DM37XX_RST_DPLL3_BIT 2

સંરચના omap_reset
{
	vir_bytes base;
	vir_bytes size;
};

સ્થિર સંરચના omap_reset omap_reset;

સ્થિર kern_phys_map reset_phys_map;

શૂન્ય
bsp_reset_init(શૂન્ય)
{
	જો (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.base = DM37XX_CM_BASE;
		omap_reset.size = DM37XX_CM_SIZE;
	} નહીંતર જો (BOARD_IS_BB(machine.board_id)) {
		omap_reset.base = AM335X_CM_BASE;
		omap_reset.size = AM335X_CM_SIZE;
	}

	kern_phys_map_ptr(omap_reset.base, omap_reset.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_bytes) & omap_reset.base);
}

શૂન્ય
bsp_reset(શૂન્ય)
{
	જો (BOARD_IS_BBXM(machine.board_id)) {
		mmio_set((omap_reset.base + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37XX_RST_DPLL3_BIT));
	} નહીંતર જો (BOARD_IS_BB(machine.board_id)) {
		mmio_set((omap_reset.base + AM335X_PRM_DEVICE_OFFSET +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335X_RST_GLOBAL_WARM_SW_BIT));
	}
}

શૂન્ય
bsp_poweroff(શૂન્ય)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	જો (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_run();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		જ્યારે (1);
	}
}

શૂન્ય bsp_disable_watchdog(શૂન્ય)
{
        જો(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0xAAAA);
		જ્યારે(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0x5555);
		જ્યારે(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
	}
}

