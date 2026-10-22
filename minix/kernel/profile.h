#اگر_تعریف_نشده PROFILE_H
#تعریف_کن PROFILE_H

#ضمن <minix/profile.h>

#إذا SPROFILE	/* statistical profiling */

#ضمن "arch_watchdog.h"

#تعریف_کن SAMPLE_میانگیر_اندازه	(64 << 20)
خارجي حرف sprof_sample_میانگیر[SAMPLE_میانگیر_اندازه];

EXTERN صحيح sprofiling;			/* whether profiling is running */
EXTERN صحيح sprofiling_type;			/* whether profiling is running */
EXTERN صحيح sprof_mem_اندازه;		/* available user memory for data */
EXTERN بنية sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_بایتها sprof_داده_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

فراغ nmi_sprofile_handler(بنية nmi_قاب * قاب);

#پایان_اگر /* SPROFILE */

#پایان_اگر /* PROFILE_H */

