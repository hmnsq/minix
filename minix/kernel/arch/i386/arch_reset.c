
#समावेश "kernel/kernel.h"

#समावेश <ctype.h>
#समावेश <string.h>
#समावेश <machine/cmos.h>
#समावेश <machine/bios.h>
#समावेश <machine/cpu.h>
#समावेश <minix/cpufeature.h>
#समावेश <sys/reboot.h>
#समावेश <assert.h>
#समावेश <signal.h>

#समावेश <minix/u64.h>

#समावेश "arch_proto.h"
#समावेश "oxpcie.h"
#समावेश "direct_utils.h"

#अगर_घोषित USE_ACPI
#समावेश "acpi.h"
#पूर्ण_अगर

#घोषणा     KBCMDP          4       /* kbd controller port (O) */
#घोषणा      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#घोषणा      IO_KBD          0x060           /* 8042 Keyboard */

पूर्णांक cpu_has_tsc;

व्योम
reset(व्योम)
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
        अगर (b != 0xff) {
                अगर ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	जबतक(1) {
		;
	}
}

अटल __dead व्योम
halt(व्योम)
{
	क्रम ( ; ; )
		halt_cpu();
}

अटल __dead व्योम
poweroff(व्योम)
{
	स्थिर अक्षर *shutdown_str;

#अगर_घोषित USE_ACPI
	acpi_poweroff();
#पूर्ण_अगर
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        जबतक (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead व्योम arch_shutdown(पूर्णांक how)
{
	अचिन्हित अक्षर unused_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	जबतक(direct_read_char(&unused_ch))
		;

	अगर(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		अगर (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		direct_cls();
		direct_print("Minix panic. System diagnostics buffer:\n\n");
		direct_print(kmess.kmess_buf);
		direct_print("\nSystem has panicked, press any key to reboot");
		जबतक (!direct_read_char(&unused_ch))
			;
		reset();
	}
		
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

	/* Reset the system by forcing a processor shutdown. 
	 * First stop the BIOS memory test by setting a soft
	 * reset flag.
	 */
	reset();
	NOT_REACHABLE;
}

#अगर_घोषित DEBUG_SERIAL
व्योम ser_putc(अक्षर c)
{
        पूर्णांक i;
        पूर्णांक lsr, thr;

#अगर CONFIG_OXPCIE
        oxpcie_putc(c);
#अन्यथा
        lsr= COM1_LSR;
        thr= COM1_THR;
        क्रम (i= 0; i<100000; i++)
        {
                अगर (inb( lsr) & LSR_THRE)
                        अवरोध;
        }
        outb( thr, c);
#पूर्ण_अगर
}

#पूर्ण_अगर
