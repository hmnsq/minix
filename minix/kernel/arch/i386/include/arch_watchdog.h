#अगर_अघोषित __I386_WATCHDOG_H__
#घोषणा __I386_WATCHDOG_H__

#समावेश "kernel/kernel.h"

काष्ठा nmi_frame {
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

पूर्णांक i386_watchdog_start(व्योम);

#घोषणा nmi_in_kernel(f)	((f)->cs == KERN_CS_SELECTOR)

#पूर्ण_अगर /* __I386_WATCHDOG_H__ */
