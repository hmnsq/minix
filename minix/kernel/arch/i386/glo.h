#ifndef __GLO_X86_H__
#define __GLO_X86_H__

#include "kernel/kernel.h"
#include "arch_proto.h"

EXTERN ຈຳນວນເຕັມ cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN sa_nracanaa tss_s tss[CONFIG_MAX_CPUS];

EXTERN ຈຳນວນເຕັມ i386_paging_enabled;

#endif /* __GLO_X86_H__ */
