#অন্তর্ভাব "kernel/kernel.h"

#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <machine/cpu.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <machine/vm.h>
#অন্তর্ভাব <io.h>

#অন্তর্ভাব <minix/board.h>
#অন্তর্ভাব <sys/reboot.h>

#অন্তর্ভাব <minix/u64.h>

#অন্তর্ভাব "archconst.h"
#অন্তর্ভাব "arch_proto.h"
#অন্তর্ভাব "bsp_reset.h"
#অন্তর্ভাব "bsp_serial.h"
#অন্তর্ভাব "kernel/proc.h"
#অন্তর্ভাব "kernel/debug.h"
#অন্তর্ভাব "direct_utils.h"
#অন্তর্ভাব <machine/multiboot.h>

শূন্য
halt_cpu(শূন্য)
{
	asm চঞ্চল("dsb");
	asm চঞ্চল("cpsie i");
	asm চঞ্চল("wfi");
	asm চঞ্চল("cpsid i");
}

শূন্য
reset(শূন্য)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	যাবত্ (1);
}

শূন্য
poweroff(শূন্য)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	যাবত্ (1);
}

__dead শূন্য
arch_shutdown(পূর্ণাঙ্ক how)
{

	যদি((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	যদি(how & RB_HALT) {
		/* Hang */
		প্রতি (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	যাবত্ (1);
}

#ifdef DEBUG_SERIAL
শূন্য
ser_putc(বর্ণ c)
{
	bsp_ser_putc(c);
}

#endif
