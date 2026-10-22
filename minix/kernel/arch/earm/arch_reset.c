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

خالی
halt_cpu(خالی)
{
	asm متلون("dsb");
	asm متلون("cpsie i");
	asm متلون("wfi");
	asm متلون("cpsid i");
}

خالی
reset(خالی)
{
	bsp_reset(); /* should not exit */
	براہ_راست_print("Reset not supported.");
	جب (1);
}

خالی
poweroff(خالی)
{
	bsp_poweroff();
	/* fallback option: hang */
	براہ_راست_print("Unable to power-off this device.");
	جب (1);
}

__dead خالی
arch_shutdown(صحیح how)
{

	اگر((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	اگر(how & RB_HALT) {
		/* Hang */
		برائے (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	جب (1);
}

#اگر_تعریف_شدہ DEBUG_SERIAL
خالی
ser_putc(حرف c)
{
	bsp_ser_putc(c);
}

#اگر_ختم
