/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <sys/types.h>
#অন্তর্ভাৱ <machine/cpu.h>
#অন্তর্ভাৱ <minix/type.h>
#অন্তর্ভাৱ <minix/board.h>
#অন্তর্ভাৱ <io.h>

#অন্তর্ভাৱ "kernel/kernel.h"
#অন্তর্ভাৱ "kernel/proc.h"
#অন্তর্ভাৱ "kernel/vm.h"
#অন্তর্ভাৱ "kernel/proto.h"
#অন্তর্ভাৱ "arch_proto.h"
#অন্তর্ভাৱ "omap_rtc.h"

#define RTC_SS_BASE 0x44e3e000
#define RTC_SS_SIZE 0x1000
#define RTC_CTRL_REG 0x40
#define RTC_CTRL_RTC_STOP_BIT 0

সংরচনা omap_rtc
{
	vir_bytes base;
	vir_bytes size;
};

স্থির সংরচনা omap_rtc omap_rtc = {
	.base = RTC_SS_BASE,
	.size = RTC_SS_SIZE
};

স্থির kern_phys_map rtc_phys_map;

শূন্য
omap3_rtc_init(শূন্য)
{
	যদি (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.base, omap_rtc.size,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_bytes) & omap_rtc.base);
	}
}

শূন্য
omap3_rtc_run(শূন্য)
{
	যদি (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_set((omap_rtc.base + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_BIT));
	}
}
