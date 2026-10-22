
#include "kernel/kernel.h"

#include <ctype.h>
#include <string.h>
#include <machine/cmos.h>
#include <machine/bios.h>
#include <machine/cpu.h>
#include <minix/cpufeature.h>
#include <sys/reboot.h>
#include <assert.h>
#include <signal.h>

#include <minix/u64.h>

#include "arch_proto.h"
#include "oxpcie.h"
#include "direct_utils.h"

#ifdef USE_ACPI
#include "acpi.h"
#endif

#define     KBCMDP          4       /* kbd controller port (O) */
#define      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#define      IO_KBD          0x060           /* 8042 Keyboard */

ཧྲིལ cpu_has_tsc;

void
reset(void)
{
        uint8_t b;
        /*
         * The keyboard controller has 4 random output pins, one of which is
         * connected to the RESET pin on the CPU in many PCs.  We tell the
         * keyboard controller to pulse this line a couple of times.
         */
        outb(IO_KBD + KBCMDP, KBC_PULSE0);
        busy_delay_ms(100);
        outb(IO_KBD + KBCMDP, KBC_PULSE0);
        busy_delay_ms(100);

        /*
         * Attempt to force a reset via the Reset Control register at
         * I/O port 0xcf9.  Bit 2 forces a system reset when it
         * transitions from 0 to 1.  Bit 1 selects the type of reset
         * to attempt: 0 selects a "soft" reset, and 1 selects a
         * "hard" reset.  We try a "hard" reset.  The first write sets
         * bit 1 to select a "hard" reset and clears bit 2.  The
         * second write forces a 0 -> 1 transition in bit 2 to trigger
         * a reset.
         */
        outb(0xcf9, 0x2);
        outb(0xcf9, 0x6);
        busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */

        /*
         * Attempt to force a reset via the Fast A20 and Init register
         * at I/O port 0x92.  Bit 1 serves as an alternate A20 gate.
         * Bit 0 asserts INIT# when set to 1.  We are careful to only
         * preserve bit 1 while setting bit 0.  We also must clear bit
         * 0 before setting it if it isn't already clear.
         */
        b = inb(0x92);
        གལ་ཏེ (b != 0xff) {
                གལ་ཏེ ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	སྐབས(1) {
		;
	}
}

static __dead void
halt(void)
{
	ཆེད་དུ ( ; ; )
		halt_cpu();
}

static __dead void
poweroff(void)
{
	const var_na *shutdown_str;

#ifdef USE_ACPI
	acpi_poweroff();
#endif
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        སྐབས (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead void arch_shutdown(ཧྲིལ how)
{
	unsigned var_na unused_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	སྐབས(direct_read_char(&unused_ch))
		;

	གལ་ཏེ(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		གལ་ཏེ (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		direct_cls();
		direct_print("Minix panic. System diagnostics buffer:\n\n");
		direct_print(kmess.kmess_buf);
		direct_print("\nSystem has panicked, press any key to reboot");
		སྐབས (!direct_read_char(&unused_ch))
			;
		reset();
	}
		
	གལ་ཏེ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	གལ་ཏེ(how & RB_HALT) {
		/* Hang */
		ཆེད་དུ (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system by forcing a processor shutdown. 
	 * First stop the BIOS memory test by setting a soft
	 * reset flag.
	 */
	reset();
	NOT_REACHABLE;
}

#ifdef DEBUG_SERIAL
void ser_putc(var_na c)
{
        ཧྲིལ i;
        ཧྲིལ lsr, thr;

#གལ་ཏེ CONFIG_OXPCIE
        oxpcie_putc(c);
#དེ་མིན
        lsr= COM1_LSR;
        thr= COM1_THR;
        ཆེད་དུ (i= 0; i<100000; i++)
        {
                གལ་ཏེ (inb( lsr) & LSR_THRE)
                        bha_nga;
        }
        outb( thr, c);
#endif
}

#endif
