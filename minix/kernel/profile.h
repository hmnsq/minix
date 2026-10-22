#ifndef PROFILE_H
#define PROFILE_H

#અન્તર્ભાવ <minix/profile.h>

#જો SPROFILE	/* statistical profiling */

#અન્તર્ભાવ "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
બાહ્ય અક્ષર sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN પૂર્ણાંક sprofiling;			/* whether profiling is running */
EXTERN પૂર્ણાંક sprofiling_type;			/* whether profiling is running */
EXTERN પૂર્ણાંક sprof_mem_size;		/* available user memory for data */
EXTERN સંરચના sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

શૂન્ય nmi_sprofile_handler(સંરચના nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

