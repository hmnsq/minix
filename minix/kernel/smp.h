#که_نه_تعریف_شوی __SMP_H__
#تعریف_کړه __SMP_H__

#که_تعریف_شوی CONFIG_SMP

#که_نه_تعریف_شوی __ASSEMBLY__

#ضمن "kernel/kernel.h"
#ضمن "arch_smp.h"
#ضمن "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN بینښه ncpus;
/* Number of virtual strands per physical core */
EXTERN بینښه ht_فی_core;
/* which cpu is bootstrapping */
EXTERN بینښه bsp_cpu_id;

#تعریف_کړه cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
فراغ smp_init(فراغ);

#تعریف_کړه CPU_IS_BSP	1
#تعریف_کړه CPU_IS_READY	2

بنية cpu {
	u32_t flags;
};

EXTERN بنية cpu cpus[CONFIG_MAX_CPUS];

#تعریف_کړه cpu_سېټ_flag(cpu, flag)	افعل { cpus[cpu].flags |= (flag); } طالما(0)
#تعریف_کړه cpu_clear_flag(cpu, flag) افعل { cpus[cpu].flags &= ~(flag); } طالما(0)
#تعریف_کړه cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#تعریف_کړه cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_قلف)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_قلف)
	
فراغ wait_for_APs_منزل_پای_booting(فراغ);
فراغ ap_boot_پای_ته_رسېدلی(بینښه cpu);
فراغ smp_shutdown_aps(فراغ );

/* IPI handlers */
فراغ smp_ipi_halt_handler(فراغ);
فراغ smp_ipi_sched_handler(فراغ);

فراغ smp_مهالوېش(بینښه cpu);
/* stop a processes on a different cpu */
فراغ smp_مهالوېش_stop_proc(بنية proc * p);
/* stop a process on a different cpu because its address space is being changed */
فراغ smp_مهالوېش_vminhibit(بنية proc * p);
/* stop the process and for saving its full context */
فراغ smp_مهالوېش_stop_proc_save_ctx(بنية proc * p);
/* migrate the full context of a process to the destination CPU */
فراغ smp_مهالوېش_migrate_proc(بنية proc * p, بینښه dest_cpu);

فراغ arch_send_smp_مهالوېش_ipi(بینښه cpu);
فراغ arch_smp_halt_cpu(فراغ);

/* deal with x-cpu scheduling event */
فراغ smp_sched_handler(فراغ);

#د_که_پای /* __ASSEMBLY__ */

#د_که_پای /* CONFIG_SMP */

#د_که_پای /* __SMP_H__ */
