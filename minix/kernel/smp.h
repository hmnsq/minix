#నిర్వచితం_కాకపోతే __SMP_H__
#నిర్వచించు __SMP_H__

#నిర్వచితమైతే CONFIG_SMP

#నిర్వచితం_కాకపోతే __ASSEMBLY__

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_smp.h"
#అన్తర్భావ "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN గుర్తులేని ncpus;
/* Number of virtual strands per physical core */
EXTERN గుర్తులేని ht_చొప్పున_core;
/* which cpu is bootstrapping */
EXTERN గుర్తులేని bsp_cpu_id;

#నిర్వచించు cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
శూన్య smp_init(శూన్య);

#నిర్వచించు CPU_IS_BSP	1
#నిర్వచించు CPU_IS_READY	2

నిర్మాణం cpu {
	u32_t flags;
};

EXTERN నిర్మాణం cpu cpus[CONFIG_MAX_CPUS];

#నిర్వచించు cpu_సమితి_flag(cpu, flag)	చేయి { cpus[cpu].flags |= (flag); } అయితే(0)
#నిర్వచించు cpu_clear_flag(cpu, flag) చేయి { cpus[cpu].flags &= ~(flag); } అయితే(0)
#నిర్వచించు cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#నిర్వచించు cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_తాళం)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_తాళం)
	
శూన్య wait_for_APs_గమ్యం_ముగింపు_booting(శూన్య);
శూన్య ap_boot_ముగిసిన(గుర్తులేని cpu);
శూన్య smp_shutdown_aps(శూన్య );

/* IPI handlers */
శూన్య smp_ipi_halt_handler(శూన్య);
శూన్య smp_ipi_sched_handler(శూన్య);

శూన్య smp_కాలపట్టిక(గుర్తులేని cpu);
/* stop a processes on a different cpu */
శూన్య smp_కాలపట్టిక_stop_proc(నిర్మాణం proc * p);
/* stop a process on a different cpu because its address space is being changed */
శూన్య smp_కాలపట్టిక_vminhibit(నిర్మాణం proc * p);
/* stop the process and for saving its full context */
శూన్య smp_కాలపట్టిక_stop_proc_save_ctx(నిర్మాణం proc * p);
/* migrate the full context of a process to the destination CPU */
శూన్య smp_కాలపట్టిక_migrate_proc(నిర్మాణం proc * p, గుర్తులేని dest_cpu);

శూన్య arch_send_smp_కాలపట్టిక_ipi(గుర్తులేని cpu);
శూన్య arch_smp_halt_cpu(శూన్య);

/* deal with x-cpu scheduling event */
శూన్య smp_sched_handler(శూన్య);

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు /* CONFIG_SMP */

#అయితే_ముగింపు /* __SMP_H__ */
