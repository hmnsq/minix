#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __GLO_X86_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __GLO_X86_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

EXTERN ਪੂਰਨ cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN ਬਣਤਰ tss_s tss[CONFIG_MAX_CPUS];

EXTERN ਪੂਰਨ i386_paging_enabled;

#ਜੇ_ਅੰਤ /* __GLO_X86_H__ */
