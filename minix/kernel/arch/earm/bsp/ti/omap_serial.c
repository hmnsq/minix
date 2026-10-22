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

#ଅନ୍ତର୍ଭାଵ "omap_serial.h"

ସଂରଚନା omap_serial
{
	vir_bytes base;
	vir_bytes size;
};

ସ୍ଥିର ସଂରଚନା omap_serial omap_serial = {
	.base = 0,
};

ସ୍ଥିର kern_phys_map serial_phys_map;

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
ଶୂନ୍ୟ
bsp_ser_init(ଶୂନ୍ୟ)
{
	ଯଦି (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.base = OMAP3_DM37XX_DEBUG_UART_BASE;
	} ଅନ୍ୟଥା ଯଦି (BOARD_IS_BB(machine.board_id)) {
		omap_serial.base = OMAP3_AM335X_DEBUG_UART_BASE;
	}
	omap_serial.size = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.base, omap_serial.size,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_bytes) & omap_serial.base);
	ପ୍ରତିଜ୍ଞା(omap_serial.base);
}

ଶୂନ୍ୟ
bsp_ser_putc(ଅକ୍ଷର c)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପ୍ରତିଜ୍ଞା(omap_serial.base);

	/* Wait until FIFO's empty */
	ପାଇଁ (i = 0; i < 100000; i++) {
		ଯଦି (mmio_read(omap_serial.base + OMAP3_LSR) & OMAP3_LSR_THRE) {
			ଭାଙ୍ଗ;
		}
	}

	/* Write character */
	mmio_write(omap_serial.base + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	ପାଇଁ (i = 0; i < 100000; i++) {
		ଯଦି (mmio_read(omap_serial.base +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			ଭାଙ୍ଗ;
		}
	}
}
