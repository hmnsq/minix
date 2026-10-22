#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __GLO_X86_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __GLO_X86_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

EXTERN ಪೂರ್ಣಾಂಕ cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN ರಚನೆ tss_s tss[CONFIG_MAX_CPUS];

EXTERN ಪೂರ್ಣಾಂಕ i386_paging_enabled;

#ಆದರೆ_ಅಂತ್ಯ /* __GLO_X86_H__ */
