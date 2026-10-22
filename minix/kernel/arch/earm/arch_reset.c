#అన్తర్భావ "kernel/kernel.h"

#అన్తర్భావ <unistd.h>
#అన్తర్భావ <ctype.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <machine/vm.h>
#అన్తర్భావ <io.h>

#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <sys/reboot.h>

#అన్తర్భావ <minix/u64.h>

#అన్తర్భావ "archconst.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "bsp_reset.h"
#అన్తర్భావ "bsp_serial.h"
#అన్తర్భావ "kernel/proc.h"
#అన్తర్భావ "kernel/debug.h"
#అన్తర్భావ "direct_utils.h"
#అన్తర్భావ <machine/multiboot.h>

శూన్య
halt_cpu(శూన్య)
{
	asm అస్థిర("dsb");
	asm అస్థిర("cpsie i");
	asm అస్థిర("wfi");
	asm అస్థిర("cpsid i");
}

శూన్య
reset(శూన్య)
{
	bsp_reset(); /* should not exit */
	ప్రత్యక్ష_print("Reset not supported.");
	అయితే (1);
}

శూన్య
poweroff(శూన్య)
{
	bsp_poweroff();
	/* fallback option: hang */
	ప్రత్యక్ష_print("Unable to power-off this device.");
	అయితే (1);
}

__dead శూన్య
arch_shutdown(పూర్ణాంకం how)
{

	ఒకవేళ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ఒకవేళ(how & RB_HALT) {
		/* Hang */
		కొరకు (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	అయితే (1);
}

#నిర్వచితమైతే DEBUG_SERIAL
శూన్య
ser_putc(అక్షరం c)
{
	bsp_ser_putc(c);
}

#అయితే_ముగింపు
