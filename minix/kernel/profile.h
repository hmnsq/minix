#جے_بنا_تعریف PROFILE_H
#تعریف_کرو PROFILE_H

#ضمن <minix/profile.h>

#إذا SPROFILE	/* statistical profiling */

#ضمن "arch_watchdog.h"

#تعریف_کرو SAMPLE_بفر_ناپ	(64 << 20)
خارجي حرف sprof_sample_بفر[SAMPLE_بفر_ناپ];

EXTERN صحيح sprofiling;			/* whether profiling is running */
EXTERN صحيح sprofiling_type;			/* whether profiling is running */
EXTERN صحيح sprof_mem_ناپ;		/* available user memory for data */
EXTERN بنية sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_بائٹاں sprof_انکڑے_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

فراغ nmi_sprofile_handler(بنية nmi_چوکھٹا * چوکھٹا);

#جے_انت /* SPROFILE */

#جے_انت /* PROFILE_H */

