/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

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
#ਅਨ੍ਤਰ੍ਭਾਵ "omap_rtc.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTC_SS_ਅਧਾਰ 0x44e3e000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTC_SS_ਮਾਪ 0x1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTC_CTRL_REG 0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTC_CTRL_RTC_STOP_ਦੋਆਧਾਰੀ_ਅੰਕ 0

ਬਣਤਰ omap_rtc
{
	vir_ਬਾਈਟਾਂ ਅਧਾਰ;
	vir_ਬਾਈਟਾਂ ਮਾਪ;
};

ਸਥਿਰ ਬਣਤਰ omap_rtc omap_rtc = {
	.ਅਧਾਰ = RTC_SS_ਅਧਾਰ,
	.ਮਾਪ = RTC_SS_ਮਾਪ
};

ਸਥਿਰ kern_phys_map rtc_phys_map;

ਖਾਲੀ
omap3_rtc_init(ਖਾਲੀ)
{
	ਜੇ (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.ਅਧਾਰ, omap_rtc.ਮਾਪ,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_ਬਾਈਟਾਂ) & omap_rtc.ਅਧਾਰ);
	}
}

ਖਾਲੀ
omap3_rtc_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
	ਜੇ (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_ਸੈੱਟ((omap_rtc.ਅਧਾਰ + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_ਦੋਆਧਾਰੀ_ਅੰਕ));
	}
}
