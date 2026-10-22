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
ساکن ساخت omap_intr
{
	vir_بائٹیں بنیاد;
	صحیح سائز;
} omap_intr;

ساکن kern_phys_map intr_phys_map;

صحیح
intr_init(ثابت صحیح auto_eoi)
{
	اگر (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.بنیاد = OMAP3_DM37_XX_INTR_بنیاد;
	} ورنہ اگر (BOARD_IS_BB(machine.board_id)) {
		omap_intr.بنیاد = OMAP3_AM335_X_INTR_بنیاد;
	} ورنہ {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.سائز = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.بنیاد, omap_intr.سائز,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_بائٹیں) & omap_intr.بنیاد);
	واپس 0;
}

خالی
bsp_irq_handle(خالی)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	صحیح irq =
	    mmio_read(omap_intr.بنیاد +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_نقاب;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.بنیاد + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

خالی
bsp_irq_unmask(صحیح irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.بنیاد, irq >> 5),
	    1 << (irq & 0x1f));
}

خالی
bsp_irq_نقاب(ثابت صحیح irq)
{
	mmio_write(OMAP3_INTR_MIR_سیٹ(omap_intr.بنیاد, irq >> 5),
	    1 << (irq & 0x1f));
}
