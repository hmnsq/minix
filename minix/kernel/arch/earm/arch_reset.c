#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <machine/vm.h>
#ಅನ್ತರ್ಭಾವ <io.h>

#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <sys/reboot.h>

#ಅನ್ತರ್ಭಾವ <minix/u64.h>

#ಅನ್ತರ್ಭಾವ "archconst.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ "bsp_reset.h"
#ಅನ್ತರ್ಭಾವ "bsp_serial.h"
#ಅನ್ತರ್ಭಾವ "kernel/proc.h"
#ಅನ್ತರ್ಭಾವ "kernel/debug.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"
#ಅನ್ತರ್ಭಾವ <machine/multiboot.h>

ಶೂನ್ಯ
halt_cpu(ಶೂನ್ಯ)
{
	asm ಅಸ್ಥಿರ("dsb");
	asm ಅಸ್ಥಿರ("cpsie i");
	asm ಅಸ್ಥಿರ("wfi");
	asm ಅಸ್ಥಿರ("cpsid i");
}

ಶೂನ್ಯ
reset(ಶೂನ್ಯ)
{
	bsp_reset(); /* should not exit */
	ನೇರ_print("Reset not supported.");
	ಆಗ (1);
}

ಶೂನ್ಯ
poweroff(ಶೂನ್ಯ)
{
	bsp_poweroff();
	/* fallback option: hang */
	ನೇರ_print("Unable to power-off this device.");
	ಆಗ (1);
}

__dead ಶೂನ್ಯ
arch_shutdown(ಪೂರ್ಣಾಂಕ how)
{

	ಆದರೆ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ಆದರೆ(how & RB_HALT) {
		/* Hang */
		ಗಾಗಿ (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	ಆಗ (1);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIAL
ಶೂನ್ಯ
ser_putc(ಅಕ್ಷರ c)
{
	bsp_ser_putc(c);
}

#ಆದರೆ_ಅಂತ್ಯ
