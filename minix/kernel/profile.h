#اگر_غیر_تعریف_شدہ PROFILE_H
#تعریف_کرو PROFILE_H

#شامل <minix/profile.h>

#اگر SPROFILE	/* statistical profiling */

#شامل "arch_watchdog.h"

#تعریف_کرو SAMPLE_بفر_سائز	(64 << 20)
خارجی حرف sprof_sample_بفر[SAMPLE_بفر_سائز];

EXTERN صحیح sprofiling;			/* whether profiling is running */
EXTERN صحیح sprofiling_type;			/* whether profiling is running */
EXTERN صحیح sprof_mem_سائز;		/* available user memory for data */
EXTERN ساخت sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_بائٹیں sprof_ڈیٹا_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

خالی nmi_sprofile_handler(ساخت nmi_چوکھٹا * چوکھٹا);

#اگر_ختم /* SPROFILE */

#اگر_ختم /* PROFILE_H */

