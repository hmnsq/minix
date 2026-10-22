/*
 * This is a mini driver for the AM335X Real Time Clock. The majority of the
 * work is done in user space in readclock, but for power-off the clock needs
 * to be put into run mode at the last possible moment in arch_reset.c. This
 * driver just implements mapping the memory and re-starting the clock.
 */

#समावेश <assert.h>
#समावेश <sys/types.h>
#समावेश <machine/cpu.h>
#समावेश <minix/type.h>
#समावेश <minix/board.h>
#समावेश <io.h>

#समावेश "kernel/kernel.h"
#समावेश "kernel/proc.h"
#समावेश "kernel/vm.h"
#समावेश "kernel/proto.h"
#समावेश "arch_proto.h"
#समावेश "omap_rtc.h"

#घोषणा RTC_SS_BASE 0x44e3e000
#घोषणा RTC_SS_आकार 0x1000
#घोषणा RTC_CTRL_REG 0x40
#घोषणा RTC_CTRL_RTC_STOP_BIT 0

काष्ठा omap_rtc
{
	vir_bytes base;
	vir_bytes आकार;
};

अटल काष्ठा omap_rtc omap_rtc = {
	.base = RTC_SS_BASE,
	.आकार = RTC_SS_आकार
};

अटल kern_phys_map rtc_phys_map;

व्योम
omap3_rtc_init(व्योम)
{
	अगर (BOARD_IS_BB(machine.board_id)) {
		kern_phys_map_ptr(omap_rtc.base, omap_rtc.आकार,
		    VMMF_UNCACHED | VMMF_WRITE, &rtc_phys_map,
		    (vir_bytes) & omap_rtc.base);
	}
}

व्योम
omap3_rtc_run(व्योम)
{
	अगर (BOARD_IS_BB(machine.board_id)) {
		/* Setting the stop bit starts the RTC running */
		mmio_set((omap_rtc.base + RTC_CTRL_REG),
		    (1 << RTC_CTRL_RTC_STOP_BIT));
	}
}
