#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/type.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_reset.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "omap_timer_registers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "omap_rtc.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335_X_CM_ਅਧਾਰ 0x44E00000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335_X_CM_ਮਾਪ 0x1000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335_X_PRM_DEVICE_ਵਿੱਥ 0xf00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_PRM_RSTCTRL_REG 0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335_X_RST_GLOBAL_WARM_SW_ਦੋਆਧਾਰੀ_ਅੰਕ 0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DM37_XX_CM_ਅਧਾਰ 0x48307000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DM37_XX_CM_ਮਾਪ 0x1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DM37XX_PRM_RSTCTRL_REG 0x250
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DM37_XX_RST_DPLL3_ਦੋਆਧਾਰੀ_ਅੰਕ 2

ਬਣਤਰ omap_reset
{
	vir_ਬਾਈਟਾਂ ਅਧਾਰ;
	vir_ਬਾਈਟਾਂ ਮਾਪ;
};

ਸਥਿਰ ਬਣਤਰ omap_reset omap_reset;

ਸਥਿਰ kern_phys_map reset_phys_map;

ਖਾਲੀ
bsp_reset_init(ਖਾਲੀ)
{
	ਜੇ (BOARD_IS_BBXM(machine.board_id)) {
		omap_reset.ਅਧਾਰ = DM37_XX_CM_ਅਧਾਰ;
		omap_reset.ਮਾਪ = DM37_XX_CM_ਮਾਪ;
	} ਨਹੀਂਤਾਂ ਜੇ (BOARD_IS_BB(machine.board_id)) {
		omap_reset.ਅਧਾਰ = AM335_X_CM_ਅਧਾਰ;
		omap_reset.ਮਾਪ = AM335_X_CM_ਮਾਪ;
	}

	kern_phys_map_ptr(omap_reset.ਅਧਾਰ, omap_reset.ਮਾਪ,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &reset_phys_map, (vir_ਬਾਈਟਾਂ) & omap_reset.ਅਧਾਰ);
}

ਖਾਲੀ
bsp_reset(ਖਾਲੀ)
{
	ਜੇ (BOARD_IS_BBXM(machine.board_id)) {
		mmio_ਸੈੱਟ((omap_reset.ਅਧਾਰ + DM37XX_PRM_RSTCTRL_REG),
		    (1 << DM37_XX_RST_DPLL3_ਦੋਆਧਾਰੀ_ਅੰਕ));
	} ਨਹੀਂਤਾਂ ਜੇ (BOARD_IS_BB(machine.board_id)) {
		mmio_ਸੈੱਟ((omap_reset.ਅਧਾਰ + AM335_X_PRM_DEVICE_ਵਿੱਥ +
			AM335X_PRM_RSTCTRL_REG),
		    (1 << AM335_X_RST_GLOBAL_WARM_SW_ਦੋਆਧਾਰੀ_ਅੰਕ));
	}
}

ਖਾਲੀ
bsp_poweroff(ਖਾਲੀ)
{

/*
 * The am335x can signal an external power management chip to cut the power
 * by toggling the PMIC_POWER_EN pin. It might fail if there isn't an
 * external PMIC or if the PMIC hasn't been configured to respond to toggles.
 * The only way to pull the pin low is via ALARM2 (see TRM 20.3.3.8).
 * At this point PM should have already signaled readclock to set the alarm.
 */
	ਜੇ (BOARD_IS_BB(machine.board_id)) {
		/* rtc was frozen to prevent premature power-off, unfreeze it
		 * now */
		omap3_rtc_ਲੜੀਵਾਰ();

		/* wait for the alarm to go off and PMIC to disable power to
		 * SoC */
		ਜਦੋਂ (1);
	}
}

ਖਾਲੀ bsp_disable_watchdog(ਖਾਲੀ)
{
        ਜੇ(BOARD_IS_BB(machine.board_id)) {
		mmio_write(AM335_X_WDT_ਅਧਾਰ+AM335X_WDT_WSPR, 0xAAAA);
		ਜਦੋਂ(mmio_read(AM335_X_WDT_ਅਧਾਰ+AM335X_WDT_WWPS) != 0) ;
		mmio_write(AM335_X_WDT_ਅਧਾਰ+AM335X_WDT_WSPR, 0x5555);
		ਜਦੋਂ(mmio_read(AM335_X_WDT_ਅਧਾਰ+AM335X_WDT_WWPS) != 0) ;
	}
}

