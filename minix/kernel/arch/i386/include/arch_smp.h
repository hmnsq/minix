#جيڪڏهن_وصف_نه_ٿيل __SMP_X86_H__
#وصف_ڪريو __SMP_X86_H__

#ضمن "arch_proto.h" /* K_STACK_SIZE */

#وصف_ڪريو MAX_NR_مداخلت_ENTRIES	128

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

/* returns the current cpu id */
#وصف_ڪريو cpuid	(((u32_t *)(((u32_t)get_ڍير_چوکٺو() + (K_ڍير_سائيز - 1)) \
						& ~(K_ڍير_سائيز - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#وصف_ڪريو smp_single_cpu_fallback() افعل {		\
	  tss_init(0, get_k_ڍير_چوٽي(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_پڄاڻي_booting();			\
} طالما(0)

خارجي بنانشان حرف cpuid2apicid[CONFIG_MAX_CPUS];

#وصف_ڪريو barrier()	افعل { mfence(); } طالما(0)

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* __SMP_X86_H__ */

