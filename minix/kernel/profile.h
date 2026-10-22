#अगर_अघोषित PROFILE_H
#घोषणा PROFILE_H

#समावेश <minix/profile.h>

#अगर SPROFILE	/* statistical profiling */

#समावेश "arch_watchdog.h"

#घोषणा SAMPLE_बफर_आकार	(64 << 20)
बाह्य अक्षर sprof_sample_बफर[SAMPLE_बफर_आकार];

EXTERN पूर्णांक sprofiling;			/* whether profiling is running */
EXTERN पूर्णांक sprofiling_type;			/* whether profiling is running */
EXTERN पूर्णांक sprof_mem_आकार;		/* available user memory for data */
EXTERN काष्ठा sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_तथ्य_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

व्योम nmi_sprofile_handler(काष्ठा nmi_frame * frame);

#पूर्ण_अगर /* SPROFILE */

#पूर्ण_अगर /* PROFILE_H */

