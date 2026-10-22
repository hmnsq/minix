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
#അന്തര്ഭാവ "hw_intr.h"

#അന്തര്ഭാവ "omap_intr_registers.h"
സ്ഥിതം ഘടന omap_intr
{
	vir_bytes base;
	പൂർണ്ണസംഖ്യ size;
} omap_intr;

സ്ഥിതം kern_phys_map intr_phys_map;

പൂർണ്ണസംഖ്യ
intr_init(സ്ഥിരം പൂർണ്ണസംഖ്യ auto_eoi)
{
	എങ്കിൽ (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.base = OMAP3_DM37XX_INTR_BASE;
	} അല്ലെങ്കിൽ എങ്കിൽ (BOARD_IS_BB(machine.board_id)) {
		omap_intr.base = OMAP3_AM335X_INTR_BASE;
	} അല്ലെങ്കിൽ {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.size = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.base, omap_intr.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & omap_intr.base);
	തിരികെ 0;
}

ശൂന്യം
bsp_irq_handle(ശൂന്യം)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	പൂർണ്ണസംഖ്യ irq =
	    mmio_read(omap_intr.base +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_MASK;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

ശൂന്യം
bsp_irq_unmask(പൂർണ്ണസംഖ്യ irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}

ശൂന്യം
bsp_irq_mask(സ്ഥിരം പൂർണ്ണസംഖ്യ irq)
{
	mmio_write(OMAP3_INTR_MIR_SET(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}
