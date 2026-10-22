#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/type.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "omap_serial.h"

ਬਣਤਰ omap_serial
{
	vir_ਬਾਈਟਾਂ ਅਧਾਰ;
	vir_ਬਾਈਟਾਂ ਮਾਪ;
};

ਸਥਿਰ ਬਣਤਰ omap_serial omap_serial = {
	.ਅਧਾਰ = 0,
};

ਸਥਿਰ kern_phys_map serial_phys_map;

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
ਖਾਲੀ
bsp_ser_init(ਖਾਲੀ)
{
	ਜੇ (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.ਅਧਾਰ = OMAP3_DM37_XX_DEBUG_UART_ਅਧਾਰ;
	} ਨਹੀਂਤਾਂ ਜੇ (BOARD_IS_BB(machine.board_id)) {
		omap_serial.ਅਧਾਰ = OMAP3_AM335_X_DEBUG_UART_ਅਧਾਰ;
	}
	omap_serial.ਮਾਪ = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.ਅਧਾਰ, omap_serial.ਮਾਪ,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_ਬਾਈਟਾਂ) & omap_serial.ਅਧਾਰ);
	ਪ੍ਰਤਿਜ੍ਞਾ(omap_serial.ਅਧਾਰ);
}

ਖਾਲੀ
bsp_ser_putc(ਅੱਖਰ c)
{
	ਪੂਰਨ i;
	ਪ੍ਰਤਿਜ੍ਞਾ(omap_serial.ਅਧਾਰ);

	/* Wait until FIFO's empty */
	ਲਈ (i = 0; i < 100000; i++) {
		ਜੇ (mmio_read(omap_serial.ਅਧਾਰ + OMAP3_LSR) & OMAP3_LSR_THRE) {
			ਰੋਕੋ;
		}
	}

	/* Write character */
	mmio_write(omap_serial.ਅਧਾਰ + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	ਲਈ (i = 0; i < 100000; i++) {
		ਜੇ (mmio_read(omap_serial.ਅਧਾਰ +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			ਰੋਕੋ;
		}
	}
}
