#समावेश <assert.h>
#समावेश <sys/types.h>
#समावेश <machine/cpu.h>
#समावेश <minix/type.h>
#समावेश <minix/board.h>
#समावेश <io.h>

#समावेश "kernel/kernel.h"
#समावेश "kernel/proc.h"
#समावेश "kernel/vm.h"
#समावेश "kernel/proto.h"
#समावेश "arch_proto.h"
#समावेश "bsp_reset.h"

#समावेश "omap_timer_registers.h"
#समावेश "omap_rtc.h"

#घोषणा AM335X_CM_BASE 0x44E00000
#घोषणा AM335_X_CM_आकार 0x1000

#घोषणा AM335X_PRM_DEVICE_OFFSET 0xf00
#घोषणा AM335X_PRM_RSTCTRL_REG 0x00
#घोषणा AM335X_RST_GLOBAL_WARM_SW_BIT 0

#घोषणा DM37XX_CM_BASE 0x48307000
#घोषणा DM37_XX_CM_आकार 0x1000
#घोषणा DM37XX_PRM_RSTCTRL_REG 0x250
#घोषणा DM37XX_RST_DPLL3_BIT 2

काष्ठा omap_reset
{
	vir_bytes base;
	vir_bytes आकार;
};

अटल काष्ठा omap_reset omap_reset;

अटल kern_phys_map reset_phys_map;

व्योम
bsp_reset_init(व्योम)
{
	अगर (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.base = DM37XX_CM_BASE;
		omap_reset.आकार = DM37_XX_CM_आकार;
	} अन्यथा अगर (BOARD_IS_BB(machine.board_id)) {
		omap_reset.base = AM335X_CM_BASE;
		omap_reset.आकार = AM335_X_CM_आकार;
	}

	kern_phys_map_ptr(omap_reset.base, omap_reset.आकार,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_bytes) & omap_reset.base);
}

व्योम
bsp_reset(व्योम)
{
	अगर (BOARD_IS_BBXM(machine.board_id)) {
		mmio_set((omap_reset.base + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37XX_RST_DPLL3_BIT));
	} अन्यथा अगर (BOARD_IS_BB(machine.board_id)) {
		mmio_set((omap_reset.base + AM335X_PRM_DEVICE_OFFSET +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335X_RST_GLOBAL_WARM_SW_BIT));
	}
}

व्योम
bsp_poweroff(व्योम)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	अगर (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_run();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		जबतक (1);
	}
}

व्योम bsp_disable_watchdog(व्योम)
{
        अगर(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0xAAAA);
		जबतक(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0x5555);
		जबतक(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
	}
}

