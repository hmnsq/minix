#यदि_अपरिभाषितम् PROFILE_H
#परिभाषय PROFILE_H

#अन्तर्भाव <minix/profile.h>

#यदि SPROFILE	/* statistical profiling */

#अन्तर्भाव "arch_watchdog.h"

#परिभाषय SAMPLE_बफर_आकार	(64 << 20)
बाह्य वर्ण sprof_sample_बफर[SAMPLE_बफर_आकार];

EXTERN पूर्णाङ्क sprofiling;			/* whether profiling is running */
EXTERN पूर्णाङ्क sprofiling_type;			/* whether profiling is running */
EXTERN पूर्णाङ्क sprof_mem_आकार;		/* available user memory for data */
EXTERN संरचना sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_अष्टकानि sprof_दत्त_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

शून्य nmi_sprofile_handler(संरचना nmi_frame * frame);

#यदि_अन्तः /* SPROFILE */

#यदि_अन्तः /* PROFILE_H */

