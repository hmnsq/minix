#నిర్వచితం_కాకపోతే PROFILE_H
#నిర్వచించు PROFILE_H

#అన్తర్భావ <minix/profile.h>

#ఒకవేళ SPROFILE	/* statistical profiling */

#అన్తర్భావ "arch_watchdog.h"

#నిర్వచించు SAMPLE_బఫరు_కొలత	(64 << 20)
బాహ్య అక్షరం sprof_sample_బఫరు[SAMPLE_బఫరు_కొలత];

EXTERN పూర్ణాంకం sprofiling;			/* whether profiling is running */
EXTERN పూర్ణాంకం sprofiling_type;			/* whether profiling is running */
EXTERN పూర్ణాంకం sprof_mem_కొలత;		/* available user memory for data */
EXTERN నిర్మాణం sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_బైట్లు sprof_దత్తాంశం_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

శూన్య nmi_sprofile_handler(నిర్మాణం nmi_చట్రం * చట్రం);

#అయితే_ముగింపు /* SPROFILE */

#అయితే_ముగింపు /* PROFILE_H */

