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

#అన్తర్భావ "omap_serial.h"

నిర్మాణం omap_serial
{
	vir_బైట్లు పునాది;
	vir_బైట్లు కొలత;
};

స్థిర నిర్మాణం omap_serial omap_serial = {
	.పునాది = 0,
};

స్థిర kern_phys_map serial_phys_map;

/* 
 * In kernel serial for the omap. The serial driver, like most other
 * drivers, needs to be started early and even before the MMU is turned on.
 * We start by directly accessing the hardware memory address. Later on
 * when the MMU is turned on we still use a 1:1 mapping for these addresses.
 *
 * Pretty soon we are going to remap these addresses at a later stage. And this
 * requires us to use a dynamic base address. The idea is to receive a callback
 * from VM with the new address to use.
 *
 * We also try on the BeagleBone port to keep the differences between the
 * drivers to a minimum by initializing a struct here and not using (too many)
 * constants in the code.
 *
 * The serial driver also gets used in the "pre_init" stage before the kernel is loaded
 * in high memory, so keep in mind there are two copies of this code in the kernel.
 */
శూన్య
bsp_ser_init(శూన్య)
{
	ఒకవేళ (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.పునాది = OMAP3_DM37_XX_DEBUG_UART_పునాది;
	} లేకపోతే ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		omap_serial.పునాది = OMAP3_AM335_X_DEBUG_UART_పునాది;
	}
	omap_serial.కొలత = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.పునాది, omap_serial.కొలత,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_బైట్లు) & omap_serial.పునాది);
	ప్రతిజ్ఞా(omap_serial.పునాది);
}

శూన్య
bsp_ser_putc(అక్షరం c)
{
	పూర్ణాంకం i;
	ప్రతిజ్ఞా(omap_serial.పునాది);

	/* Wait until FIFO's empty */
	కొరకు (i = 0; i < 100000; i++) {
		ఒకవేళ (mmio_read(omap_serial.పునాది + OMAP3_LSR) & OMAP3_LSR_THRE) {
			ఆపు;
		}
	}

	/* Write character */
	mmio_write(omap_serial.పునాది + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	కొరకు (i = 0; i < 100000; i++) {
		ఒకవేళ (mmio_read(omap_serial.పునాది +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			ఆపు;
		}
	}
}
