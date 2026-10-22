#ifndef __SMP_X86_H__
#define __SMP_X86_H__

#अन्तर्भाव "arch_proto.h" /* K_STACK_SIZE */

#define MAX_NR_INTERRUPT_ENTRIES	128

#ifndef __ASSEMBLY__

/* returns the current cpu id */
#define cpuid	(((u32_t *)(((u32_t)get_थाक_frame() + (K_थाक_SIZE - 1)) \
						& ~(K_थाक_SIZE - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#define smp_single_cpu_fallback() गर {		\
	  tss_init(0, get_k_थाक_शीर्ष(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_finish_booting();			\
} जबसम्म(0)

बाह्य अचिन्हित वर्ण cpuid2apicid[CONFIG_MAX_CPUS];

#define barrier()	गर { mfence(); } जबसम्म(0)

#endif

#endif /* __SMP_X86_H__ */

