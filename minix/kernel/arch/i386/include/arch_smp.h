#ifndef __SMP_X86_H__
#define __SMP_X86_H__

#অন্তর্ভাব "arch_proto.h" /* K_STACK_SIZE */

#define MAX_NR_INTERRUPT_ENTRIES	128

#ifndef __ASSEMBLY__

/* returns the current cpu id */
#define cpuid	(((u32_t *)(((u32_t)get_স্তূপ_frame() + (K_স্তূপ_SIZE - 1)) \
						& ~(K_স্তূপ_SIZE - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#define smp_single_cpu_fallback() কুরু {		\
	  tss_init(0, get_k_স্তূপ_শীর্ষ(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_finish_booting();			\
} যাবত্(0)

বাহ্য unsigned বর্ণ cpuid2apicid[CONFIG_MAX_CPUS];

#define barrier()	কুরু { mfence(); } যাবত্(0)

#endif

#endif /* __SMP_X86_H__ */

