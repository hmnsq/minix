/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#شامل <assert.h>
#شامل <sys/types.h>
#شامل <machine/cpu.h>
#شامل <minix/type.h>
#شامل <minix/board.h>
#شامل <io.h>

#شامل "kernel/kernel.h"
#شامل "kernel/proc.h"
#شامل "kernel/vm.h"
#شامل "kernel/proto.h"
#شامل "arch_proto.h"
#شامل "omap_rtc.h"

#تعریف_کرو RTC_SS_بنیاد 0x44e3e000
#تعریف_کرو RTC_SS_سائز 0x1000
#تعریف_کرو RTC_CTRL_REG 0x40
#تعریف_کرو RTC_CTRL_RTC_STOP_ثنائی_ہندسہ 0

ساخت omap_rtc
{
	vir_بائٹیں بنیاد;
	vir_بائٹیں سائز;
};

ساکن ساخت omap_rtc omap_rtc = {
	.بنیاد = RTC_SS_بنیاد,
	.سائز = RTC_SS_سائز
};

ساکن kern_phys_map rtc_phys_map;

خالی
omap3_rtc_init(خالی)
{
	اگر (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.بنیاد, omap_rtc.سائز,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_بائٹیں) & omap_rtc.بنیاد);
	}
}

خالی
omap3_rtc_تسلسل(خالی)
{
	اگر (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_سیٹ((omap_rtc.بنیاد + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_ثنائی_ہندسہ));
	}
}
