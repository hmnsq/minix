#اگر_غیر_تعریف_شدہ __GLO_X86_H__
#تعریف_کرو __GLO_X86_H__

#شامل "kernel/kernel.h"
#شامل "arch_proto.h"

EXTERN صحیح cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN ساخت tss_s tss[CONFIG_MAX_CPUS];

EXTERN صحیح i386_paging_enabled;

#اگر_ختم /* __GLO_X86_H__ */
