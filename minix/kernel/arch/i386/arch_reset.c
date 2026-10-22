
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <machine/cmos.h>
#ಅನ್ತರ್ಭಾವ <machine/bios.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>
#ಅನ್ತರ್ಭಾವ <sys/reboot.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <signal.h>

#ಅನ್ತರ್ಭಾವ <minix/u64.h>

#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ "oxpcie.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ACPI
#ಅನ್ತರ್ಭಾವ "acpi.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು     KBCMDP          4       /* kbd controller port (O) */
#ವ್ಯಾಖ್ಯಾನಿಸು      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#ವ್ಯಾಖ್ಯಾನಿಸು      IO_KBD          0x060           /* 8042 Keyboard */

ಪೂರ್ಣಾಂಕ cpu_has_tsc;

ಶೂನ್ಯ
reset(ಶೂನ್ಯ)
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
        ಆದರೆ (b != 0xff) {
                ಆದರೆ ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	ಆಗ(1) {
		;
	}
}

ಸ್ಥಿರ __dead ಶೂನ್ಯ
halt(ಶೂನ್ಯ)
{
	ಗಾಗಿ ( ; ; )
		halt_cpu();
}

ಸ್ಥಿರ __dead ಶೂನ್ಯ
poweroff(ಶೂನ್ಯ)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shutdown_str;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ACPI
	acpi_poweroff();
#ಆದರೆ_ಅಂತ್ಯ
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        ಆಗ (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead ಶೂನ್ಯ arch_shutdown(ಪೂರ್ಣಾಂಕ how)
{
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಬಳಸದ_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	ಆಗ(ನೇರ_read_char(&ಬಳಸದ_ch))
		;

	ಆದರೆ(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		ಆದರೆ (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		ನೇರ_cls();
		ನೇರ_print("Minix panic. System diagnostics buffer:\n\n");
		ನೇರ_print(kmess.kmess_buf);
		ನೇರ_print("\nSystem has panicked, press any key to reboot");
		ಆಗ (!ನೇರ_read_char(&ಬಳಸದ_ch))
			;
		reset();
	}
		
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

	/* Reset the system by forcing a processor shutdown. 
	 * First stop the BIOS memory test by setting a soft
	 * reset flag.
	 */
	reset();
	NOT_REACHABLE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIAL
ಶೂನ್ಯ ser_putc(ಅಕ್ಷರ c)
{
        ಪೂರ್ಣಾಂಕ i;
        ಪೂರ್ಣಾಂಕ lsr, thr;

#ಆದರೆ CONFIG_OXPCIE
        oxpcie_putc(c);
#ಇಲ್ಲದಿದ್ದರೆ
        lsr= COM1_LSR;
        thr= COM1_THR;
        ಗಾಗಿ (i= 0; i<100000; i++)
        {
                ಆದರೆ (inb( lsr) & LSR_THRE)
                        ನಿಲ್ಲಿಸು;
        }
        outb( thr, c);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ_ಅಂತ್ಯ
