#ifndef __SMP_X86_H__
#define __SMP_X86_H__

#ضمن "arch_proto.h" /* K_STACK_SIZE */

#define MAX_NR_INTERRUPT_ENTRIES	128

#ifndef __ASSEMBLY__

/* returns the current cpu id */
#define cpuid	(((u32_t *)(((u32_t)get_stack_frame() + (K_STACK_SIZE - 1)) \
						& ~(K_STACK_SIZE - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#define smp_single_cpu_fallback() 𐤐𐤏𐤋 {		\
	  tss_init(0, get_k_stack_top(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_finish_booting();			\
} طالما(0)

خارجي unsigned حرف cpuid2apicid[CONFIG_MAX_CPUS];

#define barrier()	𐤐𐤏𐤋 { mfence(); } طالما(0)

#endif

#endif /* __SMP_X86_H__ */

