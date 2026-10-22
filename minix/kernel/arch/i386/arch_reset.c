
#অন্তর্ভাব "kernel/kernel.h"

#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <machine/cmos.h>
#অন্তর্ভাব <machine/bios.h>
#অন্তর্ভাব <machine/cpu.h>
#অন্তর্ভাব <minix/cpufeature.h>
#অন্তর্ভাব <sys/reboot.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <signal.h>

#অন্তর্ভাব <minix/u64.h>

#অন্তর্ভাব "arch_proto.h"
#অন্তর্ভাব "oxpcie.h"
#অন্তর্ভাব "direct_utils.h"

#ifdef USE_ACPI
#অন্তর্ভাব "acpi.h"
#endif

#define     KBCMDP          4       /* kbd controller port (O) */
#define      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#define      IO_KBD          0x060           /* 8042 Keyboard */

পূর্ণসংখ্যা cpu_has_tsc;

শূন্য
reset(শূন্য)
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
        যদি (b != 0xff) {
                যদি ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	যখন(1) {
		;
	}
}

স্থির __dead শূন্য
halt(শূন্য)
{
	জন্য ( ; ; )
		halt_cpu();
}

স্থির __dead শূন্য
poweroff(শূন্য)
{
	ধ্রুবক অক্ষর *shutdown_str;

#ifdef USE_ACPI
	acpi_poweroff();
#endif
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        যখন (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead শূন্য arch_shutdown(পূর্ণসংখ্যা how)
{
	unsigned অক্ষর unused_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	যখন(direct_read_char(&unused_ch))
		;

	যদি(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		যদি (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		direct_cls();
		direct_print("Minix panic. System diagnostics buffer:\n\n");
		direct_print(kmess.kmess_buf);
		direct_print("\nSystem has panicked, press any key to reboot");
		যখন (!direct_read_char(&unused_ch))
			;
		reset();
	}
		
	যদি((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	যদি(how & RB_HALT) {
		/* Hang */
		জন্য (; ; ) halt_cpu();
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
শূন্য ser_putc(অক্ষর c)
{
        পূর্ণসংখ্যা i;
        পূর্ণসংখ্যা lsr, thr;

#যদি CONFIG_OXPCIE
        oxpcie_putc(c);
#অন্যথা
        lsr= COM1_LSR;
        thr= COM1_THR;
        জন্য (i= 0; i<100000; i++)
        {
                যদি (inb( lsr) & LSR_THRE)
                        বিরতি;
        }
        outb( thr, c);
#endif
}

#endif
