#جيڪڏهن_وصف_نه_ٿيل PROFILE_H
#وصف_ڪريو PROFILE_H

#ضمن <minix/profile.h>

#إذا SPROFILE	/* statistical profiling */

#ضمن "arch_watchdog.h"

#وصف_ڪريو SAMPLE_بفر_سائيز	(64 << 20)
خارجي حرف sprof_sample_بفر[SAMPLE_بفر_سائيز];

EXTERN صحيح sprofiling;			/* whether profiling is running */
EXTERN صحيح sprofiling_type;			/* whether profiling is running */
EXTERN صحيح sprof_mem_سائيز;		/* available user memory for data */
EXTERN بنية sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_بائيٽون sprof_معلومات_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

فراغ nmi_sprofile_handler(بنية nmi_چوکٺو * چوکٺو);

#جيڪڏهن_پورو /* SPROFILE */

#جيڪڏهن_پورو /* PROFILE_H */

