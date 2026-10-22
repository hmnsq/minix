/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <sys/types.h>
#ଅନ୍ତର୍ଭାଵ <machine/cpu.h>
#ଅନ୍ତର୍ଭାଵ <minix/type.h>
#ଅନ୍ତର୍ଭାଵ <minix/board.h>
#ଅନ୍ତର୍ଭାଵ <io.h>

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "kernel/proc.h"
#ଅନ୍ତର୍ଭାଵ "kernel/vm.h"
#ଅନ୍ତର୍ଭାଵ "kernel/proto.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"
#ଅନ୍ତର୍ଭାଵ "omap_rtc.h"

#define RTC_SS_BASE 0x44e3e000
#define RTC_SS_SIZE 0x1000
#define RTC_CTRL_REG 0x40
#define RTC_CTRL_RTC_STOP_BIT 0

ସଂରଚନା omap_rtc
{
	vir_bytes base;
	vir_bytes size;
};

ସ୍ଥିର ସଂରଚନା omap_rtc omap_rtc = {
	.base = RTC_SS_BASE,
	.size = RTC_SS_SIZE
};

ସ୍ଥିର kern_phys_map rtc_phys_map;

ଶୂନ୍ୟ
omap3_rtc_init(ଶୂନ୍ୟ)
{
	ଯଦି (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.base, omap_rtc.size,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_bytes) & omap_rtc.base);
	}
}

ଶୂନ୍ୟ
omap3_rtc_run(ଶୂନ୍ୟ)
{
	ଯଦି (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_set((omap_rtc.base + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_BIT));
	}
}
