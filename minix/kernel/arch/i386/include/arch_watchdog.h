#నిర్వచితం_కాకపోతే __I386_WATCHDOG_H__
#నిర్వచించు __I386_WATCHDOG_H__

#అన్తర్భావ "kernel/kernel.h"

నిర్మాణం nmi_చట్రం {
	reg_t	eax;
	reg_t	ecx;
	reg_t	edx;
	reg_t	ebx;
	reg_t	esp;
	reg_t	ebp;
	reg_t	esi;
	reg_t	edi;
	u16_t	gs;
	u16_t	fs;
	u16_t	es;
	u16_t	ds;
	reg_t	pc;	/* arch independent name for program counter */
	reg_t	cs;
	reg_t	eflags;
};

పూర్ణాంకం i386_watchdog_ప్రారంభం(శూన్య);

#నిర్వచించు nmi_లోపల_kernel(f)	((f)->cs == KERN_CS_SELECTOR)

#అయితే_ముగింపు /* __I386_WATCHDOG_H__ */
