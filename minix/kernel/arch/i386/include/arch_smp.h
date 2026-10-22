#که_نه_تعریف_شوی __SMP_X86_H__
#تعریف_کړه __SMP_X86_H__

#ضمن "arch_proto.h" /* K_STACK_SIZE */

#تعریف_کړه MAX_NR_مداخله_ENTRIES	128

#که_نه_تعریف_شوی __ASSEMBLY__

/* returns the current cpu id */
#تعریف_کړه cpuid	(((u32_t *)(((u32_t)get_ډېری_چوکاټ() + (K_ډېری_اندازه - 1)) \
						& ~(K_ډېری_اندازه - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#تعریف_کړه smp_single_cpu_fallback() افعل {		\
	  tss_init(0, get_k_ډېری_سر(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_پای_booting();			\
} طالما(0)

خارجي بینښه حرف cpuid2apicid[CONFIG_MAX_CPUS];

#تعریف_کړه barrier()	افعل { mfence(); } طالما(0)

#د_که_پای

#د_که_پای /* __SMP_X86_H__ */

