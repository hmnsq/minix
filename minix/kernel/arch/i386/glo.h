#నిర్వచితం_కాకపోతే __GLO_X86_H__
#నిర్వచించు __GLO_X86_H__

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"

EXTERN పూర్ణాంకం cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN నిర్మాణం tss_s tss[CONFIG_MAX_CPUS];

EXTERN పూర్ణాంకం i386_paging_enabled;

#అయితే_ముగింపు /* __GLO_X86_H__ */
