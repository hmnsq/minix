/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

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
#ಅನ್ತರ್ಭಾವ "omap_rtc.h"

#ವ್ಯಾಖ್ಯಾನಿಸು RTC_SS_ತಳ 0x44e3e000
#ವ್ಯಾಖ್ಯಾನಿಸು RTC_SS_ಗಾತ್ರ 0x1000
#ವ್ಯಾಖ್ಯಾನಿಸು RTC_CTRL_REG 0x40
#ವ್ಯಾಖ್ಯಾನಿಸು RTC_CTRL_RTC_STOP_ಬಿಟ್ 0

ರಚನೆ omap_rtc
{
	vir_ಬೈಟುಗಳು ತಳ;
	vir_ಬೈಟುಗಳು ಗಾತ್ರ_2;
};

ಸ್ಥಿರ ರಚನೆ omap_rtc omap_rtc = {
	.ತಳ = RTC_SS_ತಳ,
	.ಗಾತ್ರ_2 = RTC_SS_ಗಾತ್ರ
};

ಸ್ಥಿರ kern_phys_map rtc_phys_map;

ಶೂನ್ಯ
omap3_rtc_init(ಶೂನ್ಯ)
{
	ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.ತಳ, omap_rtc.ಗಾತ್ರ_2,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_ಬೈಟುಗಳು) & omap_rtc.ತಳ);
	}
}

ಶೂನ್ಯ
omap3_rtc_ಸತತ(ಶೂನ್ಯ)
{
	ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_ಗಣ((omap_rtc.ತಳ + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_ಬಿಟ್));
	}
}
