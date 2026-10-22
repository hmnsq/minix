#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROFILE_H
#ವ್ಯಾಖ್ಯಾನಿಸು PROFILE_H

#ಅನ್ತರ್ಭಾವ <minix/profile.h>

#ಆದರೆ SPROFILE	/* statistical profiling */

#ಅನ್ತರ್ಭಾವ "arch_watchdog.h"

#ವ್ಯಾಖ್ಯಾನಿಸು SAMPLE_ಬಫರ್_ಗಾತ್ರ	(64 << 20)
ಬಾಹ್ಯ ಅಕ್ಷರ sprof_sample_ಬಫರ್[SAMPLE_ಬಫರ್_ಗಾತ್ರ];

EXTERN ಪೂರ್ಣಾಂಕ sprofiling;			/* whether profiling is running */
EXTERN ಪೂರ್ಣಾಂಕ sprofiling_type;			/* whether profiling is running */
EXTERN ಪೂರ್ಣಾಂಕ sprof_mem_ಗಾತ್ರ;		/* available user memory for data */
EXTERN ರಚನೆ sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_ಬೈಟುಗಳು sprof_ದತ್ತಾಂಶ_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

ಶೂನ್ಯ nmi_sprofile_handler(ರಚನೆ nmi_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು);

#ಆದರೆ_ಅಂತ್ಯ /* SPROFILE */

#ಆದರೆ_ಅಂತ್ಯ /* PROFILE_H */

