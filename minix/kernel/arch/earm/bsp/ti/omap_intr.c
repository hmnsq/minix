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
#அந்தர்பாவ "hw_intr.h"

#அந்தர்பாவ "omap_intr_registers.h"
நிலையான கட்டமைப்பு omap_intr
{
	vir_bytes base;
	முழு size;
} omap_intr;

நிலையான kern_phys_map intr_phys_map;

முழு
intr_init(மாறிலி முழு auto_eoi)
{
	எனில் (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.base = OMAP3_DM37XX_INTR_BASE;
	} இல்லையெனில் எனில் (BOARD_IS_BB(machine.board_id)) {
		omap_intr.base = OMAP3_AM335X_INTR_BASE;
	} இல்லையெனில் {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.size = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.base, omap_intr.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & omap_intr.base);
	திரும்பு 0;
}

வெறுமை
bsp_irq_handle(வெறுமை)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	முழு irq =
	    mmio_read(omap_intr.base +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_MASK;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

வெறுமை
bsp_irq_unmask(முழு irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}

வெறுமை
bsp_irq_mask(மாறிலி முழு irq)
{
	mmio_write(OMAP3_INTR_MIR_SET(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}
