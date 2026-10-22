
#అన్తర్భావ "kernel/kernel.h"

#అన్తర్భావ <ctype.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <machine/cmos.h>
#అన్తర్భావ <machine/bios.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <minix/cpufeature.h>
#అన్తర్భావ <sys/reboot.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <signal.h>

#అన్తర్భావ <minix/u64.h>

#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "oxpcie.h"
#అన్తర్భావ "direct_utils.h"

#నిర్వచితమైతే USE_ACPI
#అన్తర్భావ "acpi.h"
#అయితే_ముగింపు

#నిర్వచించు     KBCMDP          4       /* kbd controller port (O) */
#నిర్వచించు      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#నిర్వచించు      IO_KBD          0x060           /* 8042 Keyboard */

పూర్ణాంకం cpu_has_tsc;

శూన్య
reset(శూన్య)
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
        ఒకవేళ (b != 0xff) {
                ఒకవేళ ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	అయితే(1) {
		;
	}
}

స్థిర __dead శూన్య
halt(శూన్య)
{
	కొరకు ( ; ; )
		halt_cpu();
}

స్థిర __dead శూన్య
poweroff(శూన్య)
{
	స్థిరరాశి అక్షరం *shutdown_str;

#నిర్వచితమైతే USE_ACPI
	acpi_poweroff();
#అయితే_ముగింపు
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        అయితే (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead శూన్య arch_shutdown(పూర్ణాంకం how)
{
	గుర్తులేని అక్షరం వాడని_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	అయితే(ప్రత్యక్ష_read_char(&వాడని_ch))
		;

	ఒకవేళ(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		ఒకవేళ (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		ప్రత్యక్ష_cls();
		ప్రత్యక్ష_print("Minix panic. System diagnostics buffer:\n\n");
		ప్రత్యక్ష_print(kmess.kmess_buf);
		ప్రత్యక్ష_print("\nSystem has panicked, press any key to reboot");
		అయితే (!ప్రత్యక్ష_read_char(&వాడని_ch))
			;
		reset();
	}
		
	ఒకవేళ((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ఒకవేళ(how & RB_HALT) {
		/* Hang */
		కొరకు (; ; ) halt_cpu();
		NOT_REACHABLE;
	}

	/* Reset the system by forcing a processor shutdown. 
	 * First stop the BIOS memory test by setting a soft
	 * reset flag.
	 */
	reset();
	NOT_REACHABLE;
}

#నిర్వచితమైతే DEBUG_SERIAL
శూన్య ser_putc(అక్షరం c)
{
        పూర్ణాంకం i;
        పూర్ణాంకం lsr, thr;

#ఒకవేళ CONFIG_OXPCIE
        oxpcie_putc(c);
#లేకపోతే
        lsr= COM1_LSR;
        thr= COM1_THR;
        కొరకు (i= 0; i<100000; i++)
        {
                ఒకవేళ (inb( lsr) & LSR_THRE)
                        ఆపు;
        }
        outb( thr, c);
#అయితే_ముగింపు
}

#అయితే_ముగింపు
