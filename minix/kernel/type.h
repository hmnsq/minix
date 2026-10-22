#ifndef TYPE_H
#define TYPE_H

#अन्तर्भाव <minix/com.h>
#अन्तर्भाव <machine/interrupt.h>
#अन्तर्भाव <machine/multiboot.h>

/* Process table and system property related types. */ 
प्रकारनिर्देश पूर्णाङ्क proc_nr_t;			/* process table entry number */
प्रकारनिर्देश लघु sys_id_t;			/* system process index */
प्रकारनिर्देश संरचना {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

प्रकारनिर्देश अचिन्हित दीर्घ irq_policy_t;	
प्रकारनिर्देश अचिन्हित दीर्घ irq_id_t;	

प्रकारनिर्देश संरचना irq_hook {
  संरचना irq_hook *next;		/* next hook in chain */
  पूर्णाङ्क (*handler)(संरचना irq_hook *);	/* interrupt handler */
  पूर्णाङ्क irq;				/* IRQ vector number */ 
  पूर्णाङ्क id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

प्रकारनिर्देश पूर्णाङ्क (*irq_handler_t)(संरचना irq_hook *);

#endif /* TYPE_H */
