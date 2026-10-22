#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROFILE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROFILE_H

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/profile.h>

#ਜੇ SPROFILE	/* statistical profiling */

#ਅਨ੍ਤਰ੍ਭਾਵ "arch_watchdog.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAMPLE_ਬਫਰ_ਮਾਪ	(64 << 20)
ਬਾਹਰੀ ਅੱਖਰ sprof_sample_ਬਫਰ[SAMPLE_ਬਫਰ_ਮਾਪ];

EXTERN ਪੂਰਨ sprofiling;			/* whether profiling is running */
EXTERN ਪੂਰਨ sprofiling_type;			/* whether profiling is running */
EXTERN ਪੂਰਨ sprof_mem_ਮਾਪ;		/* available user memory for data */
EXTERN ਬਣਤਰ sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_ਬਾਈਟਾਂ sprof_ਅੰਕੜੇ_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

ਖਾਲੀ nmi_sprofile_handler(ਬਣਤਰ nmi_ਚੌਖਟਾ * ਚੌਖਟਾ);

#ਜੇ_ਅੰਤ /* SPROFILE */

#ਜੇ_ਅੰਤ /* PROFILE_H */

