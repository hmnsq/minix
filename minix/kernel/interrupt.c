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

#ضمن <assert.h>

#ضمن "kernel/kernel.h"
#ضمن "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
ساكن irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
فراغ put_irq_handler( irq_hook_t* hook, صحيح irq,
  ثابت irq_handler_t handler)
{
  صحيح id;
  irq_hook_t **کرښه;
  بینښه اوږد bitmap;

  إذا( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  کرښه = &irq_handlers[irq];

  bitmap = 0;
  طالما ( *کرښه != عدم ) {
	إذا(hook == *کرښه) أرجع; /* extra initialization */
	bitmap |= (*کرښه)->id;	/* mark ids in use */
	کرښه = &(*کرښه)->بل;
  }

  /* find the lowest id not in use */
  لكل (id = 1; id != 0; id <<= 1)
  	إذا (!(bitmap & id)) اكسر;

  إذا(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->بل = عدم;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *کرښه = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  إذا((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_کارول_شوی(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
فراغ rm_irq_handler( ثابت irq_hook_t* hook ) {
  ثابت صحيح irq = hook->irq; 
  ثابت صحيح id = hook->id;
  irq_hook_t **کرښه;

  إذا( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  کرښه = &irq_handlers[irq];
  طالما( (*کرښه) != عدم ) {
	إذا((*کرښه)->id == id) {
		(*کرښه) = (*کرښه)->بل;
		إذا (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	وإلا {
		کرښه = &(*کرښه)->بل;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  إذا (irq_handlers[irq] == عدم) {
	hw_intr_ماسک(irq);
	hw_intr_not_کارول_شوی(irq);
  }
  وإلا إذا (irq_actids[irq] == 0) {
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
فراغ irq_handle(صحيح irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  أكد(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_ماسک(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  إذا(hook == عدم) {
      ساكن صحيح nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      إذا(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	چاپ_کړه("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	إذا(report_interval < لوی_عدد/2)
		report_interval *= 2;
      }
      أرجع;
  }

  /* Call list of handlers for an IRQ. */
  طالما( hook != عدم ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    إذا( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->بل;
  }

  /* reenable the IRQ only if there is no active handler */
  إذا (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
فراغ enable_irq(ثابت irq_hook_t *hook)
{
  إذا((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
صحيح disable_irq(ثابت irq_hook_t *hook)
{
  إذا(irq_actids[hook->irq] & hook->id)  /* already disabled */
    أرجع 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_ماسک(hook->irq);
  أرجع TRUE;
}

