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
#ଅନ୍ତର୍ଭାଵ "hw_intr.h"

#ଅନ୍ତର୍ଭାଵ "omap_intr_registers.h"
ସ୍ଥିର ସଂରଚନା omap_intr
{
	vir_bytes base;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size;
} omap_intr;

ସ୍ଥିର kern_phys_map intr_phys_map;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
intr_init(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା auto_eoi)
{
	ଯଦି (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.base = OMAP3_DM37XX_INTR_BASE;
	} ଅନ୍ୟଥା ଯଦି (BOARD_IS_BB(machine.board_id)) {
		omap_intr.base = OMAP3_AM335X_INTR_BASE;
	} ଅନ୍ୟଥା {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.size = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.base, omap_intr.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & omap_intr.base);
	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ
bsp_irq_handle(ଶୂନ୍ୟ)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq =
	    mmio_read(omap_intr.base +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_MASK;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

ଶୂନ୍ୟ
bsp_irq_unmask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}

ଶୂନ୍ୟ
bsp_irq_mask(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq)
{
	mmio_write(OMAP3_INTR_MIR_SET(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}
