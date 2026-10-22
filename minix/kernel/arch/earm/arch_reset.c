#شامل "kernel/kernel.h"

#شامل <unistd.h>
#شامل <ctype.h>
#شامل <string.h>
#شامل <machine/cpu.h>
#شامل <assert.h>
#شامل <signal.h>
#شامل <machine/vm.h>
#شامل <io.h>

#شامل <minix/board.h>
#شامل <sys/reboot.h>

#شامل <minix/u64.h>

#شامل "archconst.h"
#شامل "arch_proto.h"
#شامل "bsp_reset.h"
#شامل "bsp_serial.h"
#شامل "kernel/proc.h"
#شامل "kernel/debug.h"
#شامل "direct_utils.h"
#شامل <machine/multiboot.h>

تهی
halt_cpu(تهی)
{
	asm فرار("dsb");
	asm فرار("cpsie i");
	asm فرار("wfi");
	asm فرار("cpsid i");
}

تهی
reset(تهی)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	وقتی (1);
}

تهی
poweroff(تهی)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	وقتی (1);
}

__dead تهی
arch_shutdown(صحیح how)
{

	اگر((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	اگر(how & RB_HALT) {
		/* Hang */
		برای (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	وقتی (1);
}

#ifdef DEBUG_SERIAL
تهی
ser_putc(نویسه c)
{
	bsp_ser_putc(c);
}

#endif
