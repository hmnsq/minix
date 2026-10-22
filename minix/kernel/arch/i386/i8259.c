/* This file contains routines for initializing the 8259 interrupt controller:
 *	put_irq_handler: register an interrupt handler
 *	rm_irq_handler: deregister an interrupt handler
 *	intr_handle:	handle a hardware interrupt
 *	intr_init:	initialize the interrupt controller(s)
 */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ "hw_intr.h"
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>

#define ICW1_AT         0x11	/* edge triggered, cascade, need ICW4 */
#define ICW1_PC         0x13	/* edge triggered, no cascade, need ICW4 */
#define ICW1_PS         0x19	/* level triggered, cascade, need ICW4 */
#define ICW4_AT_SLAVE   0x01	/* not SFNM, not buffered, normal EOI, 8086 */
#define ICW4_AT_MASTER  0x05	/* not SFNM, not buffered, normal EOI, 8086 */
#define ICW4_PC_SLAVE   0x09	/* not SFNM, buffered, normal EOI, 8086 */
#define ICW4_PC_MASTER  0x0D	/* not SFNM, buffered, normal EOI, 8086 */
#define ICW4_AT_AEOI_SLAVE   0x03 /* not SFNM, not buffered, auto EOI, 8086 */
#define ICW4_AT_AEOI_MASTER  0x07 /* not SFNM, not buffered, auto EOI, 8086 */
#define ICW4_PC_AEOI_SLAVE   0x0B /* not SFNM, buffered, auto EOI, 8086 */
#define ICW4_PC_AEOI_MASTER  0x0F /* not SFNM, buffered, auto EOI, 8086 */

/*===========================================================================*
 *				intr_init				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ intr_init(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ auto_eoi)
{
/* Initialize the 8259s, finishing with all interrupts disabled.  */
      outb( INT_CTL, ICW1_AT);
      outb( INT_CTLMASK, IRQ0_VECTOR);
					/* ICW2 for master */
      outb( INT_CTLMASK, (1 << CASCADE_IRQ));
					/* ICW3 tells slaves */
      ಯದಿ (auto_eoi)
          outb( INT_CTLMASK, ICW4_AT_AEOI_MASTER);
      ಅನ್ಯಥಾ
          outb( INT_CTLMASK, ICW4_AT_MASTER);
      outb( INT_CTLMASK, ~(1 << CASCADE_IRQ)); /* IRQ 0-7 mask */
      outb( INT2_CTL, ICW1_AT);
      outb( INT2_CTLMASK, IRQ8_VECTOR);
						/* ICW2 for slave */
      outb( INT2_CTLMASK, CASCADE_IRQ);	/* ICW3 is slave nr */
      ಯದಿ (auto_eoi)
         outb( INT2_CTLMASK, ICW4_AT_AEOI_SLAVE);
      ಅನ್ಯಥಾ
         outb( INT2_CTLMASK, ICW4_AT_SLAVE);
      outb( INT2_CTLMASK, ~0);		/* IRQ 8-15 mask */

  ಪ್ರತ್ಯಾಗಮ OK;
}

ಶೂನ್ಯ irq_8259_unmask(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ irq)
{
	ಧ್ರುವ unsigned ctl_mask = irq < 8 ? INT_CTLMASK : INT2_CTLMASK;
	outb(ctl_mask, inb(ctl_mask) & ~(1 << (irq & 0x7)));
}

ಶೂನ್ಯ irq_8259_mask(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ irq)
{
	ಧ್ರುವ unsigned ctl_mask = irq < 8 ? INT_CTLMASK : INT2_CTLMASK;
	outb(ctl_mask, inb(ctl_mask) | (1 << (irq & 0x7)));
}

/* Disable 8259 - write 0xFF in OCW1 master and slave. */
ಶೂನ್ಯ i8259_disable(ಶೂನ್ಯ)
{
	outb(INT2_CTLMASK, 0xFF);
	outb(INT_CTLMASK, 0xFF);
	inb(INT_CTLMASK);
}

ಶೂನ್ಯ irq_8259_eoi(ಪೂರ್ಣಾಙ್ಕ irq)
{
	ಯದಿ (irq < 8)
		eoi_8259_master();
	ಅನ್ಯಥಾ
		eoi_8259_slave();
}
