#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __I386_WATCHDOG_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __I386_WATCHDOG_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

ਬਣਤਰ nmi_ਚੌਖਟਾ {
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

ਪੂਰਨ i386_watchdog_ਸ਼ੁਰੂ(ਖਾਲੀ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ nmi_ਵਿੱਚ_kernel(f)	((f)->cs == KERN_CS_SELECTOR)

#ਜੇ_ਅੰਤ /* __I386_WATCHDOG_H__ */
