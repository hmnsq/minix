/* This file contains routines for initializing the 8259 interrupt controller:
 *	put_irq_handler: register an interrupt handler
 *	rm_irq_handler: deregister an interrupt handler
 *	intr_handle:	handle a hardware interrupt
 *	intr_init:	initialize the interrupt controller(s)
 */

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "hw_intr.h"
#అన్తర్భావ <machine/cpu.h>

#నిర్వచించు ICW1_AT         0x11	/* edge triggered, cascade, need ICW4 */
#నిర్వచించు ICW1_PC         0x13	/* edge triggered, no cascade, need ICW4 */
#నిర్వచించు ICW1_PS         0x19	/* level triggered, cascade, need ICW4 */
#నిర్వచించు ICW4_AT_SLAVE   0x01	/* not SFNM, not buffered, normal EOI, 8086 */
#నిర్వచించు ICW4_AT_MASTER  0x05	/* not SFNM, not buffered, normal EOI, 8086 */
#నిర్వచించు ICW4_PC_SLAVE   0x09	/* not SFNM, buffered, normal EOI, 8086 */
#నిర్వచించు ICW4_PC_MASTER  0x0D	/* not SFNM, buffered, normal EOI, 8086 */
#నిర్వచించు ICW4_AT_AEOI_SLAVE   0x03 /* not SFNM, not buffered, auto EOI, 8086 */
#నిర్వచించు ICW4_AT_AEOI_MASTER  0x07 /* not SFNM, not buffered, auto EOI, 8086 */
#నిర్వచించు ICW4_PC_AEOI_SLAVE   0x0B /* not SFNM, buffered, auto EOI, 8086 */
#నిర్వచించు ICW4_PC_AEOI_MASTER  0x0F /* not SFNM, buffered, auto EOI, 8086 */

/*===========================================================================*
 *				intr_init				     *
 *===========================================================================*/
పూర్ణాంకం intr_init(స్థిరరాశి పూర్ణాంకం auto_eoi)
{
/* Initialize the 8259s, finishing with all interrupts disabled.  */
      outb( INT_CTL, ICW1_AT);
      outb( INT_CTLMASK, IRQ0_సదిశ);
					/* ICW2 for master */
      outb( INT_CTLMASK, (1 << CASCADE_IRQ));
					/* ICW3 tells slaves */
      ఒకవేళ (auto_eoi)
          outb( INT_CTLMASK, ICW4_AT_AEOI_MASTER);
      లేకపోతే
          outb( INT_CTLMASK, ICW4_AT_MASTER);
      outb( INT_CTLMASK, ~(1 << CASCADE_IRQ)); /* IRQ 0-7 mask */
      outb( INT2_CTL, ICW1_AT);
      outb( INT2_CTLMASK, IRQ8_సదిశ);
						/* ICW2 for slave */
      outb( INT2_CTLMASK, CASCADE_IRQ);	/* ICW3 is slave nr */
      ఒకవేళ (auto_eoi)
         outb( INT2_CTLMASK, ICW4_AT_AEOI_SLAVE);
      లేకపోతే
         outb( INT2_CTLMASK, ICW4_AT_SLAVE);
      outb( INT2_CTLMASK, ~0);		/* IRQ 8-15 mask */

  తిరిగి OK;
}

శూన్య irq_8259_unmask(స్థిరరాశి పూర్ణాంకం irq)
{
	స్థిరరాశి గుర్తులేని ctl_ముసుగు = irq < 8 ? INT_CTLMASK : INT2_CTLMASK;
	outb(ctl_ముసుగు, inb(ctl_ముసుగు) & ~(1 << (irq & 0x7)));
}

శూన్య irq_8259_ముసుగు(స్థిరరాశి పూర్ణాంకం irq)
{
	స్థిరరాశి గుర్తులేని ctl_ముసుగు = irq < 8 ? INT_CTLMASK : INT2_CTLMASK;
	outb(ctl_ముసుగు, inb(ctl_ముసుగు) | (1 << (irq & 0x7)));
}

/* Disable 8259 - write 0xFF in OCW1 master and slave. */
శూన్య i8259_disable(శూన్య)
{
	outb(INT2_CTLMASK, 0xFF);
	outb(INT_CTLMASK, 0xFF);
	inb(INT_CTLMASK);
}

శూన్య irq_8259_eoi(పూర్ణాంకం irq)
{
	ఒకవేళ (irq < 8)
		eoi_8259_master();
	లేకపోతే
		eoi_8259_slave();
}
