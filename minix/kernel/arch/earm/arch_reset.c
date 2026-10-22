#ضمن "kernel/kernel.h"

#ضمن <unistd.h>
#ضمن <ctype.h>
#ضمن <string.h>
#ضمن <machine/cpu.h>
#ضمن <assert.h>
#ضمن <signal.h>
#ضمن <machine/vm.h>
#ضمن <io.h>

#ضمن <minix/board.h>
#ضمن <sys/reboot.h>

#ضمن <minix/u64.h>

#ضمن "archconst.h"
#ضمن "arch_proto.h"
#ضمن "bsp_reset.h"
#ضمن "bsp_serial.h"
#ضمن "kernel/proc.h"
#ضمن "kernel/debug.h"
#ضمن "direct_utils.h"
#ضمن <machine/multiboot.h>

فراغ
halt_cpu(فراغ)
{
	asm متطاير("dsb");
	asm متطاير("cpsie i");
	asm متطاير("wfi");
	asm متطاير("cpsid i");
}

فراغ
reset(فراغ)
{
	bsp_reset(); /* should not exit */
	direct_اطبع("Reset not supported.");
	طالما (1);
}

فراغ
poweroff(فراغ)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_اطبع("Unable to power-off this device.");
	طالما (1);
}

__dead فراغ
arch_shutdown(صحيح how)
{

	إذا((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	إذا(how & RB_HALT) {
		/* Hang */
		لكل (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	طالما (1);
}

#ifdef DEBUG_SERIAL
فراغ
ser_putc(حرف c)
{
	bsp_ser_putc(c);
}

#endif
