#که_نه_تعریف_شوی PROFILE_H
#تعریف_کړه PROFILE_H

#ضمن <minix/profile.h>

#إذا SPROFILE	/* statistical profiling */

#ضمن "arch_watchdog.h"

#تعریف_کړه SAMPLE_بفر_اندازه	(64 << 20)
خارجي حرف sprof_sample_بفر[SAMPLE_بفر_اندازه];

EXTERN صحيح sprofiling;			/* whether profiling is running */
EXTERN صحيح sprofiling_type;			/* whether profiling is running */
EXTERN صحيح sprof_mem_اندازه;		/* available user memory for data */
EXTERN بنية sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_بایټونه sprof_معلومات_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

فراغ nmi_sprofile_handler(بنية nmi_چوکاټ * چوکاټ);

#د_که_پای /* SPROFILE */

#د_که_پای /* PROFILE_H */

