
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cmos.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/bios.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/reboot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "oxpcie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "direct_utils.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ACPI
#ਅਨ੍ਤਰ੍ਭਾਵ "acpi.h"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ     KBCMDP          4       /* kbd controller port (O) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ      IO_KBD          0x060           /* 8042 Keyboard */

ਪੂਰਨ cpu_has_tsc;

ਖਾਲੀ
reset(ਖਾਲੀ)
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
        ਜੇ (b != 0xff) {
                ਜੇ ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	ਜਦੋਂ(1) {
		;
	}
}

ਸਥਿਰ __dead ਖਾਲੀ
halt(ਖਾਲੀ)
{
	ਲਈ ( ; ; )
		halt_cpu();
}

ਸਥਿਰ __dead ਖਾਲੀ
poweroff(ਖਾਲੀ)
{
	ਅਚਲ ਅੱਖਰ *shutdown_str;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ACPI
	acpi_poweroff();
#ਜੇ_ਅੰਤ
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        ਜਦੋਂ (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead ਖਾਲੀ arch_shutdown(ਪੂਰਨ how)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਅਣਵਰਤਿਆ_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	ਜਦੋਂ(ਸਿੱਧਾ_read_char(&ਅਣਵਰਤਿਆ_ch))
		;

	ਜੇ(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		ਜੇ (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		ਸਿੱਧਾ_cls();
		ਸਿੱਧਾ_print("Minix panic. System diagnostics buffer:\n\n");
		ਸਿੱਧਾ_print(kmess.kmess_buf);
		ਸਿੱਧਾ_print("\nSystem has panicked, press any key to reboot");
		ਜਦੋਂ (!ਸਿੱਧਾ_read_char(&ਅਣਵਰਤਿਆ_ch))
			;
		reset();
	}
		
	ਜੇ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ਜੇ(how & RB_HALT) {
		/* Hang */
		ਲਈ (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system by forcing a processor shutdown. 
	 * First stop the BIOS memory test by setting a soft
	 * reset flag.
	 */
	reset();
	NOT_REACHABLE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIAL
ਖਾਲੀ ser_putc(ਅੱਖਰ c)
{
        ਪੂਰਨ i;
        ਪੂਰਨ lsr, thr;

#ਜੇ CONFIG_OXPCIE
        oxpcie_putc(c);
#ਨਹੀਂਤਾਂ
        lsr= COM1_LSR;
        thr= COM1_THR;
        ਲਈ (i= 0; i<100000; i++)
        {
                ਜੇ (inb( lsr) & LSR_THRE)
                        ਰੋਕੋ;
        }
        outb( thr, c);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅੰਤ
