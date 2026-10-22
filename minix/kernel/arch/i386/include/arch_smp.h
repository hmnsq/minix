#ifndef __SMP_X86_H__
#define __SMP_X86_H__

#ಅನ್ತರ್ಭಾವ "arch_proto.h" /* K_STACK_SIZE */

#define MAX_NR_INTERRUPT_ENTRIES	128

#ifndef __ASSEMBLY__

/* returns the current cpu id */
#define cpuid	(((u32_t *)(((u32_t)get_ರಾಶಿ_frame() + (K_ರಾಶಿ_SIZE - 1)) \
						& ~(K_ರಾಶಿ_SIZE - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#define smp_single_cpu_fallback() ಕುರು {		\
	  tss_init(0, get_k_ರಾಶಿ_ತುದಿ(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_finish_booting();			\
} ಯಾವತ್(0)

ಬಾಹ್ಯ unsigned ವರ್ಣ cpuid2apicid[CONFIG_MAX_CPUS];

#define barrier()	ಕುರು { mfence(); } ಯಾವತ್(0)

#endif

#endif /* __SMP_X86_H__ */

