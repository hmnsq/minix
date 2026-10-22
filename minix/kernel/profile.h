#ifndef PROFILE_H
#define PROFILE_H

#include <minix/profile.h>

#ຖ້າ SPROFILE	/* statistical profiling */

#include "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
extern var_na sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN ຈຳນວນເຕັມ sprofiling;			/* whether profiling is running */
EXTERN ຈຳນວນເຕັມ sprofiling_type;			/* whether profiling is running */
EXTERN ຈຳນວນເຕັມ sprof_mem_size;		/* available user memory for data */
EXTERN sa_nracanaa sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

void nmi_sprofile_handler(sa_nracanaa nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

