#ifndef __SMP_H__
#define __SMP_H__

#ifdef CONFIG_SMP

#ifndef __ASSEMBLY__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_smp.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN unsigned ncpus;
/* Number of virtual strands per physical core */
EXTERN unsigned ht_per_core;
/* which cpu is bootstrapping */
EXTERN unsigned bsp_cpu_id;

#define cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
ಶೂನ್ಯ smp_init(ಶೂನ್ಯ);

#define CPU_IS_BSP	1
#define CPU_IS_READY	2

ಸಂರಚನಾ cpu {
	u32_t flags;
};

EXTERN ಸಂರಚನಾ cpu cpus[CONFIG_MAX_CPUS];

#define cpu_set_flag(cpu, flag)	ಕುರು { cpus[cpu].flags |= (flag); } ಯಾವತ್(0)
#define cpu_clear_flag(cpu, flag) ಕುರು { cpus[cpu].flags &= ~(flag); } ಯಾವತ್(0)
#define cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#define cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_lock)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_lock)
	
ಶೂನ್ಯ wait_for_APs_to_finish_booting(ಶೂನ್ಯ);
ಶೂನ್ಯ ap_boot_finished(unsigned cpu);
ಶೂನ್ಯ smp_shutdown_aps(ಶೂನ್ಯ );

/* IPI handlers */
ಶೂನ್ಯ smp_ipi_halt_handler(ಶೂನ್ಯ);
ಶೂನ್ಯ smp_ipi_sched_handler(ಶೂನ್ಯ);

ಶೂನ್ಯ smp_schedule(unsigned cpu);
/* stop a processes on a different cpu */
ಶೂನ್ಯ smp_schedule_stop_proc(ಸಂರಚನಾ proc * p);
/* stop a process on a different cpu because its address space is being changed */
ಶೂನ್ಯ smp_schedule_vminhibit(ಸಂರಚನಾ proc * p);
/* stop the process and for saving its full context */
ಶೂನ್ಯ smp_schedule_stop_proc_save_ctx(ಸಂರಚನಾ proc * p);
/* migrate the full context of a process to the destination CPU */
ಶೂನ್ಯ smp_schedule_migrate_proc(ಸಂರಚನಾ proc * p, unsigned dest_cpu);

ಶೂನ್ಯ arch_send_smp_schedule_ipi(unsigned cpu);
ಶೂನ್ಯ arch_smp_halt_cpu(ಶೂನ್ಯ);

/* deal with x-cpu scheduling event */
ಶೂನ್ಯ smp_sched_handler(ಶೂನ್ಯ);

#endif /* __ASSEMBLY__ */

#endif /* CONFIG_SMP */

#endif /* __SMP_H__ */
