/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <sys/types.h>
#અન્તર્ભાવ <machine/cpu.h>
#અન્તર્ભાવ <minix/type.h>
#અન્તર્ભાવ <minix/board.h>
#અન્તર્ભાવ <io.h>

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "kernel/proc.h"
#અન્તર્ભાવ "kernel/vm.h"
#અન્તર્ભાવ "kernel/proto.h"
#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ "omap_rtc.h"

#define RTC_SS_BASE 0x44e3e000
#define RTC_SS_SIZE 0x1000
#define RTC_CTRL_REG 0x40
#define RTC_CTRL_RTC_STOP_BIT 0

સંરચના omap_rtc
{
	vir_bytes base;
	vir_bytes size;
};

સ્થિર સંરચના omap_rtc omap_rtc = {
	.base = RTC_SS_BASE,
	.size = RTC_SS_SIZE
};

સ્થિર kern_phys_map rtc_phys_map;

શૂન્ય
omap3_rtc_init(શૂન્ય)
{
	જો (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.base, omap_rtc.size,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_bytes) & omap_rtc.base);
	}
}

શૂન્ય
omap3_rtc_run(શૂન્ય)
{
	જો (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_set((omap_rtc.base + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_BIT));
	}
}
