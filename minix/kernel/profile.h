#ifndef PROFILE_H
#define PROFILE_H

#अन्तर्भाव <minix/profile.h>

#सचे SPROFILE	/* statistical profiling */

#अन्तर्भाव "arch_watchdog.h"

#define SAMPLE_BUFFER_SIZE	(64 << 20)
बाह्य वर्ण sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

EXTERN पूर्णाङ्क sprofiling;			/* whether profiling is running */
EXTERN पूर्णाङ्क sprofiling_type;			/* whether profiling is running */
EXTERN पूर्णाङ्क sprof_mem_size;		/* available user memory for data */
EXTERN संरचना sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

शून्य nmi_sprofile_handler(संरचना nmi_frame * frame);

#endif /* SPROFILE */

#endif /* PROFILE_H */

