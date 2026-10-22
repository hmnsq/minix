#جيڪڏهن_وصف_نه_ٿيل __I386_WATCHDOG_H__
#وصف_ڪريو __I386_WATCHDOG_H__

#ضمن "kernel/kernel.h"

بنية nmi_چوکٺو {
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

صحيح i386_watchdog_شروعات(فراغ);

#وصف_ڪريو nmi_اندريون_kernel(f)	((f)->cs == KERN_CS_SELECTOR)

#جيڪڏهن_پورو /* __I386_WATCHDOG_H__ */
