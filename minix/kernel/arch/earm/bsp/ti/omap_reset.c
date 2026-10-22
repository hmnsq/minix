#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ <machine/cpu.h>
#അന്തര്ഭാവ <minix/type.h>
#അന്തര്ഭാവ <minix/board.h>
#അന്തര്ഭാവ <io.h>

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "kernel/proc.h"
#അന്തര്ഭാവ "kernel/vm.h"
#അന്തര്ഭാവ "kernel/proto.h"
#അന്തര്ഭാവ "arch_proto.h"
#അന്തര്ഭാവ "bsp_reset.h"

#അന്തര്ഭാവ "omap_timer_registers.h"
#അന്തര്ഭാവ "omap_rtc.h"

#define AM335X_CM_BASE 0x44E00000
#define AM335X_CM_SIZE 0x1000

#define AM335X_PRM_DEVICE_OFFSET 0xf00
#define AM335X_PRM_RSTCTRL_REG 0x00
#define AM335X_RST_GLOBAL_WARM_SW_BIT 0

#define DM37XX_CM_BASE 0x48307000
#define DM37XX_CM_SIZE 0x1000
#define DM37XX_PRM_RSTCTRL_REG 0x250
#define DM37XX_RST_DPLL3_BIT 2

ഘടന omap_reset
{
	vir_bytes base;
	vir_bytes size;
};

സ്ഥിതം ഘടന omap_reset omap_reset;

സ്ഥിതം kern_phys_map reset_phys_map;

ശൂന്യം
bsp_reset_init(ശൂന്യം)
{
	എങ്കിൽ (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.base = DM37XX_CM_BASE;
		omap_reset.size = DM37XX_CM_SIZE;
	} അല്ലെങ്കിൽ എങ്കിൽ (BOARD_IS_BB(machine.board_id)) {
		omap_reset.base = AM335X_CM_BASE;
		omap_reset.size = AM335X_CM_SIZE;
	}

	kern_phys_map_ptr(omap_reset.base, omap_reset.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_bytes) & omap_reset.base);
}

ശൂന്യം
bsp_reset(ശൂന്യം)
{
	എങ്കിൽ (BOARD_IS_BBXM(machine.board_id)) {
		mmio_set((omap_reset.base + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37XX_RST_DPLL3_BIT));
	} അല്ലെങ്കിൽ എങ്കിൽ (BOARD_IS_BB(machine.board_id)) {
		mmio_set((omap_reset.base + AM335X_PRM_DEVICE_OFFSET +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335X_RST_GLOBAL_WARM_SW_BIT));
	}
}

ശൂന്യം
bsp_poweroff(ശൂന്യം)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	എങ്കിൽ (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_run();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		ആകുമ്പോൾ (1);
	}
}

ശൂന്യം bsp_disable_watchdog(ശൂന്യം)
{
        എങ്കിൽ(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0xAAAA);
		ആകുമ്പോൾ(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0x5555);
		ആകുമ്പോൾ(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
	}
}

