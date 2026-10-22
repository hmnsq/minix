#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <sys/types.h>
#অন্তর্ভাৱ <machine/cpu.h>
#অন্তর্ভাৱ <minix/type.h>
#অন্তর্ভাৱ <minix/board.h>
#অন্তর্ভাৱ <io.h>

#অন্তর্ভাৱ "kernel/kernel.h"
#অন্তর্ভাৱ "kernel/proc.h"
#অন্তর্ভাৱ "kernel/vm.h"
#অন্তর্ভাৱ "kernel/proto.h"
#অন্তর্ভাৱ "arch_proto.h"
#অন্তর্ভাৱ "bsp_reset.h"

#অন্তর্ভাৱ "omap_timer_registers.h"
#অন্তর্ভাৱ "omap_rtc.h"

#define AM335X_CM_BASE 0x44E00000
#define AM335X_CM_SIZE 0x1000

#define AM335X_PRM_DEVICE_OFFSET 0xf00
#define AM335X_PRM_RSTCTRL_REG 0x00
#define AM335X_RST_GLOBAL_WARM_SW_BIT 0

#define DM37XX_CM_BASE 0x48307000
#define DM37XX_CM_SIZE 0x1000
#define DM37XX_PRM_RSTCTRL_REG 0x250
#define DM37XX_RST_DPLL3_BIT 2

সংরচনা omap_reset
{
	vir_bytes base;
	vir_bytes size;
};

স্থির সংরচনা omap_reset omap_reset;

স্থির kern_phys_map reset_phys_map;

শূন্য
bsp_reset_init(শূন্য)
{
	যদি (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.base = DM37XX_CM_BASE;
		omap_reset.size = DM37XX_CM_SIZE;
	} অন্যথা যদি (BOARD_IS_BB(machine.board_id)) {
		omap_reset.base = AM335X_CM_BASE;
		omap_reset.size = AM335X_CM_SIZE;
	}

	kern_phys_map_ptr(omap_reset.base, omap_reset.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_bytes) & omap_reset.base);
}

শূন্য
bsp_reset(শূন্য)
{
	যদি (BOARD_IS_BBXM(machine.board_id)) {
		mmio_set((omap_reset.base + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37XX_RST_DPLL3_BIT));
	} অন্যথা যদি (BOARD_IS_BB(machine.board_id)) {
		mmio_set((omap_reset.base + AM335X_PRM_DEVICE_OFFSET +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335X_RST_GLOBAL_WARM_SW_BIT));
	}
}

শূন্য
bsp_poweroff(শূন্য)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	যদি (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_run();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		যেতিয়ালৈ (1);
	}
}

শূন্য bsp_disable_watchdog(শূন্য)
{
        যদি(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0xAAAA);
		যেতিয়ালৈ(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0x5555);
		যেতিয়ালৈ(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
	}
}

