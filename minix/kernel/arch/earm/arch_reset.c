#समावेश "kernel/kernel.h"

#समावेश <unistd.h>
#समावेश <ctype.h>
#समावेश <string.h>
#समावेश <machine/cpu.h>
#समावेश <assert.h>
#समावेश <signal.h>
#समावेश <machine/vm.h>
#समावेश <io.h>

#समावेश <minix/board.h>
#समावेश <sys/reboot.h>

#समावेश <minix/u64.h>

#समावेश "archconst.h"
#समावेश "arch_proto.h"
#समावेश "bsp_reset.h"
#समावेश "bsp_serial.h"
#समावेश "kernel/proc.h"
#समावेश "kernel/debug.h"
#समावेश "direct_utils.h"
#समावेश <machine/multiboot.h>

व्योम
halt_cpu(व्योम)
{
	यंत्र अस्थिर("dsb");
	यंत्र अस्थिर("cpsie i");
	यंत्र अस्थिर("wfi");
	यंत्र अस्थिर("cpsid i");
}

व्योम
reset(व्योम)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	जबतक (1);
}

व्योम
poweroff(व्योम)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	जबतक (1);
}

__dead व्योम
arch_shutdown(पूर्णांक how)
{

	अगर((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	अगर(how & RB_HALT) {
		/* Hang */
		क्रम (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	जबतक (1);
}

#अगर_घोषित DEBUG_SERIAL
व्योम
ser_putc(अक्षर c)
{
	bsp_ser_putc(c);
}

#पूर्ण_अगर
