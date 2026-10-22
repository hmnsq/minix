#ifndef PROFILE_H
#define PROFILE_H

#அந்தர்பாவ <minix/profile.h>

#எனில் SPROFILE	/* statistical profiling */

#அந்தர்பாவ "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
வெளி எழுத்து sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN முழு sprofiling;			/* whether profiling is running */
EXTERN முழு sprofiling_type;			/* whether profiling is running */
EXTERN முழு sprof_mem_size;		/* available user memory for data */
EXTERN கட்டமைப்பு sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

வெறுமை nmi_sprofile_handler(கட்டமைப்பு nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

