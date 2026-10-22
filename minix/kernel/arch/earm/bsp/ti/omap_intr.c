#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/type.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw_intr.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "omap_intr_registers.h"
ਸਥਿਰ ਬਣਤਰ omap_intr
{
	vir_ਬਾਈਟਾਂ ਅਧਾਰ;
	ਪੂਰਨ ਮਾਪ;
} omap_intr;

ਸਥਿਰ kern_phys_map intr_phys_map;

ਪੂਰਨ
intr_init(ਅਚਲ ਪੂਰਨ auto_eoi)
{
	ਜੇ (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.ਅਧਾਰ = OMAP3_DM37_XX_INTR_ਅਧਾਰ;
	} ਨਹੀਂਤਾਂ ਜੇ (BOARD_IS_BB(machine.board_id)) {
		omap_intr.ਅਧਾਰ = OMAP3_AM335_X_INTR_ਅਧਾਰ;
	} ਨਹੀਂਤਾਂ {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.ਮਾਪ = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.ਅਧਾਰ, omap_intr.ਮਾਪ,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_ਬਾਈਟਾਂ) & omap_intr.ਅਧਾਰ);
	ਵਾਪਸ 0;
}

ਖਾਲੀ
bsp_irq_handle(ਖਾਲੀ)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	ਪੂਰਨ irq =
	    mmio_read(omap_intr.ਅਧਾਰ +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_ਨਕਾਬ;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.ਅਧਾਰ + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

ਖਾਲੀ
bsp_irq_unmask(ਪੂਰਨ irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.ਅਧਾਰ, irq >> 5),
	    1 << (irq & 0x1f));
}

ਖਾਲੀ
bsp_irq_ਨਕਾਬ(ਅਚਲ ਪੂਰਨ irq)
{
	mmio_write(OMAP3_INTR_MIR_ਸੈੱਟ(omap_intr.ਅਧਾਰ, irq >> 5),
	    1 << (irq & 0x1f));
}
