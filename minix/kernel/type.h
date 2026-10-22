#ifndef TYPE_H
#define TYPE_H

#அந்தர்பாவ <minix/com.h>
#அந்தர்பாவ <machine/interrupt.h>
#அந்தர்பாவ <machine/multiboot.h>

/* Process table and system property related types. */ 
வகைவரை முழு proc_nr_t;			/* process table entry number */
வகைவரை short sys_id_t;			/* system process index */
வகைவரை கட்டமைப்பு {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

வகைவரை unsigned long irq_policy_t;	
வகைவரை unsigned long irq_id_t;	

வகைவரை கட்டமைப்பு irq_hook {
  கட்டமைப்பு irq_hook *next;		/* next hook in chain */
  முழு (*handler)(கட்டமைப்பு irq_hook *);	/* interrupt handler */
  முழு irq;				/* IRQ vector number */ 
  முழு id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

வகைவரை முழு (*irq_handler_t)(கட்டமைப்பு irq_hook *);

#endif /* TYPE_H */
