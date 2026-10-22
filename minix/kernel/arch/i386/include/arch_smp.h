#నిర్వచితం_కాకపోతే __SMP_X86_H__
#నిర్వచించు __SMP_X86_H__

#అన్తర్భావ "arch_proto.h" /* K_STACK_SIZE */

#నిర్వచించు MAX_NR_అంతరాయం_ENTRIES	128

#నిర్వచితం_కాకపోతే __ASSEMBLY__

/* returns the current cpu id */
#నిర్వచించు cpuid	(((u32_t *)(((u32_t)get_దొంతర_చట్రం() + (K_దొంతర_కొలత - 1)) \
						& ~(K_దొంతర_కొలత - 1)))[-1])
/* 
 * in case apic or smp is disabled in boot monitor, we need to finish single cpu
 * boot using the legacy PIC
 */
#నిర్వచించు smp_single_cpu_fallback() చేయి {		\
	  tss_init(0, get_k_దొంతర_శిఖరం(0));	\
	  bsp_cpu_id = 0;			\
	  ncpus = 1;				\
	  bsp_ముగింపు_booting();			\
} అయితే(0)

బాహ్య గుర్తులేని అక్షరం cpuid2apicid[CONFIG_MAX_CPUS];

#నిర్వచించు barrier()	చేయి { mfence(); } అయితే(0)

#అయితే_ముగింపు

#అయితే_ముగింపు /* __SMP_X86_H__ */

