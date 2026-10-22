#ifndef PROFILE_H
#define PROFILE_H

#অন্তর্ভাব <minix/profile.h>

#যদি SPROFILE	/* statistical profiling */

#অন্তর্ভাব "arch_watchdog.h"

#define SAMPLE_বাফার_আকার	(64 << 20)
বাহ্যিক অক্ষর sprof_sample_বাফার[SAMPLE_বাফার_আকার];

EXTERN পূর্ণসংখ্যা sprofiling;			/* whether profiling is running */
EXTERN পূর্ণসংখ্যা sprofiling_type;			/* whether profiling is running */
EXTERN পূর্ণসংখ্যা sprof_mem_আকার;		/* available user memory for data */
EXTERN কাঠামো sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_তথ্য_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

শূন্য nmi_sprofile_handler(কাঠামো nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

