#ifndef __SMP_H__
#define __SMP_H__

#ifdef CONFIG_SMP

#ifndef __ASSEMBLY__

#ضمن "kernel/kernel.h"
#ضمن "arch_smp.h"
#ضمن "spinlock.h"

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
فراغ smp_init(فراغ);

#define CPU_IS_BSP	1
#define CPU_IS_READY	2

بنية cpu {
	u32_t flags;
};

EXTERN بنية cpu cpus[CONFIG_MAX_CPUS];

#define cpu_set_flag(cpu, flag)	افعل { cpus[cpu].flags |= (flag); } طالما(0)
#define cpu_clear_flag(cpu, flag) افعل { cpus[cpu].flags &= ~(flag); } طالما(0)
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
	
فراغ wait_for_APs_to_finish_booting(فراغ);
فراغ ap_boot_finished(unsigned cpu);
فراغ smp_shutdown_aps(فراغ );

/* IPI handlers */
فراغ smp_ipi_halt_handler(فراغ);
فراغ smp_ipi_sched_handler(فراغ);

فراغ smp_schedule(unsigned cpu);
/* stop a processes on a different cpu */
فراغ smp_schedule_stop_proc(بنية proc * p);
/* stop a process on a different cpu because its address space is being changed */
فراغ smp_schedule_vminhibit(بنية proc * p);
/* stop the process and for saving its full context */
فراغ smp_schedule_stop_proc_save_ctx(بنية proc * p);
/* migrate the full context of a process to the destination CPU */
فراغ smp_schedule_migrate_proc(بنية proc * p, unsigned dest_cpu);

فراغ arch_send_smp_schedule_ipi(unsigned cpu);
فراغ arch_smp_halt_cpu(فراغ);

/* deal with x-cpu scheduling event */
فراغ smp_sched_handler(فراغ);

#endif /* __ASSEMBLY__ */

#endif /* CONFIG_SMP */

#endif /* __SMP_H__ */
