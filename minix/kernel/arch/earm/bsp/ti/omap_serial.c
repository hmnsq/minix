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

#அந்தர்பாவ "omap_serial.h"

கட்டமைப்பு omap_serial
{
	vir_bytes base;
	vir_bytes size;
};

நிலையான கட்டமைப்பு omap_serial omap_serial = {
	.base = 0,
};

நிலையான kern_phys_map serial_phys_map;

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
வெறுமை
bsp_ser_init(வெறுமை)
{
	எனில் (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.base = OMAP3_DM37XX_DEBUG_UART_BASE;
	} இல்லையெனில் எனில் (BOARD_IS_BB(machine.board_id)) {
		omap_serial.base = OMAP3_AM335X_DEBUG_UART_BASE;
	}
	omap_serial.size = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.base, omap_serial.size,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_bytes) & omap_serial.base);
	ப்ரதிஜ்ஞா(omap_serial.base);
}

வெறுமை
bsp_ser_putc(எழுத்து c)
{
	முழு i;
	ப்ரதிஜ்ஞா(omap_serial.base);

	/* Wait until FIFO's empty */
	க்கு (i = 0; i < 100000; i++) {
		எனில் (mmio_read(omap_serial.base + OMAP3_LSR) & OMAP3_LSR_THRE) {
			நிறுத்து;
		}
	}

	/* Write character */
	mmio_write(omap_serial.base + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	க்கு (i = 0; i < 100000; i++) {
		எனில் (mmio_read(omap_serial.base +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			நிறுத்து;
		}
	}
}
