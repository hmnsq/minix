#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __I386_WATCHDOG_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __I386_WATCHDOG_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

ರಚನೆ nmi_ಚೌಕಟ್ಟು {
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

ಪೂರ್ಣಾಂಕ i386_watchdog_ಆರಂಭ(ಶೂನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿಸು nmi_ಒಳ_kernel(f)	((f)->cs == KERN_CS_SELECTOR)

#ಆದರೆ_ಅಂತ್ಯ /* __I386_WATCHDOG_H__ */
