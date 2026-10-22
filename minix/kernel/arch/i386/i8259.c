/* This file contains routines for initializing the 8259 interrupt controller:
 *	put_irq_handler: register an interrupt handler
 *	rm_irq_handler: deregister an interrupt handler
 *	intr_handle:	handle a hardware interrupt
 *	intr_init:	initialize the interrupt controller(s)
 */

#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"
#समावेश "hw_intr.h"
#समावेश <machine/cpu.h>

#घोषणा ICW1_AT         0x11	/* edge triggered, cascade, need ICW4 */
#घोषणा ICW1_PC         0x13	/* edge triggered, no cascade, need ICW4 */
#घोषणा ICW1_PS         0x19	/* level triggered, cascade, need ICW4 */
#घोषणा ICW4_AT_SLAVE   0x01	/* not SFNM, not buffered, normal EOI, 8086 */
#घोषणा ICW4_AT_MASTER  0x05	/* not SFNM, not buffered, normal EOI, 8086 */
#घोषणा ICW4_PC_SLAVE   0x09	/* not SFNM, buffered, normal EOI, 8086 */
#घोषणा ICW4_PC_MASTER  0x0D	/* not SFNM, buffered, normal EOI, 8086 */
#घोषणा ICW4_AT_AEOI_SLAVE   0x03 /* not SFNM, not buffered, auto EOI, 8086 */
#घोषणा ICW4_AT_AEOI_MASTER  0x07 /* not SFNM, not buffered, auto EOI, 8086 */
#घोषणा ICW4_PC_AEOI_SLAVE   0x0B /* not SFNM, buffered, auto EOI, 8086 */
#घोषणा ICW4_PC_AEOI_MASTER  0x0F /* not SFNM, buffered, auto EOI, 8086 */

/*===========================================================================*
 *				intr_init				     *
 *===========================================================================*/
पूर्णांक intr_init(स्थिर पूर्णांक auto_eoi)
{
/* Initialize the 8259s, finishing with all interrupts disabled.  */
      outb( INT_CTL, ICW1_AT);
      outb( INT_CTLMASK, IRQ0_VECTOR);
					/* ICW2 for master */
      outb( INT_CTLMASK, (1 << CASCADE_IRQ));
					/* ICW3 tells slaves */
      अगर (auto_eoi)
          outb( INT_CTLMASK, ICW4_AT_AEOI_MASTER);
      अन्यथा
          outb( INT_CTLMASK, ICW4_AT_MASTER);
      outb( INT_CTLMASK, ~(1 << CASCADE_IRQ)); /* IRQ 0-7 mask */
      outb( INT2_CTL, ICW1_AT);
      outb( INT2_CTLMASK, IRQ8_VECTOR);
						/* ICW2 for slave */
      outb( INT2_CTLMASK, CASCADE_IRQ);	/* ICW3 is slave nr */
      अगर (auto_eoi)
         outb( INT2_CTLMASK, ICW4_AT_AEOI_SLAVE);
      अन्यथा
         outb( INT2_CTLMASK, ICW4_AT_SLAVE);
      outb( INT2_CTLMASK, ~0);		/* IRQ 8-15 mask */

  वापस OK;
}

व्योम irq_8259_unmask(स्थिर पूर्णांक irq)
{
	स्थिर अचिन्हित ctl_mask = irq < 8 ? INT_CTLMASK : INT2_CTLMASK;
	outb(ctl_mask, inb(ctl_mask) & ~(1 << (irq & 0x7)));
}

व्योम irq_8259_mask(स्थिर पूर्णांक irq)
{
	स्थिर अचिन्हित ctl_mask = irq < 8 ? INT_CTLMASK : INT2_CTLMASK;
	outb(ctl_mask, inb(ctl_mask) | (1 << (irq & 0x7)));
}

/* Disable 8259 - write 0xFF in OCW1 master and slave. */
व्योम i8259_disable(व्योम)
{
	outb(INT2_CTLMASK, 0xFF);
	outb(INT_CTLMASK, 0xFF);
	inb(INT_CTLMASK);
}

व्योम irq_8259_eoi(पूर्णांक irq)
{
	अगर (irq < 8)
		eoi_8259_master();
	अन्यथा
		eoi_8259_slave();
}
