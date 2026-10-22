#ifndef TYPE_H
#define TYPE_H

#شامل <minix/com.h>
#شامل <machine/interrupt.h>
#شامل <machine/multiboot.h>

/* Process table and system property related types. */ 
تعریفنوع صحیح proc_nr_t;			/* process table entry number */
تعریفنوع short sys_id_t;			/* system process index */
تعریفنوع ساختار {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

تعریفنوع unsigned long irq_policy_t;	
تعریفنوع unsigned long irq_id_t;	

تعریفنوع ساختار irq_hook {
  ساختار irq_hook *next;		/* next hook in chain */
  صحیح (*handler)(ساختار irq_hook *);	/* interrupt handler */
  صحیح irq;				/* IRQ vector number */ 
  صحیح id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

تعریفنوع صحیح (*irq_handler_t)(ساختار irq_hook *);

#endif /* TYPE_H */
