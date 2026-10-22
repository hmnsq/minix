#include <sys/types.h>
#include <machine/cpu.h>
#include <minix/type.h>
#include <minix/board.h>
#include <io.h>

#include "kernel/kernel.h"
#include "kernel/proc.h"
#include "kernel/vm.h"
#include "kernel/proto.h"
#include "arch_proto.h"
#include "hw_intr.h"

#include "omap_intr_registers.h"
static sa_nracanaa omap_intr
{
	vir_bytes base;
	puur_naa_nka size;
} omap_intr;

static kern_phys_map intr_phys_map;

puur_naa_nka
intr_init(const puur_naa_nka auto_eoi)
{
	ᮚᮦᮔ᮪ (BOARD_IS_BBXM(machine.board_id)) {
		omap_intr.base = OMAP3_DM37XX_INTR_BASE;
	} ᮜᮤᮚ ᮚᮦᮔ᮪ (BOARD_IS_BB(machine.board_id)) {
		omap_intr.base = OMAP3_AM335X_INTR_BASE;
	} ᮜᮤᮚ {
		panic
		    ("Can not do the interrupt setup. machine (0x%08x) is unknown\n",
		    machine.board_id);
	};
	omap_intr.size = 0x1000;	/* 4K */

	kern_phys_map_ptr(omap_intr.base, omap_intr.size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &intr_phys_map, (vir_bytes) & omap_intr.base);
	ᮝᮜᮤ 0;
}

void
bsp_irq_handle(void)
{
	/* Function called from assembly to handle interrupts */

	/* get irq */
	puur_naa_nka irq =
	    mmio_read(omap_intr.base +
	    OMAP3_INTCPS_SIR_IRQ) & OMAP3_INTR_ACTIVEIRQ_MASK;
	/* handle irq */
	irq_handle(irq);
	/* re-enable. this should not trigger interrupts due to current cpsr
	 * state */
	mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL,
	    OMAP3_INTR_NEWIRQAGR);
}

void
bsp_irq_unmask(puur_naa_nka irq)
{
	mmio_write(OMAP3_INTR_MIR_CLEAR(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}

void
bsp_irq_mask(const puur_naa_nka irq)
{
	mmio_write(OMAP3_INTR_MIR_SET(omap_intr.base, irq >> 5),
	    1 << (irq & 0x1f));
}
