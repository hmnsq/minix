#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <minix/type.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <io.h>

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "kernel/proc.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ "hw_intr.h"

#ಅನ್ತರ್ಭಾವ "omap_intr_registers.h"
ಸ್ಥಿರ ರಚನೆ omap_intr
{
	vir_ಬೈಟುಗಳು ತಳ;
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2;
} omap_intr;

ಸ್ಥಿರ kern_phys_map intr_phys_map;

ಪೂರ್ಣಾಂಕ
intr_init(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ auto_eoi)
{
	ಆದರೆ (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.ತಳ = OMAP3_DM37_XX_INTR_ತಳ;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		omap_intr.ತಳ = OMAP3_AM335_X_INTR_ತಳ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.ಗಾತ್ರ_2 = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.ತಳ, omap_intr.ಗಾತ್ರ_2,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_ಬೈಟುಗಳು) & omap_intr.ತಳ);
	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ
bsp_irq_handle(ಶೂನ್ಯ)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	ಪೂರ್ಣಾಂಕ irq =
	    mmio_read(omap_intr.ತಳ +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_ಮುಖವಾಡ;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.ತಳ + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

ಶೂನ್ಯ
bsp_irq_unmask(ಪೂರ್ಣಾಂಕ irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.ತಳ, irq >> 5),
	    1 << (irq & 0x1f));
}

ಶೂನ್ಯ
bsp_irq_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ irq)
{
	mmio_write(OMAP3_INTR_MIR_ಗಣ(omap_intr.ತಳ, irq >> 5),
	    1 << (irq & 0x1f));
}
