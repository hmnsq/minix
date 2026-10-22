/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#ضمن <assert.h>
#ضمن <sys/types.h>
#ضمن <machine/cpu.h>
#ضمن <minix/type.h>
#ضمن <minix/board.h>
#ضمن <io.h>

#ضمن "kernel/kernel.h"
#ضمن "kernel/proc.h"
#ضمن "kernel/vm.h"
#ضمن "kernel/proto.h"
#ضمن "arch_proto.h"
#ضمن "omap_rtc.h"

#وصف_ڪريو RTC_SS_بنياد 0x44e3e000
#وصف_ڪريو RTC_SS_سائيز 0x1000
#وصف_ڪريو RTC_CTRL_REG 0x40
#وصف_ڪريو RTC_CTRL_RTC_STOP_ٻٽو_انگ 0

بنية omap_rtc
{
	vir_بائيٽون بنياد;
	vir_بائيٽون سائيز;
};

ساكن بنية omap_rtc omap_rtc = {
	.بنياد = RTC_SS_بنياد,
	.سائيز = RTC_SS_سائيز
};

ساكن kern_phys_map rtc_phys_map;

فراغ
omap3_rtc_init(فراغ)
{
	إذا (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.بنياد, omap_rtc.سائيز,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_بائيٽون) & omap_rtc.بنياد);
	}
}

فراغ
omap3_rtc_لاڳيتو(فراغ)
{
	إذا (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_سيٽ((omap_rtc.بنياد + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_ٻٽو_انگ));
	}
}
