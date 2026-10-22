#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "arch_proto.h"

EXTERN পূর্ণসংখ্যা cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN কাঠামো tss_s tss[CONFIG_MAX_CPUS];

EXTERN পূর্ণসংখ্যা i386_paging_enabled;

#endif /* __GLO_X86_H__ */
