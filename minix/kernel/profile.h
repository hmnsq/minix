#ifndef PROFILE_H
#define PROFILE_H

#ضمن <minix/profile.h>

#إذا SPROFILE	/* statistical profiling */

#ضمن "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
خارجي حرف sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN صحيح sprofiling;			/* whether profiling is running */
EXTERN صحيح sprofiling_type;			/* whether profiling is running */
EXTERN صحيح sprof_mem_size;		/* available user memory for data */
EXTERN بنية sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

فراغ nmi_sprofile_handler(بنية nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

