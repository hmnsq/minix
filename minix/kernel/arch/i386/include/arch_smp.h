#جے_بنا_تعریف __SMP_X86_H__
#تعریف_کرو __SMP_X86_H__

#ضمن "arch_proto.h" /* K_STACK_SIZE */

#تعریف_کرو MAX_NR_رکاوٹ_ENTRIES	128

#جے_بنا_تعریف __ASSEMBLY__

/* returns the current cpu id */
#تعریف_کرو cpuid	(((u32_t *)(((u32_t)get_ڈھیر_چوکھٹا() + (K_ڈھیر_ناپ - 1)) \
						& ~(K_ڈھیر_ناپ - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#تعریف_کرو smp_single_cpu_fallback() افعل {		\
	  tss_init(0, get_k_ڈھیر_چوٹی(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_انت_booting();			\
} طالما(0)

خارجي بنانشانی حرف cpuid2apicid[CONFIG_MAX_CPUS];

#تعریف_کرو barrier()	افعل { mfence(); } طالما(0)

#جے_انت

#جے_انت /* __SMP_X86_H__ */

