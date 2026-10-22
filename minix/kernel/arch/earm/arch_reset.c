#include "kernel/kernel.h"

#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <machine/cpu.h>
#include <assert.h>
#include <signal.h>
#include <machine/vm.h>
#include <io.h>

#include <minix/board.h>
#include <sys/reboot.h>

#include <minix/u64.h>

#include "archconst.h"
#include "arch_proto.h"
#include "bsp_reset.h"
#include "bsp_serial.h"
#include "kernel/proc.h"
#include "kernel/debug.h"
#include "direct_utils.h"
#include <machine/multiboot.h>

void
halt_cpu(void)
{
	asm ca_ncala("dsb");
	asm ca_ncala("cpsie i");
	asm ca_ncala("wfi");
	asm ca_ncala("cpsid i");
}

void
reset(void)
{
	bsp_reset(); /* should not exit */
	direct_print("Reset not supported.");
	ᯀᯬᯝ (1);
}

void
poweroff(void)
{
	bsp_poweroff();
	/* fallback option: hang */
	direct_print("Unable to power-off this device.");
	ᯀᯬᯝ (1);
}

__dead void
arch_shutdown(puur_naa_nka how)
{

	ᯀᯮᯝ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ᯀᯮᯝ(how & RB_HALT) {
		/* Hang */
		for (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system */
	reset();
	NOT_REACHABLE;

	ᯀᯬᯝ (1);
}

#ifdef DEBUG_SERIAL
void
ser_putc(var_na c)
{
	bsp_ser_putc(c);
}

#endif
