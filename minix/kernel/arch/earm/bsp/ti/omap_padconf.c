/* Implements sys_padconf() for the AM335X and DM37XX. */

#include "kernel/kernel.h"
#include "arch_proto.h"
#include <sys/types.h>
#include <machine/cpu.h>
#include <minix/mmio.h>
#include <minix/padconf.h>
#include <minix/board.h>
#include <minix/com.h>
#include <assert.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>

#include "bsp_padconf.h"

struct omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes size;
	unsigned цео board_filter_value;
	unsigned цео board_filter_mask;
};

static struct omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .size = PADCONF_DM37XX_REGISTERS_SIZE,
		    .board_filter_value = BOARD_FILTER_BBXM_VALUE,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .size = PADCONF_AM335X_REGISTERS_SIZE,
		    .board_filter_value = BOARD_FILTER_BB_VALUE,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
static struct omap_padconf *omap_padconf;

static kern_phys_map padconf_phys_map;

цео
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t value)
{
	/* check that the value will be inside the padconf memory range */
	ако (padconf >= (omap_padconf->size - omap_padconf->offset)) {
		врати EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    value);

	врати OK;
}

void
bsp_padconf_init(void)
{
	цео x;
	omap_padconf = NULL;
	/* find the correct padconf */
	за (x = 0; x < sizeof(omap_padconfs) / sizeof(omap_padconfs[0]); x++) {
		ако ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_value) {
			omap_padconf = &omap_padconfs[x];
			break;
		}
	}
	assert(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->size,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	врати;
}
