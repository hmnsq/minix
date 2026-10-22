/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ <machine/cpu.h>
#അന്തര്ഭാവ <minix/type.h>
#അന്തര്ഭാവ <minix/board.h>
#അന്തര്ഭാവ <io.h>

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "kernel/proc.h"
#അന്തര്ഭാവ "kernel/vm.h"
#അന്തര്ഭാവ "kernel/proto.h"
#അന്തര്ഭാവ "arch_proto.h"
#അന്തര്ഭാവ "omap_rtc.h"

#define RTC_SS_BASE 0x44e3e000
#define RTC_SS_SIZE 0x1000
#define RTC_CTRL_REG 0x40
#define RTC_CTRL_RTC_STOP_BIT 0

ഘടന omap_rtc
{
	vir_bytes base;
	vir_bytes size;
};

സ്ഥിതം ഘടന omap_rtc omap_rtc = {
	.base = RTC_SS_BASE,
	.size = RTC_SS_SIZE
};

സ്ഥിതം kern_phys_map rtc_phys_map;

ശൂന്യം
omap3_rtc_init(ശൂന്യം)
{
	എങ്കിൽ (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.base, omap_rtc.size,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_bytes) & omap_rtc.base);
	}
}

ശൂന്യം
omap3_rtc_run(ശൂന്യം)
{
	എങ്കിൽ (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_set((omap_rtc.base + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_BIT));
	}
}
