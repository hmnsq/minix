#അന്തര്ഭാവ "kernel/kernel.h"

#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <machine/cpu.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <machine/vm.h>
#അന്തര്ഭാവ <io.h>

#അന്തര്ഭാവ <minix/board.h>
#അന്തര്ഭാവ <sys/reboot.h>

#അന്തര്ഭാവ <minix/u64.h>

#അന്തര്ഭാവ "archconst.h"
#അന്തര്ഭാവ "arch_proto.h"
#അന്തര്ഭാവ "bsp_reset.h"
#അന്തര്ഭാവ "bsp_serial.h"
#അന്തര്ഭാവ "kernel/proc.h"
#അന്തര്ഭാവ "kernel/debug.h"
#അന്തര്ഭാവ "direct_utils.h"
#അന്തര്ഭാവ <machine/multiboot.h>

ശൂന്യം
halt_cpu(ശൂന്യം)
{
	asm അസ്ഥിര("dsb");
	asm അസ്ഥിര("cpsie i");
	asm അസ്ഥിര("wfi");
	asm അസ്ഥിര("cpsid i");
}

ശൂന്യം
reset(ശൂന്യം)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	ആകുമ്പോൾ (1);
}

ശൂന്യം
poweroff(ശൂന്യം)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	ആകുമ്പോൾ (1);
}

__dead ശൂന്യം
arch_shutdown(പൂർണ്ണസംഖ്യ how)
{

	എങ്കിൽ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	എങ്കിൽ(how & RB_HALT) {
		/* Hang */
		വേണ്ടി (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	ആകുമ്പോൾ (1);
}

#ifdef DEBUG_SERIAL
ശൂന്യം
ser_putc(അക്ഷരം c)
{
	bsp_ser_putc(c);
}

#endif
