#अगर_अघोषित __SMP_X86_H__
#घोषणा __SMP_X86_H__

#समावेश "arch_proto.h" /* K_STACK_SIZE */

#घोषणा MAX_NR_INTERRUPT_ENTRIES	128

#अगर_अघोषित __ASSEMBLY__

/* returns the current cpu id */
#घोषणा cpuid	(((u32_t *)(((u32_t)get_ढेर_frame() + (K_ढेर_आकार - 1)) \
						& ~(K_ढेर_आकार - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#घोषणा smp_single_cpu_fallback() करो {		\
	  tss_init(0, get_k_ढेर_शीर्ष(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_finish_booting();			\
} जबतक(0)

बाह्य अचिन्हित अक्षर cpuid2apicid[CONFIG_MAX_CPUS];

#घोषणा barrier()	करो { mfence(); } जबतक(0)

#पूर्ण_अगर

#पूर्ण_अगर /* __SMP_X86_H__ */

