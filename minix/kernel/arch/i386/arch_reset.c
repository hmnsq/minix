
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

#ifdef USE_ACPI
#ಅನ್ತರ್ಭಾವ "acpi.h"
#endif

#define     KBCMDP          4       /* kbd controller port (O) */
#define      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#define      IO_KBD          0x060           /* 8042 Keyboard */

ಪೂರ್ಣಾಙ್ಕ cpu_has_tsc;

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
        ಯದಿ (b != 0xff) {
                ಯದಿ ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	ಯಾವತ್(1) {
		;
	}
}

ಸ್ಥಿರ __dead ಶೂನ್ಯ
halt(ಶೂನ್ಯ)
{
	ಪ್ರತಿ ( ; ; )
		halt_cpu();
}

ಸ್ಥಿರ __dead ಶೂನ್ಯ
poweroff(ಶೂನ್ಯ)
{
	ಧ್ರುವ ವರ್ಣ *shutdown_str;

#ifdef USE_ACPI
	acpi_poweroff();
#endif
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        ಯಾವತ್ (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead ಶೂನ್ಯ arch_shutdown(ಪೂರ್ಣಾಙ್ಕ how)
{
	unsigned ವರ್ಣ unused_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	ಯಾವತ್(direct_read_char(&unused_ch))
		;

	ಯದಿ(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		ಯದಿ (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		direct_cls();
		direct_print("Minix panic. System diagnostics buffer:\n\n");
		direct_print(kmess.kmess_buf);
		direct_print("\nSystem has panicked, press any key to reboot");
		ಯಾವತ್ (!direct_read_char(&unused_ch))
			;
		reset();
	}
		
	ಯದಿ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ಯದಿ(how & RB_HALT) {
		/* Hang */
		ಪ್ರತಿ (; ; ) halt_cpu();
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
ಶೂನ್ಯ ser_putc(ವರ್ಣ c)
{
        ಪೂರ್ಣಾಙ್ಕ i;
        ಪೂರ್ಣಾಙ್ಕ lsr, thr;

#ಯದಿ CONFIG_OXPCIE
        oxpcie_putc(c);
#ಅನ್ಯಥಾ
        lsr= COM1_LSR;
        thr= COM1_THR;
        ಪ್ರತಿ (i= 0; i<100000; i++)
        {
                ಯದಿ (inb( lsr) & LSR_THRE)
                        ಭಙ್ಗ;
        }
        outb( thr, c);
#endif
}

#endif
