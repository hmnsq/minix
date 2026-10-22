#ifndef TYPE_H
#define TYPE_H

#અન્તર્ભાવ <minix/com.h>
#અન્તર્ભાવ <machine/interrupt.h>
#અન્તર્ભાવ <machine/multiboot.h>

/* Process table and system property related types. */ 
પ્રકારવ્યાખ્યા પૂર્ણાંક proc_nr_t;			/* process table entry number */
પ્રકારવ્યાખ્યા short sys_id_t;			/* system process index */
પ્રકારવ્યાખ્યા સંરચના {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

પ્રકારવ્યાખ્યા unsigned long irq_policy_t;	
પ્રકારવ્યાખ્યા unsigned long irq_id_t;	

પ્રકારવ્યાખ્યા સંરચના irq_hook {
  સંરચના irq_hook *next;		/* next hook in chain */
  પૂર્ણાંક (*handler)(સંરચના irq_hook *);	/* interrupt handler */
  પૂર્ણાંક irq;				/* IRQ vector number */ 
  પૂર્ણાંક id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

પ્રકારવ્યાખ્યા પૂર્ણાંક (*irq_handler_t)(સંરચના irq_hook *);

#endif /* TYPE_H */
