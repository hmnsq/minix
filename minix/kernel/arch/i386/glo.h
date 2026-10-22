#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"

EXTERN पूर्णाङ्क cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN संरचना tss_s tss[CONFIG_MAX_CPUS];

EXTERN पूर्णाङ्क i386_paging_enabled;

#endif /* __GLO_X86_H__ */
