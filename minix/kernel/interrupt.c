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

#include <assert.h>

#include "kernel/kernel.h"
#include "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
static irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
void put_irq_handler( irq_hook_t* hook, ကိန်းပြည့် irq,
  const irq_handler_t handler)
{
  ကိန်းပြည့် id;
  irq_hook_t **line;
  unsigned long bitmap;

  အကယ်၍( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  line = &irq_handlers[irq];

  bitmap = 0;
  နေစဉ် ( *line != NULL ) {
	အကယ်၍(hook == *line) ပြန်ပေး; /* extra initialization */
	bitmap |= (*line)->id;	/* mark ids in use */
	line = &(*line)->next;
  }

  /* find the lowest id not in use */
  အတွက် (id = 1; id != 0; id <<= 1)
  	အကယ်၍ (!(bitmap & id)) bha_nga;

  အကယ်၍(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = NULL;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *line = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  အကယ်၍((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
void rm_irq_handler( const irq_hook_t* hook ) {
  const ကိန်းပြည့် irq = hook->irq; 
  const ကိန်းပြည့် id = hook->id;
  irq_hook_t **line;

  အကယ်၍( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  line = &irq_handlers[irq];
  နေစဉ်( (*line) != NULL ) {
	အကယ်၍((*line)->id == id) {
		(*line) = (*line)->next;
		အကယ်၍ (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	မဟုတ်ပါက {
		line = &(*line)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  အကယ်၍ (irq_handlers[irq] == NULL) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  မဟုတ်ပါက အကယ်၍ (irq_actids[irq] == 0) {
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
void irq_handle(ကိန်းပြည့် irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  assert(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  အကယ်၍(hook == NULL) {
      static ကိန်းပြည့် nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      အကယ်၍(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	အကယ်၍(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      ပြန်ပေး;
  }

  /* Call list of handlers for an IRQ. */
  နေစဉ်( hook != NULL ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    အကယ်၍( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  အကယ်၍ (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
void enable_irq(const irq_hook_t *hook)
{
  အကယ်၍((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
ကိန်းပြည့် disable_irq(const irq_hook_t *hook)
{
  အကယ်၍(irq_actids[hook->irq] & hook->id)  /* already disabled */
    ပြန်ပေး 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  ပြန်ပေး TRUE;
}

