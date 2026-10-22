#اگر_تعریف_نشده __SMP_X86_H__
#تعریف_کن __SMP_X86_H__

#ضمن "arch_proto.h" /* K_STACK_SIZE */

#تعریف_کن MAX_NR_وقفه_ENTRIES	128

#اگر_تعریف_نشده __ASSEMBLY__

/* returns the current cpu id */
#تعریف_کن cpuid	(((u32_t *)(((u32_t)get_پشته_قاب() + (K_پشته_اندازه - 1)) \
						& ~(K_پشته_اندازه - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#تعریف_کن smp_single_cpu_fallback() افعل {		\
	  tss_init(0, get_k_پشته_بالاترین(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_پایان_booting();			\
} طالما(0)

خارجي بیعلامت حرف cpuid2apicid[CONFIG_MAX_CPUS];

#تعریف_کن barrier()	افعل { mfence(); } طالما(0)

#پایان_اگر

#پایان_اگر /* __SMP_X86_H__ */

