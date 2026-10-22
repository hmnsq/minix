#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_proto.h"

EXTERN પૂર્ણાંક cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN સંરચના tss_s tss[CONFIG_MAX_CPUS];

EXTERN પૂર્ણાંક i386_paging_enabled;

#endif /* __GLO_X86_H__ */
