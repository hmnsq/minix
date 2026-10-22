#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <minix/type.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <io.h>

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "kernel/proc.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ "omap_serial.h"

ರಚನೆ omap_serial
{
	vir_ಬೈಟುಗಳು ತಳ;
	vir_ಬೈಟುಗಳು ಗಾತ್ರ_2;
};

ಸ್ಥಿರ ರಚನೆ omap_serial omap_serial = {
	.ತಳ = 0,
};

ಸ್ಥಿರ kern_phys_map serial_phys_map;

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
ಶೂನ್ಯ
bsp_ser_init(ಶೂನ್ಯ)
{
	ಆದರೆ (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.ತಳ = OMAP3_DM37_XX_DEBUG_UART_ತಳ;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		omap_serial.ತಳ = OMAP3_AM335_X_DEBUG_UART_ತಳ;
	}
	omap_serial.ಗಾತ್ರ_2 = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.ತಳ, omap_serial.ಗಾತ್ರ_2,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_ಬೈಟುಗಳು) & omap_serial.ತಳ);
	ಪ್ರತಿಜ್ಞಾ(omap_serial.ತಳ);
}

ಶೂನ್ಯ
bsp_ser_putc(ಅಕ್ಷರ c)
{
	ಪೂರ್ಣಾಂಕ i;
	ಪ್ರತಿಜ್ಞಾ(omap_serial.ತಳ);

	/* Wait until FIFO's empty */
	ಗಾಗಿ (i = 0; i < 100000; i++) {
		ಆದರೆ (mmio_read(omap_serial.ತಳ + OMAP3_LSR) & OMAP3_LSR_THRE) {
			ನಿಲ್ಲಿಸು;
		}
	}

	/* Write character */
	mmio_write(omap_serial.ತಳ + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	ಗಾಗಿ (i = 0; i < 100000; i++) {
		ಆದರೆ (mmio_read(omap_serial.ತಳ +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			ನಿಲ್ಲಿಸು;
		}
	}
}
