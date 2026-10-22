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
#અન્તર્ભાવ "hw_intr.h"

#અન્તર્ભાવ "omap_intr_registers.h"
સ્થિર સંરચના omap_intr
{
	vir_bytes base;
	પૂર્ણાંક size;
} omap_intr;

સ્થિર kern_phys_map intr_phys_map;

પૂર્ણાંક
intr_init(અચલ પૂર્ણાંક auto_eoi)
{
	જો (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.base = OMAP3_DM37XX_INTR_BASE;
	} નહીંતર જો (BOARD_IS_BB(machine.board_id)) {
		omap_intr.base = OMAP3_AM335X_INTR_BASE;
	} નહીંતર {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.size = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.base, omap_intr.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & omap_intr.base);
	પાછા 0;
}

શૂન્ય
bsp_irq_handle(શૂન્ય)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	પૂર્ણાંક irq =
	    mmio_read(omap_intr.base +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_MASK;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

શૂન્ય
bsp_irq_unmask(પૂર્ણાંક irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}

શૂન્ય
bsp_irq_mask(અચલ પૂર્ણાંક irq)
{
	mmio_write(OMAP3_INTR_MIR_SET(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}
