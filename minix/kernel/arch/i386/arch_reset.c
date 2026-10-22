
#شامل "kernel/kernel.h"

#شامل <ctype.h>
#شامل <string.h>
#شامل <machine/cmos.h>
#شامل <machine/bios.h>
#شامل <machine/cpu.h>
#شامل <minix/cpufeature.h>
#شامل <sys/reboot.h>
#شامل <assert.h>
#شامل <signal.h>

#شامل <minix/u64.h>

#شامل "arch_proto.h"
#شامل "oxpcie.h"
#شامل "direct_utils.h"

#اگر_تعریف_شدہ USE_ACPI
#شامل "acpi.h"
#اگر_ختم

#تعریف_کرو     KBCMDP          4       /* kbd controller port (O) */
#تعریف_کرو      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#تعریف_کرو      IO_KBD          0x060           /* 8042 Keyboard */

صحیح cpu_has_tsc;

خالی
reset(خالی)
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
        اگر (b != 0xff) {
                اگر ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	جب(1) {
		;
	}
}

ساکن __dead خالی
halt(خالی)
{
	برائے ( ; ; )
		halt_cpu();
}

ساکن __dead خالی
poweroff(خالی)
{
	ثابت حرف *shutdown_str;

#اگر_تعریف_شدہ USE_ACPI
	acpi_poweroff();
#اگر_ختم
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        جب (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead خالی arch_shutdown(صحیح how)
{
	غیرعلامتی حرف غیر_مستعمل_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	جب(براہ_راست_read_char(&غیر_مستعمل_ch))
		;

	اگر(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		اگر (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		براہ_راست_cls();
		براہ_راست_print("Minix panic. System diagnostics buffer:\n\n");
		براہ_راست_print(kmess.kmess_buf);
		براہ_راست_print("\nSystem has panicked, press any key to reboot");
		جب (!براہ_راست_read_char(&غیر_مستعمل_ch))
			;
		reset();
	}
		
	اگر((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	اگر(how & RB_HALT) {
		/* Hang */
		برائے (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system by forcing a processor shutdown. 
	 * First stop the BIOS memory test by setting a soft
	 * reset flag.
	 */
	reset();
	NOT_REACHABLE;
}

#اگر_تعریف_شدہ DEBUG_SERIAL
خالی ser_putc(حرف c)
{
        صحیح i;
        صحیح lsr, thr;

#اگر CONFIG_OXPCIE
        oxpcie_putc(c);
#ورنہ
        lsr= COM1_LSR;
        thr= COM1_THR;
        برائے (i= 0; i<100000; i++)
        {
                اگر (inb( lsr) & LSR_THRE)
                        توڑو;
        }
        outb( thr, c);
#اگر_ختم
}

#اگر_ختم
