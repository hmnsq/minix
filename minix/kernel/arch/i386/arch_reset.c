
#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <machine/cmos.h>
#ଅନ୍ତର୍ଭାଵ <machine/bios.h>
#ଅନ୍ତର୍ଭାଵ <machine/cpu.h>
#ଅନ୍ତର୍ଭାଵ <minix/cpufeature.h>
#ଅନ୍ତର୍ଭାଵ <sys/reboot.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>

#ଅନ୍ତର୍ଭାଵ <minix/u64.h>

#ଅନ୍ତର୍ଭାଵ "arch_proto.h"
#ଅନ୍ତର୍ଭାଵ "oxpcie.h"
#ଅନ୍ତର୍ଭାଵ "direct_utils.h"

#ifdef USE_ACPI
#ଅନ୍ତର୍ଭାଵ "acpi.h"
#endif

#define     KBCMDP          4       /* kbd controller port (O) */
#define      KBC_PULSE0     0xfe    /* pulse output bit 0 */
#define      IO_KBD          0x060           /* 8042 Keyboard */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_has_tsc;

ଶୂନ୍ୟ
reset(ଶୂନ୍ୟ)
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
        ଯଦି (b != 0xff) {
                ଯଦି ((b & 0x1) != 0)
                        outb(0x92, b & 0xfe);
                outb(0x92, b | 0x1);
                busy_delay_ms(500);  /* wait 0.5 sec to see if that did it */
        }

	/* Triple fault */
	x86_triplefault();

	/* Give up on resetting */
	ଯେତେବେଳେ(1) {
		;
	}
}

ସ୍ଥିର __dead ଶୂନ୍ୟ
halt(ଶୂନ୍ୟ)
{
	ପାଇଁ ( ; ; )
		halt_cpu();
}

ସ୍ଥିର __dead ଶୂନ୍ୟ
poweroff(ଶୂନ୍ୟ)
{
	ଅଚଳ ଅକ୍ଷର *shutdown_str;

#ifdef USE_ACPI
	acpi_poweroff();
#endif
	/* Bochs/QEMU poweroff */
	shutdown_str = "Shutdown";
        ଯେତେବେଳେ (*shutdown_str) outb(0x8900, *(shutdown_str++));

	/* VMware magic power off; likely to halt CPU */
	poweroff_vmware_clihlt();

	/* fallback option: hang */
	halt();
}

__dead ଶୂନ୍ୟ arch_shutdown(ପୂର୍ଣ୍ଣସଂଖ୍ୟା how)
{
	unsigned ଅକ୍ଷର unused_ch;
	/* Mask all interrupts, including the clock. */
	outb( INT_CTLMASK, ~0);

	/* Empty buffer */
	ଯେତେବେଳେ(direct_read_char(&unused_ch))
		;

	ଯଦି(kinfo.minix_panicing) {
		/* Printing is done synchronously over serial. */
		ଯଦି (kinfo.do_serial_debug)
			reset();

		/* Print accumulated diagnostics buffer and reset. */
		direct_cls();
		direct_print("Minix panic. System diagnostics buffer:\n\n");
		direct_print(kmess.kmess_buf);
		direct_print("\nSystem has panicked, press any key to reboot");
		ଯେତେବେଳେ (!direct_read_char(&unused_ch))
			;
		reset();
	}
		
	ଯଦି((how & RB_POWERDOWN) == RB_POWERDOWN) {
		/* Power off if possible, hang otherwise */
		poweroff();
		NOT_REACHABLE;
	}

	ଯଦି(how & RB_HALT) {
		/* Hang */
		ପାଇଁ (; ; ) halt_cpu();
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
ଶୂନ୍ୟ ser_putc(ଅକ୍ଷର c)
{
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsr, thr;

#ଯଦି CONFIG_OXPCIE
        oxpcie_putc(c);
#ଅନ୍ୟଥା
        lsr= COM1_LSR;
        thr= COM1_THR;
        ପାଇଁ (i= 0; i<100000; i++)
        {
                ଯଦି (inb( lsr) & LSR_THRE)
                        ଭାଙ୍ଗ;
        }
        outb( thr, c);
#endif
}

#endif
