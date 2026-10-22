#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __SMP_X86_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __SMP_X86_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h" /* K_STACK_SIZE */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_NR_ਰੁਕਾਵਟ_ENTRIES	128

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

/* returns the current cpu id */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpuid	(((u32_t *)(((u32_t)get_ਢੇਰ_ਚੌਖਟਾ() + (K_ਢੇਰ_ਮਾਪ - 1)) \
						& ~(K_ਢੇਰ_ਮਾਪ - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ smp_single_cpu_fallback() ਕਰੋ {		\
	  tss_init(0, get_k_ਢੇਰ_ਸਿਖਰ(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_ਅੰਤ_booting();			\
} ਜਦੋਂ(0)

ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ cpuid2apicid[CONFIG_MAX_CPUS];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ barrier()	ਕਰੋ { mfence(); } ਜਦੋਂ(0)

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* __SMP_X86_H__ */

