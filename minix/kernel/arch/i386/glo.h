#جيڪڏهن_وصف_نه_ٿيل __GLO_X86_H__
#وصف_ڪريو __GLO_X86_H__

#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"

EXTERN صحيح cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN بنية tss_s tss[CONFIG_MAX_CPUS];

EXTERN صحيح i386_paging_enabled;

#جيڪڏهن_پورو /* __GLO_X86_H__ */
