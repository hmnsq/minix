#अन्तर्भाव <assert.h>
#अन्तर्भाव <sys/types.h>
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <minix/type.h>
#अन्तर्भाव <minix/board.h>
#अन्तर्भाव <io.h>

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "kernel/proc.h"
#अन्तर्भाव "kernel/vm.h"
#अन्तर्भाव "kernel/proto.h"
#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव "omap_serial.h"

संरचना omap_serial
{
	vir_bytes base;
	vir_bytes size;
};

स्थिर संरचना omap_serial omap_serial = {
	.base = 0,
};

स्थिर kern_phys_map serial_phys_map;

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
शून्य
bsp_ser_init(शून्य)
{
	जइ (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.base = OMAP3_DM37XX_DEBUG_UART_BASE;
	} अण्णहा जइ (BOARD_IS_BB(machine.board_id)) {
		omap_serial.base = OMAP3_AM335X_DEBUG_UART_BASE;
	}
	omap_serial.size = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.base, omap_serial.size,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_bytes) & omap_serial.base);
	प्रतिज्ञा(omap_serial.base);
}

शून्य
bsp_ser_putc(वर्ण c)
{
	पूर्णाङ्क i;
	प्रतिज्ञा(omap_serial.base);

	/* Wait until FIFO's empty */
	प्रति (i = 0; i < 100000; i++) {
		जइ (mmio_read(omap_serial.base + OMAP3_LSR) & OMAP3_LSR_THRE) {
			भङ्ग;
		}
	}

	/* Write character */
	mmio_write(omap_serial.base + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	प्रति (i = 0; i < 100000; i++) {
		जइ (mmio_read(omap_serial.base +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			भङ्ग;
		}
	}
}
