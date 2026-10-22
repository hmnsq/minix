#ifndef TYPE_H
#define TYPE_H

#অন্তর্ভাব <minix/com.h>
#অন্তর্ভাব <machine/interrupt.h>
#অন্তর্ভাব <machine/multiboot.h>

/* Process table and system property related types. */ 
প্রকারনির্দেশ পূর্ণাঙ্ক proc_nr_t;			/* process table entry number */
প্রকারনির্দেশ short sys_id_t;			/* system process index */
প্রকারনির্দেশ সংরচনা {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

প্রকারনির্দেশ unsigned long irq_policy_t;	
প্রকারনির্দেশ unsigned long irq_id_t;	

প্রকারনির্দেশ সংরচনা irq_hook {
  সংরচনা irq_hook *next;		/* next hook in chain */
  পূর্ণাঙ্ক (*handler)(সংরচনা irq_hook *);	/* interrupt handler */
  পূর্ণাঙ্ক irq;				/* IRQ vector number */ 
  পূর্ণাঙ্ক id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

প্রকারনির্দেশ পূর্ণাঙ্ক (*irq_handler_t)(সংরচনা irq_hook *);

#endif /* TYPE_H */
