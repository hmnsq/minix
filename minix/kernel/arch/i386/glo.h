#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

EXTERN ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN ସଂରଚନା tss_s tss[CONFIG_MAX_CPUS];

EXTERN ପୂର୍ଣ୍ଣସଂଖ୍ୟା i386_paging_enabled;

#endif /* __GLO_X86_H__ */
