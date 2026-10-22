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
#अन्तर्भाव "hw_intr.h"

#अन्तर्भाव "omap_intr_registers.h"
स्थिर संरचना omap_intr
{
	vir_अष्टकानि आधारः;
	पूर्णाङ्क आकार;
} omap_intr;

स्थिर kern_phys_map intr_phys_map;

पूर्णाङ्क
intr_init(ध्रुव पूर्णाङ्क auto_eoi)
{
	यदि (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.आधारः = OMAP3_DM37_XX_INTR_आधारः;
	} अन्यथा यदि (BOARD_IS_BB(machine.board_id)) {
		omap_intr.आधारः = OMAP3_AM335_X_INTR_आधारः;
	} अन्यथा {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.आकार = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.आधारः, omap_intr.आकार,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_अष्टकानि) & omap_intr.आधारः);
	प्रत्यागम 0;
}

शून्य
bsp_irq_handle(शून्य)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	पूर्णाङ्क irq =
	    mmio_read(omap_intr.आधारः +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_आवरणम्;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.आधारः + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

शून्य
bsp_irq_unmask(पूर्णाङ्क irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.आधारः, irq >> 5),
	    1 << (irq & 0x1f));
}

शून्य
bsp_irq_आवरणम्(ध्रुव पूर्णाङ्क irq)
{
	mmio_write(OMAP3_INTR_MIR_समुच्चयः(omap_intr.आधारः, irq >> 5),
	    1 << (irq & 0x1f));
}
