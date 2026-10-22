#அந்தர்பாவ "kernel/kernel.h"

#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <machine/cpu.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <machine/vm.h>
#அந்தர்பாவ <io.h>

#அந்தர்பாவ <minix/board.h>
#அந்தர்பாவ <sys/reboot.h>

#அந்தர்பாவ <minix/u64.h>

#அந்தர்பாவ "archconst.h"
#அந்தர்பாவ "arch_proto.h"
#அந்தர்பாவ "bsp_reset.h"
#அந்தர்பாவ "bsp_serial.h"
#அந்தர்பாவ "kernel/proc.h"
#அந்தர்பாவ "kernel/debug.h"
#அந்தர்பாவ "direct_utils.h"
#அந்தர்பாவ <machine/multiboot.h>

வெறுமை
halt_cpu(வெறுமை)
{
	asm நிலையற்ற("dsb");
	asm நிலையற்ற("cpsie i");
	asm நிலையற்ற("wfi");
	asm நிலையற்ற("cpsid i");
}

வெறுமை
reset(வெறுமை)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	போது (1);
}

வெறுமை
poweroff(வெறுமை)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	போது (1);
}

__dead வெறுமை
arch_shutdown(முழு how)
{

	எனில்((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	எனில்(how & RB_HALT) {
		/* Hang */
		க்கு (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	போது (1);
}

#ifdef DEBUG_SERIAL
வெறுமை
ser_putc(எழுத்து c)
{
	bsp_ser_putc(c);
}

#endif
