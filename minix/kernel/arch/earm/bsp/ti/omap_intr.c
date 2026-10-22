#شامل <sys/types.h>
#شامل <machine/cpu.h>
#شامل <minix/type.h>
#شامل <minix/board.h>
#شامل <io.h>

#شامل "kernel/kernel.h"
#شامل "kernel/proc.h"
#شامل "kernel/vm.h"
#شامل "kernel/proto.h"
#شامل "arch_proto.h"
#شامل "hw_intr.h"

#شامل "omap_intr_registers.h"
ایستا ساختار omap_intr
{
	vir_bytes base;
	صحیح size;
} omap_intr;

ایستا kern_phys_map intr_phys_map;

صحیح
intr_init(ثابت صحیح auto_eoi)
{
	اگر (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.base = OMAP3_DM37XX_INTR_BASE;
	} وگرنه اگر (BOARD_IS_BB(machine.board_id)) {
		omap_intr.base = OMAP3_AM335X_INTR_BASE;
	} وگرنه {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.size = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.base, omap_intr.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & omap_intr.base);
	بازگشت 0;
}

تهی
bsp_irq_handle(تهی)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	صحیح irq =
	    mmio_read(omap_intr.base +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_MASK;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

تهی
bsp_irq_unmask(صحیح irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}

تهی
bsp_irq_mask(ثابت صحیح irq)
{
	mmio_write(OMAP3_INTR_MIR_SET(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}
