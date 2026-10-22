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

#شامل "omap_serial.h"

ساخت omap_serial
{
	vir_بائٹیں بنیاد;
	vir_بائٹیں سائز;
};

ساکن ساخت omap_serial omap_serial = {
	.بنیاد = 0,
};

ساکن kern_phys_map serial_phys_map;

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
خالی
bsp_ser_init(خالی)
{
	اگر (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.بنیاد = OMAP3_DM37_XX_DEBUG_UART_بنیاد;
	} ورنہ اگر (BOARD_IS_BB(machine.board_id)) {
		omap_serial.بنیاد = OMAP3_AM335_X_DEBUG_UART_بنیاد;
	}
	omap_serial.سائز = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.بنیاد, omap_serial.سائز,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_بائٹیں) & omap_serial.بنیاد);
	أكد(omap_serial.بنیاد);
}

خالی
bsp_ser_putc(حرف c)
{
	صحیح i;
	أكد(omap_serial.بنیاد);

	/* Wait until FIFO's empty */
	برائے (i = 0; i < 100000; i++) {
		اگر (mmio_read(omap_serial.بنیاد + OMAP3_LSR) & OMAP3_LSR_THRE) {
			توڑو;
		}
	}

	/* Write character */
	mmio_write(omap_serial.بنیاد + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	برائے (i = 0; i < 100000; i++) {
		اگر (mmio_read(omap_serial.بنیاد +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			توڑو;
		}
	}
}
