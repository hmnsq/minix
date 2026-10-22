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

#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
ಸ್ಥಿರ irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
ಶೂನ್ಯ put_irq_handler( irq_hook_t* hook, ಪೂರ್ಣಾಙ್ಕ irq,
  ಧ್ರುವ irq_handler_t handler)
{
  ಪೂರ್ಣಾಙ್ಕ id;
  irq_hook_t **ಸಾಲು;
  unsigned long bitmap;

  ಯದಿ( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  ಸಾಲು = &irq_handlers[irq];

  bitmap = 0;
  ಯಾವತ್ ( *ಸಾಲು != ರಿಕ್ತ ) {
	ಯದಿ(hook == *ಸಾಲು) ಪ್ರತ್ಯಾಗಮ; /* extra initialization */
	bitmap |= (*ಸಾಲು)->id;	/* mark ids in use */
	ಸಾಲು = &(*ಸಾಲು)->next;
  }

  /* find the lowest id not in use */
  ಪ್ರತಿ (id = 1; id != 0; id <<= 1)
  	ಯದಿ (!(bitmap & id)) ಭಙ್ಗ;

  ಯದಿ(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = ರಿಕ್ತ;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *ಸಾಲು = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  ಯದಿ((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
ಶೂನ್ಯ rm_irq_handler( ಧ್ರುವ irq_hook_t* hook ) {
  ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ irq = hook->irq; 
  ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ id = hook->id;
  irq_hook_t **ಸಾಲು;

  ಯದಿ( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  ಸಾಲು = &irq_handlers[irq];
  ಯಾವತ್( (*ಸಾಲು) != ರಿಕ್ತ ) {
	ಯದಿ((*ಸಾಲು)->id == id) {
		(*ಸಾಲು) = (*ಸಾಲು)->next;
		ಯದಿ (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	ಅನ್ಯಥಾ {
		ಸಾಲು = &(*ಸಾಲು)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  ಯದಿ (irq_handlers[irq] == ರಿಕ್ತ) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  ಅನ್ಯಥಾ ಯದಿ (irq_actids[irq] == 0) {
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
ಶೂನ್ಯ irq_handle(ಪೂರ್ಣಾಙ್ಕ irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  ಪ್ರತಿಜ್ಞಾ(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  ಯದಿ(hook == ರಿಕ್ತ) {
      ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      ಯದಿ(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	ಯದಿ(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      ಪ್ರತ್ಯಾಗಮ;
  }

  /* Call list of handlers for an IRQ. */
  ಯಾವತ್( hook != ರಿಕ್ತ ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    ಯದಿ( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  ಯದಿ (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
ಶೂನ್ಯ enable_irq(ಧ್ರುವ irq_hook_t *hook)
{
  ಯದಿ((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
ಪೂರ್ಣಾಙ್ಕ disable_irq(ಧ್ರುವ irq_hook_t *hook)
{
  ಯದಿ(irq_actids[hook->irq] & hook->id)  /* already disabled */
    ಪ್ರತ್ಯಾಗಮ 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

