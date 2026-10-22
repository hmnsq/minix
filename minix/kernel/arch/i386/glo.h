#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_proto.h"

EXTERN പൂർണ്ണസംഖ്യ cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN ഘടന tss_s tss[CONFIG_MAX_CPUS];

EXTERN പൂർണ്ണസംഖ്യ i386_paging_enabled;

#endif /* __GLO_X86_H__ */
