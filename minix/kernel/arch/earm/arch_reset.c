#अन्तर्भाव "kernel/kernel.h"

#अन्तर्भाव <unistd.h>
#अन्तर्भाव <ctype.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <machine/vm.h>
#अन्तर्भाव <io.h>

#अन्तर्भाव <minix/board.h>
#अन्तर्भाव <sys/reboot.h>

#अन्तर्भाव <minix/u64.h>

#अन्तर्भाव "archconst.h"
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव "bsp_reset.h"
#अन्तर्भाव "bsp_serial.h"
#अन्तर्भाव "kernel/proc.h"
#अन्तर्भाव "kernel/debug.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव <machine/multiboot.h>

शून्य
halt_cpu(शून्य)
{
	यंत्र अस्थिर("dsb");
	यंत्र अस्थिर("cpsie i");
	यंत्र अस्थिर("wfi");
	यंत्र अस्थिर("cpsid i");
}

शून्य
reset(शून्य)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	पर्यंत (1);
}

शून्य
poweroff(शून्य)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	पर्यंत (1);
}

__dead शून्य
arch_shutdown(पूर्णांक how)
{

	जर((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	जर(how & RB_HALT) {
		/* Hang */
		साठी (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	पर्यंत (1);
}

#ifdef DEBUG_SERIAL
शून्य
ser_putc(अक्षर c)
{
	bsp_ser_putc(c);
}

#endif
