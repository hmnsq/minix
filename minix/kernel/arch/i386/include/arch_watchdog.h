#اگر_غیر_تعریف_شدہ __I386_WATCHDOG_H__
#تعریف_کرو __I386_WATCHDOG_H__

#شامل "kernel/kernel.h"

ساخت nmi_چوکھٹا {
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

صحیح i386_watchdog_آغاز(خالی);

#تعریف_کرو nmi_اندر_kernel(f)	((f)->cs == KERN_CS_SELECTOR)

#اگر_ختم /* __I386_WATCHDOG_H__ */
