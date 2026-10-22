#ifndef PROFILE_H
#define PROFILE_H

#ಅನ್ತರ್ಭಾವ <minix/profile.h>

#ಯದಿ SPROFILE	/* statistical profiling */

#ಅನ್ತರ್ಭಾವ "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
ಬಾಹ್ಯ ವರ್ಣ sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN ಪೂರ್ಣಾಙ್ಕ sprofiling;			/* whether profiling is running */
EXTERN ಪೂರ್ಣಾಙ್ಕ sprofiling_type;			/* whether profiling is running */
EXTERN ಪೂರ್ಣಾಙ್ಕ sprof_mem_size;		/* available user memory for data */
EXTERN ಸಂರಚನಾ sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

ಶೂನ್ಯ nmi_sprofile_handler(ಸಂರಚನಾ nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

