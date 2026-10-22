#यदि_अपरिभाषितम् __I386_WATCHDOG_H__
#परिभाषय __I386_WATCHDOG_H__

#अन्तर्भाव "kernel/kernel.h"

संरचना nmi_frame {
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

पूर्णाङ्क i386_watchdog_start(शून्य);

#परिभाषय nmi_अन्तर्_kernel(f)	((f)->cs == KERN_CS_SELECTOR)

#यदि_अन्तः /* __I386_WATCHDOG_H__ */
