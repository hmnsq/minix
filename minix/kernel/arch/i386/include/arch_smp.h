#यदि_अपरिभाषितम् __SMP_X86_H__
#परिभाषय __SMP_X86_H__

#अन्तर्भाव "arch_proto.h" /* K_STACK_SIZE */

#परिभाषय MAX_NR_व्यवधानम्_ENTRIES	128

#यदि_अपरिभाषितम् __ASSEMBLY__

/* returns the current cpu id */
#परिभाषय cpuid	(((u32_t *)(((u32_t)get_राशिः_frame() + (K_राशिः_आकार - 1)) \
						& ~(K_राशिः_आकार - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#परिभाषय smp_single_cpu_fallback() कुरु {		\
	  tss_init(0, get_k_राशिः_शिखरम्(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_finish_booting();			\
} यावत्(0)

बाह्य अचिह्नित वर्ण cpuid2apicid[CONFIG_MAX_CPUS];

#परिभाषय barrier()	कुरु { mfence(); } यावत्(0)

#यदि_अन्तः

#यदि_अन्तः /* __SMP_X86_H__ */

