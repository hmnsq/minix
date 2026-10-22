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

#ضمن "omap_serial.h"

بنية omap_serial
{
	vir_بائيٽون بنياد;
	vir_بائيٽون سائيز;
};

ساكن بنية omap_serial omap_serial = {
	.بنياد = 0,
};

ساكن kern_phys_map serial_phys_map;

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
فراغ
bsp_ser_init(فراغ)
{
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.بنياد = OMAP3_DM37_XX_DEBUG_UART_بنياد;
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		omap_serial.بنياد = OMAP3_AM335_X_DEBUG_UART_بنياد;
	}
	omap_serial.سائيز = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.بنياد, omap_serial.سائيز,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_بائيٽون) & omap_serial.بنياد);
	أكد(omap_serial.بنياد);
}

فراغ
bsp_ser_putc(حرف c)
{
	صحيح i;
	أكد(omap_serial.بنياد);

	/* Wait until FIFO's empty */
	لكل (i = 0; i < 100000; i++) {
		إذا (mmio_read(omap_serial.بنياد + OMAP3_LSR) & OMAP3_LSR_THRE) {
			اكسر;
		}
	}

	/* Write character */
	mmio_write(omap_serial.بنياد + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	لكل (i = 0; i < 100000; i++) {
		إذا (mmio_read(omap_serial.بنياد +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			اكسر;
		}
	}
}
