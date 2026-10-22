#ifndef TYPE_H
#define TYPE_H

#অন্তর্ভাব <minix/com.h>
#অন্তর্ভাব <machine/interrupt.h>
#অন্তর্ভাব <machine/multiboot.h>

/* Process table and system property related types. */ 
প্রকারলেখ পূর্ণসংখ্যা proc_nr_t;			/* process table entry number */
প্রকারলেখ short sys_id_t;			/* system process index */
প্রকারলেখ কাঠামো {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

প্রকারলেখ unsigned long irq_policy_t;	
প্রকারলেখ unsigned long irq_id_t;	

প্রকারলেখ কাঠামো irq_hook {
  কাঠামো irq_hook *পরবর্তী;		/* next hook in chain */
  পূর্ণসংখ্যা (*handler)(কাঠামো irq_hook *);	/* interrupt handler */
  পূর্ণসংখ্যা irq;				/* IRQ vector number */ 
  পূর্ণসংখ্যা id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

প্রকারলেখ পূর্ণসংখ্যা (*irq_handler_t)(কাঠামো irq_hook *);

#endif /* TYPE_H */
