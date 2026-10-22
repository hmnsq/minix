#ifndef PROFILE_H
#define PROFILE_H

#شامل <minix/profile.h>

#اگر SPROFILE	/* statistical profiling */

#شامل "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
بیرونی نویسه sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN صحیح sprofiling;			/* whether profiling is running */
EXTERN صحیح sprofiling_type;			/* whether profiling is running */
EXTERN صحیح sprof_mem_size;		/* available user memory for data */
EXTERN ساختار sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

تهی nmi_sprofile_handler(ساختار nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

