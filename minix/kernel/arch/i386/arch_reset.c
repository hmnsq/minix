
#અન્તર્ભાવ "kernel/kernel.h"

#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <machine/cmos.h>
#અન્તર્ભાવ <machine/bios.h>
#અન્તર્ભાવ <machine/cpu.h>
#અન્તર્ભાવ <minix/cpufeature.h>
#અન્તર્ભાવ <sys/reboot.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <signal.h>

#અન્તર્ભાવ <minix/u64.h>

#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ "oxpcie.h"
#અન્તર્ભાવ "direct_utils.h"

#ifdef USE_ACPI
#અન્તર્ભાવ "acpi.h"
#endif

#define     KBCMDP          4       /* kbd controller port (O) */
#define      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#define      IO_KBD          0x060           /* 8042 Keyboard */

પૂર્ણાંક cpu_has_tsc;

શૂન્ય
reset(શૂન્ય)
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
        જો (b != 0xff) {
                જો ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	જ્યારે(1) {
		;
	}
}

સ્થિર __dead શૂન્ય
halt(શૂન્ય)
{
	માટે ( ; ; )
		halt_cpu();
}

સ્થિર __dead શૂન્ય
poweroff(શૂન્ય)
{
	અચલ અક્ષર *shutdown_str;

#ifdef USE_ACPI
	acpi_poweroff();
#endif
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        જ્યારે (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead શૂન્ય arch_shutdown(પૂર્ણાંક how)
{
	unsigned અક્ષર unused_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	જ્યારે(direct_read_char(&unused_ch))
		;

	જો(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		જો (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		direct_cls();
		direct_print("Minix panic. System diagnostics buffer:\n\n");
		direct_print(kmess.kmess_buf);
		direct_print("\nSystem has panicked, press any key to reboot");
		જ્યારે (!direct_read_char(&unused_ch))
			;
		reset();
	}
		
	જો((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	જો(how & RB_HALT) {
		/* Hang */
		માટે (; ; ) halt_cpu();
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
શૂન્ય ser_putc(અક્ષર c)
{
        પૂર્ણાંક i;
        પૂર્ણાંક lsr, thr;

#જો CONFIG_OXPCIE
        oxpcie_putc(c);
#નહીંતર
        lsr= COM1_LSR;
        thr= COM1_THR;
        માટે (i= 0; i<100000; i++)
        {
                જો (inb( lsr) & LSR_THRE)
                        તોડો;
        }
        outb( thr, c);
#endif
}

#endif
