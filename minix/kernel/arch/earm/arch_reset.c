#অন্তর্ভাৱ "kernel/kernel.h"

#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <ctype.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <machine/cpu.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <machine/vm.h>
#অন্তর্ভাৱ <io.h>

#অন্তর্ভাৱ <minix/board.h>
#অন্তর্ভাৱ <sys/reboot.h>

#অন্তর্ভাৱ <minix/u64.h>

#অন্তর্ভাৱ "archconst.h"
#অন্তর্ভাৱ "arch_proto.h"
#অন্তর্ভাৱ "bsp_reset.h"
#অন্তর্ভাৱ "bsp_serial.h"
#অন্তর্ভাৱ "kernel/proc.h"
#অন্তর্ভাৱ "kernel/debug.h"
#অন্তর্ভাৱ "direct_utils.h"
#অন্তর্ভাৱ <machine/multiboot.h>

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
	যেতিয়ালৈ (1);
}

শূন্য
poweroff(শূন্য)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	যেতিয়ালৈ (1);
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
		বাবে (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	যেতিয়ালৈ (1);
}

#ifdef DEBUG_SERIAL
শূন্য
ser_putc(ৱর্ণ c)
{
	bsp_ser_putc(c);
}

#endif
