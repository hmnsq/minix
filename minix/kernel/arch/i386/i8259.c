/* This file contains routines for initializing the 8259 interrupt controller:
 *	put_irq_handler: register an interrupt handler
 *	rm_irq_handler: deregister an interrupt handler
 *	intr_handle:	handle a hardware interrupt
 *	intr_init:	initialize the interrupt controller(s)
 */

#شامل "kernel/kernel.h"
#شامل "arch_proto.h"
#شامل "hw_intr.h"
#شامل <machine/cpu.h>

#تعریف_کرو ICW1_AT         0x11	/* edge triggered, cascade, need ICW4 */
#تعریف_کرو ICW1_PC         0x13	/* edge triggered, no cascade, need ICW4 */
#تعریف_کرو ICW1_PS         0x19	/* level triggered, cascade, need ICW4 */
#تعریف_کرو ICW4_AT_SLAVE   0x01	/* not SFNM, not buffered, normal EOI, 8086 */
#تعریف_کرو ICW4_AT_MASTER  0x05	/* not SFNM, not buffered, normal EOI, 8086 */
#تعریف_کرو ICW4_PC_SLAVE   0x09	/* not SFNM, buffered, normal EOI, 8086 */
#تعریف_کرو ICW4_PC_MASTER  0x0D	/* not SFNM, buffered, normal EOI, 8086 */
#تعریف_کرو ICW4_AT_AEOI_SLAVE   0x03 /* not SFNM, not buffered, auto EOI, 8086 */
#تعریف_کرو ICW4_AT_AEOI_MASTER  0x07 /* not SFNM, not buffered, auto EOI, 8086 */
#تعریف_کرو ICW4_PC_AEOI_SLAVE   0x0B /* not SFNM, buffered, auto EOI, 8086 */
#تعریف_کرو ICW4_PC_AEOI_MASTER  0x0F /* not SFNM, buffered, auto EOI, 8086 */

/*===========================================================================*
 *				intr_init				     *
 *===========================================================================*/
صحیح intr_init(ثابت صحیح auto_eoi)
{
/* Initialize the 8259s, finishing with all interrupts disabled.  */
      outb( INT_CTL, ICW1_AT);
      outb( INT_CTLMASK, IRQ0_سمتیہ);
					/* ICW2 for master */
      outb( INT_CTLMASK, (1 << CASCADE_IRQ));
					/* ICW3 tells slaves */
      اگر (auto_eoi)
          outb( INT_CTLMASK, ICW4_AT_AEOI_MASTER);
      ورنہ
          outb( INT_CTLMASK, ICW4_AT_MASTER);
      outb( INT_CTLMASK, ~(1 << CASCADE_IRQ)); /* IRQ 0-7 mask */
      outb( INT2_CTL, ICW1_AT);
      outb( INT2_CTLMASK, IRQ8_سمتیہ);
						/* ICW2 for slave */
      outb( INT2_CTLMASK, CASCADE_IRQ);	/* ICW3 is slave nr */
      اگر (auto_eoi)
         outb( INT2_CTLMASK, ICW4_AT_AEOI_SLAVE);
      ورنہ
         outb( INT2_CTLMASK, ICW4_AT_SLAVE);
      outb( INT2_CTLMASK, ~0);		/* IRQ 8-15 mask */

  واپس OK;
}

خالی irq_8259_unmask(ثابت صحیح irq)
{
	ثابت غیرعلامتی ctl_نقاب = irq < 8 ? INT_CTLMASK : INT2_CTLMASK;
	outb(ctl_نقاب, inb(ctl_نقاب) & ~(1 << (irq & 0x7)));
}

خالی irq_8259_نقاب(ثابت صحیح irq)
{
	ثابت غیرعلامتی ctl_نقاب = irq < 8 ? INT_CTLMASK : INT2_CTLMASK;
	outb(ctl_نقاب, inb(ctl_نقاب) | (1 << (irq & 0x7)));
}

/* Disable 8259 - write 0xFF in OCW1 master and slave. */
خالی i8259_disable(خالی)
{
	outb(INT2_CTLMASK, 0xFF);
	outb(INT_CTLMASK, 0xFF);
	inb(INT_CTLMASK);
}

خالی irq_8259_eoi(صحیح irq)
{
	اگر (irq < 8)
		eoi_8259_master();
	ورنہ
		eoi_8259_slave();
}
