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

#define AM335X_CM_BASE 0x44E00000
#define AM335X_CM_SIZE 0x1000

#define AM335X_PRM_DEVICE_OFFSET 0xf00
#define AM335X_PRM_RSTCTRL_REG 0x00
#define AM335X_RST_GLOBAL_WARM_SW_BIT 0

#define DM37XX_CM_BASE 0x48307000
#define DM37XX_CM_SIZE 0x1000
#define DM37XX_PRM_RSTCTRL_REG 0x250
#define DM37XX_RST_DPLL3_BIT 2

संरचना omap_reset
{
	vir_bytes base;
	vir_bytes size;
};

स्थिर संरचना omap_reset omap_reset;

स्थिर kern_phys_map reset_phys_map;

शून्य
bsp_reset_init(शून्य)
{
	जइ (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.base = DM37XX_CM_BASE;
		omap_reset.size = DM37XX_CM_SIZE;
	} अण्णहा जइ (BOARD_IS_BB(machine.board_id)) {
		omap_reset.base = AM335X_CM_BASE;
		omap_reset.size = AM335X_CM_SIZE;
	}

	kern_phys_map_ptr(omap_reset.base, omap_reset.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_bytes) & omap_reset.base);
}

शून्य
bsp_reset(शून्य)
{
	जइ (BOARD_IS_BBXM(machine.board_id)) {
		mmio_set((omap_reset.base + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37XX_RST_DPLL3_BIT));
	} अण्णहा जइ (BOARD_IS_BB(machine.board_id)) {
		mmio_set((omap_reset.base + AM335X_PRM_DEVICE_OFFSET +
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
	जइ (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_run();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		जाव (1);
	}
}

शून्य bsp_disable_watchdog(शून्य)
{
        जइ(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0xAAAA);
		जाव(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0x5555);
		जाव(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
	}
}

