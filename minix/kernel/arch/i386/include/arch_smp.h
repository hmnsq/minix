#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __SMP_X86_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __SMP_X86_H__

#ಅನ್ತರ್ಭಾವ "arch_proto.h" /* K_STACK_SIZE */

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_NR_ಅಡಚಣೆ_ENTRIES	128

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

/* returns the current cpu id */
#ವ್ಯಾಖ್ಯಾನಿಸು cpuid	(((u32_t *)(((u32_t)get_ರಾಶಿ_ಚೌಕಟ್ಟು() + (K_ರಾಶಿ_ಗಾತ್ರ - 1)) \
						& ~(K_ರಾಶಿ_ಗಾತ್ರ - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#ವ್ಯಾಖ್ಯಾನಿಸು smp_single_cpu_fallback() ಮಾಡು {		\
	  tss_init(0, get_k_ರಾಶಿ_ತುದಿ(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_ಮುಕ್ತಾಯ_booting();			\
} ಆಗ(0)

ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ cpuid2apicid[CONFIG_MAX_CPUS];

#ವ್ಯಾಖ್ಯಾನಿಸು barrier()	ಮಾಡು { mfence(); } ಆಗ(0)

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* __SMP_X86_H__ */

