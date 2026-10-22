#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <sys/types.h>
#அந்தர்பாவ <machine/cpu.h>
#அந்தர்பாவ <minix/type.h>
#அந்தர்பாவ <minix/board.h>
#அந்தர்பாவ <io.h>

#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "kernel/proc.h"
#அந்தர்பாவ "kernel/vm.h"
#அந்தர்பாவ "kernel/proto.h"
#அந்தர்பாவ "arch_proto.h"
#அந்தர்பாவ "bsp_reset.h"

#அந்தர்பாவ "omap_timer_registers.h"
#அந்தர்பாவ "omap_rtc.h"

#define AM335X_CM_BASE 0x44E00000
#define AM335X_CM_SIZE 0x1000

#define AM335X_PRM_DEVICE_OFFSET 0xf00
#define AM335X_PRM_RSTCTRL_REG 0x00
#define AM335X_RST_GLOBAL_WARM_SW_BIT 0

#define DM37XX_CM_BASE 0x48307000
#define DM37XX_CM_SIZE 0x1000
#define DM37XX_PRM_RSTCTRL_REG 0x250
#define DM37XX_RST_DPLL3_BIT 2

கட்டமைப்பு omap_reset
{
	vir_bytes base;
	vir_bytes size;
};

நிலையான கட்டமைப்பு omap_reset omap_reset;

நிலையான kern_phys_map reset_phys_map;

வெறுமை
bsp_reset_init(வெறுமை)
{
	எனில் (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.base = DM37XX_CM_BASE;
		omap_reset.size = DM37XX_CM_SIZE;
	} இல்லையெனில் எனில் (BOARD_IS_BB(machine.board_id)) {
		omap_reset.base = AM335X_CM_BASE;
		omap_reset.size = AM335X_CM_SIZE;
	}

	kern_phys_map_ptr(omap_reset.base, omap_reset.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_bytes) & omap_reset.base);
}

வெறுமை
bsp_reset(வெறுமை)
{
	எனில் (BOARD_IS_BBXM(machine.board_id)) {
		mmio_set((omap_reset.base + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37XX_RST_DPLL3_BIT));
	} இல்லையெனில் எனில் (BOARD_IS_BB(machine.board_id)) {
		mmio_set((omap_reset.base + AM335X_PRM_DEVICE_OFFSET +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335X_RST_GLOBAL_WARM_SW_BIT));
	}
}

வெறுமை
bsp_poweroff(வெறுமை)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	எனில் (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_run();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		போது (1);
	}
}

வெறுமை bsp_disable_watchdog(வெறுமை)
{
        எனில்(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0xAAAA);
		போது(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0x5555);
		போது(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
	}
}

