#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "arch_proto.h"

EXTERN முழு cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN கட்டமைப்பு tss_s tss[CONFIG_MAX_CPUS];

EXTERN முழு i386_paging_enabled;

#endif /* __GLO_X86_H__ */
