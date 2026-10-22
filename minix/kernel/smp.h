#جيڪڏهن_وصف_نه_ٿيل __SMP_H__
#وصف_ڪريو __SMP_H__

#جيڪڏهن_وصف_ٿيل CONFIG_SMP

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

#ضمن "kernel/kernel.h"
#ضمن "arch_smp.h"
#ضمن "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN بنانشان ncpus;
/* Number of virtual strands per physical core */
EXTERN بنانشان ht_في_core;
/* which cpu is bootstrapping */
EXTERN بنانشان bsp_cpu_id;

#وصف_ڪريو cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
فراغ smp_init(فراغ);

#وصف_ڪريو CPU_IS_BSP	1
#وصف_ڪريو CPU_IS_READY	2

بنية cpu {
	u32_t flags;
};

EXTERN بنية cpu cpus[CONFIG_MAX_CPUS];

#وصف_ڪريو cpu_سيٽ_flag(cpu, flag)	افعل { cpus[cpu].flags |= (flag); } طالما(0)
#وصف_ڪريو cpu_clear_flag(cpu, flag) افعل { cpus[cpu].flags &= ~(flag); } طالما(0)
#وصف_ڪريو cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#وصف_ڪريو cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_تالو)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_تالو)
	
فراغ wait_for_APs_منزل_پڄاڻي_booting(فراغ);
فراغ ap_boot_ختم(بنانشان cpu);
فراغ smp_shutdown_aps(فراغ );

/* IPI handlers */
فراغ smp_ipi_halt_handler(فراغ);
فراغ smp_ipi_sched_handler(فراغ);

فراغ smp_وقت_جدول(بنانشان cpu);
/* stop a processes on a different cpu */
فراغ smp_وقت_جدول_stop_proc(بنية proc * p);
/* stop a process on a different cpu because its address space is being changed */
فراغ smp_وقت_جدول_vminhibit(بنية proc * p);
/* stop the process and for saving its full context */
فراغ smp_وقت_جدول_stop_proc_save_ctx(بنية proc * p);
/* migrate the full context of a process to the destination CPU */
فراغ smp_وقت_جدول_migrate_proc(بنية proc * p, بنانشان dest_cpu);

فراغ arch_send_smp_وقت_جدول_ipi(بنانشان cpu);
فراغ arch_smp_halt_cpu(فراغ);

/* deal with x-cpu scheduling event */
فراغ smp_sched_handler(فراغ);

#جيڪڏهن_پورو /* __ASSEMBLY__ */

#جيڪڏهن_پورو /* CONFIG_SMP */

#جيڪڏهن_پورو /* __SMP_H__ */
