#ifndef __SMP_H__
#define __SMP_H__

#ifdef CONFIG_SMP

#ifndef __ASSEMBLY__

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_smp.h"
#अन्तर्भाव "spinlock.h"

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
शून्य smp_init(शून्य);

#define CPU_IS_BSP	1
#define CPU_IS_READY	2

संरचना cpu {
	u32_t flags;
};

EXTERN संरचना cpu cpus[CONFIG_MAX_CPUS];

#define cpu_set_flag(cpu, flag)	करोहि { cpus[cpu].flags |= (flag); } याव(0)
#define cpu_clear_flag(cpu, flag) करोहि { cpus[cpu].flags &= ~(flag); } याव(0)
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
	
शून्य wait_for_APs_to_finish_booting(शून्य);
शून्य ap_boot_finished(unsigned cpu);
शून्य smp_shutdown_aps(शून्य );

/* IPI handlers */
शून्य smp_ipi_halt_handler(शून्य);
शून्य smp_ipi_sched_handler(शून्य);

शून्य smp_schedule(unsigned cpu);
/* stop a processes on a different cpu */
शून्य smp_schedule_stop_proc(संरचना proc * p);
/* stop a process on a different cpu because its address space is being changed */
शून्य smp_schedule_vminhibit(संरचना proc * p);
/* stop the process and for saving its full context */
शून्य smp_schedule_stop_proc_save_ctx(संरचना proc * p);
/* migrate the full context of a process to the destination CPU */
शून्य smp_schedule_migrate_proc(संरचना proc * p, unsigned dest_cpu);

शून्य arch_send_smp_schedule_ipi(unsigned cpu);
शून्य arch_smp_halt_cpu(शून्य);

/* deal with x-cpu scheduling event */
शून्य smp_sched_handler(शून्य);

#endif /* __ASSEMBLY__ */

#endif /* CONFIG_SMP */

#endif /* __SMP_H__ */
