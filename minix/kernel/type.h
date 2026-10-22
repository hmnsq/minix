#اگر_غیر_تعریف_شدہ TYPE_H
#تعریف_کرو TYPE_H

#شامل <minix/com.h>
#شامل <machine/interrupt.h>
#شامل <machine/multiboot.h>

/* Process table and system property related types. */ 
قسمتعریف صحیح proc_nr_t;			/* process table entry number */
قسمتعریف مختصر sys_id_t;			/* system process index */
قسمتعریف ساخت {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

قسمتعریف غیرعلامتی طویل irq_policy_t;	
قسمتعریف غیرعلامتی طویل irq_id_t;	

قسمتعریف ساخت irq_hook {
  ساخت irq_hook *اگلا;		/* next hook in chain */
  صحیح (*handler)(ساخت irq_hook *);	/* interrupt handler */
  صحیح irq;				/* IRQ vector number */ 
  صحیح id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

قسمتعریف صحیح (*irq_handler_t)(ساخت irq_hook *);

#اگر_ختم /* TYPE_H */
