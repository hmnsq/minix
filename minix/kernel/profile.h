#ifndef PROFILE_H
#define PROFILE_H

#অন্তর্ভাব <minix/profile.h>

#যদি SPROFILE	/* statistical profiling */

#অন্তর্ভাব "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
বাহ্য বর্ণ sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN পূর্ণাঙ্ক sprofiling;			/* whether profiling is running */
EXTERN পূর্ণাঙ্ক sprofiling_type;			/* whether profiling is running */
EXTERN পূর্ণাঙ্ক sprof_mem_size;		/* available user memory for data */
EXTERN সংরচনা sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

শূন্য nmi_sprofile_handler(সংরচনা nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

