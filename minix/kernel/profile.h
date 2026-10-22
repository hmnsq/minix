#ifndef PROFILE_H
#define PROFILE_H

#അന്തര്ഭാവ <minix/profile.h>

#എങ്കിൽ SPROFILE	/* statistical profiling */

#അന്തര്ഭാവ "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
ബാഹ്യ അക്ഷരം sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN പൂർണ്ണസംഖ്യ sprofiling;			/* whether profiling is running */
EXTERN പൂർണ്ണസംഖ്യ sprofiling_type;			/* whether profiling is running */
EXTERN പൂർണ്ണസംഖ്യ sprof_mem_size;		/* available user memory for data */
EXTERN ഘടന sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

ശൂന്യം nmi_sprofile_handler(ഘടന nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

