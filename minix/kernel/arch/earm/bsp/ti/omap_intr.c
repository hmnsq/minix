#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <minix/type.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <io.h>

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "kernel/proc.h"
#అన్తర్భావ "kernel/vm.h"
#అన్తర్భావ "kernel/proto.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "hw_intr.h"

#అన్తర్భావ "omap_intr_registers.h"
స్థిర నిర్మాణం omap_intr
{
	vir_బైట్లు పునాది;
	పూర్ణాంకం కొలత;
} omap_intr;

స్థిర kern_phys_map intr_phys_map;

పూర్ణాంకం
intr_init(స్థిరరాశి పూర్ణాంకం auto_eoi)
{
	ఒకవేళ (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.పునాది = OMAP3_DM37_XX_INTR_పునాది;
	} లేకపోతే ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		omap_intr.పునాది = OMAP3_AM335_X_INTR_పునాది;
	} లేకపోతే {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.కొలత = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.పునాది, omap_intr.కొలత,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_బైట్లు) & omap_intr.పునాది);
	తిరిగి 0;
}

శూన్య
bsp_irq_handle(శూన్య)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	పూర్ణాంకం irq =
	    mmio_read(omap_intr.పునాది +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_ముసుగు;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.పునాది + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

శూన్య
bsp_irq_unmask(పూర్ణాంకం irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.పునాది, irq >> 5),
	    1 << (irq & 0x1f));
}

శూన్య
bsp_irq_ముసుగు(స్థిరరాశి పూర్ణాంకం irq)
{
	mmio_write(OMAP3_INTR_MIR_సమితి(omap_intr.పునాది, irq >> 5),
	    1 << (irq & 0x1f));
}
