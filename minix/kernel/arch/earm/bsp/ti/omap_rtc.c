/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <sys/types.h>
#அந்தர்பாவ <machine/cpu.h>
#அந்தர்பாவ <minix/type.h>
#அந்தர்பாவ <minix/board.h>
#அந்தர்பாவ <io.h>

#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "kernel/proc.h"
#அந்தர்பாவ "kernel/vm.h"
#அந்தர்பாவ "kernel/proto.h"
#அந்தர்பாவ "arch_proto.h"
#அந்தர்பாவ "omap_rtc.h"

#define RTC_SS_BASE 0x44e3e000
#define RTC_SS_SIZE 0x1000
#define RTC_CTRL_REG 0x40
#define RTC_CTRL_RTC_STOP_BIT 0

கட்டமைப்பு omap_rtc
{
	vir_bytes base;
	vir_bytes size;
};

நிலையான கட்டமைப்பு omap_rtc omap_rtc = {
	.base = RTC_SS_BASE,
	.size = RTC_SS_SIZE
};

நிலையான kern_phys_map rtc_phys_map;

வெறுமை
omap3_rtc_init(வெறுமை)
{
	எனில் (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.base, omap_rtc.size,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_bytes) & omap_rtc.base);
	}
}

வெறுமை
omap3_rtc_run(வெறுமை)
{
	எனில் (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_set((omap_rtc.base + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_BIT));
	}
}
