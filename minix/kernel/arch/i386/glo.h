#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#অন্তর্ভাৱ "kernel/kernel.h"
#অন্তর্ভাৱ "arch_proto.h"

EXTERN পূর্ণাঙ্ক cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN সংরচনা tss_s tss[CONFIG_MAX_CPUS];

EXTERN পূর্ণাঙ্ক i386_paging_enabled;

#endif /* __GLO_X86_H__ */
