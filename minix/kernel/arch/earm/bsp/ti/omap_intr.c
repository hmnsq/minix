#समावेश <sys/types.h>
#समावेश <machine/cpu.h>
#समावेश <minix/type.h>
#समावेश <minix/board.h>
#समावेश <io.h>

#समावेश "kernel/kernel.h"
#समावेश "kernel/proc.h"
#समावेश "kernel/vm.h"
#समावेश "kernel/proto.h"
#समावेश "arch_proto.h"
#समावेश "hw_intr.h"

#समावेश "omap_intr_registers.h"
अटल काष्ठा omap_intr
{
	vir_bytes base;
	पूर्णांक आकार;
} omap_intr;

अटल kern_phys_map intr_phys_map;

पूर्णांक
intr_init(स्थिर पूर्णांक auto_eoi)
{
	अगर (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.base = OMAP3_DM37XX_INTR_BASE;
	} अन्यथा अगर (BOARD_IS_BB(machine.board_id)) {
		omap_intr.base = OMAP3_AM335X_INTR_BASE;
	} अन्यथा {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.आकार = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.base, omap_intr.आकार,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & omap_intr.base);
	वापस 0;
}

व्योम
bsp_irq_handle(व्योम)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	पूर्णांक irq =
	    mmio_read(omap_intr.base +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_MASK;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

व्योम
bsp_irq_unmask(पूर्णांक irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}

व्योम
bsp_irq_mask(स्थिर पूर्णांक irq)
{
	mmio_write(OMAP3_INTR_MIR_SET(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}
