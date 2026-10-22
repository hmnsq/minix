#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/reboot.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "archconst.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_reset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_serial.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/debug.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "direct_utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/multiboot.h>

ਖਾਲੀ
halt_cpu(ਖਾਲੀ)
{
	asm ਅਸਥਿਰ("dsb");
	asm ਅਸਥਿਰ("cpsie i");
	asm ਅਸਥਿਰ("wfi");
	asm ਅਸਥਿਰ("cpsid i");
}

ਖਾਲੀ
reset(ਖਾਲੀ)
{
	bsp_reset(); /* should not exit */
	ਸਿੱਧਾ_print("Reset not supported.");
	ਜਦੋਂ (1);
}

ਖਾਲੀ
poweroff(ਖਾਲੀ)
{
	bsp_poweroff();
	/* fallback option: hang */
	ਸਿੱਧਾ_print("Unable to power-off this device.");
	ਜਦੋਂ (1);
}

__dead ਖਾਲੀ
arch_shutdown(ਪੂਰਨ how)
{

	ਜੇ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ਜੇ(how & RB_HALT) {
		/* Hang */
		ਲਈ (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	ਜਦੋਂ (1);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIAL
ਖਾਲੀ
ser_putc(ਅੱਖਰ c)
{
	bsp_ser_putc(c);
}

#ਜੇ_ਅੰਤ
