#ifndef PROFILE_H
#define PROFILE_H

#ଅନ୍ତର୍ଭାଵ <minix/profile.h>

#ଯଦି SPROFILE	/* statistical profiling */

#ଅନ୍ତର୍ଭାଵ "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
ବାହ୍ୟ ଅକ୍ଷର sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN ପୂର୍ଣ୍ଣସଂଖ୍ୟା sprofiling;			/* whether profiling is running */
EXTERN ପୂର୍ଣ୍ଣସଂଖ୍ୟା sprofiling_type;			/* whether profiling is running */
EXTERN ପୂର୍ଣ୍ଣସଂଖ୍ୟା sprof_mem_size;		/* available user memory for data */
EXTERN ସଂରଚନା sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

ଶୂନ୍ୟ nmi_sprofile_handler(ସଂରଚନା nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

