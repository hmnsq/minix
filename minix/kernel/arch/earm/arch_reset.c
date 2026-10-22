#અન્તર્ભાવ "kernel/kernel.h"

#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <machine/cpu.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <machine/vm.h>
#અન્તર્ભાવ <io.h>

#અન્તર્ભાવ <minix/board.h>
#અન્તર્ભાવ <sys/reboot.h>

#અન્તર્ભાવ <minix/u64.h>

#અન્તર્ભાવ "archconst.h"
#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ "bsp_reset.h"
#અન્તર્ભાવ "bsp_serial.h"
#અન્તર્ભાવ "kernel/proc.h"
#અન્તર્ભાવ "kernel/debug.h"
#અન્તર્ભાવ "direct_utils.h"
#અન્તર્ભાવ <machine/multiboot.h>

શૂન્ય
halt_cpu(શૂન્ય)
{
	asm અસ્થિર("dsb");
	asm અસ્થિર("cpsie i");
	asm અસ્થિર("wfi");
	asm અસ્થિર("cpsid i");
}

શૂન્ય
reset(શૂન્ય)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	જ્યારે (1);
}

શૂન્ય
poweroff(શૂન્ય)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	જ્યારે (1);
}

__dead શૂન્ય
arch_shutdown(પૂર્ણાંક how)
{

	જો((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	જો(how & RB_HALT) {
		/* Hang */
		માટે (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	જ્યારે (1);
}

#ifdef DEBUG_SERIAL
શૂન્ય
ser_putc(અક્ષર c)
{
	bsp_ser_putc(c);
}

#endif
