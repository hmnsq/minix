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
#অন্তর্ভাব "hw_intr.h"

#অন্তর্ভাব "omap_intr_registers.h"
স্থির কাঠামো omap_intr
{
	vir_bytes base;
	পূর্ণসংখ্যা আকার_2;
} omap_intr;

স্থির kern_phys_map intr_phys_map;

পূর্ণসংখ্যা
intr_init(ধ্রুবক পূর্ণসংখ্যা auto_eoi)
{
	যদি (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.base = OMAP3_DM37XX_INTR_BASE;
	} অন্যথা যদি (BOARD_IS_BB(machine.board_id)) {
		omap_intr.base = OMAP3_AM335X_INTR_BASE;
	} অন্যথা {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.আকার_2 = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.base, omap_intr.আকার_2,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & omap_intr.base);
	ফেরত 0;
}

শূন্য
bsp_irq_handle(শূন্য)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	পূর্ণসংখ্যা irq =
	    mmio_read(omap_intr.base +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_MASK;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

শূন্য
bsp_irq_unmask(পূর্ণসংখ্যা irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}

শূন্য
bsp_irq_mask(ধ্রুবক পূর্ণসংখ্যা irq)
{
	mmio_write(OMAP3_INTR_MIR_SET(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}
