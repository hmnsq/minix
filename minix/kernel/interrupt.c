/*
 *   The Minix hardware interrupt system.
 *   
 *   This file contains routines for managing the interrupt
 *   controller.
 *  
 *   put_irq_handler: register an interrupt handler.
 *   rm_irq_handler:  deregister an interrupt handler.
 *   irq_handle:     handle a hardware interrupt.
 *                    called by the system dependent part when an
 *                    external interrupt occurs.
 *   enable_irq:      enable hook for IRQ.
 *   disable_irq:     disable hook for IRQ.
 */

#অন্তর্ভাব <assert.h>

#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
স্থির irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
শূন্য put_irq_handler( irq_hook_t* hook, পূর্ণাঙ্ক irq,
  ধ্রুব irq_handler_t handler)
{
  পূর্ণাঙ্ক id;
  irq_hook_t **সারি;
  unsigned long bitmap;

  যদি( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  সারি = &irq_handlers[irq];

  bitmap = 0;
  যাবত্ ( *সারি != রিক্ত ) {
	যদি(hook == *সারি) প্রত্যাগম; /* extra initialization */
	bitmap |= (*সারি)->id;	/* mark ids in use */
	সারি = &(*সারি)->next;
  }

  /* find the lowest id not in use */
  প্রতি (id = 1; id != 0; id <<= 1)
  	যদি (!(bitmap & id)) ভঙ্গ;

  যদি(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = রিক্ত;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *সারি = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  যদি((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
শূন্য rm_irq_handler( ধ্রুব irq_hook_t* hook ) {
  ধ্রুব পূর্ণাঙ্ক irq = hook->irq; 
  ধ্রুব পূর্ণাঙ্ক id = hook->id;
  irq_hook_t **সারি;

  যদি( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  সারি = &irq_handlers[irq];
  যাবত্( (*সারি) != রিক্ত ) {
	যদি((*সারি)->id == id) {
		(*সারি) = (*সারি)->next;
		যদি (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	অন্যথা {
		সারি = &(*সারি)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  যদি (irq_handlers[irq] == রিক্ত) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  অন্যথা যদি (irq_actids[irq] == 0) {
	hw_intr_unmask(irq);
  }
}

/*===========================================================================*
 *				irq_handle				     *
 *===========================================================================*/
/*
 * The function first disables interrupt is need be and restores the state at
 * the end. Before returning, it unmasks the IRQ if and only if all active ID
 * bits are cleared, and restart a process.
 */
শূন্য irq_handle(পূর্ণাঙ্ক irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  প্রতিজ্ঞা(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  যদি(hook == রিক্ত) {
      স্থির পূর্ণাঙ্ক nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      যদি(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	যদি(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      প্রত্যাগম;
  }

  /* Call list of handlers for an IRQ. */
  যাবত্( hook != রিক্ত ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    যদি( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  যদি (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
শূন্য enable_irq(ধ্রুব irq_hook_t *hook)
{
  যদি((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
পূর্ণাঙ্ক disable_irq(ধ্রুব irq_hook_t *hook)
{
  যদি(irq_actids[hook->irq] & hook->id)  /* already disabled */
    প্রত্যাগম 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  প্রত্যাগম TRUE;
}

