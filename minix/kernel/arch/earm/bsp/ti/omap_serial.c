#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <sys/types.h>
#অন্তর্ভাব <machine/cpu.h>
#অন্তর্ভাব <minix/type.h>
#অন্তর্ভাব <minix/board.h>
#অন্তর্ভাব <io.h>

#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "kernel/proc.h"
#অন্তর্ভাব "kernel/vm.h"
#অন্তর্ভাব "kernel/proto.h"
#অন্তর্ভাব "arch_proto.h"

#অন্তর্ভাব "omap_serial.h"

কাঠামো omap_serial
{
	vir_bytes base;
	vir_bytes আকার_2;
};

স্থির কাঠামো omap_serial omap_serial = {
	.base = 0,
};

স্থির kern_phys_map serial_phys_map;

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
শূন্য
bsp_ser_init(শূন্য)
{
	যদি (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.base = OMAP3_DM37XX_DEBUG_UART_BASE;
	} অন্যথা যদি (BOARD_IS_BB(machine.board_id)) {
		omap_serial.base = OMAP3_AM335X_DEBUG_UART_BASE;
	}
	omap_serial.আকার_2 = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.base, omap_serial.আকার_2,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_bytes) & omap_serial.base);
	প্রতিজ্ঞা(omap_serial.base);
}

শূন্য
bsp_ser_putc(অক্ষর c)
{
	পূর্ণসংখ্যা i;
	প্রতিজ্ঞা(omap_serial.base);

	/* Wait until FIFO's empty */
	জন্য (i = 0; i < 100000; i++) {
		যদি (mmio_read(omap_serial.base + OMAP3_LSR) & OMAP3_LSR_THRE) {
			বিরতি;
		}
	}

	/* Write character */
	mmio_write(omap_serial.base + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	জন্য (i = 0; i < 100000; i++) {
		যদি (mmio_read(omap_serial.base +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			বিরতি;
		}
	}
}
