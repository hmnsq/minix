/* Implements sys_padconf() for the AM335X and DM37XX. */

#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"
#समावेश <sys/types.h>
#समावेश <machine/cpu.h>
#समावेश <minix/mmio.h>
#समावेश <minix/padconf.h>
#समावेश <minix/board.h>
#समावेश <minix/com.h>
#समावेश <assert.h>
#समावेश <io.h>
#समावेश <stdlib.h>
#समावेश <stdio.h>

#समावेश "bsp_padconf.h"

काष्ठा omap_padconf
{
	vir_bytes base;
	vir_bytes offset;
	vir_bytes आकार;
	अचिन्हित पूर्णांक board_filter_मूल्य;
	अचिन्हित पूर्णांक board_filter_mask;
};

अटल काष्ठा omap_padconf omap_padconfs[] = {
	{
		    .base = PADCONF_DM37XX_REGISTERS_BASE,
		    .offset = PADCONF_DM37XX_REGISTERS_OFFSET,
		    .आकार = PADCONF_DM37_XX_REGISTERS_आकार,
		    .board_filter_मूल्य = BOARD_FILTER_BBXM_मूल्य,
		    .board_filter_mask = BOARD_FILTER_BBXM_MASK,
	    },
	{
		    .base = PADCONF_AM335X_REGISTERS_BASE,
		    .offset = PADCONF_AM335X_REGISTERS_OFFSET,
		    .आकार = PADCONF_AM335_X_REGISTERS_आकार,
		    .board_filter_मूल्य = BOARD_FILTER_BB_मूल्य,
		    .board_filter_mask = BOARD_FILTER_BB_MASK,
	    },
};

/* initialized in init */
अटल काष्ठा omap_padconf *omap_padconf;

अटल kern_phys_map padconf_phys_map;

पूर्णांक
bsp_padconf_set(u32_t padconf, u32_t mask, u32_t मूल्य)
{
	/* check that the value will be inside the padconf memory range */
	अगर (padconf >= (omap_padconf->आकार - omap_padconf->offset)) {
		वापस EINVAL;	/* outside of valid range */
	}

	set32(padconf + omap_padconf->base + omap_padconf->offset, mask,
	    मूल्य);

	वापस OK;
}

व्योम
bsp_padconf_init(व्योम)
{
	पूर्णांक x;
	omap_padconf = शून्य;
	/* find the correct padconf */
	क्रम (x = 0; x < माप(omap_padconfs) / माप(omap_padconfs[0]); x++) {
		अगर ((omap_padconfs[x].board_filter_mask & machine.board_id) ==
		    omap_padconfs[x].board_filter_मूल्य) {
			omap_padconf = &omap_padconfs[x];
			अवरोध;
		}
	}
	निश्चित(omap_padconf);

	kern_phys_map_ptr(omap_padconf->base, omap_padconf->आकार,
	    VMMF_UNCACHED | VMMF_WRITE,
	    &padconf_phys_map, (vir_bytes) & omap_padconf->base);

	वापस;
}
