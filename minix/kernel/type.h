#ifndef TYPE_H
#define TYPE_H

#ଅନ୍ତର୍ଭାଵ <minix/com.h>
#ଅନ୍ତର୍ଭାଵ <machine/interrupt.h>
#ଅନ୍ତର୍ଭାଵ <machine/multiboot.h>

/* Process table and system property related types. */ 
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr_t;			/* process table entry number */
ପ୍ରକାରବ୍ୟାଖ୍ୟା short sys_id_t;			/* system process index */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned long irq_policy_t;	
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned long irq_id_t;	

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା irq_hook {
  ସଂରଚନା irq_hook *next;		/* next hook in chain */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*handler)(ସଂରଚନା irq_hook *);	/* interrupt handler */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq;				/* IRQ vector number */ 
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*irq_handler_t)(ସଂରଚନା irq_hook *);

#endif /* TYPE_H */
