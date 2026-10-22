#ifndef __CLOCK_X86_H__
#define __CLOCK_X86_H__

#अन्तर्भाव "../apic_asm.h"

पूर्णाङ्क init_8253A_timer(अचिन्हित freq);
शून्य stop_8253A_timer(शून्य);
शून्य arch_timer_int_handler(शून्य);

#endif /* __CLOCK_X86_H__ */
