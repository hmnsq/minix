#اگر_تعریف_نشده __I386_WATCHDOG_H__
#تعریف_کن __I386_WATCHDOG_H__

#ضمن "kernel/kernel.h"

بنية nmi_قاب {
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

صحيح i386_watchdog_شروع(فراغ);

#تعریف_کن nmi_درون_kernel(f)	((f)->cs == KERN_CS_SELECTOR)

#پایان_اگر /* __I386_WATCHDOG_H__ */
