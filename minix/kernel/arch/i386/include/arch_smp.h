#ifndef __SMP_X86_H__
#define __SMP_X86_H__

#شامل "arch_proto.h" /* K_STACK_SIZE */

#define MAX_NR_INTERRUPT_ENTRIES	128

#ifndef __ASSEMBLY__

/* returns the current cpu id */
#define cpuid	(((u32_t *)(((u32_t)get_پشته_frame() + (K_پشته_SIZE - 1)) \
						& ~(K_پشته_SIZE - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#define smp_single_cpu_fallback() انجامبده {		\
	  tss_init(0, get_k_پشته_بالاترین(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_finish_booting();			\
} وقتی(0)

بیرونی unsigned نویسه cpuid2apicid[CONFIG_MAX_CPUS];

#define barrier()	انجامبده { mfence(); } وقتی(0)

#endif

#endif /* __SMP_X86_H__ */

