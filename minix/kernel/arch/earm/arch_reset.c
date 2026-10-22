#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <machine/cpu.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <machine/vm.h>
#ଅନ୍ତର୍ଭାଵ <io.h>

#ଅନ୍ତର୍ଭାଵ <minix/board.h>
#ଅନ୍ତର୍ଭାଵ <sys/reboot.h>

#ଅନ୍ତର୍ଭାଵ <minix/u64.h>

#ଅନ୍ତର୍ଭାଵ "archconst.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"
#ଅନ୍ତର୍ଭାଵ "bsp_reset.h"
#ଅନ୍ତର୍ଭାଵ "bsp_serial.h"
#ଅନ୍ତର୍ଭାଵ "kernel/proc.h"
#ଅନ୍ତର୍ଭାଵ "kernel/debug.h"
#ଅନ୍ତର୍ଭାଵ "direct_utils.h"
#ଅନ୍ତର୍ଭାଵ <machine/multiboot.h>

ଶୂନ୍ୟ
halt_cpu(ଶୂନ୍ୟ)
{
	asm ଅସ୍ଥିର("dsb");
	asm ଅସ୍ଥିର("cpsie i");
	asm ଅସ୍ଥିର("wfi");
	asm ଅସ୍ଥିର("cpsid i");
}

ଶୂନ୍ୟ
reset(ଶୂନ୍ୟ)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	ଯେତେବେଳେ (1);
}

ଶୂନ୍ୟ
poweroff(ଶୂନ୍ୟ)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	ଯେତେବେଳେ (1);
}

__dead ଶୂନ୍ୟ
arch_shutdown(ପୂର୍ଣ୍ଣସଂଖ୍ୟା how)
{

	ଯଦି((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ଯଦି(how & RB_HALT) {
		/* Hang */
		ପାଇଁ (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	ଯେତେବେଳେ (1);
}

#ifdef DEBUG_SERIAL
ଶୂନ୍ୟ
ser_putc(ଅକ୍ଷର c)
{
	bsp_ser_putc(c);
}

#endif
