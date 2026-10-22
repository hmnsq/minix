/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <minix/type.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <io.h>

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "kernel/proc.h"
#అన్తర్భావ "kernel/vm.h"
#అన్తర్భావ "kernel/proto.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "omap_rtc.h"

#నిర్వచించు RTC_SS_పునాది 0x44e3e000
#నిర్వచించు RTC_SS_కొలత 0x1000
#నిర్వచించు RTC_CTRL_REG 0x40
#నిర్వచించు RTC_CTRL_RTC_STOP_బిట్టు 0

నిర్మాణం omap_rtc
{
	vir_బైట్లు పునాది;
	vir_బైట్లు కొలత;
};

స్థిర నిర్మాణం omap_rtc omap_rtc = {
	.పునాది = RTC_SS_పునాది,
	.కొలత = RTC_SS_కొలత
};

స్థిర kern_phys_map rtc_phys_map;

శూన్య
omap3_rtc_init(శూన్య)
{
	ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.పునాది, omap_rtc.కొలత,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_బైట్లు) & omap_rtc.పునాది);
	}
}

శూన్య
omap3_rtc_నిరంతరం(శూన్య)
{
	ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_సమితి((omap_rtc.పునాది + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_బిట్టు));
	}
}
