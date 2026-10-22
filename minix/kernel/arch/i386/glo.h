#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#include "kernel/kernel.h"
#include "arch_proto.h"

EXTERN පූර්ණ_සංඛ්‍යා cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN sa_nracanaa tss_s tss[CONFIG_MAX_CPUS];

EXTERN පූර්ණ_සංඛ්‍යා i386_paging_enabled;

#endif /* __GLO_X86_H__ */
