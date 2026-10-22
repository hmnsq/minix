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

#அந்தர்பாவ <assert.h>

#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
நிலையான irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
வெறுமை put_irq_handler( irq_hook_t* hook, முழு irq,
  மாறிலி irq_handler_t handler)
{
  முழு id;
  irq_hook_t **வரி;
  unsigned long bitmap;

  எனில்( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  வரி = &irq_handlers[irq];

  bitmap = 0;
  போது ( *வரி != வெற்று ) {
	எனில்(hook == *வரி) திரும்பு; /* extra initialization */
	bitmap |= (*வரி)->id;	/* mark ids in use */
	வரி = &(*வரி)->next;
  }

  /* find the lowest id not in use */
  க்கு (id = 1; id != 0; id <<= 1)
  	எனில் (!(bitmap & id)) நிறுத்து;

  எனில்(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = வெற்று;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *வரி = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  எனில்((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
வெறுமை rm_irq_handler( மாறிலி irq_hook_t* hook ) {
  மாறிலி முழு irq = hook->irq; 
  மாறிலி முழு id = hook->id;
  irq_hook_t **வரி;

  எனில்( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  வரி = &irq_handlers[irq];
  போது( (*வரி) != வெற்று ) {
	எனில்((*வரி)->id == id) {
		(*வரி) = (*வரி)->next;
		எனில் (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	இல்லையெனில் {
		வரி = &(*வரி)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  எனில் (irq_handlers[irq] == வெற்று) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  இல்லையெனில் எனில் (irq_actids[irq] == 0) {
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
வெறுமை irq_handle(முழு irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  ப்ரதிஜ்ஞா(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  எனில்(hook == வெற்று) {
      நிலையான முழு nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      எனில்(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	எனில்(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      திரும்பு;
  }

  /* Call list of handlers for an IRQ. */
  போது( hook != வெற்று ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    எனில்( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  எனில் (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
வெறுமை enable_irq(மாறிலி irq_hook_t *hook)
{
  எனில்((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
முழு disable_irq(மாறிலி irq_hook_t *hook)
{
  எனில்(irq_actids[hook->irq] & hook->id)  /* already disabled */
    திரும்பு 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  திரும்பு TRUE;
}

