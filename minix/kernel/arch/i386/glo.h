#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

EXTERN ಪೂರ್ಣಾಙ್ಕ cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN ಸಂರಚನಾ tss_s tss[CONFIG_MAX_CPUS];

EXTERN ಪೂರ್ಣಾಙ್ಕ i386_paging_enabled;

#endif /* __GLO_X86_H__ */
