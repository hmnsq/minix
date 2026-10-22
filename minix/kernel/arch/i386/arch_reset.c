
#अन्तर्भाव "kernel/kernel.h"

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <machine/cmos.h>
#अन्तर्भाव <machine/bios.h>
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <sys/reboot.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <signal.h>

#अन्तर्भाव <minix/u64.h>

#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव "oxpcie.h"
#अन्तर्भाव "direct_utils.h"

#यदि_परिभाषितम् USE_ACPI
#अन्तर्भाव "acpi.h"
#यदि_अन्तः

#परिभाषय     KBCMDP          4       /* kbd controller port (O) */
#परिभाषय      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#परिभाषय      IO_KBD          0x060           /* 8042 Keyboard */

पूर्णाङ्क cpu_has_tsc;

शून्य
reset(शून्य)
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
        यदि (b != 0xff) {
                यदि ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	यावत्(1) {
		;
	}
}

स्थिर __dead शून्य
halt(शून्य)
{
	प्रति ( ; ; )
		halt_cpu();
}

स्थिर __dead शून्य
poweroff(शून्य)
{
	ध्रुव वर्ण *shutdown_str;

#यदि_परिभाषितम् USE_ACPI
	acpi_poweroff();
#यदि_अन्तः
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        यावत् (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead शून्य arch_shutdown(पूर्णाङ्क how)
{
	अचिह्नित वर्ण अप्रयुक्तम्_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	यावत्(प्रत्यक्षम्_read_char(&अप्रयुक्तम्_ch))
		;

	यदि(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		यदि (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		प्रत्यक्षम्_cls();
		प्रत्यक्षम्_print("Minix panic. System diagnostics buffer:\n\n");
		प्रत्यक्षम्_print(kmess.kmess_buf);
		प्रत्यक्षम्_print("\nSystem has panicked, press any key to reboot");
		यावत् (!प्रत्यक्षम्_read_char(&अप्रयुक्तम्_ch))
			;
		reset();
	}
		
	यदि((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	यदि(how & RB_HALT) {
		/* Hang */
		प्रति (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system by forcing a processor shutdown. 
	 * First stop the BIOS memory test by setting a soft
	 * reset flag.
	 */
	reset();
	NOT_REACHABLE;
}

#यदि_परिभाषितम् DEBUG_SERIAL
शून्य ser_putc(वर्ण c)
{
        पूर्णाङ्क i;
        पूर्णाङ्क lsr, thr;

#यदि CONFIG_OXPCIE
        oxpcie_putc(c);
#अन्यथा
        lsr= COM1_LSR;
        thr= COM1_THR;
        प्रति (i= 0; i<100000; i++)
        {
                यदि (inb( lsr) & LSR_THRE)
                        भङ्ग;
        }
        outb( thr, c);
#यदि_अन्तः
}

#यदि_अन्तः
