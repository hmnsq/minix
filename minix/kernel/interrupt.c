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
ಶೂನ್ಯ put_irq_handler( irq_hook_t* hook, ಪೂರ್ಣಾಂಕ irq,
  ಸ್ಥಿರಾಂಕ irq_handler_t handler)
{
  ಪೂರ್ಣಾಂಕ id;
  irq_hook_t **ಸಾಲು;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bitmap;

  ಆದರೆ( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  ಸಾಲು = &irq_handlers[irq];

  bitmap = 0;
  ಆಗ ( *ಸಾಲು != ಖಾಲಿ ) {
	ಆದರೆ(hook == *ಸಾಲು) ಹಿಂತಿರುಗಿ; /* extra initialization */
	bitmap |= (*ಸಾಲು)->id;	/* mark ids in use */
	ಸಾಲು = &(*ಸಾಲು)->ಮುಂದಿನ;
  }

  /* find the lowest id not in use */
  ಗಾಗಿ (id = 1; id != 0; id <<= 1)
  	ಆದರೆ (!(bitmap & id)) ನಿಲ್ಲಿಸು;

  ಆದರೆ(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->ಮುಂದಿನ = ಖಾಲಿ;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *ಸಾಲು = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  ಆದರೆ((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_ಬಳಸಿದ(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
ಶೂನ್ಯ rm_irq_handler( ಸ್ಥಿರಾಂಕ irq_hook_t* hook ) {
  ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ irq = hook->irq; 
  ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ id = hook->id;
  irq_hook_t **ಸಾಲು;

  ಆದರೆ( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  ಸಾಲು = &irq_handlers[irq];
  ಆಗ( (*ಸಾಲು) != ಖಾಲಿ ) {
	ಆದರೆ((*ಸಾಲು)->id == id) {
		(*ಸಾಲು) = (*ಸಾಲು)->ಮುಂದಿನ;
		ಆದರೆ (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	ಇಲ್ಲದಿದ್ದರೆ {
		ಸಾಲು = &(*ಸಾಲು)->ಮುಂದಿನ;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  ಆದರೆ (irq_handlers[irq] == ಖಾಲಿ) {
	hw_intr_ಮುಖವಾಡ(irq);
	hw_intr_not_ಬಳಸಿದ(irq);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (irq_actids[irq] == 0) {
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
ಶೂನ್ಯ irq_handle(ಪೂರ್ಣಾಂಕ irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  ಪ್ರತಿಜ್ಞಾ(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_ಮುಖವಾಡ(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  ಆದರೆ(hook == ಖಾಲಿ) {
      ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      ಆದರೆ(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	ಬರೆ("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	ಆದರೆ(report_interval < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ/2)
		report_interval *= 2;
      }
      ಹಿಂತಿರುಗಿ;
  }

  /* Call list of handlers for an IRQ. */
  ಆಗ( hook != ಖಾಲಿ ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    ಆದರೆ( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->ಮುಂದಿನ;
  }

  /* reenable the IRQ only if there is no active handler */
  ಆದರೆ (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
ಶೂನ್ಯ enable_irq(ಸ್ಥಿರಾಂಕ irq_hook_t *hook)
{
  ಆದರೆ((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
ಪೂರ್ಣಾಂಕ disable_irq(ಸ್ಥಿರಾಂಕ irq_hook_t *hook)
{
  ಆದರೆ(irq_actids[hook->irq] & hook->id)  /* already disabled */
    ಹಿಂತಿರುಗಿ 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_ಮುಖವಾಡ(hook->irq);
  ಹಿಂತಿರುಗಿ TRUE;
}

