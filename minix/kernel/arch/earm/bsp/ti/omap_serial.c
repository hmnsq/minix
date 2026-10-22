#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ <machine/cpu.h>
#അന്തര്ഭാവ <minix/type.h>
#അന്തര്ഭാവ <minix/board.h>
#അന്തര്ഭാവ <io.h>

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "kernel/proc.h"
#അന്തര്ഭാവ "kernel/vm.h"
#അന്തര്ഭാവ "kernel/proto.h"
#അന്തര്ഭാവ "arch_proto.h"

#അന്തര്ഭാവ "omap_serial.h"

ഘടന omap_serial
{
	vir_bytes base;
	vir_bytes size;
};

സ്ഥിതം ഘടന omap_serial omap_serial = {
	.base = 0,
};

സ്ഥിതം kern_phys_map serial_phys_map;

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
ശൂന്യം
bsp_ser_init(ശൂന്യം)
{
	എങ്കിൽ (BOARD_IS_BBXM(machine.board_id)) {
		omap_serial.base = OMAP3_DM37XX_DEBUG_UART_BASE;
	} അല്ലെങ്കിൽ എങ്കിൽ (BOARD_IS_BB(machine.board_id)) {
		omap_serial.base = OMAP3_AM335X_DEBUG_UART_BASE;
	}
	omap_serial.size = 0x1000;	/* 4k */

	kern_phys_map_ptr(omap_serial.base, omap_serial.size,
	    VMMF_UNCACHED | VMMF_WRITE, &serial_phys_map,
	    (vir_bytes) & omap_serial.base);
	പ്രതിജ്ഞാ(omap_serial.base);
}

ശൂന്യം
bsp_ser_putc(അക്ഷരം c)
{
	പൂർണ്ണസംഖ്യ i;
	പ്രതിജ്ഞാ(omap_serial.base);

	/* Wait until FIFO's empty */
	വേണ്ടി (i = 0; i < 100000; i++) {
		എങ്കിൽ (mmio_read(omap_serial.base + OMAP3_LSR) & OMAP3_LSR_THRE) {
			നിർത്തുക;
		}
	}

	/* Write character */
	mmio_write(omap_serial.base + OMAP3_THR, c);

	/* And wait again until FIFO's empty to prevent TTY from overwriting */
	വേണ്ടി (i = 0; i < 100000; i++) {
		എങ്കിൽ (mmio_read(omap_serial.base +
			OMAP3_LSR) & (OMAP3_LSR_THRE | OMAP3_LSR_TEMT)) {
			നിർത്തുക;
		}
	}
}
