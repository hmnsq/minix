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

#અન્તર્ભાવ "omap_serial.h"

સંરચના omap_serial
{
	vir_bytes base;
	vir_bytes size;
};

સ્થિર સંરચના omap_serial omap_serial = {
	.base = 0,
};

સ્થિર kern_phys_map serial_phys_map;

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
શૂન્ય
bsp_ser_init(શૂન્ય)
{
	જો (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.base = OMAP3_DM37XX_DEBUG_UART_BASE;
	} નહીંતર જો (BOARD_IS_BB(machine.board_id)) {
		omap_serial.base = OMAP3_AM335X_DEBUG_UART_BASE;
	}
	omap_serial.size = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.base, omap_serial.size,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_bytes) & omap_serial.base);
	પ્રતિજ્ઞા(omap_serial.base);
}

શૂન્ય
bsp_ser_putc(અક્ષર c)
{
	પૂર્ણાંક i;
	પ્રતિજ્ઞા(omap_serial.base);

	/* Wait until FIFO's empty */
	માટે (i = 0; i < 100000; i++) {
		જો (mmio_read(omap_serial.base + OMAP3_LSR) & OMAP3_LSR_THRE) {
			તોડો;
		}
	}

	/* Write character */
	mmio_write(omap_serial.base + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	માટે (i = 0; i < 100000; i++) {
		જો (mmio_read(omap_serial.base +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			તોડો;
		}
	}
}
